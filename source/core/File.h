#pragma once
#include "Types.h"

namespace Core
{

	class File
	{
	public:
		enum FileMode
		{
			NONE = 0,
			READ_FILE = 1,
			WRITE_FILE = 2,
			APPEND = 4,
			ATE = 8,
			TRUNC = 16,
			BINARY = 32

		};

		File(const char* filepath, FileMode mode = FileMode::READ_FILE);
		File() = default;
		~File();

		void Open(const char* filepath, FileMode mode);
		void Flush();

		uint32 GetSize() const { return m_FileSize; }
		const char* const GetBuffer() const { return m_Buffer; }
		void Write(const void* data, uint32 element_size, uint32 nof_elements);

	private:
		void GetFlags(char* const fileMode);

		void Resize(const uint32 element_size, const uint32 nof_elements);

		void OpenForWrite();
		void OpenForRead();
		const char* m_Filepath{ 0 };
		FileMode m_Mode = FileMode::NONE;
		char* m_Buffer = nullptr;
		uint32 m_FileSize = 0;
		uint32 m_AllocatedSize = 0;
	};

}; // namespace Core
