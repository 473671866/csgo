#pragma once
#include <iostream>
#include <string>
#include <windows.h>
#include <TlHelp32.h>

#ifdef _AMD64_
typedef unsigned __int64 uint_ptr;
#else
typedef unsigned __int32 uint_ptr;
#endif

class Helper
{
public:
	Helper(HANDLE hprocess, uint32_t m_pid);
	~Helper();
	static Helper* Make();
	boolean ReadMemory(uint_ptr address, void* buffer, size_t size);
	boolean WriteMemory(uint_ptr address, void* buffer, size_t size);
	uint_ptr GetModuleAddress(std::string module_name);
private:
	HANDLE m_hprocess;
	uint32_t m_pid;
};
