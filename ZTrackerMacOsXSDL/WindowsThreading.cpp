//
//  WindowsThreading.cpp
//  ZTrackerMacOsXSDL
//
//  Created by Robert on 4/28/15.
//  Copyright (c) 2015 Aardbei. All rights reserved.
//

#include "zt.h"
#include "WindowsThreading.h"

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms686277%28v=vs.85%29.aspx
//
// BOOL WINAPI SetThreadPriority(_In_  HANDLE hThread, _In_  int nPriority);

bool SetThreadPriority(HANDLE thread, int priority)
{
#warning TODO
    
    return true;
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683182%28v=vs.85%29.aspx
//
// HANDLE WINAPI GetCurrentThread(void);

HANDLE GetCurrentThread(void)
{
#warning TODO
    
    return 0;
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms687032%28v=vs.85%29.aspx
//
//DWORD WINAPI WaitForSingleObject(
//                                 _In_  HANDLE hHandle,
//                                 _In_  DWORD dwMilliseconds
//                                 );

DWORD WaitForSingleObject(HANDLE handle, DWORD milliseconds)
{
#warning TODO

    return 0;
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682453%28v=vs.85%29.aspx
//
// HANDLE WINAPI CreateThread(
//                           _In_opt_   LPSECURITY_ATTRIBUTES lpThreadAttributes,
//                           _In_       SIZE_T dwStackSize,
//                           _In_       LPTHREAD_START_ROUTINE lpStartAddress,
//                           _In_opt_   LPVOID lpParameter,
//                           _In_       DWORD dwCreationFlags,
//                           _Out_opt_  LPDWORD lpThreadId
//                           );

HANDLE CreateThread(DWORD attributes, DWORD size, void *startAddress, void *parameter, DWORD creationFLags, DWORD *threadId)
{
#warning TODO

    return 0;
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms686717%28v=vs.85%29.aspx
//
// BOOL WINAPI TerminateThread(
//                            _Inout_  HANDLE hThread,
//                            _In_     DWORD dwExitCode
//                            );

bool TerminateThread(HANDLE thread, DWORD exitCode)
{
#warning TODO
    
    return true;
}