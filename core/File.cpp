#include "File.h"

#include <cstdio>
#include <cassert>

namespace Core
{
	File::File(const char* filepath)
	{
		if (FILE* file = fopen(filepath, "rb"))
		{
			fseek(file, 0, SEEK_END);
			m_FileSize= ftell(file);
			rewind(file);
			m_Buffer = new unsigned char[m_FileSize];

			if (fread(m_Buffer, 1, m_FileSize, file) != m_FileSize)
				assert(!"Failed to read file into stream!");

			fclose(file);
		}
		else
		{
			assert(!"Failed to open file. Missing?");
		}

	}

	File::~File()
	{
		delete m_Buffer;
		m_Buffer = nullptr;
	}

}; //namespace Core