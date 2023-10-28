#pragma once
#include "rhypch.h"

namespace Resources {

    class MemoryPool {

    public:
        MemoryPool(size_t size);
        ~MemoryPool();
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool operator=(const MemoryPool&) = delete;

        template <typename T>
        T* Allocate() {
            if (m_Offset + sizeof(T) <= m_Buffer + m_Size) {
                void* offset = m_Offset;
                m_Offset = static_cast<std::byte*>(m_Offset) + sizeof(T);
                return static_cast<T*>(offset);
            }
            return nullptr;
        }
    private:

        size_t m_Size;
        std::byte* m_Buffer;
        std::byte* m_Offset;
    };

}
