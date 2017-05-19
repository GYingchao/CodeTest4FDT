#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

//  Memory statistics class, unit is MB
class memoryInWin
{
    private:
        MEMORYSTATUSEX memInfo;
    public:
        void init();
        DWORDLONG getTotalVM();
        DWORDLONG getCurrentUsedVM();
        size_t getCurrentUsedVMbyMe();
        DWORDLONG getTotalPhM();
        DWORDLONG getCurrentUsedPhM();
        size_t getCurrentUsedPhMbyMe();
};

void memoryInWin::init()
{
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return;
}

DWORDLONG memoryInWin::getTotalVM()
{
    return (double)memInfo.ullTotalPageFile / (1024*1024);
}

DWORDLONG memoryInWin::getCurrentUsedVM()
{
    return (double)(memInfo.ullTotalPageFile - memInfo.ullAvailPageFile) / (1024*1024);
}

size_t memoryInWin::getCurrentUsedVMbyMe()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return (double)pmc.PrivateUsage / (1024*1024);
}

DWORDLONG memoryInWin::getTotalPhM()
{
    return (double)memInfo.ullTotalPhys / (1024*1024);
}

DWORDLONG memoryInWin::getCurrentUsedPhM()
{
    return (double)(memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024*1024);
}

size_t memoryInWin::getCurrentUsedPhMbyMe()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return (double)pmc.WorkingSetSize / (1024*1024);
}
