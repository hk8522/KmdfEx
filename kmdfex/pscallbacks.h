#pragma once

///////////////////// 
// PSCALLBACKS.CPP //
/////////////////////


#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
    extern BOOLEAN KmdfExPsCreateProcessEx2NotifyRoutineRegistered;

    VOID
    KmdfExPsCreateProcessNotifyRoutineEx2(
        _Inout_ PEPROCESS Process,
        _In_ HANDLE ProcessId,
        _In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
    );
#elif (NTDDI_VERSION < NTDDI_VISTA)
    extern BOOLEAN KmdfExPsCreateProcessNotifyRoutineRegistered;

    VOID
    KmdfExPsCreateProcessNotifyRoutine(
        _In_ HANDLE ParentId,
        _In_ HANDLE ProcessId,
        _In_ BOOLEAN Create
    );
#else
    extern BOOLEAN KmdfExPsCreateProcessExNotifyRoutineRegistered;

    VOID
    KmdfExPsCreateProcessNotifyRoutineEx(
        _Inout_ PEPROCESS Process,
        _In_ HANDLE ProcessId,
        _In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
    extern BOOLEAN KmdfExPsCreateThreadNotifyRoutineExRegistered;

    VOID
    KmdfExPsCreateThreadNotifyRoutineEx(
        _In_ HANDLE ProcessId,
        _In_ HANDLE ThreadId,
        _In_ BOOLEAN Create
    );
#else
    extern BOOLEAN KmdfExPsCreateThreadNotifyRoutineRegistered;

    VOID
        KmdfExPsCreateThreadNotifyRoutine(
            _In_ HANDLE ProcessId,
            _In_ HANDLE ThreadId,
            _In_ BOOLEAN Create
        );
#endif

extern BOOLEAN KmdfExPsLoadImageNotifyRoutineRegistered;

VOID
KmdfExPsLoadImageNotifyRoutine(
    _In_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
);


