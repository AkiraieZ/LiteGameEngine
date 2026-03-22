#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <chrono>

namespace LGE {
namespace Utils {
namespace Crypto {

class MD5 {
public:
    MD5() {
        reset();
    }

    void reset() {
        count_[0] = count_[1] = 0;
        state_[0] = 0x67452301;
        state_[1] = 0xefcdab89;
        state_[2] = 0x98badcfe;
        state_[3] = 0x10325476;
        memset(buffer_, 0, sizeof(buffer_));
    }

    void update(const uint8_t* data, size_t len) {
        uint32_t index = (count_[0] >> 3) & 0x3F;
        
        if ((count_[0] += len << 3) < (len << 3)) {
            count_[1]++;
        }
        count_[1] += len >> 29;

        uint32_t partLen = 64 - index;
        uint32_t i = 0;

        if (len >= partLen) {
            memcpy(&buffer_[index], data, partLen);
            transform(buffer_);

            for (i = partLen; i + 63 < len; i += 64) {
                transform(&data[i]);
            }

            index = 0;
        }

        memcpy(&buffer_[index], &data[i], len - i);
    }

    void update(const std::string& data) {
        update(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    }

    void update(const std::vector<uint8_t>& data) {
        update(data.data(), data.size());
    }

    std::string finalize() {
        uint8_t digest[16];
        uint8_t padding[64];
        padding[0] = 0x80;
        memset(padding + 1, 0, 63);

        uint8_t bits[8];
        encode(count_, bits, 8);

        uint32_t index = (count_[0] >> 3) & 0x3F;
        uint32_t padLen = (index < 56) ? (56 - index) : (120 - index);
        update(padding, padLen);
        update(bits, 8);

        encode(state_, digest, 16);

        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; ++i) {
            ss << std::setw(2) << static_cast<uint32_t>(digest[i]);
        }
        return ss.str();
    }

    static std::string hash(const std::string& data) {
        MD5 md5;
        md5.update(data);
        return md5.finalize();
    }

    static std::string hash(const std::vector<uint8_t>& data) {
        MD5 md5;
        md5.update(data);
        return md5.finalize();
    }

private:
    void transform(const uint8_t* block) {
        uint32_t a = state_[0], b = state_[1], c = state_[2], d = state_[3];
        uint32_t x[16];

        decode(block, x, 64);

        FF(a, b, c, d, x[0], 7, 0xd76aa478);
        FF(d, a, b, c, x[1], 12, 0xe8c7b756);
        FF(c, d, a, b, x[2], 17, 0x242070db);
        FF(b, c, d, a, x[3], 22, 0xc1bdceee);
        FF(a, b, c, d, x[4], 7, 0xf57c0faf);
        FF(d, a, b, c, x[5], 12, 0x4787c62a);
        FF(c, d, a, b, x[6], 17, 0xa8304613);
        FF(b, c, d, a, x[7], 22, 0xfd469501);
        FF(a, b, c, d, x[8], 7, 0x698098d8);
        FF(d, a, b, c, x[9], 12, 0x8b44f7af);
        FF(c, d, a, b, x[10], 17, 0xffff5bb1);
        FF(b, c, d, a, x[11], 22, 0x895cd7be);
        FF(a, b, c, d, x[12], 7, 0x6b901122);
        FF(d, a, b, c, x[13], 12, 0xfd987193);
        FF(c, d, a, b, x[14], 17, 0xa679438e);
        FF(b, c, d, a, x[15], 22, 0x49b40821);

        GG(a, b, c, d, x[1], 5, 0xf61e2562);
        GG(d, a, b, c, x[6], 9, 0xc040b340);
        GG(c, d, a, b, x[11], 14, 0x265e5a51);
        GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
        GG(a, b, c, d, x[5], 5, 0xd62f105d);
        GG(d, a, b, c, x[10], 9, 0x02441453);
        GG(c, d, a, b, x[15], 14, 0xd8a1e681);
        GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
        GG(a, b, c, d, x[9], 5, 0x21e1cde6);
        GG(d, a, b, c, x[14], 9, 0xc33707d6);
        GG(c, d, a, b, x[3], 14, 0xf4d50d87);
        GG(b, c, d, a, x[8], 20, 0x455a14ed);
        GG(a, b, c, d, x[13], 5, 0xa9e3e905);
        GG(d, a, b, c, x[2], 9, 0xfcefa3f8);
        GG(c, d, a, b, x[7], 14, 0x676f02d9);
        GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);

        HH(a, b, c, d, x[5], 4, 0xfffa3942);
        HH(d, a, b, c, x[8], 11, 0x8771f681);
        HH(c, d, a, b, x[11], 16, 0x6d9d6122);
        HH(b, c, d, a, x[14], 23, 0xfde5380c);
        HH(a, b, c, d, x[1], 4, 0xa4beea44);
        HH(d, a, b, c, x[4], 11, 0x4bdecfa9);
        HH(c, d, a, b, x[7], 16, 0xf6bb4b60);
        HH(b, c, d, a, x[10], 23, 0xbebfbc70);
        HH(a, b, c, d, x[13], 4, 0x289b7ec6);
        HH(d, a, b, c, x[0], 11, 0xeaa127fa);
        HH(c, d, a, b, x[3], 16, 0xd4ef3085);
        HH(b, c, d, a, x[6], 23, 0x04881d05);
        HH(a, b, c, d, x[9], 4, 0xd9d4d039);
        HH(d, a, b, c, x[12], 11, 0xe6db99e5);
        HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
        HH(b, c, d, a, x[2], 23, 0xc4ac5665);

        II(a, b, c, d, x[0], 6, 0xf4292244);
        II(d, a, b, c, x[7], 10, 0x432aff97);
        II(c, d, a, b, x[14], 15, 0xab9423a7);
        II(b, c, d, a, x[5], 21, 0xfc93a039);
        II(a, b, c, d, x[12], 6, 0x655b59c3);
        II(d, a, b, c, x[3], 10, 0x8f0ccc92);
        II(c, d, a, b, x[10], 15, 0xffeff47d);
        II(b, c, d, a, x[1], 21, 0x85845dd1);
        II(a, b, c, d, x[8], 6, 0x6fa87e4f);
        II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
        II(c, d, a, b, x[6], 15, 0xa3014314);
        II(b, c, d, a, x[13], 21, 0x4e0811a1);
        II(a, b, c, d, x[4], 6, 0xf7537e82);
        II(d, a, b, c, x[11], 10, 0xbd3af235);
        II(c, d, a, b, x[2], 15, 0x2ad7d2bb);
        II(b, c, d, a, x[9], 21, 0xeb86d391);

        state_[0] += a;
        state_[1] += b;
        state_[2] += c;
        state_[3] += d;
    }

    void encode(const uint32_t* input, uint8_t* output, size_t len) {
        for (size_t i = 0, j = 0; j < len; i++, j += 4) {
            output[j] = input[i] & 0xff;
            output[j + 1] = (input[i] >> 8) & 0xff;
            output[j + 2] = (input[i] >> 16) & 0xff;
            output[j + 3] = (input[i] >> 24) & 0xff;
        }
    }

    void decode(const uint8_t* input, uint32_t* output, size_t len) {
        for (size_t i = 0, j = 0; j < len; i++, j += 4) {
            output[i] = input[j] | (input[j + 1] << 8) |
                        (input[j + 2] << 16) | (input[j + 3] << 24);
        }
    }

    static uint32_t rotateLeft(uint32_t x, uint32_t n) {
        return (x << n) | (x >> (32 - n));
    }

    static void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
        a += ((b & c) | (~b & d)) + x + t;
        a = rotateLeft(a, s);
        a += b;
    }

    static void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
        a += ((b & d) | (c & ~d)) + x + t;
        a = rotateLeft(a, s);
        a += b;
    }

    static void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
        a += (b ^ c ^ d) + x + t;
        a = rotateLeft(a, s);
        a += b;
    }

    static void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
        a += (c ^ (b | ~d)) + x + t;
        a = rotateLeft(a, s);
        a += b;
    }

    uint32_t state_[4];
    uint32_t count_[2];
    uint8_t buffer_[64];
};

class Base64 {
public:
    static std::string encode(const uint8_t* data, size_t len) {
        static const char* chars = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
        std::string result;
        result.reserve(((len + 2) / 3) * 4);

        for (size_t i = 0; i < len; i += 3) {
            uint32_t n = static_cast<uint32_t>(data[i]) << 16;
            if (i + 1 < len) n |= static_cast<uint32_t>(data[i + 1]) << 8;
            if (i + 2 < len) n |= static_cast<uint32_t>(data[i + 2]);

            result += chars[(n >> 18) & 0x3F];
            result += chars[(n >> 12) & 0x3F];
            result += (i + 1 < len) ? chars[(n >> 6) & 0x3F] : '=';
            result += (i + 2 < len) ? chars[n & 0x3F] : '=';
        }

        return result;
    }

    static std::string encode(const std::string& data) {
        return encode(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    }

    static std::string encode(const std::vector<uint8_t>& data) {
        return encode(data.data(), data.size());
    }

    static std::vector<uint8_t> decode(const std::string& encoded) {
        static const int decodeTable[256] = {
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
            52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
            -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
            15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
            -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
            41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
        };

        std::vector<uint8_t> result;
        result.reserve((encoded.size() * 3) / 4);

        uint32_t n = 0;
        int bits = 0;

        for (char c : encoded) {
            if (c == '=') break;
            int val = decodeTable[static_cast<uint8_t>(c)];
            if (val == -1) continue;

            n = (n << 6) | val;
            bits += 6;

            if (bits >= 8) {
                bits -= 8;
                result.push_back(static_cast<uint8_t>((n >> bits) & 0xFF));
            }
        }

        return result;
    }

    static std::string decodeToString(const std::string& encoded) {
        auto bytes = decode(encoded);
        return std::string(bytes.begin(), bytes.end());
    }
};

class CRC32 {
public:
    CRC32() : crc_(0xFFFFFFFF) {
        initTable();
    }

    void reset() {
        crc_ = 0xFFFFFFFF;
    }

    void update(const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            crc_ = table_[(crc_ ^ data[i]) & 0xFF] ^ (crc_ >> 8);
        }
    }

    void update(const std::string& data) {
        update(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    }

    void update(const std::vector<uint8_t>& data) {
        update(data.data(), data.size());
    }

    uint32_t finalize() {
        return crc_ ^ 0xFFFFFFFF;
    }

    static uint32_t compute(const uint8_t* data, size_t len) {
        CRC32 crc;
        crc.update(data, len);
        return crc.finalize();
    }

    static uint32_t compute(const std::string& data) {
        return compute(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    }

    static uint32_t compute(const std::vector<uint8_t>& data) {
        return compute(data.data(), data.size());
    }

private:
    void initTable() {
        static bool tableInitialized = false;
        if (tableInitialized) return;

        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t crc = i;
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
            }
            table_[i] = crc;
        }

        tableInitialized = true;
    }

    uint32_t crc_;
    static uint32_t table_[256];
};

uint32_t CRC32::table_[256] = {};

class XORCipher {
public:
    static std::string encrypt(const std::string& data, const std::string& key) {
        std::string result = data;
        if (key.empty()) return result;
        
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] ^= key[i % key.size()];
        }
        return result;
    }

    static std::string decrypt(const std::string& data, const std::string& key) {
        return encrypt(data, key);
    }

    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        std::vector<uint8_t> result = data;
        if (key.empty()) return result;
        
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] ^= key[i % key.size()];
        }
        return result;
    }

    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
        return encrypt(data, key);
    }
};

class RandomGenerator {
public:
    RandomGenerator(uint64_t seed = 0) : seed_(seed) {
        if (seed_ == 0) {
            seed_ = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
        }
    }

    void seed(uint64_t s) {
        seed_ = s;
    }

    uint64_t next() {
        seed_ ^= seed_ >> 12;
        seed_ ^= seed_ << 25;
        seed_ ^= seed_ >> 27;
        return seed_ * 0x2545F4914F6CDD1DULL;
    }

    uint32_t nextUInt32() {
        return static_cast<uint32_t>(next() >> 32);
    }

    int32_t nextInt32() {
        return static_cast<int32_t>(nextUInt32());
    }

    int32_t nextInt(int32_t min, int32_t max) {
        if (min >= max) return min;
        uint64_t range = static_cast<uint64_t>(max) - min;
        return min + static_cast<int32_t>(next() % range);
    }

    float nextFloat() {
        return static_cast<float>(nextUInt32()) / static_cast<float>(0xFFFFFFFF);
    }

    float nextFloat(float min, float max) {
        return min + nextFloat() * (max - min);
    }

    double nextDouble() {
        uint64_t x = next();
        return static_cast<double>(x >> 11) * (1.0 / 9007199254740992.0);
    }

    double nextDouble(double min, double max) {
        return min + nextDouble() * (max - min);
    }

    bool nextBool() {
        return (next() & 1) != 0;
    }

    void nextBytes(uint8_t* buffer, size_t len) {
        for (size_t i = 0; i < len; i += 8) {
            uint64_t n = next();
            size_t copy = std::min(size_t(8), len - i);
            memcpy(buffer + i, &n, copy);
        }
    }

    std::vector<uint8_t> nextBytes(size_t len) {
        std::vector<uint8_t> result(len);
        nextBytes(result.data(), len);
        return result;
    }

private:
    uint64_t seed_;
};

class UUIDGenerator {
public:
    UUIDGenerator() : rng_(std::chrono::steady_clock::now().time_since_epoch().count()) {}

    std::string generate() {
        uint8_t bytes[16];
        rng_.nextBytes(bytes, 16);

        bytes[6] = (bytes[6] & 0x0F) | 0x40;
        bytes[8] = (bytes[8] & 0x3F) | 0x80;

        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; ++i) {
            if (i == 4 || i == 6 || i == 8 || i == 10) ss << '-';
            ss << std::setw(2) << static_cast<uint32_t>(bytes[i]);
        }
        return ss.str();
    }

    std::vector<std::string> generate(size_t count) {
        std::vector<std::string> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(generate());
        }
        return result;
    }

private:
    RandomGenerator rng_;
};

}