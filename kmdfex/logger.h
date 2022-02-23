/*++

Module Name:

    logger.h

Abstract:

    This file contains the log function definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include <ntstrsafe.h>

EXTERN_C_START

//
// Function to log a message
//
NTSTATUS
kmdfexLog(
    _In_ LPCSTR fmt
);

#define  LOG_BUFFER_SIZE 3000

EXTERN_C_END
