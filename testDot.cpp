#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

using namespace std;
using std::hex;


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

uintptr_t FindAddrOffsets(HANDLE hProcess, uintptr_t baseAddres, vector<unsigned int>& offsets)
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

	uintptr_t module_base = GetModuleBaseAddress(procID, L"client.dll");
	cout << "0x" << hex << module_base << endl;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (hProcess == nullptr)
	{
		std::cout << "ERROR(-2): Failed open process" << std::endl;
		return -1;
	}

	uintptr_t client_dll_cam = module_base + 0x046471D0;

	vector <unsigned int> offsets_cam = { 0x648 };
	uintptr_t cam_address = FindAddrOffsets(hProcess, client_dll_cam, offsets_cam);

	float old_value = 0.0f;
	float new_value = 1500.0f;

	ReadProcessMemory(hProcess, (BYTE*)cam_address, &old_value, sizeof(float), nullptr);
	cout << old_value << endl;
	if (old_value < 1201.0f)
	{
		WriteProcessMemory(hProcess, (BYTE*)cam_address, &new_value, sizeof(float), nullptr);
	}

	uintptr_t client_dll_fog = module_base + 0x0462BDD0;
	vector <unsigned int> offsets_fog = { 0x40 };

	uintptr_t fog_address = FindAddrOffsets(hProcess, client_dll_fog, offsets_fog);

	int value = 0;
	WriteProcessMemory(hProcess, (BYTE*)fog_address, &value, sizeof(value), nullptr);


	return 0;
}