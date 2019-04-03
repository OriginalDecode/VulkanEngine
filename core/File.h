#pragma once


namespace Core
{
	class File
	{
	public:
		File(const char* filepath);
		~File();

		const unsigned char* GetBuffer() const { return m_Buffer; }
		unsigned int GetFileSize() const { return m_FileSize; }

	private:
		unsigned char* m_Buffer = nullptr;
		unsigned int m_FileSize = 0;


	};

}; // namespace Core