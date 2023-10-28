#include "rhypch.h"
#include "MemoryPool.h"

namespace Resources {

    MemoryPool::MemoryPool(size_t size)
        : m_Size(size) 
    {
        m_Buffer = static_cast<std::byte*>(std::malloc(m_Size));
        m_Offset = m_Buffer;
    }

    MemoryPool::~MemoryPool()
    {
        std::free(m_Buffer);
    }
}