#ifndef memory_h
#define memory_h

#include <Windows.h>
#include <vector>
#include <TlHelp32.h>

uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modName,SIZE_T &modSize)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(reinterpret_cast<const wchar_t*>(modEntry.szModule), modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    modSize = modEntry.modBaseSize;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

uintptr_t FindAddrOffsets(HANDLE hProcess, uintptr_t& baseAddres, std::vector<unsigned int>& offsets)
{
    uintptr_t address = baseAddres;
    for (unsigned int offset : offsets)
    {
        ReadProcessMemory(hProcess, (BYTE*)address, &address, sizeof(address), nullptr);
        address += offset;
    }
    return address;
}

uintptr_t FindSignature(HANDLE hProcess, uintptr_t baseAddress, SIZE_T moduleSize, BYTE* sigBytes, char* sigMask, SIZE_T sigSize)
{
    BYTE* buffer = new BYTE[moduleSize];
    SIZE_T bytesRead;

    if (!ReadProcessMemory(hProcess, (void*)baseAddress, buffer, moduleSize, &bytesRead))
    {
        delete[] buffer;
        return 0;
    }

    for (SIZE_T i = 0; i < bytesRead - sigSize; ++i)
    {
        bool found = true;
        for (SIZE_T j = 0; j < sigSize; ++j)
        {
            if (sigMask[j] == 'x' && buffer[i + j] != sigBytes[j])
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            delete[] buffer;
            return baseAddress + i;
        }
    }

    delete[] buffer;
    return 0;
}

#endif // !memory_h

