/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "ProfilerInstrumentation.h"
#include "Profiler.h"

extern "C" void __declspec(naked) _cdecl _penter(void)
{
    using namespace dit;

#ifdef DIT_USE_INSTRUMENTATION
    Profiler* prfl;

    _asm {
        // make space for local variables
        push     ebp
        mov      ebp, esp
        sub      esp, __LOCAL_SIZE
        
        // save the world...
        push eax
        push ebx
        push ecx
        push edx
        push ebp
        push edi
        push esi
    }

    prfl = profiler();
    if (prfl != nullptr)
        prfl->Enter(__rdtsc(), _ReturnAddress());

    _asm {
        // recover the world...
        pop esi
        pop edi
        pop ebp
        pop edx
        pop ecx
        pop ebx
        pop eax

        // give back stack space
        mov      esp, ebp
        pop      ebp
        ret
    }
#endif
}
extern "C" void __declspec(naked) _cdecl _pexit(void)
{
    using namespace dit;

#ifdef DIT_USE_INSTRUMENTATION
    Profiler* prfl;

    _asm {
        // make space for local variables
        push     ebp
        mov      ebp, esp
        sub      esp, __LOCAL_SIZE

        // save the world...
        push eax
        push ebx
        push ecx
        push edx
        push ebp
        push edi
        push esi
    }

    prfl = profiler();
    if (prfl != nullptr)
        prfl->Exit(__rdtsc(), _ReturnAddress());

    _asm {
        // recover the world...
        pop esi
        pop edi
        pop ebp
        pop edx
        pop ecx
        pop ebx
        pop eax

        // give back stack space
        mov      esp, ebp
        pop      ebp
        ret
    }
#endif
}