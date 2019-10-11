#pragma once

#include <windows.h>
#include "StackWalker\StackWalker.h"

namespace Log
{
	class StackWalker : public BaseStackWalker
	{
	public:
		StackWalker();
		StackWalker( unsigned int aProcessId, HANDLE aProcess );
		virtual void OnOutput( char* aString );
	};

}; // namespace Log
