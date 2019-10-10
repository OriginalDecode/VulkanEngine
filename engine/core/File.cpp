#include "File.h"

#include <cstdio>
#include <cassert>
#include <memory>

namespace Core
{
	File::File( const char* filepath, FileMode mode )
	{
		Open( filepath, mode );
	}

	File::~File()
	{
		if( m_Mode == FileMode::WRITE_FILE )
		{
			if( FILE* hFile = fopen( m_Filepath, "wb" ) )
			{
				fwrite( m_Buffer, 1, m_FileSize, hFile );
				fclose( hFile );
			}
		}

		delete m_Buffer;
		m_Buffer = nullptr;
	}

	void File::Open( const char* filepath, FileMode mode )
	{
		m_Mode = mode;
		m_Filepath = filepath;
		if( m_Mode == FileMode::READ_FILE )
			OpenForRead();
		else if( m_Mode == FileMode::WRITE_FILE )
			OpenForWrite();
		else
			assert( !"Failed to open file!" );
	}

	void File::Resize( const uint32 element_size, const uint32 nof_elements )
	{
		if( m_FileSize + ( element_size * nof_elements ) > m_AllocatedSize )
		{
			const uint32 newSize = static_cast<uint32>( static_cast<float>( m_FileSize + ( element_size * nof_elements ) ) * 1.5f );
			char* buffer = new char[newSize];
			memcpy( &buffer[0], &m_Buffer[0], m_FileSize );
			m_AllocatedSize = newSize;
			delete[] m_Buffer;
			m_Buffer = nullptr;
			m_Buffer = buffer;
		}
	}

	void File::Write( const void* data, uint32 element_size, uint32 nof_elements )
	{
		Resize( element_size, nof_elements );
		if( m_AllocatedSize > m_FileSize + ( element_size * nof_elements ) )
		{
			memcpy( &m_Buffer[m_FileSize], data, ( element_size * nof_elements ) );
			m_FileSize += element_size * nof_elements;
		}
		else
		{
			assert( !"Failed to resize allocated size to above fileSize + element_size * nof_elements!" );
		}
	}

	void File::OpenForWrite()
	{
		constexpr int allocSize = 1024;
		m_Buffer = new char[allocSize];
		m_AllocatedSize = allocSize;
	}

	void File::OpenForRead()
	{
		if( FILE* hFile = fopen( m_Filepath, "rb" ) )
		{
			fseek( hFile, 0, SEEK_END );
			m_FileSize = ftell( hFile );
			rewind( hFile );

			m_Buffer = new char[m_FileSize];
			memset( m_Buffer, 0, m_FileSize );

			fread( m_Buffer, 1, m_FileSize, hFile );
			fclose( hFile );
		}
	}
}; //namespace Core