#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

using namespace std;
using std::hex;


struct Camera
{
	const wchar_t module_name[19] = L"resourcesystem.dll";
	unsigned int main_offsets = 0x00064D80;
	vector <unsigned int> offsets = { 0x190, 0x10, 0x0, 0xA8, 0x70, 0x678 };
}camera;

struct Fog
{
	const wchar_t module_name[11] = L"tier0.dll";
	unsigned int main_offsets = 0x00383590;
	vector <unsigned int> offsets = { 0x310, 0x8, 0x10, 0x10, 0xB38 };
}fog;

struct R_Farz
{
	const wchar_t module_name[19] = L"panorama.dll";
	unsigned int main_offsets = 0x00539E40;
	vector <unsigned int> offsets = { 0x18, 0x18, 0x10, 0x10, 0x138 };
}r_farz;



uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modName)
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

	uintptr_t module_base = GetModuleBaseAddress(procID, camera.module_name);
	cout << "0x" << hex << module_base << endl;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (hProcess == nullptr)
	{
		std::cout << "ERROR(-2): Failed open process" << std::endl;
		return -2;
	}

	//camera values
	uintptr_t client_dll_cam = module_base + camera.main_offsets;

	uintptr_t cam_address = FindAddrOffsets(hProcess, client_dll_cam, camera.offsets);

	float old_value = 0.0f;
	float new_value = 1600.0f;

	ReadProcessMemory(hProcess, (BYTE*)cam_address, &old_value, sizeof(float), nullptr);
	cout << old_value << endl;
	if (old_value < 1201.0f)
	{
		WriteProcessMemory(hProcess, (BYTE*)cam_address, &new_value, sizeof(float), nullptr);
	}

	//fog values
	module_base = GetModuleBaseAddress(procID, fog.module_name);

	uintptr_t module_dll_fog = module_base + fog.main_offsets;

	uintptr_t fog_address = FindAddrOffsets(hProcess, module_dll_fog, fog.offsets);

	int value = 0;
	WriteProcessMemory(hProcess, (BYTE*)fog_address, &value, sizeof(int), nullptr);

	//r_farz values
	module_base = GetModuleBaseAddress(procID, r_farz.module_name);

	uintptr_t module_dll_r_farz = module_base + r_farz.main_offsets;

	uintptr_t r_farz_address = FindAddrOffsets(hProcess, module_dll_r_farz, r_farz.offsets);

	value = new_value * 3;

	WriteProcessMemory(hProcess, (BYTE*)r_farz_address, &value, sizeof(float), nullptr);


	CloseHandle(hProcess);
	return 0;
}