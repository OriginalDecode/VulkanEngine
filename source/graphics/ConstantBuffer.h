#pragma once
#include "core/Types.h"
#include "core/containers/Array.h"

class ConstantBuffer
{
public:
	ConstantBuffer() = default;
	~ConstantBuffer() = default;

	template <typename T>
	void RegisterVariable( T* variable )
	{
		m_Variables.Add( { variable, sizeof( T ) } );
		m_Size += sizeof( T );
	}

	void* GetBuffer() { return m_Buffer; }

private:
	struct Variable
	{
		Variable() = default;
		Variable( void* variable, int32 size )
			: m_Variable( variable )
			, m_Size( size )
		{
		}
		void* m_Variable = nullptr;
		int32 m_Size = 0;
	};
	uint32 m_Size{ 0 };
	void* m_Buffer{ nullptr };

	Core::Array<Variable> m_Variables;
};
