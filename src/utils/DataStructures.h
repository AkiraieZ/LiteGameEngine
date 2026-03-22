#pragma once

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <functional>
#include <algorithm>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <stdexcept>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace LGE {
namespace Utils {

template<typename T>
class DynamicArray {
public:
    DynamicArray() : data_(nullptr), size_(0), capacity_(0) {}
    
    explicit DynamicArray(size_t capacity) : size_(0), capacity_(capacity) {
        if (capacity > 0) {
            data_ = new T[capacity];
        } else {
            data_ = nullptr;
        }
    }

    DynamicArray(const DynamicArray& other) : size_(other.size_), capacity_(other.capacity_) {
        if (capacity_ > 0) {
            data_ = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        } else {
            data_ = nullptr;
        }
    }

    DynamicArray(DynamicArray&& other) noexcept 
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~DynamicArray() {
        delete[] data_;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            if (capacity_ > 0) {
                data_ = new T[capacity_];
                for (size_t i = 0; i < size_; ++i) {
                    data_[i] = other.data_[i];
                }
            } else {
                data_ = nullptr;
            }
        }
        return *this;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 8 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void push_back(T&& value) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 8 : capacity_ * 2);
        }
        data_[size_++] = std::move(value);
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 8 : capacity_ * 2);
        }
        new (&data_[size_]) T(std::forward<Args>(args)...);
        ++size_;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }

    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    void reserve(size_t newCapacity) {
        if (newCapacity <= capacity_) return;
        
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < size_; ++i) {
            newData[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
    }

    void resize(size_t newSize) {
        if (newSize < size_) {
            for (size_t i = newSize; i < size_; ++i) {
                data_[i].~T();
            }
        } else if (newSize > size_) {
            if (newSize > capacity_) {
                reserve(newSize);
            }
            for (size_t i = size_; i < newSize; ++i) {
                new (&data_[i]) T();
            }
        }
        size_ = newSize;
    }

    T& operator[](size_t index) {
        assert(index < size_);
        return data_[index];
    }

    const T& operator[](size_t index) const {
        assert(index < size_);
        return data_[index];
    }

    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("DynamicArray index out of range");
        }
        return data_[index];
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("DynamicArray index out of range");
        }
        return data_[index];
    }

    T& front() {
        assert(size_ > 0);
        return data_[0];
    }

    const T& front() const {
        assert(size_ > 0);
        return data_[0];
    }

    T& back() {
        assert(size_ > 0);
        return data_[size_ - 1];
    }

    const T& back() const {
        assert(size_ > 0);
        return data_[size_ - 1];
    }

    T* data() { return data_; }
    const T* data() const { return data_; }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

    void insert(size_t index, const T& value) {
        assert(index <= size_);
        if (size_ >= capacity_) {
            reserve(capacity_ == 0 ? 8 : capacity_ * 2);
        }
        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = value;
        ++size_;
    }

    void erase(size_t index) {
        assert(index < size_);
        for (size_t i = index; i < size_ - 1; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;
        data_[size_].~T();
    }

    void erase(size_t first, size_t last) {
        assert(first <= last && last <= size_);
        size_t count = last - first;
        for (size_t i = first; i < size_ - count; ++i) {
            data_[i] = std::move(data_[i + count]);
        }
        for (size_t i = size_ - count; i < size_; ++i) {
            data_[i].~T();
        }
        size_ -= count;
    }

    void swap(DynamicArray& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        
        Node(const T& d) : data(d), prev(nullptr), next(nullptr) {}
        Node(T&& d) : data(std::move(d)), prev(nullptr), next(nullptr) {}
        template<typename... Args>
        Node(Args&&... args) : data(std::forward<Args>(args)...), prev(nullptr), next(nullptr) {}
    };

public:
    LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

    LinkedList(const LinkedList& other) : LinkedList() {
        for (const T& data : other) {
            push_back(data);
        }
    }

    LinkedList(LinkedList&& other) noexcept 
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    ~LinkedList() {
        clear();
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            for (const T& data : other) {
                push_back(data);
            }
        }
        return *this;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    void push_front(const T& data) {
        Node* newNode = new Node(data);
        if (head_) {
            newNode->next = head_;
            head_->prev = newNode;
            head_ = newNode;
        } else {
            head_ = tail_ = newNode;
        }
        ++size_;
    }

    void push_front(T&& data) {
        Node* newNode = new Node(std::move(data));
        if (head_) {
            newNode->next = head_;
            head_->prev = newNode;
            head_ = newNode;
        } else {
            head_ = tail_ = newNode;
        }
        ++size_;
    }

    void push_back(const T& data) {
        Node* newNode = new Node(data);
        if (tail_) {
            newNode->prev = tail_;
            tail_->next = newNode;
            tail_ = newNode;
        } else {
            head_ = tail_ = newNode;
        }
        ++size_;
    }

    void push_back(T&& data) {
        Node* newNode = new Node(std::move(data));
        if (tail_) {
            newNode->prev = tail_;
            tail_->next = newNode;
            tail_ = newNode;
        } else {
            head_ = tail_ = newNode;
        }
        ++size_;
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        Node* newNode = new Node(std::forward<Args>(args)...);
        if (head_) {
            newNode->next = head_;
            head_->prev = newNode;
            head_ = newNode;
        } else {
            head_ = tail_ = newNode;
        }
        ++size_;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        Node* newNode = new Node(std::forward<Args>(args)...);
        if (tail_) {
            newNode->prev = tail_;
            tail_->next = newNode;
            tail_ = newNode;
        } else {
            head_ = tail_ = newNode;
        }
        ++size_;
    }

    void pop_front() {
        if (!head_) return;
        Node* oldHead = head_;
        head_ = head_->next;
        if (head_) {
            head_->prev = nullptr;
        } else {
            tail_ = nullptr;
        }
        delete oldHead;
        --size_;
    }

    void pop_back() {
        if (!tail_) return;
        Node* oldTail = tail_;
        tail_ = tail_->prev;
        if (tail_) {
            tail_->next = nullptr;
        } else {
            head_ = nullptr;
        }
        delete oldTail;
        --size_;
    }

    T& front() {
        assert(head_);
        return head_->data;
    }

    const T& front() const {
        assert(head_);
        return head_->data;
    }

    T& back() {
        assert(tail_);
        return tail_->data;
    }

    const T& back() const {
        assert(tail_);
        return tail_->data;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    void clear() {
        while (head_) {
            Node* next = head_->next;
            delete head_;
            head_ = next;
        }
        tail_ = nullptr;
        size_ = 0;
    }

    class Iterator {
    public:
        Iterator() : node_(nullptr) {}
        Iterator(Node* node) : node_(node) {}

        T& operator*() { return node_->data; }
        const T& operator*() const { return node_->data; }
        T* operator->() { return &node_->data; }
        const T* operator->() const { return &node_->data; }

        Iterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            node_ = node_->next;
            return tmp;
        }

        Iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            node_ = node_->prev;
            return tmp;
        }

        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }

    private:
        Node* node_;
        friend class LinkedList;
    };

    class ConstIterator {
    public:
        ConstIterator() : node_(nullptr) {}
        ConstIterator(const Node* node) : node_(node) {}
        ConstIterator(const Iterator& it) : node_(it.node_) {}

        const T& operator*() const { return node_->data; }
        const T* operator->() const { return &node_->data; }

        ConstIterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator tmp = *this;
            node_ = node_->next;
            return tmp;
        }

        bool operator==(const ConstIterator& other) const { return node_ == other.node_; }
        bool operator!=(const ConstIterator& other) const { return node_ != other.node_; }

    private:
        const Node* node_;
    };

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }
    ConstIterator begin() const { return ConstIterator(head_); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    ConstIterator cbegin() const { return ConstIterator(head_); }
    ConstIterator cend() const { return ConstIterator(nullptr); }

    Iterator insert(Iterator pos, const T& data) {
        if (pos == begin()) {
            push_front(data);
            return begin();
        }
        if (pos == end()) {
            push_back(data);
            return Iterator(tail_);
        }
        
        Node* nextNode = pos.node_;
        Node* prevNode = nextNode->prev;
        Node* newNode = new Node(data);
        newNode->prev = prevNode;
        newNode->next = nextNode;
        prevNode->next = newNode;
        nextNode->prev = newNode;
        ++size_;
        return Iterator(newNode);
    }

    Iterator erase(Iterator pos) {
        if (pos == end()) return end();
        
        Node* node = pos.node_;
        Node* nextNode = node->next;
        
        if (node == head_) {
            pop_front();
            return begin();
        }
        if (node == tail_) {
            pop_back();
            return end();
        }
        
        node->prev->next = nextNode;
        nextNode->prev = node->prev;
        delete node;
        --size_;
        return Iterator(nextNode);
    }

    void remove(const T& value) {
        for (Iterator it = begin(); it != end(); ) {
            if (*it == value) {
                it = erase(it);
            } else {
                ++it;
            }
        }
    }

    template<typename Predicate>
    void remove_if(Predicate pred) {
        for (Iterator it = begin(); it != end(); ) {
            if (pred(*it)) {
                it = erase(it);
            } else {
                ++it;
            }
        }
    }

    void reverse() {
        Node* current = head_;
        Node* temp = nullptr;
        while (current) {
            temp = current->prev;
            current->prev = current->next;
            current->next = temp;
            current = current->prev;
        }
        temp = head_;
        head_ = tail_;
        tail_ = temp;
    }

private:
    Node* head_;
    Node* tail_;
    size_t size_;
};

template<typename T, size_t N>
class RingBuffer {
public:
    RingBuffer() : head_(0), tail_(0), count_(0) {}

    bool empty() const { return count_ == 0; }
    bool full() const { return count_ == N; }
    size_t size() const { return count_; }
    size_t capacity() const { return N; }

    void push(const T& value) {
        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % N;
        if (count_ < N) {
            ++count_;
        } else {
            head_ = (head_ + 1) % N;
        }
    }

    void push(T&& value) {
        buffer_[tail_] = std::move(value);
        tail_ = (tail_ + 1) % N;
        if (count_ < N) {
            ++count_;
        } else {
            head_ = (head_ + 1) % N;
        }
    }

    void pop() {
        if (count_ > 0) {
            head_ = (head_ + 1) % N;
            --count_;
        }
    }

    T& front() {
        assert(count_ > 0);
        return buffer_[head_];
    }

    const T& front() const {
        assert(count_ > 0);
        return buffer_[head_];
    }

    T& back() {
        assert(count_ > 0);
        size_t index = (tail_ == 0) ? N - 1 : tail_ - 1;
        return buffer_[index];
    }

    const T& back() const {
        assert(count_ > 0);
        size_t index = (tail_ == 0) ? N - 1 : tail_ - 1;
        return buffer_[index];
    }

    T& operator[](size_t index) {
        assert(index < count_);
        return buffer_[(head_ + index) % N];
    }

    const T& operator[](size_t index) const {
        assert(index < count_);
        return buffer_[(head_ + index) % N];
    }

    void clear() {
        head_ = tail_ = count_ = 0;
    }

private:
    T buffer_[N];
    size_t head_;
    size_t tail_;
    size_t count_;
};

template<typename T, typename Compare = std::less<T>>
class PriorityQueue {
public:
    PriorityQueue() = default;

    explicit PriorityQueue(const Compare& comp) : compare_(comp) {}

    void push(const T& value) {
        data_.push_back(value);
        heapifyUp(data_.size() - 1);
    }

    void push(T&& value) {
        data_.push_back(std::move(value));
        heapifyUp(data_.size() - 1);
    }

    template<typename... Args>
    void emplace(Args&&... args) {
        data_.emplace_back(std::forward<Args>(args)...);
        heapifyUp(data_.size() - 1);
    }

    void pop() {
        if (data_.empty()) return;
        data_[0] = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) {
            heapifyDown(0);
        }
    }

    T& top() {
        assert(!data_.empty());
        return data_[0];
    }

    const T& top() const {
        assert(!data_.empty());
        return data_[0];
    }

    size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    void clear() { data_.clear(); }

private:
    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (compare_(data_[parent], data_[index])) {
                std::swap(data_[index], data_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(size_t index) {
        size_t size = data_.size();
        while (true) {
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            size_t largest = index;

            if (left < size && compare_(data_[largest], data_[left])) {
                largest = left;
            }
            if (right < size && compare_(data_[largest], data_[right])) {
                largest = right;
            }

            if (largest != index) {
                std::swap(data_[index], data_[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

    std::vector<T> data_;
    Compare compare_;
};

template<typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(size_t capacity) : capacity_(capacity) {}

    bool contains(const K& key) const {
        return cacheMap_.find(key) != cacheMap_.end();
    }

    V& get(const K& key) {
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            throw std::out_of_range("Key not found in cache");
        }
        moveToFront(it->second);
        return it->second->second;
    }

    const V& get(const K& key) const {
        auto it = cacheMap_.find(key);
        if (it == cacheMap_.end()) {
            throw std::out_of_range("Key not found in cache");
        }
        return it->second->second;
    }

    void put(const K& key, const V& value) {
        auto it = cacheMap_.find(key);
        if (it != cacheMap_.end()) {
            it->second->second = value;
            moveToFront(it->second);
            return;
        }

        if (cacheList_.size() >= capacity_) {
            evict();
        }

        cacheList_.push_front({key, value});
        cacheMap_[key] = cacheList_.begin();
    }

    void put(const K& key, V&& value) {
        auto it = cacheMap_.find(key);
        if (it != cacheMap_.end()) {
            it->second->second = std::move(value);
            moveToFront(it->second);
            return;
        }

        if (cacheList_.size() >= capacity_) {
            evict();
        }

        cacheList_.push_front({key, std::move(value)});
        cacheMap_[key] = cacheList_.begin();
    }

    void remove(const K& key) {
        auto it = cacheMap_.find(key);
        if (it != cacheMap_.end()) {
            cacheList_.erase(it->second);
            cacheMap_.erase(it);
        }
    }

    void clear() {
        cacheMap_.clear();
        cacheList_.clear();
    }

    size_t size() const { return cacheList_.size(); }
    size_t capacity() const { return capacity_; }
    bool empty() const { return cacheList_.empty(); }

    void setCapacity(size_t capacity) {
        capacity_ = capacity;
        while (cacheList_.size() > capacity_) {
            evict();
        }
    }

private:
    using ListNode = std::pair<K, V>;
    using ListIterator = typename std::list<ListNode>::iterator;

    void moveToFront(ListIterator it) {
        if (it != cacheList_.begin()) {
            cacheList_.splice(cacheList_.begin(), cacheList_, it);
        }
    }

    void evict() {
        if (!cacheList_.empty()) {
            cacheMap_.erase(cacheList_.back().first);
            cacheList_.pop_back();
        }
    }

    size_t capacity_;
    std::list<ListNode> cacheList_;
    std::unordered_map<K, ListIterator> cacheMap_;
};

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        cv_.notify_one();
    }

    void push(T&& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
        cv_.notify_one();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        value = std::move(queue_.front());
        queue_.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty()) {
            queue_.pop();
        }
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<T> queue_;
};

template<typename T>
class ObjectPool {
public:
    ObjectPool() = default;

    explicit ObjectPool(size_t initialSize) {
        for (size_t i = 0; i < initialSize; ++i) {
            pool_.push(new T());
        }
    }

    ~ObjectPool() {
        clear();
    }

    T* acquire() {
        if (pool_.empty()) {
            return new T();
        }
        T* obj = pool_.top();
        pool_.pop();
        return obj;
    }

    template<typename... Args>
    T* acquire(Args&&... args) {
        if (pool_.empty()) {
            return new T(std::forward<Args>(args)...);
        }
        T* obj = pool_.top();
        pool_.pop();
        *obj = T(std::forward<Args>(args)...);
        return obj;
    }

    void release(T* obj) {
        if (obj) {
            pool_.push(obj);
        }
    }

    void clear() {
        while (!pool_.empty()) {
            delete pool_.top();
            pool_.pop();
        }
    }

    size_t size() const { return pool_.size(); }
    bool empty() const { return pool_.empty(); }

    void reserve(size_t count) {
        while (pool_.size() < count) {
            pool_.push(new T());
        }
    }

private:
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    std::stack<T*> pool_;
};

template<typename T, size_t BlockSize = 4096>
class MemoryPool {
public:
    MemoryPool() : currentBlock_(nullptr), currentSlot_(nullptr), 
                   lastSlot_(nullptr), freeSlots_(nullptr) {}

    ~MemoryPool() {
        for (Block* block : blocks_) {
            operator delete(block);
        }
    }

    T* allocate() {
        if (freeSlots_ != nullptr) {
            T* result = reinterpret_cast<T*>(freeSlots_);
            freeSlots_ = freeSlots_->next;
            return result;
        }

        if (currentSlot_ >= lastSlot_) {
            allocateBlock();
        }

        return reinterpret_cast<T*>(currentSlot_++);
    }

    void deallocate(T* p) {
        if (p != nullptr) {
            reinterpret_cast<Slot*>(p)->next = freeSlots_;
            freeSlots_ = reinterpret_cast<Slot*>(p);
        }
    }

    template<typename... Args>
    T* construct(Args&&... args) {
        T* p = allocate();
        new (p) T(std::forward<Args>(args)...);
        return p;
    }

    void destroy(T* p) {
        p->~T();
        deallocate(p);
    }

    void clear() {
        for (Block* block : blocks_) {
            operator delete(block);
        }
        blocks_.clear();
        currentBlock_ = nullptr;
        currentSlot_ = nullptr;
        lastSlot_ = nullptr;
        freeSlots_ = nullptr;
    }

private:
    struct Slot {
        Slot* next;
    };

    struct Block {
        Block* next;
    };

    void allocateBlock() {
        size_t blockSize = BlockSize * sizeof(T);
        char* newBlock = reinterpret_cast<char*>(operator new(blockSize + sizeof(Block)));
        Block* block = reinterpret_cast<Block*>(newBlock);
        block->next = currentBlock_;
        currentBlock_ = block;
        blocks_.push_back(block);

        currentSlot_ = reinterpret_cast<char*>(currentBlock_) + sizeof(Block);
        lastSlot_ = currentSlot_ + BlockSize * sizeof(T);
    }

    Block* currentBlock_;
    char* currentSlot_;
    char* lastSlot_;
    Slot* freeSlots_;
    std::vector<Block*> blocks_;
};

template<typename K, typename V, typename Hash = std::hash<K>, typename KeyEqual = std::equal_to<K>>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;
        bool occupied;

        Entry() : next(nullptr), occupied(false) {}
        Entry(const K& k, const V& v) : key(k), value(v), next(nullptr), occupied(true) {}
        Entry(K&& k, V&& v) : key(std::move(k)), value(std::move(v)), next(nullptr), occupied(true) {}
    };

public:
    HashMap() : size_(0), capacity_(8), loadFactor_(0.75f) {
        buckets_ = new Entry*[capacity_]();
    }

    explicit HashMap(size_t capacity) : size_(0), capacity_(capacity), loadFactor_(0.75f) {
        if (capacity_ < 8) capacity_ = 8;
        buckets_ = new Entry*[capacity_]();
    }

    HashMap(const HashMap& other) : size_(0), capacity_(other.capacity_), loadFactor_(other.loadFactor_) {
        buckets_ = new Entry*[capacity_]();
        for (size_t i = 0; i < other.capacity_; ++i) {
            Entry* entry = other.buckets_[i];
            while (entry) {
                if (entry->occupied) {
                    insert(entry->key, entry->value);
                }
                entry = entry->next;
            }
        }
    }

    HashMap(HashMap&& other) noexcept 
        : buckets_(other.buckets_), size_(other.size_), capacity_(other.capacity_), loadFactor_(other.loadFactor_) {
        other.buckets_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~HashMap() {
        clear();
        delete[] buckets_;
    }

    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            delete[] buckets_;
            capacity_ = other.capacity_;
            size_ = 0;
            loadFactor_ = other.loadFactor_;
            buckets_ = new Entry*[capacity_]();
            for (size_t i = 0; i < other.capacity_; ++i) {
                Entry* entry = other.buckets_[i];
                while (entry) {
                    if (entry->occupied) {
                        insert(entry->key, entry->value);
                    }
                    entry = entry->next;
                }
            }
        }
        return *this;
    }

    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) {
            clear();
            delete[] buckets_;
            buckets_ = other.buckets_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            loadFactor_ = other.loadFactor_;
            other.buckets_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void insert(const K& key, const V& value) {
        if (static_cast<float>(size_) / capacity_ >= loadFactor_) {
            rehash(capacity_ * 2);
        }

        size_t index = hash_(key) % capacity_;
        Entry* entry = buckets_[index];
        Entry* prev = nullptr;

        while (entry) {
            if (entry->occupied && keyEqual_(entry->key, key)) {
                entry->value = value;
                return;
            }
            prev = entry;
            entry = entry->next;
        }

        Entry* newEntry = new Entry(key, value);
        if (prev) {
            prev->next = newEntry;
        } else {
            buckets_[index] = newEntry;
        }
        ++size_;
    }

    void insert(K&& key, V&& value) {
        if (static_cast<float>(size_) / capacity_ >= loadFactor_) {
            rehash(capacity_ * 2);
        }

        size_t index = hash_(key) % capacity_;
        Entry* entry = buckets_[index];
        Entry* prev = nullptr;

        while (entry) {
            if (entry->occupied && keyEqual_(entry->key, key)) {
                entry->value = std::move(value);
                return;
            }
            prev = entry;
            entry = entry->next;
        }

        Entry* newEntry = new Entry(std::move(key), std::move(value));
        if (prev) {
            prev->next = newEntry;
        } else {
            buckets_[index] = newEntry;
        }
        ++size_;
    }

    bool contains(const K& key) const {
        size_t index = hash_(key) % capacity_;
        Entry* entry = buckets_[index];
        while (entry) {
            if (entry->occupied && keyEqual_(entry->key, key)) {
                return true;
            }
            entry = entry->next;
        }
        return false;
    }

    V& get(const K& key) {
        size_t index = hash_(key) % capacity_;
        Entry* entry = buckets_[index];
        while (entry) {
            if (entry->occupied && keyEqual_(entry->key, key)) {
                return entry->value;
            }
            entry = entry->next;
        }
        throw std::out_of_range("Key not found");
    }

    const V& get(const K& key) const {
        size_t index = hash_(key) % capacity_;
        Entry* entry = buckets_[index];
        while (entry) {
            if (entry->occupied && keyEqual_(entry->key, key)) {
                return entry->value;
            }
            entry = entry->next;
        }
        throw std::out_of_range("Key not found");
    }

    V& operator[](const K& key) {
        if (!contains(key)) {
            insert(key, V());
        }
        return get(key);
    }

    bool remove(const K& key) {
        size_t index = hash_(key) % capacity_;
        Entry* entry = buckets_[index];
        Entry* prev = nullptr;

        while (entry) {
            if (entry->occupied && keyEqual_(entry->key, key)) {
                entry->occupied = false;
                --size_;
                return true;
            }
            prev = entry;
            entry = entry->next;
        }
        return false;
    }

    void clear() {
        for (size_t i = 0; i < capacity_; ++i) {
            Entry* entry = buckets_[i];
            while (entry) {
                Entry* next = entry->next;
                delete entry;
                entry = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    float loadFactor() const { return loadFactor_; }
    void setLoadFactor(float factor) { 
        loadFactor_ = std::max(0.1f, std::min(0.95f, factor)); 
    }

    void rehash(size_t newCapacity) {
        if (newCapacity < capacity_) return;

        Entry** oldBuckets = buckets_;
        size_t oldCapacity = capacity_;

        capacity_ = newCapacity;
        buckets_ = new Entry*[capacity_]();
        size_ = 0;

        for (size_t i = 0; i < oldCapacity; ++i) {
            Entry* entry = oldBuckets[i];
            while (entry) {
                Entry* next = entry->next;
                if (entry->occupied) {
                    insert(std::move(entry->key), std::move(entry->value));
                }
                delete entry;
                entry = next;
            }
        }

        delete[] oldBuckets;
    }

    std::vector<K> keys() const {
        std::vector<K> result;
        result.reserve(size_);
        for (size_t i = 0; i < capacity_; ++i) {
            Entry* entry = buckets_[i];
            while (entry) {
                if (entry->occupied) {
                    result.push_back(entry->key);
                }
                entry = entry->next;
            }
        }
        return result;
    }

    std::vector<V> values() const {
        std::vector<V> result;
        result.reserve(size_);
        for (size_t i = 0; i < capacity_; ++i) {
            Entry* entry = buckets_[i];
            while (entry) {
                if (entry->occupied) {
                    result.push_back(entry->value);
                }
                entry = entry->next;
            }
        }
        return result;
    }

private:
    Entry** buckets_;
    size_t size_;
    size_t capacity_;
    float loadFactor_;
    Hash hash_;
    KeyEqual keyEqual_;
};

template<typename T>
class TrieNode {
public:
    TrieNode() : isEndOfWord_(false) {}

    bool hasChild(char c) const {
        return children_.find(c) != children_.end();
    }

    TrieNode* getChild(char c) {
        auto it = children_.find(c);
        return it != children_.end() ? it->second : nullptr;
    }

    const TrieNode* getChild(char c) const {
        auto it = children_.find(c);
        return it != children_.end() ? it->second : nullptr;
    }

    TrieNode* addChild(char c) {
        TrieNode* child = new TrieNode();
        children_[c] = child;
        return child;
    }

    bool isEndOfWord() const { return isEndOfWord_; }
    void setEndOfWord(bool value) { isEndOfWord_ = value; }

    T& value() { return value_; }
    const T& value() const { return value_; }
    void setValue(const T& value) { value_ = value; }

    const std::map<char, TrieNode*>& children() const { return children_; }

private:
    std::map<char, TrieNode*> children_;
    bool isEndOfWord_;
    T value_;
};

template<typename T>
class Trie {
public:
    Trie() : root_(new TrieNode<T>()) {}

    ~Trie() {
        clear();
        delete root_;
    }

    void insert(const std::string& key, const T& value) {
        TrieNode<T>* current = root_;
        for (char c : key) {
            if (!current->hasChild(c)) {
                current = current->addChild(c);
            } else {
                current = current->getChild(c);
            }
        }
        current->setEndOfWord(true);
        current->setValue(value);
    }

    bool contains(const std::string& key) const {
        const TrieNode<T>* node = findNode(key);
        return node != nullptr && node->isEndOfWord();
    }

    bool get(const std::string& key, T& value) const {
        const TrieNode<T>* node = findNode(key);
        if (node != nullptr && node->isEndOfWord()) {
            value = node->value();
            return true;
        }
        return false;
    }

    bool remove(const std::string& key) {
        return removeHelper(root_, key, 0);
    }

    bool startsWith(const std::string& prefix) const {
        return findNode(prefix) != nullptr;
    }

    std::vector<std::string> keysWithPrefix(const std::string& prefix) const {
        std::vector<std::string> result;
        const TrieNode<T>* node = findNode(prefix);
        if (node) {
            collectKeys(node, prefix, result);
        }
        return result;
    }

    void clear() {
        clearHelper(root_);
        root_ = new TrieNode<T>();
    }

private:
    const TrieNode<T>* findNode(const std::string& key) const {
        const TrieNode<T>* current = root_;
        for (char c : key) {
            current = current->getChild(c);
            if (!current) return nullptr;
        }
        return current;
    }

    bool removeHelper(TrieNode<T>* node, const std::string& key, size_t depth) {
        if (depth == key.length()) {
            if (!node->isEndOfWord()) {
                return false;
            }
            node->setEndOfWord(false);
            return node->children().empty();
        }

        char c = key[depth];
        TrieNode<T>* child = node->getChild(c);
        if (!child) {
            return false;
        }

        bool shouldDeleteChild = removeHelper(child, key, depth + 1);

        if (shouldDeleteChild) {
            delete child;
            node->children().erase(c);
            return node->children().empty() && !node->isEndOfWord();
        }

        return false;
    }

    void clearHelper(TrieNode<T>* node) {
        for (auto& pair : node->children()) {
            clearHelper(pair.second);
            delete pair.second;
        }
    }

    void collectKeys(const TrieNode<T>* node, const std::string& prefix, std::vector<std::string>& result) const {
        if (node->isEndOfWord()) {
            result.push_back(prefix);
        }
        for (const auto& pair : node->children()) {
            collectKeys(pair.second, prefix + pair.first, result);
        }
    }

    TrieNode<T>* root_;
};

}
}
