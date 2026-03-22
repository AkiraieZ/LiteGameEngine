#pragma once

#include <glm/glm.hpp>

namespace LGE {

struct AABB {
    glm::vec3 Min;
    glm::vec3 Max;
    
    AABB() : Min(0.0f), Max(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : Min(min), Max(max) {}
    
    glm::vec3 GetCenter() const { return (Min + Max) * 0.5f; }
    glm::vec3 GetSize() const { return Max - Min; }
};

struct Sphere {
    glm::vec3 Center;
    float Radius;
    
    Sphere() : Center(0.0f), Radius(1.0f) {}
    Sphere(const glm::vec3& center, float radius) : Center(center), Radius(radius) {}
};

struct Plane {
    glm::vec3 Normal;
    float Distance;
    
    Plane() : Normal(0.0f, 1.0f, 0.0f), Distance(0.0f) {}
    Plane(const glm::vec3& normal, float distance) : Normal(normal), Distance(distance) {}
};

struct Ray {
    glm::vec3 Origin;
    glm::vec3 Direction;
    
    Ray() : Origin(0.0f), Direction(0.0f, 0.0f, -1.0f) {}
    Ray(const glm::vec3& origin, const glm::vec3& direction) : Origin(origin), Direction(direction) {}
};

struct HitInfo {
    bool Hit;
    float Distance;
    glm::vec3 Point;
    glm::vec3 Normal;
    
    HitInfo() : Hit(false), Distance(0.0f), Point(0.0f), Normal(0.0f) {}
};

class Collision {
public:
    static bool AABBvsAABB(const AABB& a, const AABB& b);
    static bool SpherevsSphere(const Sphere& a, const Sphere& b);
    static bool SpherevsAABB(const Sphere& sphere, const AABB& aabb);
    static bool RayvsAABB(const Ray& ray, const AABB& aabb, HitInfo& hitInfo);
    static bool RayvsSphere(const Ray& ray, const Sphere& sphere, HitInfo& hitInfo);
    static bool RayvsPlane(const Ray& ray, const Plane& plane, HitInfo& hitInfo);
    
    static AABB MergeAABB(const AABB& a, const AABB& b);
    static AABB TransformAABB(const AABB& aabb, const glm::mat4& transform);
};

}
