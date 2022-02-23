/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_kmdfex,
    0xe4075503,0x3876,0x4cc1,0x9b,0xd5,0xa6,0x62,0x12,0x19,0xfe,0x04);
// {e4075503-3876-4cc1-9bd5-a6621219fe04}
