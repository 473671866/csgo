#include "helper.h"

Helper::Helper(HANDLE hprocess, uint32_t pid) :m_hprocess(hprocess), m_pid(pid)
{
	;
}

Helper::~Helper()
{
	CloseHandle(m_hprocess);
}

Helper* Helper::Make()
{
	static Helper* helper = nullptr;
	if (helper != nullptr) {
		return helper;
	}

	uint32_t pid = 0;
	HANDLE hprocess = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 process_entry = { .dwSize = sizeof(PROCESSENTRY32) };

	HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot_handle == INVALID_HANDLE_VALUE || snapshot_handle == nullptr) {
		return nullptr;
	}

	boolean success = Process32First(snapshot_handle, &process_entry);
	while (success) {
		if (_stricmp(process_entry.szExeFile, "csgo.exe") == 0) {
			hprocess = OpenProcess(PROCESS_ALL_ACCESS, false, process_entry.th32ProcessID);
			pid = process_entry.th32ProcessID;
			break;
		}
		success = Process32Next(snapshot_handle, &process_entry);
	}
	CloseHandle(snapshot_handle);

	if (hprocess != INVALID_HANDLE_VALUE && hprocess != nullptr) {
		helper = new Helper(hprocess, pid);
		return helper;
	}

	return nullptr;
}

boolean Helper::ReadMemory(uint_ptr address, void* buffer, size_t size)
{
	return ReadProcessMemory(m_hprocess, reinterpret_cast<void*>(address), buffer, size, nullptr);
}

boolean Helper::WriteMemory(uint_ptr address, void* buffer, size_t size)
{
	return WriteProcessMemory(m_hprocess, reinterpret_cast<void*>(address), buffer, size, nullptr);
}

uint_ptr Helper::GetModuleAddress(std::string module_name)
{
	HANDLE handle = nullptr;
	HMODULE hmodule = nullptr;
	handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pid);  //所有进程的句柄
	MODULEENTRY32 module_entry{ .dwSize = sizeof(MODULEENTRY32) };

	if (handle == nullptr || handle == INVALID_HANDLE_VALUE) {
		return 0;
	}

	boolean success = Module32First(handle, &module_entry);
	if (success) {
		do {
			if (_stricmp(module_entry.szModule, module_name.c_str()) == 0) {
				hmodule = module_entry.hModule;
				break;
			}
		} while (Module32Next(handle, &module_entry));
	}

	CloseHandle(handle);

	return reinterpret_cast<uint_ptr>(hmodule);
}