/**********************************************************************
 *
 * StackWalker.h
 *
 *
 *
 * LICENSE (http://www.opensource.org/licenses/bsd-license.php)
 *
 *   Copyright (c) 2005-2009, Jochen Kalmbach
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without modification,
 *   are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *   Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *   Neither the name of Jochen Kalmbach nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * **********************************************************************/
// #pragma once is supported starting with _MCS_VER 1000, 
// so we need not to check the version (because we only support _MSC_VER >= 1100)!
#pragma once

#define NOMINMAX
#include <windows.h>
#include <DbgHelp.h>
#undef NOMINMAX

// special defines for VC5/6 (if no actual PSDK is installed):
#if _MSC_VER < 1300
typedef unsigned __int64 DWORD64, *PDWORD64;
#if defined(_WIN64)
typedef unsigned __int64 SIZE_T, *PSIZE_T;
#else
typedef unsigned long SIZE_T, *PSIZE_T;
#endif
#endif  // _MSC_VER < 1300

class StackWalkerInternal;  // forward
class StackWalker
{
public:
    //-----------------------------------//

    static const size_t MaxSymbolNameLength = 1024;

    //-----------------------------------//

    struct Symbol
    {
        typedef std::basic_string<char, std::char_traits<char>, Mallocator<char>> String;

        IMAGEHLP_SYMBOL64 sym;
        IMAGEHLP_LINE64 line;

        String name;
    };

    //-----------------------------------//

    struct CallstackEntry
    {
        DWORD64 offset;  // if 0, we have no valid entry
        CHAR name[MaxSymbolNameLength];
        CHAR undName[MaxSymbolNameLength];
        CHAR undFullName[MaxSymbolNameLength];
        DWORD64 offsetFromSmybol;
        DWORD offsetFromLine;
        DWORD lineNumber;
        CHAR lineFileName[MaxSymbolNameLength];
        DWORD symType;
        LPCSTR symTypeString;
        CHAR moduleName[MaxSymbolNameLength];
        DWORD64 baseOfImage;
        CHAR loadedImageName[MaxSymbolNameLength];
    };

    //-----------------------------------//

    enum CallstackEntryType
    {
        FirstEntry
        , NextEntry
        , LastEntry
    };

    //-----------------------------------//

    enum StackWalkOptions
    {
        // No addition info will be retrived 
        // (only the address is available)
        RetrieveNone = 0,

        // Try to get the symbol-name
        RetrieveSymbol = 1,

        // Try to get the line for this symbol
        RetrieveLine = 2,

        // Try to retrieve the module-infos
        RetrieveModuleInfo = 4,

        // Also retrieve the version for the DLL/EXE
        RetrieveFileVersion = 8,

        // Contains all the abouve
        RetrieveVerbose = 0xF,

        // Generate a "good" symbol-search-path
        SymBuildPath = 0x10,

        // Also use the public Microsoft-Symbol-Server
        SymUseSymSrv = 0x20,

        // Contains all the abouve "Sym"-options
        SymAll = 0x30,

        // Contains all options (default)
        OptionsAll = 0x3F
    };

public:
    //-----------------------------------//

    StackWalker(int options = OptionsAll, LPCSTR szSymPath = NULL, DWORD dwProcessId = GetCurrentProcessId(), HANDLE hProcess = GetCurrentProcess());
    StackWalker(DWORD dwProcessId, HANDLE hProcess);

    virtual ~StackWalker();

    //-----------------------------------//

    typedef BOOL(__stdcall *PReadProcessMemoryRoutine)(
        HANDLE      hProcess
        , DWORD64     qwBaseAddress
        , PVOID       lpBuffer
        , DWORD       nSize
        , LPDWORD     lpNumberOfBytesRead
        , LPVOID      pUserData);

    //-----------------------------------//

    BOOL LoadModules();

    //-----------------------------------//

    Symbol GetSymbolFromAddr(CONTEXT* ctx, void * p);

    //-----------------------------------//

    BOOL ShowCallstack(HANDLE hThread = GetCurrentThread()
        , const CONTEXT *context = NULL
        , PReadProcessMemoryRoutine readMemoryFunction = NULL
        , LPVOID pUserData = NULL);

    //-----------------------------------//

public:

    virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName);
    virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion);
    virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);
    virtual void OnOutput(LPCSTR szText);

    StackWalkerInternal *m_sw;
    HANDLE m_hProcess;
    DWORD m_dwProcessId;
    BOOL m_modulesLoaded;
    LPSTR m_szSymPath;

    int m_options;
    int m_MaxRecursionCount;

    static BOOL __stdcall myReadProcMem(HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);
};  // class StackWalker


// The "ugly" assembler-implementation is needed for systems before XP
// If you have a new PSDK and you only compile for XP and later, then you can use 
// the "RtlCaptureContext"
// Currently there is no define which determines the PSDK-Version... 
// So we just use the compiler-version (and assumes that the PSDK is 
// the one which was installed by the VS-IDE)

// INFO: If you want, you can use the RtlCaptureContext if you only target XP and later...
//       But I currently use it in x64/IA64 environments...
//#if defined(_M_IX86) && (_WIN32_WINNT <= 0x0500) && (_MSC_VER < 1400)

#if defined(_M_IX86)
#ifdef CURRENT_THREAD_VIA_EXCEPTION
// TODO: The following is not a "good" implementation, 
// because the callstack is only valid in the "__except" block...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    EXCEPTION_POINTERS *pExp = NULL; \
    __try { \
      throw 0; \
    } __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
    if (pExp != NULL) \
      memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT)); \
      c.ContextFlags = contextFlags; \
    } while(0);
#else
// The following should be enough for walking the callstack...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    __asm    call x \
    __asm x: pop eax \
    __asm    mov c.Eip, eax \
    __asm    mov c.Ebp, ebp \
    __asm    mov c.Esp, esp \
    } while(0);
#endif

#else

// The following is defined for x86 (XP and higher), x64 and IA64:
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    RtlCaptureContext(&c); \
} while(0);
#endif
