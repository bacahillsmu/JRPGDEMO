#include "Engine/Callstack/Callstack.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

// Callstack CallstackGet(unsigned int skipFrames_ /*= 0*/)
// {
// 
// }

// std::vector<std::string> CallstackToString(const Callstack& callstack)
// {
// 
// }

Callstack GetCallstack()
{
	Callstack callStack;
	
	callStack.frames = CaptureStackBackTrace(FRAMESTOSKIP, STACKFRAMES, callStack.addrs, &callStack.BackTraceHash);

	return callStack;
}

std::vector<std::string> CallstackToString(const Callstack& callstack)
{
	HANDLE handle = ::GetCurrentProcess();
	::SymInitialize(handle, nullptr, true);
	
	std::vector<std::string> callstackString;
	::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);

	for (USHORT i = 0; i < callstack.frames - 2; i++)
	{
		// Allocate a buffer large enough to hold the symbol information on the stack and get 
		// a pointer to the buffer.  We also have to set the size of the symbol structure itself
		// and the number of bytes reserved for the name.

		ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAXNAMELEN + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = { 0 };
		SYMBOL_INFO* info = (SYMBOL_INFO*)buffer;
		info->SizeOfStruct = sizeof(SYMBOL_INFO);
		info->MaxNameLen = MAXNAMELEN;

		IMAGEHLP_LINE64 lineinfo;
		lineinfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		// Attempt to get information about the symbol and add it to our output parameter.
		// Attempt to get line information about the symbol and not add it to our output parameter.
		DWORD64 Displacement = 0;
		DWORD dwDisplacement = 0;

		bool symFromAddrResult = ::SymFromAddr(handle, (DWORD64)callstack.addrs[i], &Displacement, info);
		bool symGetFromLineResult = ::SymGetLineFromAddr64(handle, (DWORD64)callstack.addrs[i], &dwDisplacement, &lineinfo);

		if (symFromAddrResult && symGetFromLineResult)
		{
			DebuggerPrintf("%s (%lu, %lu): %s\n", lineinfo.FileName, lineinfo.LineNumber, dwDisplacement, info->Name);
			callstackString.push_back(lineinfo.FileName);
		}
		else
		{
			DebuggerPrintf("Error Recieved:  %lu \n", GetLastError());

		}
	}

	::SymCleanup(::GetCurrentProcess());

	return callstackString;
}


