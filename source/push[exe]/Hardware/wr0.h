#include <push.h>


//-----------------------------------------------------------------------------
//
// The Device type codes form 32768 to 65535 are for customer use.
//
//-----------------------------------------------------------------------------

#define OLS_TYPE 40000
#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe

#define IOCTL_OLS_READ_MSR \
    CTL_CODE(OLS_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_READ_PCI_CONFIG \
    CTL_CODE(OLS_TYPE, 0x851, METHOD_BUFFERED, FILE_READ_ACCESS)


//-----------------------------------------------------------------------------
//
// Typedef Struct
//
//-----------------------------------------------------------------------------

#pragma pack(push,4)

typedef struct  _OLS_READ_PCI_CONFIG_INPUT {
    ULONG PciAddress;
    ULONG PciOffset;
}   OLS_READ_PCI_CONFIG_INPUT;

#pragma pack(pop)

BOOL Wr0Rdmsr(DWORD Index, DWORD* EAX, DWORD* EDX);
BOOL Wr0ReadPciConfig(DWORD pciAddress, DWORD regAddress, BYTE* value, DWORD size);

extern HANDLE gHandle;
extern BOOLEAN Wr0DriverLoaded;