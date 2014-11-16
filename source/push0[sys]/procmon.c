#include "push0.h"
#include "ramdisk.h"


unsigned char g_bActivated = NULL;


typedef struct _MODULE
{
    WCHAR* FileName;
    UINT32 FileIndex;
} MODULE;


MODULE PmModules[] = {
  {L"d3d8.dll"},
  {L"d3d9.dll"},
  {L"d3d10.dll"},
  {L"d3d11.dll"}
};


VOID
ProcessCallback(VOID *hParentId,
                VOID *hProcessId,
                BOOLEAN bCreate)
{
    if (bCreate)
    {
        DEVICE_EXTENSION *extension;

        //
        // Assign extension variable
        //
        extension = (DEVICE_EXTENSION*) g_DeviceObject->DeviceExtension;
        //
        // Assign current values into device extension.
        // User-mode apps will pick it up using DeviceIoControl calls.
        //
        extension->ProcessEvent.ProcessID = (UINT16) hProcessId;
        //
        // Signal the event thus the user-mode apps listening will be aware
        // that something interesting has happened.
        //

        KeSetEvent(ProcessEvent, 0, FALSE);

        KeClearEvent(ProcessEvent);
    }
}


VOID
ThreadCallback(VOID *processID,
               VOID *threadID,
               BOOLEAN create)
{
    DEVICE_EXTENSION *devExt;

    devExt = (DEVICE_EXTENSION*) g_DeviceObject->DeviceExtension;

    //devExt->ThreadEvent.threadOwner = (UINT16) processID;
    devExt->threadCallbackInfo.threadOwner = (UINT16) processID;
    devExt->threadCallbackInfo.threadID = (UINT16) threadID;
    devExt->threadCallbackInfo.create = create;


    KeSetEvent(ThreadEvent, 0, FALSE);

    KeClearEvent(ThreadEvent);
}


NTSTATUS
ToggleProcessMonitoring(PIRP Irp)
{
    BOOLEAN bActivate;

    bActivate = *(BOOLEAN *) Irp->AssociatedIrp.SystemBuffer;

    if (bActivate && !g_bActivated)
    {
        PsSetCreateProcessNotifyRoutine(ProcessCallback, 0);

        PsSetCreateThreadNotifyRoutine(ThreadCallback);

        FltFilterInstall( PushDriverObject );

        g_bActivated = 1;

        DbgPrint("[PUSH] Process Monitoring Activated\n");
    }
    else if (!bActivate && g_bActivated)
    {
        PsSetCreateProcessNotifyRoutine(ProcessCallback, 1);

        PsRemoveCreateThreadNotifyRoutine(ThreadCallback);

        FltStopFiltering();

        g_bActivated = 0;

        DbgPrint("[PUSH] Process Monitoring De-Activated\n");
    }

    return STATUS_SUCCESS;
}
