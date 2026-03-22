#include "Collision.h"
#include <algorithm>

namespace LGE {

bool Collision::AABBvsAABB(const AABB& a, const AABB& b) {
    return a.Min.x <= b.Max.x && a.Max.x >= b.Min.x &&
           a.Min.y <= b.Max.y && a.Max.y >= b.Min.y &&
           a.Min.z <= b.Max.z && a.Max.z >= b.Min.z;
}

bool Collision::SpherevsSphere(const Sphere& a, const Sphere& b) {
    float radiusSum = a.Radius + b.Radius;
    float distanceSq = glm::dot(a.Center - b.Center, a.Center - b.Center);
    return distanceSq <= radiusSum * radiusSum;
}

bool Collision::SpherevsAABB(const Sphere& sphere, const AABB& aabb) {
    glm::vec3 closestPoint;
    closestPoint.x = std::max(aabb.Min.x, std::min(sphere.Center.x, aabb.Max.x));
    closestPoint.y = std::max(aabb.Min.y, std::min(sphere.Center.y, aabb.Max.y));
    closestPoint.z = std::max(aabb.Min.z, std::min(sphere.Center.z, aabb.Max.z));
    
    float distanceSq = glm::dot(sphere.Center - closestPoint, sphere.Center - closestPoint);
    return distanceSq <= sphere.Radius * sphere.Radius;
}

bool Collision::RayvsAABB(const Ray& ray, const AABB& aabb, HitInfo& hitInfo) {
    float tMin = -1e30f;
    float tMax = 1e30f;
    
    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / ray.Direction[i];
        float t0 = (aabb.Min[i] - ray.Origin[i]) * invD;
        float t1 = (aabb.Max[i] - ray.Origin[i]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        tMin = std::max(t0, tMin);
        tMax = std::min(t1, tMax);
        if (tMax < tMin) return false;
    }
    
    hitInfo.Hit = true;
    hitInfo.Distance = tMin;
    hitInfo.Point = ray.Origin + ray.Direction * tMin;
    
    glm::vec3 center = aabb.GetCenter();
    glm::vec3 diff = hitInfo.Point - center;
    glm::vec3 size = aabb.GetSize() * 0.5f;
    
    float minDist = std::abs(size.x - std::abs(diff.x));
    hitInfo.Normal = glm::vec3(diff.x > 0 ? 1 : -1, 0, 0);
    
    float dist = std::abs(size.y - std::abs(diff.y));
    if (dist < minDist) {
        minDist = dist;
        hitInfo.Normal = glm::vec3(0, diff.y > 0 ? 1 : -1, 0);
    }
    
    dist = std::abs(size.z - std::abs(diff.z));
    if (dist < minDist) {
        hitInfo.Normal = glm::vec3(0, 0, diff.z > 0 ? 1 : -1);
    }
    
    return true;
}

bool Collision::RayvsSphere(const Ray& ray, const Sphere& sphere, HitInfo& hitInfo) {
    glm::vec3 oc = ray.Origin - sphere.Center;
    float a = glm::dot(ray.Direction, ray.Direction);
    float b = 2.0f * glm::dot(oc, ray.Direction);
    float c = glm::dot(oc, oc) - sphere.Radius * sphere.Radius;
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) {
        hitInfo.Hit = false;
        return false;
    }
    
    float sqrtd = std::sqrt(discriminant);
    float t = (-b - sqrtd) / (2.0f * a);
    if (t < 0) {
        t = (-b + sqrtd) / (2.0f * a);
        if (t < 0) {
            hitInfo.Hit = false;
            return false;
        }
    }
    
    hitInfo.Hit = true;
    hitInfo.Distance = t;
    hitInfo.Point = ray.Origin + ray.Direction * t;
    hitInfo.Normal = glm::normalize(hitInfo.Point - sphere.Center);
    return true;
}

bool Collision::RayvsPlane(const Ray& ray, const Plane& plane, HitInfo& hitInfo) {
    float denom = glm::dot(plane.Normal, ray.Direction);
    if (std::abs(denom) < 1e-6f) {
        hitInfo.Hit = false;
        return false;
    }
    
    float t = (plane.Distance - glm::dot(plane.Normal, ray.Origin)) / denom;
    if (t < 0) {
        hitInfo.Hit = false;
        return false;
    }
    
    hitInfo.Hit = true;
    hitInfo.Distance = t;
    hitInfo.Point = ray.Origin + ray.Direction * t;
    hitInfo.Normal = denom > 0 ? -plane.Normal : plane.Normal;
    return true;
}

AABB Collision::MergeAABB(const AABB& a, const AABB& b) {
    return AABB(
        glm::vec3(std::min(a.Min.x, b.Min.x), std::min(a.Min.y, b.Min.y), std::min(a.Min.z, b.Min.z)),
        glm::vec3(std::max(a.Max.x, b.Max.x), std::max(a.Max.y, b.Max.y), std::max(a.Max.z, b.Max.z))
    );
}

AABB Collision::TransformAABB(const AABB& aabb, const glm::mat4& transform) {
    glm::vec3 corners[8] = {
        glm::vec3(aabb.Min.x, aabb.Min.y, aabb.Min.z),
        glm::vec3(aabb.Max.x, aabb.Min.y, aabb.Min.z),
        glm::vec3(aabb.Min.x, aabb.Max.y, aabb.Min.z),
        glm::vec3(aabb.Max.x, aabb.Max.y, aabb.Min.z),
        glm::vec3(aabb.Min.x, aabb.Min.y, aabb.Max.z),
        glm::vec3(aabb.Max.x, aabb.Min.y, aabb.Max.z),
        glm::vec3(aabb.Min.x, aabb.Max.y, aabb.Max.z),
        glm::vec3(aabb.Max.x, aabb.Max.y, aabb.Max.z)
    };
    
    AABB result;
    result.Min = glm::vec3(1e30f);
    result.Max = glm::vec3(-1e30f);
    
    for (int i = 0; i < 8; i++) {
        glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
        result.Min = glm::min(result.Min, glm::vec3(transformed));
        result.Max = glm::max(result.Max, glm::vec3(transformed));
    }
    
    return result;
}

}
