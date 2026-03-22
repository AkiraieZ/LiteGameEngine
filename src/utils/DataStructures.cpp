#include "DataStructures.h"

namespace LGE {
namespace Utils {

template class DynamicArray<int>;
template class DynamicArray<float>;
template class DynamicArray<double>;
template class DynamicArray<size_t>;
template class DynamicArray<std::string>;

template class LinkedList<int>;
template class LinkedList<float>;
template class LinkedList<std::string>;

template class PriorityQueue<int>;
template class PriorityQueue<float>;
template class PriorityQueue<double>;

template class LRUCache<int, int>;
template class LRUCache<std::string, int>;
template class LRUCache<std::string, std::string>;

template class ThreadSafeQueue<int>;
template class ThreadSafeQueue<float>;
template class ThreadSafeQueue<std::string>;

template class ObjectPool<int>;
template class ObjectPool<float>;

template class MemoryPool<int>;
template class MemoryPool<float>;
template class MemoryPool<double>;

template class HashMap<int, int>;
template class HashMap<std::string, int>;
template class HashMap<std::string, std::string>;
template class HashMap<int, std::string>;

template class TrieNode<int>;
template class TrieNode<float>;
template class TrieNode<std::string>;

template class Trie<int>;
template class Trie<float>;
template class Trie<std::string>;

}
}
