/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, kmdfexEvtDeviceAdd)
#pragma alloc_text (PAGE, kmdfexEvtDriverContextCleanup)
#endif

void registerCallback();

VOID
KmdfExUnload(
    PDRIVER_OBJECT DriverObject);

VOID
KmdfExUnregisterAllCallbacks(
    VOID
);


NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    //
    // Initialize WPP Tracing
    //
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = kmdfexEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config,
                           kmdfexEvtDeviceAdd
                           );

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE
                             );

    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        WPP_CLEANUP(DriverObject);
        return status;
    }

    kmdfexLog(__FUNCDNAME__);

    registerCallback();
    DriverObject->DriverUnload = KmdfExUnload;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    if (!NT_SUCCESS(status)) {
        // 
        // Undo what we've done and fail.
        // 
        KmdfExUnregisterAllCallbacks();
    }

    return status;
}

NTSTATUS
kmdfexEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = kmdfexCreateDevice(DeviceInit);

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

VOID
kmdfexEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
/*++
Routine Description:

    Free all the resources allocated in DriverEntry.

Arguments:

    DriverObject - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Stop WPP Tracing
    //
    WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)DriverObject));
}

VOID
KmdfExUnload(
    PDRIVER_OBJECT DriverObject)
{

    UNREFERENCED_PARAMETER(DriverObject);

    kmdfexLog(__FUNCDNAME__);

    KmdfExUnregisterAllCallbacks();
    return;
}

VOID
KmdfExUnregisterAllCallbacks(
    VOID)
{

    NTSTATUS status;

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2) 
    if (KmdfExPsCreateProcessEx2NotifyRoutineRegistered == TRUE) {
        status = PsSetCreateProcessNotifyRoutineEx2(
            PsCreateProcessNotifySubsystems,
            (PVOID)KmdfExPsCreateProcessNotifyRoutineEx2,
            TRUE);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#elif (NTDDI_VERSION < NTDDI_VISTA)
    if (KmdfExPsCreateProcessNotifyRoutineRegistered == TRUE) {
        status = PsSetCreateProcessNotifyRoutine(
            KmdfExPsCreateProcessNotifyRoutine,
            TRUE);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#else
    if (KmdfExPsCreateProcessExNotifyRoutineRegistered == TRUE) {
        status = PsSetCreateProcessNotifyRoutineEx(
            KmdfExPsCreateProcessNotifyRoutineEx,
            TRUE);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#endif

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
    if (KmdfExPsCreateThreadNotifyRoutineExRegistered == TRUE) {
        status = PsRemoveCreateThreadNotifyRoutine(
            KmdfExPsCreateThreadNotifyRoutineEx);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#else
    if (KmdfExPsCreateThreadNotifyRoutineRegistered == TRUE) {
        status = PsRemoveCreateThreadNotifyRoutine(
            KmdfExPsCreateThreadNotifyRoutine);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
#endif

    if (KmdfExPsLoadImageNotifyRoutineRegistered == TRUE) {
        status = PsRemoveLoadImageNotifyRoutine(KmdfExPsLoadImageNotifyRoutine);
        // 
        // This should work because we know we registered.
        // 
        ASSERT(NT_SUCCESS(status));
    }
}

void registerCallback()
{
    NTSTATUS status;

    CHAR     buffer[LOG_BUFFER_SIZE];

    kmdfexLog(__FUNCDNAME__);

    // 
    // PS CALLBACKS 
    // 
#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    status = PsSetCreateProcessNotifyRoutineEx2(PsCreateProcessNotifySubsystems,
        (PVOID)KmdfExPsCreateProcessNotifyRoutineEx2,
        FALSE);

    if (!NT_SUCCESS(status)) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "PsSetCreateProcessNotifyRoutineEx2 failed! Status 0x%x\n",
            status);
        kmdfexLog(buffer);
        return;
    }

    kmdfexLog("PsSetCreateProcessNotifyRoutineEx2");

    KmdfExPsCreateProcessEx2NotifyRoutineRegistered = TRUE;
#elif (NTDDI_VERSION < NTDDI_VISTA)

    status = PsSetCreateProcessNotifyRoutine(KmdfExPsCreateProcessNotifyRoutine,
        FALSE);

    if (!NT_SUCCESS(status)) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "PsSetCreateProcessNotifyRoutine failed! Status 0x%x\n",
            status);
        kmdfexLog(buffer);
        goto Exit;
    }

    KmdfExPsCreateProcessNotifyRoutineRegistered = TRUE;

#else

    status = PsSetCreateProcessNotifyRoutineEx(
        KmdfExPsCreateProcessNotifyRoutineEx,
        FALSE);

    if (!NT_SUCCESS(status)) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "PsSetCreateProcessNotifyRoutineEx failed! Status 0x%x\n",
            status);
        kmdfexLog(buffer);
        return;
    }

    kmdfexLog("PsSetCreateProcessNotifyRoutineEx");

    KmdfExPsCreateProcessExNotifyRoutineRegistered = TRUE;

#endif

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
    status = PsSetCreateThreadNotifyRoutineEx(PsCreateThreadNotifyNonSystem,
        (PVOID)KmdfExPsCreateThreadNotifyRoutineEx);

    if (!NT_SUCCESS(status)) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "PsSetCreateThreadNotifyRoutineEx failed! Status 0x%x\n",
            status);
        kmdfexLog(buffer);
        return;
    }

    kmdfexLog("PsSetCreateThreadNotifyRoutineEx");

    KmdfExPsCreateThreadNotifyRoutineExRegistered = TRUE;
#else
    status = PsSetCreateThreadNotifyRoutine(KmdfExPsCreateThreadNotifyRoutine);

    if (!NT_SUCCESS(status)) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "PsSetCreateThreadNotifyRoutine failed! Status 0x%x\n",
            status);
        kmdfexLog(buffer);
        return;
    }

    kmdfexLog("PsSetCreateThreadNotifyRoutine");

    KmdfExPsCreateThreadNotifyRoutineRegistered = TRUE;
#endif

    status = PsSetLoadImageNotifyRoutine(KmdfExPsLoadImageNotifyRoutine);

    if (!NT_SUCCESS(status)) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "PsSetLoadImageNotifyRoutine failed! Status 0x%x\n",
            status);
        kmdfexLog(buffer);
        return;
    }

    kmdfexLog("PsSetLoadImageNotifyRoutine");

    KmdfExPsLoadImageNotifyRoutineRegistered = TRUE;
}
