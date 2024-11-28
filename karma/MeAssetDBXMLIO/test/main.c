#include <crtdbg.h>
#include <stdio.h>
#include <MeMath.h>

extern void DoTest(char *file);


int main(int argc, char **argv)
{

#if defined WIN32 && defined _DEBUG && 0
    {
        int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

        debugFlag |= _CRTDBG_ALLOC_MEM_DF;
        debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
        //debugFlag |= _CRTDBG_CHECK_CRT_DF;
        debugFlag |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag(debugFlag);
    }
#endif    

    DoTest("test_format_1_0.ka");

    return 0;
}

