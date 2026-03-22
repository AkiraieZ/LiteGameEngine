#include "MathUtils.h"

namespace LGE {
namespace Utils {
namespace Math {

constexpr float PI_CONSTANT = 3.14159265358979323846f;
constexpr float TWO_PI_CONSTANT = PI_CONSTANT * 2.0f;
constexpr float HALF_PI_CONSTANT = PI_CONSTANT * 0.5f;
constexpr float QUARTER_PI_CONSTANT = PI_CONSTANT * 0.25f;
constexpr float INV_PI_CONSTANT = 1.0f / PI_CONSTANT;
constexpr float INV_TWO_PI_CONSTANT = 1.0f / TWO_PI_CONSTANT;
constexpr float DEG_TO_RAD_CONSTANT = PI_CONSTANT / 180.0f;
constexpr float RAD_TO_DEG_CONSTANT = 180.0f / PI_CONSTANT;
constexpr float EPSILON_CONSTANT = 1e-6f;

float degToRadImpl(float degrees) {
    return degrees * DEG_TO_RAD_CONSTANT;
}

float radToDegImpl(float radians) {
    return radians * RAD_TO_DEG_CONSTANT;
}

float clampImpl(float value, float minVal, float maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

int clampImpl(int value, int minVal, int maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

float saturateImpl(float value) {
    return clampImpl(value, 0.0f, 1.0f);
}

float signImpl(float value) {
    return value > 0.0f ? 1.0f : (value < 0.0f ? -1.0f : 0.0f);
}

float lerpImpl(float a, float b, float t) {
    return a + (b - a) * clampImpl(t, 0.0f, 1.0f);
}

float inverseLerpImpl(float a, float b, float value) {
    if (a == b) return 0.0f;
    return (value - a) / (b - a);
}

float smoothstepImpl(float edge0, float edge1, float x) {
    float t = clampImpl((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float smootherstepImpl(float edge0, float edge1, float x) {
    float t = clampImpl((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float pingPongImpl(float t, float length) {
    t = std::fmod(t, length * 2.0f);
    return length - std::fabs(t - length);
}

float repeatImpl(float t, float length) {
    return t - std::floor(t / length) * length;
}

float deltaAngleImpl(float current, float target) {
    float delta = repeatImpl(target - current, 360.0f);
    if (delta > 180.0f) delta -= 360.0f;
    return delta;
}

float moveTowardsImpl(float current, float target, float maxDelta) {
    if (std::fabs(target - current) <= maxDelta) {
        return target;
    }
    return current + signImpl(target - current) * maxDelta;
}

float moveTowardsAngleImpl(float current, float target, float maxDelta) {
    float delta = deltaAngleImpl(current, target);
    return current + clampImpl(delta, -maxDelta, maxDelta);
}

bool approximatelyImpl(float a, float b, float epsilon) {
    return std::fabs(a - b) <= epsilon;
}

bool isPowerOfTwoImpl(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

int nextPowerOfTwoImpl(int n) {
    if (n <= 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

int roundToNearestImpl(float value) {
    return static_cast<int>(std::round(value));
}

int floorToIntImpl(float value) {
    return static_cast<int>(std::floor(value));
}

int ceilToIntImpl(float value) {
    return static_cast<int>(std::ceil(value));
}

float fractImpl(float value) {
    return value - std::floor(value);
}

float min3Impl(float a, float b, float c) {
    return std::min(std::min(a, b), c);
}

float max3Impl(float a, float b, float c) {
    return std::max(std::max(a, b), c);
}

float clamp01Impl(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

float hermiteImpl(float value0, float tangent0, float value1, float tangent1, float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    float t2 = t * t;
    float t3 = t2 * t;
    return (2.0f * t3 - 3.0f * t2 + 1.0f) * value0 +
           (t3 - 2.0f * t2 + t) * tangent0 +
           (-2.0f * t3 + 3.0f * t2) * value1 +
           (t3 - t2) * tangent1;
}

float catmullRomImpl(float p0, float p1, float p2, float p3, float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * ((2.0f * p1) +
                  (-p0 + p2) * t +
                  (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                  (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

float easeInImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t * t;
}

float easeOutImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t * (2.0f - t);
}

float easeInOutImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

float easeInCubicImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t * t * t;
}

float easeOutCubicImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return 1.0f - std::pow(1.0f - t, 3.0f);
}

float easeInOutCubicImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

float easeInQuadImpl(float t) {
    return easeInImpl(t);
}

float easeOutQuadImpl(float t) {
    return easeOutImpl(t);
}

float easeInOutQuadImpl(float t) {
    return easeInOutImpl(t);
}

float easeInQuartImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t * t * t * t;
}

float easeOutQuartImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return 1.0f - std::pow(1.0f - t, 4.0f);
}

float easeInOutQuartImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}

float easeInQuintImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t * t * t * t * t;
}

float easeOutQuintImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return 1.0f - std::pow(1.0f - t, 5.0f);
}

float easeInOutQuintImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
}

float easeInSineImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return 1.0f - std::cos(t * HALF_PI_CONSTANT);
}

float easeOutSineImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return std::sin(t * HALF_PI_CONSTANT);
}

float easeInOutSineImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return -(std::cos(PI_CONSTANT * t) - 1.0f) / 2.0f;
}

float easeInExpoImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
}

float easeOutExpoImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}

float easeInOutExpoImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return t < 0.5f ?
           std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f :
           (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

float easeInCircImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return 1.0f - std::sqrt(1.0f - t * t);
}

float easeOutCircImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
}

float easeInOutCircImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t < 0.5f ?
           (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) / 2.0f :
           (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

float easeInBackImpl(float t, float c1) {
    t = clampImpl(t, 0.0f, 1.0f);
    float c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
}

float easeOutBackImpl(float t, float c1) {
    t = clampImpl(t, 0.0f, 1.0f);
    float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}

float easeInOutBackImpl(float t, float c1) {
    t = clampImpl(t, 0.0f, 1.0f);
    float c2 = c1 * 1.525f;
    return t < 0.5f ?
           (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f :
           (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

float easeInBounceImpl(float t) {
    return 1.0f - easeOutBounceImpl(1.0f - t);
}

float easeOutBounceImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
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

float easeInOutBounceImpl(float t) {
    t = clampImpl(t, 0.0f, 1.0f);
    return t < 0.5f ?
           (1.0f - easeOutBounceImpl(1.0f - 2.0f * t)) / 2.0f :
           (1.0f + easeOutBounceImpl(2.0f * t - 1.0f)) / 2.0f;
}

float remapImpl(float value, float inMin, float inMax, float outMin, float outMax) {
    float t = (value - inMin) / (inMax - inMin);
    return lerpImpl(outMin, outMax, t);
}

float remapClampedImpl(float value, float inMin, float inMax, float outMin, float outMax) {
    float t = clampImpl((value - inMin) / (inMax - inMin), 0.0f, 1.0f);
    return lerpImpl(outMin, outMax, t);
}

int gcdImpl(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int lcmImpl(int a, int b) {
    if (a == 0 || b == 0) return 0;
    return (a / gcdImpl(a, b)) * b;
}

bool isPrimeImpl(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

int factorialImpl(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

float gammaImpl(float x) {
    if (x <= 0.0f) return 0.0f;
    if (x < 1.0f) return gammaImpl(x + 1.0f) / x;
    float lnGamma = (x - 0.5f) * std::log(x) - x + 0.5f * std::log(TWO_PI_CONSTANT);
    for (int k = 0; k < 5; ++k) {
        x += 1.0f;
        lnGamma += 1.0f / (12.0f * x);
    }
    return std::exp(lnGamma);
}

float betaImpl(float a, float b) {
    return std::exp(std::lgamma(a) + std::lgamma(b) - std::lgamma(a + b));
}

float angleDifferenceImpl(float a, float b) {
    float diff = std::fmod(b - a, 360.0f);
    if (diff > 180.0f) diff -= 360.0f;
    if (diff < -180.0f) diff += 360.0f;
    return diff;
}

float shortestAngleImpl(float from, float to) {
    float diff = to - from;
    diff = std::fmod(diff + 180.0f, 360.0f) - 180.0f;
    return diff;
}

int wrapImpl(int value, int min, int max) {
    int range = max - min + 1;
    if (value < min) {
        value += range * ((min - value) / range + 1);
    }
    return min + (value - min) % range;
}

float wrapImpl(float value, float min, float max) {
    float range = max - min;
    if (range == 0.0f) return min;
    return min + std::fmod(value - min, range);
}

float springImpl(float current, float target, float& velocity, float damping, float stiffness, float dt) {
    float force = (target - current) * stiffness;
    float acceleration = force - velocity * damping;
    velocity += acceleration * dt;
    return current + velocity * dt;
}

}