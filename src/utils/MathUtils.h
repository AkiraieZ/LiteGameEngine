#pragma once

#include <cmath>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <random>
#include <chrono>

namespace LGE {
namespace Utils {
namespace Math {

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = PI * 2.0f;
constexpr float HALF_PI = PI * 0.5f;
constexpr float QUARTER_PI = PI * 0.25f;
constexpr float INV_PI = 1.0f / PI;
constexpr float INV_TWO_PI = 1.0f / TWO_PI;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;
constexpr float EPSILON = 1e-6f;
constexpr float FLOAT_EPSILON = std::numeric_limits<float>::epsilon();

inline float degToRad(float degrees) {
    return degrees * DEG_TO_RAD;
}

inline float radToDeg(float radians) {
    return radians * RAD_TO_DEG;
}

inline float clamp(float value, float minVal, float maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

inline int clamp(int value, int minVal, int maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

inline float saturate(float value) {
    return clamp(value, 0.0f, 1.0f);
}

inline float sign(float value) {
    return value > 0.0f ? 1.0f : (value < 0.0f ? -1.0f : 0.0f);
}

inline float lerp(float a, float b, float t) {
    return a + (b - a) * clamp(t, 0.0f, 1.0f);
}

inline float inverseLerp(float a, float b, float value) {
    if (a == b) return 0.0f;
    return (value - a) / (b - a);
}

inline float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float smootherstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline float pingPong(float t, float length) {
    t = std::fmod(t, length * 2.0f);
    return length - std::fabs(t - length);
}

inline float repeat(float t, float length) {
    return t - std::floor(t / length) * length;
}

inline float deltaAngle(float current, float target) {
    float delta = repeat(target - current, 360.0f);
    if (delta > 180.0f) delta -= 360.0f;
    return delta;
}

inline float moveTowards(float current, float target, float maxDelta) {
    if (std::fabs(target - current) <= maxDelta) {
        return target;
    }
    return current + sign(target - current) * maxDelta;
}

inline float moveTowardsAngle(float current, float target, float maxDelta) {
    float delta = deltaAngle(current, target);
    return current + clamp(delta, -maxDelta, maxDelta);
}

inline bool approximately(float a, float b, float epsilon = EPSILON) {
    return std::fabs(a - b) <= epsilon;
}

inline bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

inline int nextPowerOfTwo(int n) {
    if (n <= 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

inline int roundToNearest(float value) {
    return static_cast<int>(std::round(value));
}

inline int floorToInt(float value) {
    return static_cast<int>(std::floor(value));
}

inline int ceilToInt(float value) {
    return static_cast<int>(std::ceil(value));
}

inline float fract(float value) {
    return value - std::floor(value);
}

inline float min3(float a, float b, float c) {
    return std::min(std::min(a, b), c);
}

inline float max3(float a, float b, float c) {
    return std::max(std::max(a, b), c);
}

inline float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

inline float hermite(float value0, float tangent0, float value1, float tangent1, float t) {
    t = clamp(t, 0.0f, 1.0f);
    float t2 = t * t;
    float t3 = t2 * t;
    return (2.0f * t3 - 3.0f * t2 + 1.0f) * value0 +
           (t3 - 2.0f * t2 + t) * tangent0 +
           (-2.0f * t3 + 3.0f * t2) * value1 +
           (t3 - t2) * tangent1;
}

inline float catmullRom(float p0, float p1, float p2, float p3, float t) {
    t = clamp(t, 0.0f, 1.0f);
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * ((2.0f * p1) +
                  (-p0 + p2) * t +
                  (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                  (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

inline float easeIn(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t * t;
}

inline float easeOut(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t * (2.0f - t);
}

inline float easeInOut(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

inline float easeInCubic(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t * t * t;
}

inline float easeOutCubic(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return 1.0f - std::pow(1.0f - t, 3.0f);
}

inline float easeInOutCubic(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

inline float easeInQuad(float t) {
    return easeIn(t);
}

inline float easeOutQuad(float t) {
    return easeOut(t);
}

inline float easeInOutQuad(float t) {
    return easeInOut(t);
}

inline float easeInQuart(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t * t * t * t;
}

inline float easeOutQuart(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return 1.0f - std::pow(1.0f - t, 4.0f);
}

inline float easeInOutQuart(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}

inline float easeInQuint(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t * t * t * t * t;
}

inline float easeOutQuint(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return 1.0f - std::pow(1.0f - t, 5.0f);
}

inline float easeInOutQuint(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
}

inline float easeInSine(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return 1.0f - std::cos(t * HALF_PI);
}

inline float easeOutSine(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return std::sin(t * HALF_PI);
}

inline float easeInOutSine(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return -(std::cos(PI * t) - 1.0f) / 2.0f;
}

inline float easeInExpo(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
}

inline float easeOutExpo(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}

inline float easeInOutExpo(float t) {
    t = clamp(t, 0.0f, 1.0f);
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return t < 0.5f ?
           std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f :
           (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

inline float easeInCirc(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return 1.0f - std::sqrt(1.0f - t * t);
}

inline float easeOutCirc(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
}

inline float easeInOutCirc(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t < 0.5f ?
           (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f :
           (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

inline float easeInBack(float t, float c1 = 1.70158f) {
    t = clamp(t, 0.0f, 1.0f);
    float c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
}

inline float easeOutBack(float t, float c1 = 1.70158f) {
    t = clamp(t, 0.0f, 1.0f);
    float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}

inline float easeInOutBack(float t, float c1 = 1.70158f) {
    t = clamp(t, 0.0f, 1.0f);
    float c2 = c1 * 1.525f;
    return t < 0.5f ?
           (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f :
           (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

inline float easeInElastic(float t) {
    t = clamp(t, 0.0f, 1.0f);
    const float c4 = TWO_PI / 3.0f;
    return t == 0.0f ? 0.0f :
           t == 1.0f ? 1.0f :
           -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
}

inline float easeOutElastic(float t) {
    t = clamp(t, 0.0f, 1.0f);
    const float c4 = TWO_PI / 3.0f;
    return t == 0.0f ? 0.0f :
           t == 1.0f ? 1.0f :
           std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
}

inline float easeInOutElastic(float t) {
    t = clamp(t, 0.0f, 1.0f);
    const float c5 = TWO_PI / 4.5f;
    return t == 0.0f ? 0.0f :
           t == 1.0f ? 1.0f :
           t < 0.5f ?
           -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f :
           (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
}

inline float easeInBounce(float t) {
    return 1.0f - easeOutBounce(1.0f - t);
}

inline float easeOutBounce(float t) {
    t = clamp(t, 0.0f, 1.0f);
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    
    if (t < 1.0f / d1) {
        return n1 * t * t;
    } else if (t < 2.0f / d1) {
        return n1 * (t -= 1.5f / d1) * t + 0.75f;
    } else if (t < 2.5f / d1) {
        return n1 * (t -= 2.25f / d1) * t + 0.9375f;
    } else {
        return n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }
}

inline float easeInOutBounce(float t) {
    t = clamp(t, 0.0f, 1.0f);
    return t < 0.5f ?
           (1.0f - easeOutBounce(1.0f - 2.0f * t)) / 2.0f :
           (1.0f + easeOutBounce(2.0f * t - 1.0f)) / 2.0f;
}

class PerlinNoise {
public:
    PerlinNoise(uint64_t seed = 0) {
        if (seed == 0) {
            seed = std::chrono::steady_clock::now().time_since_epoch().count();
        }
        std::mt19937_64 rng(seed);
        for (int i = 0; i < 256; ++i) {
            permutation_[i] = i;
        }
        for (int i = 255; i > 0; --i) {
            std::uniform_int_distribution<int> dist(0, i);
            int j = dist(rng);
            std::swap(permutation_[i], permutation_[j]);
        }
        for (int i = 0; i < 256; ++i) {
            permutation_[256 + i] = permutation_[i];
        }
    }

    float noise(float x, float y = 0.0f, float z = 0.0f) const {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        int Z = static_cast<int>(std::floor(z)) & 255;

        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);

        float u = fade(x);
        float v = fade(y);
        float w = fade(z);

        int A = permutation_[X] + Y;
        int AA = permutation_[A] + Z;
        int AB = permutation_[A + 1] + Z;
        int B = permutation_[X + 1] + Y;
        int BA = permutation_[B] + Z;
        int BB = permutation_[B + 1] + Z;

        return lerp(
            lerp(
                lerp(grad(permutation_[AA], x, y, z), grad(permutation_[BA], x - 1, y, z), u),
                lerp(grad(permutation_[AB], x, y - 1, z), grad(permutation_[BB], x - 1, y - 1, z), u),
                v
            ),
            lerp(
                lerp(grad(permutation_[AA + 1], x, y, z - 1), grad(permutation_[BA + 1], x - 1, y, z - 1), u),
                lerp(grad(permutation_[AB + 1], x, y - 1, z - 1), grad(permutation_[BB + 1], x - 1, y - 1, z - 1), u),
                v
            ),
            w
        );
    }

    float octaveNoise(float x, float y, float z, int octaves, float persistence = 0.5f) const {
        float result = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            result += noise(x * frequency, y * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2.0f;
        }

        return result / maxValue;
    }

private:
    float fade(float t) const {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    float grad(int hash, float x, float y, float z) const {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    int permutation_[512];
};

class Random {
public:
    Random(uint64_t seed = 0) : rng_(seed ? seed : std::chrono::steady_clock::now().time_since_epoch().count()) {}

    void seed(uint64_t s) {
        rng_.seed(s);
    }

    float nextFloat() {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(rng_);
    }

    float nextFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng_);
    }

    int nextInt() {
        std::uniform_int_distribution<int> dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        return dist(rng_);
    }

    int nextInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng_);
    }

    bool nextBool() {
        return nextFloat() > 0.5f;
    }

    float nextGaussian(float mean = 0.0f, float stddev = 1.0f) {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(rng_);
    }

    float unitCircle(float& x, float& y) {
        float angle = nextFloat(0.0f, TWO_PI);
        x = std::cos(angle);
        y = std::sin(angle);
        return angle;
    }

    float insideUnitCircle(float& x, float& y) {
        float r = std::sqrt(nextFloat());
        float theta = nextFloat(0.0f, TWO_PI);
        x = r * std::cos(theta);
        y = r * std::sin(theta);
        return r;
    }

    void unitSphere(float& x, float& y, float& z) {
        float theta = nextFloat(0.0f, TWO_PI);
        float phi = std::acos(2.0f * nextFloat() - 1.0f);
        float sinPhi = std::sin(phi);
        x = sinPhi * std::cos(theta);
        y = sinPhi * std::sin(theta);
        z = std::cos(phi);
    }

    void insideUnitSphere(float& x, float& y, float& z) {
        float u = nextFloat();
        float v = nextFloat();
        float theta = u * TWO_PI;
        float phi = std::acos(2.0f * v - 1.0f);
        float r = std::cbrt(nextFloat());
        float sinPhi = std::sin(phi);
        x = r * sinPhi * std::cos(theta);
        y = r * sinPhi * std::sin(theta);
        z = r * std::cos(phi);
    }

    template<typename T>
    const T& choice(const std::vector<T>& items) {
        int index = nextInt(0, static_cast<int>(items.size()) - 1);
        return items[index];
    }

    template<typename T>
    void shuffle(std::vector<T>& items) {
        for (size_t i = items.size() - 1; i > 0; --i) {
            size_t j = nextInt(0, static_cast<int>(i));
            std::swap(items[i], items[j]);
        }
    }

    template<typename T>
    std::vector<T> sample(const std::vector<T>& items, size_t count) {
        std::vector<T> result = items;
        shuffle(result);
        if (count < result.size()) {
            result.resize(count);
        }
        return result;
    }

    float range(float start, float end) {
        return nextFloat(start, end);
    }

    int range(int start, int end) {
        return nextInt(start, end);
    }

private:
    std::mt19937_64 rng_;
};

inline float remap(float value, float inMin, float inMax, float outMin, float outMax) {
    float t = (value - inMin) / (inMax - inMin);
    return lerp(outMin, outMax, t);
}

inline float remapClamped(float value, float inMin, float inMax, float outMin, float outMax) {
    float t = clamp((value - inMin) / (inMax - inMin), 0.0f, 1.0f);
    return lerp(outMin, outMax, t);
}

inline int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

inline int lcm(int a, int b) {
    if (a == 0 || b == 0) return 0;
    return (a / gcd(a, b)) * b;
}

inline bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

inline int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

inline float gamma(float x) {
    if (x <= 0.0f) return 0.0f;
    if (x < 1.0f) return gamma(x + 1.0f) / x;
    float lnGamma = (x - 0.5f) * std::log(x) - x + 0.5f * std::log(TWO_PI);
    for (int k = 0; k < 5; ++k) {
        x += 1.0f;
        lnGamma += 1.0f / (12.0f * x);
    }
    return std::exp(lnGamma);
}

inline float beta(float a, float b) {
    return std::exp(std::lgamma(a) + std::lgamma(b) - std::lgamma(a + b));
}

template<typename T>
inline T interpolate(const T& a, const T& b, float t) {
    return a * (1.0f - t) + b * t;
}

template<typename T>
inline T interpolate(const T& a, const T& b, const T& c, const T& d, float t) {
    T ab = interpolate(a, b, t);
    T bc = interpolate(b, c, t);
    T cd = interpolate(c, d, t);
    T abc = interpolate(ab, bc, t);
    T bcd = interpolate(bc, cd, t);
    return interpolate(abc, bcd, t);
}

inline float angleDifference(float a, float b) {
    float diff = std::fmod(b - a, 360.0f);
    if (diff > 180.0f) diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;
    return diff;
}

inline float shortestAngle(float from, float to) {
    float diff = to - from;
    diff = std::fmod(diff + 180.0f, 360.0f) - 180.0f;
    return diff;
}

inline int wrap(int value, int min, int max) {
    int range = max - min + 1;
    if (value < min) {
        value += range * ((min - value) / range + 1);
    }
    return min + (value - min) % range;
}

inline float wrap(float value, float min, float max) {
    float range = max - min;
    if (range == 0.0f) return min;
    return min + std::fmod(value - min, range);
}

inline float spring(float current, float target, float& velocity, float damping, float stiffness, float dt) {
    float force = (target - current) * stiffness;
    float acceleration = force - velocity * damping;
    velocity += acceleration * dt;
    return current + velocity * dt;
}

class PIDController {
public:
    PIDController(float kp, float ki, float kd) 
        : kp_(kp), ki_(ki), kd_(kd), integral_(0.0f), prevError_(0.0f) {}

    void setGains(float kp, float ki, float kd) {
        kp_ = kp;
        ki_ = ki;
        kd_ = kd;
    }

    void reset() {
        integral_ = 0.0f;
        prevError_ = 0.0f;
    }

    float update(float setpoint, float measured, float dt) {
        float error = setpoint - measured;
        integral_ += error * dt;
        float derivative = (error - prevError_) / dt;
        prevError_ = error;
        return kp_ * error + ki_ * integral_ + kd_ * derivative;
    }

private:
    float kp_, ki_, kd_;
    float integral_;
    float prevError_;
};

class LowPassFilter {
public:
    LowPassFilter(float alpha = 0.1f) : alpha_(alpha), value_(0.0f), initialized_(false) {}

    void setAlpha(float alpha) {
        alpha_ = clamp(alpha, 0.0f, 1.0f);
    }

    void reset(float initialValue = 0.0f) {
        value_ = initialValue;
        initialized_ = true;
    }

    float update(float newValue) {
        if (!initialized_) {
            value_ = newValue;
            initialized_ = true;
        } else {
            value_ = alpha_ * newValue + (1.0f - alpha_) * value_;
        }
        return value_;
    }

    float value() const { return value_; }

private:
    float alpha_;
    float value_;
    bool initialized_;
};

class HighPassFilter {
public:
    HighPassFilter(float alpha = 0.1f) : alpha_(alpha), prevInput_(0.0f), prevOutput_(0.0f), initialized_(false) {}

    void setAlpha(float alpha) {
        alpha_ = clamp(alpha, 0.0f, 1.0f);
    }

    void reset(float initialValue = 0.0f) {
        prevInput_ = initialValue;
        prevOutput_ = initialValue;
        initialized_ = true;
    }

    float update(float newValue) {
        if (!initialized_) {
            prevInput_ = newValue;
            prevOutput_ = newValue;
            initialized_ = true;
            return 0.0f;
        }
        float output = alpha_ * (prevOutput_ + newValue - prevInput_);
        prevInput_ = newValue;
        prevOutput_ = output;
        return output;
    }

private:
    float alpha_;
    float prevInput_;
    float prevOutput_;
    bool initialized_;
};

class MovingAverage {
public:
    MovingAverage(size_t windowSize = 10) : windowSize_(windowSize) {}

    void setWindowSize(size_t size) {
        windowSize_ = size;
        while (values_.size() > windowSize_) {
            sum_ -= values_.front();
            values_.pop_front();
        }
    }

    void reset() {
        values_.clear();
        sum_ = 0.0f;
    }

    float update(float newValue) {
        values_.push_back(newValue);
        sum_ += newValue;
        if (values_.size() > windowSize_) {
            sum_ -= values_.front();
            values_.pop_front();
        }
        return sum_ / static_cast<float>(values_.size());
    }

    float average() const {
        return values_.empty() ? 0.0f : sum_ / static_cast<float>(values_.size());
    }

    size_t count() const {
        return values_.size();
    }

private:
    size_t windowSize_;
    std::deque<float> values_;
    float sum_ = 0.0f;
};

}