#include "Driver.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, KmdfExPsCreateProcessNotifyRoutineEx2)
#endif


#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
BOOLEAN KmdfExPsCreateProcessEx2NotifyRoutineRegistered = FALSE;
#else
BOOLEAN KmdfExPsCreateProcessNotifyRoutineRegistered = FALSE;
BOOLEAN KmdfExPsCreateProcessExNotifyRoutineRegistered = FALSE;
#endif
BOOLEAN KmdfExPsCreateThreadNotifyRoutineRegistered = FALSE;
#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
BOOLEAN KmdfExPsCreateThreadNotifyRoutineExRegistered = FALSE;
#endif
BOOLEAN KmdfExPsLoadImageNotifyRoutineRegistered = FALSE;

VOID
KmdfExPsCreateProcessNotifyRoutine(
    HANDLE ParentId,
    HANDLE ProcessId,
    BOOLEAN Create) 
{
    // 
    // This routine called after creation of first thread in
    // process!
    // 
    CHAR     buffer[LOG_BUFFER_SIZE];
    RtlStringCbPrintfA(buffer, sizeof(buffer), __FUNCTION__ ": PID 0x%p %s. "\
             "Parent PID 0x%p\n", 
             ProcessId, 
             Create ? " being created" : "exiting",
             ParentId);
    kmdfexLog(buffer);
    return;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
VOID
KmdfExPsCreateProcessNotifyRoutineEx(
    PEPROCESS Process,
    HANDLE ProcessId,
    PPS_CREATE_NOTIFY_INFO CreateInfo) 
{

    CHAR     buffer[LOG_BUFFER_SIZE];

    if (CreateInfo == NULL) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), __FUNCTION__ ": PID 0x%p exiting. "\
            "Process PID 0x%p\n",
            Process,
            ProcessId);
        kmdfexLog(buffer);
        return;
    }
    // 
    // This routine called after creation of first thread in
    // process!
    // 
    RtlStringCbPrintfA(buffer, sizeof(buffer), __FUNCTION__ ": Process 0x%p (PID 0x%p). "\
             "Parent PID 0x%p. Creating Thread 0x%p.%p\n",
             Process,
             ProcessId,
             CreateInfo->ParentProcessId,
             CreateInfo->CreatingThreadId.UniqueProcess,
             CreateInfo->CreatingThreadId.UniqueThread);
    kmdfexLog(buffer);
    if (CreateInfo->CommandLine != NULL) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Command Line %wZ\n",
                 ProcessId, 
                 CreateInfo->CommandLine);
        kmdfexLog(buffer);
    }
    RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Backing File Object 0x%p\n",
             ProcessId, 
             CreateInfo->FileObject);
    kmdfexLog(buffer);

    if (CreateInfo->FileOpenNameAvailable) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Full Image Name %wZ\n",
                 ProcessId, 
                 CreateInfo->ImageFileName);
        kmdfexLog(buffer);
    } else {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Partial Image Name %wZ\n",
                 ProcessId, 
                 CreateInfo->ImageFileName);
        kmdfexLog(buffer);
    }

    return;
}
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
VOID
KmdfExPsCreateProcessNotifyRoutineEx2(
    PEPROCESS Process,
    HANDLE ProcessId,
    PPS_CREATE_NOTIFY_INFO CreateInfo) 
{
    CHAR     buffer[LOG_BUFFER_SIZE];

    if (CreateInfo == NULL) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), __FUNCTION__ "Ex2: PID 0x%p exiting. "\
                 "Process PID 0x%p\n", 
                 Process, 
                 ProcessId);
        kmdfexLog(buffer);
        return;
    }
    // 
    // This routine called after creation of first thread in
    // process!
    // 
    RtlStringCbPrintfA(buffer, sizeof(buffer), __FUNCTION__ "Ex2: Process 0x%p (PID 0x%p). "\
             "Parent PID 0x%p. Creating Thread 0x%p.%p\n",
             Process,
             ProcessId,
             CreateInfo->ParentProcessId,
             CreateInfo->CreatingThreadId.UniqueProcess,
             CreateInfo->CreatingThreadId.UniqueThread);
    kmdfexLog(buffer);
    if (CreateInfo->CommandLine != NULL) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Command Line %wZ\n",
                 ProcessId, 
                 CreateInfo->CommandLine);
        kmdfexLog(buffer);
    }
    RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Backing File Object 0x%p\n",
             ProcessId, 
             CreateInfo->FileObject);
    kmdfexLog(buffer);

    if (CreateInfo->FileOpenNameAvailable) {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Full Image Name %wZ\n",
                 ProcessId, 
                 CreateInfo->ImageFileName);
        kmdfexLog(buffer);
    } else {
        RtlStringCbPrintfA(buffer, sizeof(buffer), "\t(0x%p) Partial Image Name %wZ\n",
                 ProcessId, 
                 CreateInfo->ImageFileName);
        kmdfexLog(buffer);
    }

    return;
}
#endif

VOID
KmdfExPsCreateThreadNotifyRoutine(
    HANDLE ProcessId,
    HANDLE ThreadId,
    BOOLEAN Create) 
{
    CHAR     buffer[LOG_BUFFER_SIZE];

    // 
    // When in this routine, target thread has been created and is
    // in the INITIALIZED state. It will not transition to READY
    // until this routine exits.
    // 

    RtlStringCbPrintfA(buffer, sizeof(buffer), "KmdfExPsCreateThreadNotifyRoutine: Thread 0x%p %s in "\
             "Process 0x%p\n", 
             ThreadId, 
             Create ? "being created" : "exiting",
             ProcessId);
    kmdfexLog(buffer);

    return;
}

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
VOID
KmdfExPsCreateThreadNotifyRoutineEx(
    HANDLE ProcessId,
    HANDLE ThreadId,
    BOOLEAN Create) 
{
    CHAR     buffer[LOG_BUFFER_SIZE];

    // 
    // When in this routine, target thread has been created and is
    // in the INITIALIZED state. It will not transition to READY
    // until this routine exits.
    // 

    RtlStringCbPrintfA(buffer, sizeof(buffer), "KmdfExPsCreateThreadNotifyRoutineEx: Thread 0x%p %s in "\
             "Process 0x%p\n", 
             ThreadId, 
             Create ? "being created" : "exiting",
             ProcessId);
    kmdfexLog(buffer);

    return;
}
#endif

VOID
KmdfExPsLoadImageNotifyRoutine(
    PUNICODE_STRING  FullImageName,
    HANDLE ProcessId,
    PIMAGE_INFO  ImageInfo) 
{
    CHAR     buffer[LOG_BUFFER_SIZE];

    PIMAGE_INFO_EX imageInfoEx = NULL;
    PFILE_OBJECT   backingFileObject;

    // 
    // IMAGE_INFO_EX available on Vista, which will give us the
    // backing file object of the image section.
    // 
    if (ImageInfo->ExtendedInfoPresent) {

        imageInfoEx = CONTAINING_RECORD(ImageInfo, IMAGE_INFO_EX, ImageInfo);

        backingFileObject = imageInfoEx->FileObject;

    } else {

        backingFileObject = NULL;

    }   


    RtlStringCbPrintfA(buffer, sizeof(buffer), "KmdfExPsLoadImageNotifyRoutine: %wZ being loaded into "\
             "Process 0x%p. Backing File Object %s (0x%p)\n", 
             FullImageName,
             ProcessId,
             backingFileObject != NULL ? "Available" : "Unavailable",
             backingFileObject);
    kmdfexLog(buffer);
    return;
}

