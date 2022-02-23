#include "Driver.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, kmdfexLog)
#endif

#define MAX_FILE_SIZE (0x1024 * 32)

NTSTATUS
kmdfexLog(
    _In_ LPCSTR fmt
)
{
    ////////
    // Refer to a file by its object name
    UNICODE_STRING     uniName;
    OBJECT_ATTRIBUTES  objAttr;

    RtlInitUnicodeString(&uniName, L"\\DosDevices\\c:\\kmdfex.log");// L"\\SystemRoot\\events.log");  // or L"\\DosDevices\\C:\\WINDOWS\\example.txt"
    InitializeObjectAttributes(&objAttr, &uniName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);


    ////////
    // Optain a file handle
    HANDLE   handle;
    NTSTATUS ntstatus;
    IO_STATUS_BLOCK    ioStatusBlock;

    // Do not try to perform any file operations at higher IRQL levels.
    // Instead, you may use a work item or a system worker thread to perform file operations.

    if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        return STATUS_INVALID_DEVICE_STATE;

    ntstatus = ZwCreateFile(&handle,
        FILE_APPEND_DATA,
        //GENERIC_WRITE | SYNCHRONIZE,
        &objAttr, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0);

    FILE_STANDARD_INFORMATION fileInfo = { 0 };
    ntstatus = ZwQueryInformationFile(
        handle,
        &ioStatusBlock,
        &fileInfo,
        sizeof(fileInfo),
        FileStandardInformation
    );
    if (NT_SUCCESS(ntstatus)) {
        if (fileInfo.EndOfFile.LowPart > MAX_FILE_SIZE) {
            ZwClose(handle);

            ntstatus = ZwCreateFile(&handle,
                FILE_APPEND_DATA,
                //GENERIC_WRITE | SYNCHRONIZE,
                &objAttr, &ioStatusBlock, NULL,
                FILE_ATTRIBUTE_NORMAL,
                0,
                FILE_OVERWRITE_IF,
                FILE_SYNCHRONOUS_IO_NONALERT,
                NULL, 0);
        }
    }

    ////////
    // Read from or write to a file

    ////////
    // The following code example illustrates how to write to a file.
    CHAR     buffer[LOG_BUFFER_SIZE];
    size_t  cb = 0;

    if (NT_SUCCESS(ntstatus)) {
        ntstatus = RtlStringCbPrintfA(buffer, sizeof(buffer), "%s\r\n", fmt);
        if (NT_SUCCESS(ntstatus)) {
            ntstatus = RtlStringCbLengthA(buffer, sizeof(buffer), &cb);
            if (NT_SUCCESS(ntstatus)) {
                ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock,
                    buffer, (ULONG)cb, NULL, NULL);
            }
        }
        ZwClose(handle);
    }

    ////////
    // The following code example illustrates how to read from a file.

    //LARGE_INTEGER      byteOffset;

    //ntstatus = ZwCreateFile(&handle,
    //    GENERIC_READ,
    //    &objAttr, &ioStatusBlock,
    //    NULL,
    //    FILE_ATTRIBUTE_NORMAL,
    //    0,
    //    FILE_OPEN,
    //    FILE_SYNCHRONOUS_IO_NONALERT,
    //    NULL, 0);
    //if (NT_SUCCESS(ntstatus)) {
    //    byteOffset.LowPart = byteOffset.HighPart = 0;
    //    ntstatus = ZwReadFile(handle, NULL, NULL, NULL, &ioStatusBlock,
    //        buffer, BUFFER_SIZE, &byteOffset, NULL);
    //    if (NT_SUCCESS(ntstatus)) {
    //        buffer[BUFFER_SIZE - 1] = '\0';
    //        DbgPrint("%s\n", buffer);
    //    }
    //    ZwClose(handle);
    //}

    return STATUS_SUCCESS;
}
