#pragma once


#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr;
#define SAFE_DELETEA(array) delete[] array; array = nullptr;

#define BIT(x) (1 << x)

