#include "StackWalker.h"
#include "Debug.h"

namespace Log
{
	StackWalker::StackWalker()
		: BaseStackWalker()
	{
	}

	StackWalker::StackWalker(unsigned int aProcessId, HANDLE aProcess)
		: BaseStackWalker(aProcessId, aProcess)
	{
	}
	void StackWalker::OnOutput(char* aString) { LOG("%s", aString); }
} // namespace Log
