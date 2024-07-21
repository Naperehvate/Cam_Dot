#include <iostream>
#include <Windows.h>
#include "offsets.h"
#include "memory.h"

using namespace std;

int main()
{
    DWORD procID = 0;
    HWND hwnd = FindWindowA(nullptr, "Dota 2");
    GetWindowThreadProcessId(hwnd, &procID);

    cout << procID << endl;
    if (procID == NULL)
    {
        cout << "ERROR(-1): GAME NOT RUNNING" << endl;
        return -1;
    }

    Camera camera;
    Fog fog;
    R_Farz r_farz;
    DotPlus dotPlus;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (hProcess == nullptr)
    {
        std::cout << "ERROR(-2): Failed open process" << std::endl;
        return -2;
    }

    //dotPlus values
    uintptr_t module_base = GetModuleBaseAddress(procID, dotPlus.module_name, dotPlus.sigSize);
    while (!module_base)
    {
        Sleep(1000); cout << "Waiting for Module to load..." << endl;
        module_base = GetModuleBaseAddress(procID, dotPlus.module_name, dotPlus.sigSize);
    }

    uintptr_t dotPlus_address = FindSignature(hProcess, module_base, dotPlus.sigSize, dotPlus.oldSig, dotPlus.mask, sizeof(dotPlus.oldSig));
    if (dotPlus_address)
    {
        BYTE patch[41];
        SIZE_T bytesRead;

        if (ReadProcessMemory(hProcess, (LPVOID)dotPlus_address, patch, sizeof(dotPlus.oldSig), &bytesRead))
        {
            // Замена байтов
            for (int i = 0; i < sizeof(dotPlus.oldSig); ++i)
            {
                patch[1] = 0x01;
                patch[2] = 0x00;
				patch[3] = 0x00;
                patch[4] = 0x00;

                patch[13] = 0x01;
				patch[14] = 0x00;
				patch[15] = 0x00;
				patch[16] = 0x00;

                patch[40] = 0x01;
            }

            if (WriteProcessMemory(hProcess, (LPVOID)dotPlus_address, patch, sizeof(patch), nullptr))
            {
                cout << "Signature DotPlus replaced successfully at address: 0x" << hex << dotPlus_address << endl;
            }
            else
            {
                cout << "ERROR(-5): Failed to write new signature DotPlus" << endl;
            }
        }
        else
        {
            cout << "ERROR(-6): Failed to read memory for patching" << endl;
        }
    }
    else
    {
        cout << "Signature DotPlus not found." << endl;
    }


    

    //camera values
    module_base = GetModuleBaseAddress(procID, camera.module_name, camera.sigSize);
    uintptr_t client_dll_cam = module_base + camera.main_offsets;
    uintptr_t cam_address = FindAddrOffsets(hProcess, client_dll_cam, camera.offsets);

    ReadProcessMemory(hProcess, (BYTE*)cam_address, &camera.old_value, sizeof(float), nullptr);
    cout << camera.old_value << endl;
    if (camera.old_value < 1201.0f)
    {
        WriteProcessMemory(hProcess, (BYTE*)cam_address, &camera.new_value, sizeof(float), nullptr);
    }

    //fog values
    module_base = GetModuleBaseAddress(procID, fog.module_name, fog.sigSize);
    uintptr_t module_dll_fog = module_base + fog.main_offsets;
    uintptr_t fog_address = FindAddrOffsets(hProcess, module_dll_fog, fog.offsets);

    /*WriteProcessMemory(hProcess, (BYTE*)fog_address, &fog.value, sizeof(int), nullptr);*/


    /*r_farz values*/
    module_base = GetModuleBaseAddress(procID, r_farz.module_name, r_farz.sigSize);
    uintptr_t module_dll_r_farz = module_base + r_farz.main_offsets;
    uintptr_t r_farz_address = FindAddrOffsets(hProcess, module_dll_r_farz, r_farz.offsets);

    /*WriteProcessMemory(hProcess, (BYTE*)r_farz_address, &r_farz.value, sizeof(float), nullptr);*/



    CloseHandle(hProcess);
    return 0;
}