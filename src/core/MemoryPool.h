#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

namespace LGE {

template<typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t blockSize = 1024);
    ~MemoryPool();
    
    T* Allocate();
    void Deallocate(T* ptr);
    void Clear();
    
    size_t GetUsedCount() const { return m_UsedCount; }
    size_t GetTotalCount() const { return m_Blocks.size() * m_BlockSize; }
    
private:
    void AllocateNewBlock();
    
    size_t m_BlockSize;
    std::vector<uint8_t*> m_Blocks;
    std::vector<T*> m_FreeList;
    size_t m_UsedCount;
};

template<typename T>
MemoryPool<T>::MemoryPool(size_t blockSize)
    : m_BlockSize(blockSize), m_UsedCount(0) {
    AllocateNewBlock();
}

template<typename T>
MemoryPool<T>::~MemoryPool() {
    Clear();
    for (auto block : m_Blocks) {
        delete[] block;
    }
}

template<typename T>
void MemoryPool<T>::AllocateNewBlock() {
    uint8_t* block = new uint8_t[m_BlockSize * sizeof(T)];
    m_Blocks.push_back(block);
    
    for (size_t i = 0; i < m_BlockSize; ++i) {
        m_FreeList.push_back(reinterpret_cast<T*>(block + i * sizeof(T)));
    }
}

template<typename T>
T* MemoryPool<T>::Allocate() {
    if (m_FreeList.empty()) {
        AllocateNewBlock();
    }
    
    T* ptr = m_FreeList.back();
    m_FreeList.pop_back();
    ++m_UsedCount;
    
    new (ptr) T();
    return ptr;
}

template<typename T>
void MemoryPool<T>::Deallocate(T* ptr) {
    if (!ptr) return;
    
    ptr->~T();
    m_FreeList.push_back(ptr);
    --m_UsedCount;
}

template<typename T>
void MemoryPool<T>::Clear() {
    m_FreeList.clear();
    m_UsedCount = 0;
    
    for (auto block : m_Blocks) {
        for (size_t i = 0; i < m_BlockSize; ++i) {
            T* ptr = reinterpret_cast<T*>(block + i * sizeof(T));
            m_FreeList.push_back(ptr);
        }
    }
}

}
