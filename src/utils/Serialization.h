#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <cstring>

namespace LGE {

class BinaryWriter {
public:
    BinaryWriter() = default;
    
    explicit BinaryWriter(const std::string& filename) {
        open(filename);
    }
    
    ~BinaryWriter() {
        close();
    }
    
    bool open(const std::string& filename) {
        m_stream.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
        return m_stream.is_open();
    }
    
    void close() {
        if (m_stream.is_open()) {
            m_stream.flush();
            m_stream.close();
        }
    }
    
    bool isOpen() const {
        return m_stream.is_open();
    }
    
    void flush() {
        m_stream.flush();
    }
    
    std::streampos position() const {
        return m_stream.tellp();
    }
    
    void seek(std::streampos pos) {
        m_stream.seekp(pos);
    }
    
    void writeBool(bool value) {
        write<uint8_t>(value ? 1 : 0);
    }
    
    void writeInt8(int8_t value) {
        write(value);
    }
    
    void writeUInt8(uint8_t value) {
        write(value);
    }
    
    void writeInt16(int16_t value) {
        write(value);
    }
    
    void writeUInt16(uint16_t value) {
        write(value);
    }
    
    void writeInt32(int32_t value) {
        write(value);
    }
    
    void writeUInt32(uint32_t value) {
        write(value);
    }
    
    void writeInt64(int64_t value) {
        write(value);
    }
    
    void writeUInt64(uint64_t value) {
        write(value);
    }
    
    void writeFloat(float value) {
        write(value);
    }
    
    void writeDouble(double value) {
        write(value);
    }
    
    void writeString(const std::string& value) {
        writeUInt32(static_cast<uint32_t>(value.size()));
        m_stream.write(value.data(), static_cast<std::streamsize>(value.size()));
    }
    
    void writeBytes(const void* data, size_t size) {
        m_stream.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    }
    
    template<typename T>
    void write(const T& value) {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
        m_stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }
    
    template<typename T>
    void writeVector(const std::vector<T>& vec) {
        writeUInt32(static_cast<uint32_t>(vec.size()));
        for (const auto& item : vec) {
            write(item);
        }
    }
    
    void writeVectorString(const std::vector<std::string>& vec) {
        writeUInt32(static_cast<uint32_t>(vec.size()));
        for (const auto& item : vec) {
            writeString(item);
        }
    }
    
    template<typename K, typename V>
    void writeMap(const std::map<K, V>& map) {
        writeUInt32(static_cast<uint32_t>(map.size()));
        for (const auto& [key, value] : map) {
            write(key);
            write(value);
        }
    }
    
    template<typename K, typename V>
    void writeUnorderedMap(const std::unordered_map<K, V>& map) {
        writeUInt32(static_cast<uint32_t>(map.size()));
        for (const auto& [key, value] : map) {
            write(key);
            write(value);
        }
    }
    
    template<typename T>
    void writeSet(const std::set<T>& set) {
        writeUInt32(static_cast<uint32_t>(set.size()));
        for (const auto& item : set) {
            write(item);
        }
    }
    
    template<typename T>
    void writeUnorderedSet(const std::unordered_set<T>& set) {
        writeUInt32(static_cast<uint32_t>(set.size()));
        for (const auto& item : set) {
            write(item);
        }
    }
    
    template<typename T>
    void writeList(const std::list<T>& lst) {
        writeUInt32(static_cast<uint32_t>(lst.size()));
        for (const auto& item : lst) {
            write(item);
        }
    }
    
    template<typename T>
    void writeDeque(const std::deque<T>& deq) {
        writeUInt32(static_cast<uint32_t>(deq.size()));
        for (const auto& item : deq) {
            write(item);
        }
    }
    
private:
    std::ofstream m_stream;
};

class BinaryReader {
public:
    BinaryReader() = default;
    
    explicit BinaryReader(const std::string& filename) {
        open(filename);
    }
    
    ~BinaryReader() {
        close();
    }
    
    bool open(const std::string& filename) {
        m_stream.open(filename, std::ios::binary | std::ios::in);
        return m_stream.is_open();
    }
    
    void close() {
        if (m_stream.is_open()) {
            m_stream.close();
        }
    }
    
    bool isOpen() const {
        return m_stream.is_open();
    }
    
    bool eof() const {
        return m_stream.eof();
    }
    
    std::streampos position() const {
        return m_stream.tellg();
    }
    
    void seek(std::streampos pos) {
        m_stream.seekg(pos);
    }
    
    void seekBegin() {
        m_stream.seekg(0, std::ios::beg);
    }
    
    void seekEnd() {
        m_stream.seekg(0, std::ios::end);
    }
    
    bool readBool() {
        return read<uint8_t>() != 0;
    }
    
    int8_t readInt8() {
        return read<int8_t>();
    }
    
    uint8_t readUInt8() {
        return read<uint8_t>();
    }
    
    int16_t readInt16() {
        return read<int16_t>();
    }
    
    uint16_t readUInt16() {
        return read<uint16_t>();
    }
    
    int32_t readInt32() {
        return read<int32_t>();
    }
    
    uint32_t readUInt32() {
        return read<uint32_t>();
    }
    
    int64_t readInt64() {
        return read<int64_t>();
    }
    
    uint64_t readUInt64() {
        return read<uint64_t>();
    }
    
    float readFloat() {
        return read<float>();
    }
    
    double readDouble() {
        return read<double>();
    }
    
    std::string readString() {
        uint32_t size = readUInt32();
        std::string result(size, '\0');
        m_stream.read(&result[0], static_cast<std::streamsize>(size));
        return result;
    }
    
    void readBytes(void* data, size_t size) {
        m_stream.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
    }
    
    template<typename T>
    T read() {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
        T value;
        m_stream.read(reinterpret_cast<char*>(&value), sizeof(T));
        return value;
    }
    
    template<typename T>
    std::vector<T> readVector() {
        uint32_t size = readUInt32();
        std::vector<T> result;
        result.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            result.push_back(read<T>());
        }
        return result;
    }
    
    std::vector<std::string> readVectorString() {
        uint32_t size = readUInt32();
        std::vector<std::string> result;
        result.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            result.push_back(readString());
        }
        return result;
    }
    
    template<typename K, typename V>
    std::map<K, V> readMap() {
        uint32_t size = readUInt32();
        std::map<K, V> result;
        for (uint32_t i = 0; i < size; i++) {
            K key = read<K>();
            V value = read<V>();
            result[key] = value;
        }
        return result;
    }
    
    template<typename K, typename V>
    std::unordered_map<K, V> readUnorderedMap() {
        uint32_t size = readUInt32();
        std::unordered_map<K, V> result;
        result.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            K key = read<K>();
            V value = read<V>();
            result[key] = value;
        }
        return result;
    }
    
    template<typename T>
    std::set<T> readSet() {
        uint32_t size = readUInt32();
        std::set<T> result;
        for (uint32_t i = 0; i < size; i++) {
            result.insert(read<T>());
        }
        return result;
    }
    
    template<typename T>
    std::unordered_set<T> readUnorderedSet() {
        uint32_t size = readUInt32();
        std::unordered_set<T> result;
        result.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            result.insert(read<T>());
        }
        return result;
    }
    
    template<typename T>
    std::list<T> readList() {
        uint32_t size = readUInt32();
        std::list<T> result;
        for (uint32_t i = 0; i < size; i++) {
            result.push_back(read<T>());
        }
        return result;
    }
    
    template<typename T>
    std::deque<T> readDeque() {
        uint32_t size = readUInt32();
        std::deque<T> result;
        for (uint32_t i = 0; i < size; i++) {
            result.push_back(read<T>());
        }
        return result;
    }
    
private:
    std::ifstream m_stream;
};

class MemoryBuffer {
public:
    MemoryBuffer() = default;
    
    explicit MemoryBuffer(size_t size) {
        m_data.reserve(size);
    }
    
    MemoryBuffer(const void* data, size_t size) {
        write(data, size);
    }
    
    void clear() {
        m_data.clear();
        m_readPosition = 0;
    }
    
    size_t size() const {
        return m_data.size();
    }
    
    size_t capacity() const {
        return m_data.capacity();
    }
    
    void reserve(size_t size) {
        m_data.reserve(size);
    }
    
    const uint8_t* data() const {
        return m_data.data();
    }
    
    uint8_t* data() {
        return m_data.data();
    }
    
    bool eof() const {
        return m_readPosition >= m_data.size();
    }
    
    size_t readPosition() const {
        return m_readPosition;
    }
    
    void seekRead(size_t pos) {
        m_readPosition = pos;
    }
    
    void writeBool(bool value) {
        write<uint8_t>(value ? 1 : 0);
    }
    
    void writeInt8(int8_t value) {
        write(value);
    }
    
    void writeUInt8(uint8_t value) {
        write(value);
    }
    
    void writeInt16(int16_t value) {
        write(value);
    }
    
    void writeUInt16(uint16_t value) {
        write(value);
    }
    
    void writeInt32(int32_t value) {
        write(value);
    }
    
    void writeUInt32(uint32_t value) {
        write(value);
    }
    
    void writeInt64(int64_t value) {
        write(value);
    }
    
    void writeUInt64(uint64_t value) {
        write(value);
    }
    
    void writeFloat(float value) {
        write(value);
    }
    
    void writeDouble(double value) {
        write(value);
    }
    
    void writeString(const std::string& value) {
        writeUInt32(static_cast<uint32_t>(value.size()));
        write(value.data(), value.size());
    }
    
    void write(const void* data, size_t size) {
        const uint8_t* src = static_cast<const uint8_t*>(data);
        m_data.insert(m_data.end(), src, src + size);
    }
    
    template<typename T>
    void write(const T& value) {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
        write(&value, sizeof(T));
    }
    
    bool readBool() {
        return read<uint8_t>() != 0;
    }
    
    int8_t readInt8() {
        return read<int8_t>();
    }
    
    uint8_t readUInt8() {
        return read<uint8_t>();
    }
    
    int16_t readInt16() {
        return read<int16_t>();
    }
    
    uint16_t readUInt16() {
        return read<uint16_t>();
    }
    
    int32_t readInt32() {
        return read<int32_t>();
    }
    
    uint32_t readUInt32() {
        return read<uint32_t>();
    }
    
    int64_t readInt64() {
        return read<int64_t>();
    }
    
    uint64_t readUInt64() {
        return read<uint64_t>();
    }
    
    float readFloat() {
        return read<float>();
    }
    
    double readDouble() {
        return read<double>();
    }
    
    std::string readString() {
        uint32_t size = readUInt32();
        std::string result;
        result.resize(size);
        read(&result[0], size);
        return result;
    }
    
    void read(void* data, size_t size) {
        if (m_readPosition + size > m_data.size()) {
            throw std::runtime_error("MemoryBuffer: read past end of buffer");
        }
        std::memcpy(data, m_data.data() + m_readPosition, size);
        m_readPosition += size;
    }
    
    template<typename T>
    T read() {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
        T value;
        read(&value, sizeof(T));
        return value;
    }
    
    void writeToFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("MemoryBuffer: could not open file for writing");
        }
        file.write(reinterpret_cast<const char*>(m_data.data()), static_cast<std::streamsize>(m_data.size()));
    }
    
    void readFromFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file) {
            throw std::runtime_error("MemoryBuffer: could not open file for reading");
        }
        size_t size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        m_data.resize(size);
        file.read(reinterpret_cast<char*>(m_data.data()), static_cast<std::streamsize>(size));
        m_readPosition = 0;
    }
    
private:
    std::vector<uint8_t> m_data;
    size_t m_readPosition = 0;
};

}
