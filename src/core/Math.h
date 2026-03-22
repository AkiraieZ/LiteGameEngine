#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <random>
#include <limits>

namespace LGE {

namespace Math {

constexpr float PI = 3.14159265358979323846f;
constexpr float HALF_PI = PI / 2.0f;
constexpr float TWO_PI = PI * 2.0f;
constexpr float DEG2RAD = PI / 180.0f;
constexpr float RAD2DEG = 180.0f / PI;
constexpr float EPSILON = std::numeric_limits<float>::epsilon();

inline bool FloatEquals(float a, float b, float epsilon = EPSILON) {
    return std::abs(a - b) <= epsilon;
}

inline glm::mat4 ComposeTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    transform = glm::scale(transform, scale);
    return transform;
}

inline glm::mat4 ComposeTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    transform = transform * glm::toMat4(rotation);
    transform = glm::scale(transform, scale);
    return transform;
}

inline void DecomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) {
    position = glm::vec3(transform[3]);
    scale.x = glm::length(glm::vec3(transform[0]));
    scale.y = glm::length(glm::vec3(transform[1]));
    scale.z = glm::length(glm::vec3(transform[2]));
    glm::mat3 rotationMatrix = glm::mat3(transform);
    rotationMatrix[0] /= scale.x;
    rotationMatrix[1] /= scale.y;
    rotationMatrix[2] /= scale.z;
    glm::quat quat = glm::quat(rotationMatrix);
    rotation = glm::eulerAngles(quat) * RAD2DEG;
}

inline glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
    return a + (b - a) * t;
}

inline glm::vec4 Lerp(const glm::vec4& a, const glm::vec4& b, float t) {
    return a + (b - a) * t;
}

inline glm::quat Slerp(const glm::quat& a, const glm::quat& b, float t) {
    return glm::slerp(a, b, t);
}

inline float Clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

inline int Clamp(int value, int min, int max) {
    return std::max(min, std::min(max, value));
}

inline glm::vec3 Clamp(const glm::vec3& value, const glm::vec3& min, const glm::vec3& max) {
    return glm::vec3(
        Clamp(value.x, min.x, max.x),
        Clamp(value.y, min.y, max.y),
        Clamp(value.z, min.z, max.z)
    );
}

inline float SmoothStep(float edge0, float edge1, float x) {
    float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float SmootherStep(float edge0, float edge1, float x) {
    float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline glm::vec3 Reflect(const glm::vec3& vector, const glm::vec3& normal) {
    return vector - 2.0f * glm::dot(vector, normal) * normal;
}

inline glm::vec3 Refract(const glm::vec3& vector, const glm::vec3& normal, float eta) {
    float dot = glm::dot(vector, normal);
    float k = 1.0f - eta * eta * (1.0f - dot * dot);
    if (k < 0.0f) return glm::vec3(0.0f);
    return eta * vector - (eta * dot + std::sqrt(k)) * normal;
}

inline float Distance(const glm::vec3& a, const glm::vec3& b) {
    return glm::distance(a, b);
}

inline float DistanceSquared(const glm::vec3& a, const glm::vec3& b) {
    return glm::dot(a - b, a - b);
}

inline float Length(const glm::vec3& v) {
    return glm::length(v);
}

inline float LengthSquared(const glm::vec3& v) {
    return glm::dot(v, v);
}

inline glm::vec3 Normalize(const glm::vec3& v) {
    return glm::normalize(v);
}

inline float Dot(const glm::vec3& a, const glm::vec3& b) {
    return glm::dot(a, b);
}

inline glm::vec3 Cross(const glm::vec3& a, const glm::vec3& b) {
    return glm::cross(a, b);
}

inline glm::vec3 Project(const glm::vec3& vector, const glm::vec3& onto) {
    return onto * Dot(vector, onto) / LengthSquared(onto);
}

inline glm::vec3 Reject(const glm::vec3& vector, const glm::vec3& onto) {
    return vector - Project(vector, onto);
}

inline bool PointInSphere(const glm::vec3& point, const glm::vec3& center, float radius) {
    return DistanceSquared(point, center) <= radius * radius;
}

inline bool PointInAABB(const glm::vec3& point, const glm::vec3& min, const glm::vec3& max) {
    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

inline glm::vec3 ClosestPointOnLine(const glm::vec3& point, const glm::vec3& lineStart, const glm::vec3& lineEnd) {
    glm::vec3 line = lineEnd - lineStart;
    float t = Clamp(Dot(point - lineStart, line) / LengthSquared(line), 0.0f, 1.0f);
    return lineStart + line * t;
}

inline float DegreesToRadians(float degrees) {
    return degrees * DEG2RAD;
}

inline float RadiansToDegrees(float radians) {
    return radians * RAD2DEG;
}

inline glm::vec3 DegreesToRadians(const glm::vec3& degrees) {
    return degrees * DEG2RAD;
}

inline glm::vec3 RadiansToDegrees(const glm::vec3& radians) {
    return radians * RAD2DEG;
}

}

class Random {
public:
    static void Init(unsigned int seed = 0);
    
    static float Float();
    static float Float(float min, float max);
    static int Int(int min, int max);
    static glm::vec3 Vec3();
    static glm::vec3 Vec3(const glm::vec3& min, const glm::vec3& max);
    static glm::vec3 InUnitSphere();
    static glm::vec3 UnitVector();
    static glm::vec2 InUnitCircle();
    static glm::vec2 UnitVector2D();
    static bool Bool();
    
private:
    static std::mt19937 s_RNG;
    static std::uniform_real_distribution<float> s_FloatDist;
    static std::uniform_int_distribution<int> s_IntDist;
};

}
