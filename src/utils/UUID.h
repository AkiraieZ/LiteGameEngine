#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <stdexcept>

namespace LGE {

class UUID {
public:
    UUID() : m_data{} {}

    UUID(const UUID& other) = default;
    UUID& operator=(const UUID& other) = default;

    static UUID generate() {
        UUID uuid;
        static std::mt19937_64 rng(static_cast<uint64_t>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        ));
        static std::uniform_int_distribution<uint64_t> dist;

        uint64_t random1 = dist(rng);
        uint64_t random2 = dist(rng);

        std::memcpy(uuid.m_data.data(), &random1, 8);
        std::memcpy(uuid.m_data.data() + 8, &random2, 8);

        uuid.m_data[6] = (uuid.m_data[6] & 0x0F) | 0x40;
        uuid.m_data[8] = (uuid.m_data[8] & 0x3F) | 0x80;

        return uuid;
    }

    static UUID nil() {
        return UUID();
    }

    bool isNil() const {
        for (uint8_t byte : m_data) {
            if (byte != 0) return false;
        }
        return true;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        
        for (size_t i = 0; i < 4; i++) {
            oss << std::setw(2) << static_cast<int>(m_data[i]);
        }
        oss << '-';
        for (size_t i = 4; i < 6; i++) {
            oss << std::setw(2) << static_cast<int>(m_data[i]);
        }
        oss << '-';
        for (size_t i = 6; i < 8; i++) {
            oss << std::setw(2) << static_cast<int>(m_data[i]);
        }
        oss << '-';
        for (size_t i = 8; i < 10; i++) {
            oss << std::setw(2) << static_cast<int>(m_data[i]);
        }
        oss << '-';
        for (size_t i = 10; i < 16; i++) {
            oss << std::setw(2) << static_cast<int>(m_data[i]);
        }
        
        return oss.str();
    }

    std::string toShortString() const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        
        for (uint8_t byte : m_data) {
            oss << std::setw(2) << static_cast<int>(byte);
        }
        
        return oss.str();
    }

    static UUID fromString(const std::string& str) {
        UUID uuid;
        std::string cleanStr;
        cleanStr.reserve(32);

        for (char c : str) {
            if (c != '-') {
                cleanStr.push_back(c);
            }
        }

        if (cleanStr.length() != 32) {
            throw std::invalid_argument("Invalid UUID string length");
        }

        for (size_t i = 0; i < 16; i++) {
            unsigned int byte;
            std::istringstream iss(cleanStr.substr(i * 2, 2));
            iss >> std::hex >> byte;
            uuid.m_data[i] = static_cast<uint8_t>(byte);
        }

        return uuid;
    }

    const std::array<uint8_t, 16>& data() const { return m_data; }
    std::array<uint8_t, 16>& data() { return m_data; }

    uint64_t getMostSignificantBits() const {
        uint64_t result = 0;
        std::memcpy(&result, m_data.data(), 8);
        return result;
    }

    uint64_t getLeastSignificantBits() const {
        uint64_t result = 0;
        std::memcpy(&result, m_data.data() + 8, 8);
        return result;
    }

    int version() const {
        return (m_data[6] >> 4) & 0x0F;
    }

    int variant() const {
        int variant = m_data[8] >> 6;
        if (variant == 0) return 0;
        if (variant == 1) return 1;
        if (variant == 2) return variant;
        return 3;
    }

    bool operator==(const UUID& other) const {
        return m_data == other.m_data;
    }

    bool operator!=(const UUID& other) const {
        return m_data != other.m_data;
    }

    bool operator<(const UUID& other) const {
        return m_data < other.m_data;
    }

    bool operator<=(const UUID& other) const {
        return m_data <= other.m_data;
    }

    bool operator>(const UUID& other) const {
        return m_data > other.m_data;
    }

    bool operator>=(const UUID& other) const {
        return m_data >= other.m_data;
    }

    size_t hash() const {
        size_t result = 0;
        for (uint8_t byte : m_data) {
            result = (result * 31) + byte;
        }
        return result;
    }

    operator bool() const {
        return !isNil();
    }

private:
    std::array<uint8_t, 16> m_data;
};

class UUIDGenerator {
public:
    UUIDGenerator() {
        auto seed = static_cast<uint64_t>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
        m_rng.seed(seed);
    }

    explicit UUIDGenerator(uint64_t seed) {
        m_rng.seed(seed);
    }

    UUID generate() {
        UUID uuid;
        std::uniform_int_distribution<uint64_t> dist;

        uint64_t random1 = dist(m_rng);
        uint64_t random2 = dist(m_rng);

        std::memcpy(uuid.data().data(), &random1, 8);
        std::memcpy(uuid.data().data() + 8, &random2, 8);

        uuid.data()[6] = (uuid.data()[6] & 0x0F) | 0x40;
        uuid.data()[8] = (uuid.data()[8] & 0x3F) | 0x80;

        return uuid;
    }

    UUID generateTimeBased() {
        UUID uuid;
        
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        uint64_t timestamp = nanos / 100 + 0x01B21DD213814000ULL;

        uint32_t time_low = static_cast<uint32_t>(timestamp & 0xFFFFFFFF);
        uint16_t time_mid = static_cast<uint16_t>((timestamp >> 32) & 0xFFFF);
        uint16_t time_hi_and_version = static_cast<uint16_t>(((timestamp >> 48) & 0x0FFF) | 0x1000);

        uint16_t clock_seq = 0;
        std::uniform_int_distribution<uint16_t> clockDist;
        clock_seq = clockDist(m_rng);
        uint8_t clock_seq_low = static_cast<uint8_t>(clock_seq & 0xFF);
        uint8_t clock_seq_hi_and_reserved = static_cast<uint8_t>(((clock_seq >> 8) & 0x3F) | 0x80);

        uint8_t node[6];
        std::uniform_int_distribution<uint8_t> byteDist(0, 255);
        for (int i = 0; i < 6; i++) {
            node[i] = byteDist(m_rng);
        }
        node[0] |= 0x01;

        std::memcpy(uuid.data().data(), &time_low, 4);
        std::memcpy(uuid.data().data() + 4, &time_mid, 2);
        std::memcpy(uuid.data().data() + 6, &time_hi_and_version, 2);
        uuid.data()[8] = clock_seq_hi_and_reserved;
        uuid.data()[9] = clock_seq_low;
        std::memcpy(uuid.data().data() + 10, node, 6);

        return uuid;
    }

    UUID generateNameBased(const UUID& namespaceId, const std::string& name) {
        UUID uuid;
        
        std::array<uint8_t, 16 + 256> buffer;
        std::memcpy(buffer.data(), namespaceId.data().data(), 16);
        size_t nameLen = std::min(name.length(), size_t(256));
        std::memcpy(buffer.data() + 16, name.data(), nameLen);

        uint64_t hash1 = 0;
        uint64_t hash2 = 0;
        for (size_t i = 0; i < 16 + nameLen; i++) {
            hash1 = (hash1 * 31) + buffer[i];
            hash2 = (hash2 * 17) + buffer[i];
        }

        std::memcpy(uuid.data().data(), &hash1, 8);
        std::memcpy(uuid.data().data() + 8, &hash2, 8);

        uuid.data()[6] = (uuid.data()[6] & 0x0F) | 0x50;
        uuid.data()[8] = (uuid.data()[8] & 0x3F) | 0x80;

        return uuid;
    }

    void seed(uint64_t seed) {
        m_rng.seed(seed);
    }

private:
    std::mt19937_64 m_rng;
};

class UUIDPool {
public:
    UUIDPool() : m_index(0), m_poolSize(1000) {
        preallocate();
    }

    explicit UUIDPool(size_t poolSize) : m_index(0), m_poolSize(poolSize) {
        preallocate();
    }

    UUID get() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_index >= m_uuids.size()) {
            preallocate();
        }
        return m_uuids[m_index++];
    }

    void refill() {
        std::lock_guard<std::mutex> lock(m_mutex);
        preallocate();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_uuids.size();
    }

    size_t remaining() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_uuids.size() - m_index;
    }

    void setPoolSize(size_t size) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_poolSize = size;
    }

    size_t getPoolSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_poolSize;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_uuids.clear();
        m_index = 0;
    }

private:
    void preallocate() {
        m_uuids.clear();
        m_uuids.reserve(m_poolSize);
        UUIDGenerator gen;
        for (size_t i = 0; i < m_poolSize; i++) {
            m_uuids.push_back(gen.generate());
        }
        m_index = 0;
    }

    std::vector<UUID> m_uuids;
    size_t m_index;
    size_t m_poolSize;
    mutable std::mutex m_mutex;
};

class UUIDNamespace {
public:
    static const UUID& DNS() {
        static UUID uuid = UUID::fromString("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
        return uuid;
    }

    static const UUID& URL() {
        static UUID uuid = UUID::fromString("6ba7b811-9dad-11d1-80b4-00c04fd430c8");
        return uuid;
    }

    static const UUID& OID() {
        static UUID uuid = UUID::fromString("6ba7b812-9dad-11d1-80b4-00c04fd430c8");
        return uuid;
    }

    static const UUID& X500() {
        static UUID uuid = UUID::fromString("6ba7b814-9dad-11d1-80b4-00c04fd430c8");
        return uuid;
    }
};

}

namespace std {
    template<>
    struct hash<LGE::UUID> {
        size_t operator()(const LGE::UUID& uuid) const {
            return uuid.hash();
        }
    };
}
