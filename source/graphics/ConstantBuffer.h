// #pragma once
// #include <CommonLib/Types.hpp>
// #include <CommonLib/containers/GrowingArray.h>

// struct ID3D11Buffer;
// class ConstantBuffer
// {
// public:
// 	ConstantBuffer() = default;
// 	~ConstantBuffer();

// 	template<typename T>
// 	void RegisterVariable(T* variable);

// 	void Bind(const uint32 index, const uint32 shader_binding);

// 	void Bind(const uint32 index[], const uint32 shader_binding);


// private:
// 	struct BufferVariable
// 	{
// 		BufferVariable() = default;
// 		BufferVariable(void* variable, int32 size) : m_Variable(variable), m_Size(size) { }
// 		void* m_Variable = nullptr;
// 		int32 m_Size = 0;
// 	};
// 	uint32 m_BufferSize = 0;
// 	ID3D11Buffer* m_Buffer = nullptr;

// 	CU::GrowingArray<BufferVariable> m_Variables;
// };

// template<typename T>
// void ConstantBuffer::RegisterVariable(T* variable)
// {
// 	m_Variables.Add({ variable, sizeof(T) });
// 	m_BufferSize += sizeof(T);
// }

