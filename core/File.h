#pragma once
#include "Types.h"

namespace Core
{
	enum class FileMode
	{
		NONE,
		READ_FILE,
		WRITE_FILE
	};

	class File
	{
	public:
		File( const char* filepath, FileMode mode = FileMode::READ_FILE );
		~File();

		void Open( const char* filepath, FileMode mode );

		uint32 GetSize() const { return m_FileSize; }
		const char* const GetBuffer() const { return m_Buffer; }
		void Write( const void* data, uint32 element_size, uint32 nof_elements );

	private:
		void Resize( const uint32 element_size, const uint32 nof_elements );

		void OpenForWrite();
		void OpenForRead();
		const char* m_Filepath{ 0 };
		FileMode m_Mode = FileMode::NONE;
		char* m_Buffer = nullptr;
		uint32 m_FileSize = 0;
		uint32 m_AllocatedSize = 0;
	};

}; // namespace Core