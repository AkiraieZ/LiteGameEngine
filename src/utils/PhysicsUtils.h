#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <cmath>

namespace LGE {
namespace Utils {

class PhysicsUtils {
public:
    static constexpr float GRAVITY = 9.81f;
    static constexpr float AIR_DENSITY = 1.225f;

    static glm::vec3 calculateGravityForce(float mass) {
        return glm::vec3(0.0f, -mass * GRAVITY, 0.0f);
    }

    static glm::vec3 calculateDragForce(
        const glm::vec3& velocity,
        float dragCoefficient,
        float crossSectionalArea
    ) {
        float speed = glm::length(velocity);
        if (speed < 0.001f) return glm::vec3(0.0f);
        
        glm::vec3 dragDirection = -glm::normalize(velocity);
        float dragMagnitude = 0.5f * AIR_DENSITY * speed * speed * dragCoefficient * crossSectionalArea;
        
        return dragDirection * dragMagnitude;
    }

    static glm::vec3 calculateBuoyancyForce(
        float submergedVolume,
        float fluidDensity = 1000.0f
    ) {
        return glm::vec3(0.0f, fluidDensity * submergedVolume * GRAVITY, 0.0f);
    }

    static glm::vec3 calculateSpringForce(
        const glm::vec3& displacement,
        float springConstant,
        float restLength = 0.0f
    ) {
        float currentLength = glm::length(displacement);
        if (currentLength < 0.001f) return glm::vec3(0.0f);
        
        float extension = currentLength - restLength;
        glm::vec3 direction = displacement / currentLength;
        
        return -springConstant * extension * direction;
    }

    static glm::vec3 calculateDampedSpringForce(
        const glm::vec3& displacement,
        const glm::vec3& velocity,
        float springConstant,
        float dampingCoefficient,
        float restLength = 0.0f
    ) {
        glm::vec3 springForce = calculateSpringForce(displacement, springConstant, restLength);
        glm::vec3 dampingForce = -dampingCoefficient * velocity;
        
        return springForce + dampingForce;
    }

    static float calculateKineticEnergy(float mass, const glm::vec3& velocity) {
        return 0.5f * mass * glm::dot(velocity, velocity);
    }

    static float calculateGravitationalPotentialEnergy(float mass, float height) {
        return mass * GRAVITY * height;
    }

    static float calculateElasticPotentialEnergy(float springConstant, float displacement) {
        return 0.5f * springConstant * displacement * displacement;
    }

    static glm::vec3 calculateMomentum(float mass, const glm::vec3& velocity) {
        return mass * velocity;
    }

    static glm::vec3 calculateImpulse(const glm::vec3& force, float deltaTime) {
        return force * deltaTime;
    }

    static void applyImpulse(
        glm::vec3& velocity,
        float mass,
        const glm::vec3& impulse
    ) {
        velocity += impulse / mass;
    }

    static glm::vec3 calculateTorque(
        const glm::vec3& force,
        const glm::vec3& position,
        const glm::vec3& pivot
    ) {
        return glm::cross(position - pivot, force);
    }

    static glm::vec3 calculateAngularMomentum(
        const glm::vec3& position,
        const glm::vec3& momentum,
        const glm::vec3& pivot
    ) {
        return glm::cross(position - pivot, momentum);
    }

    static float calculateMomentOfInertiaSolidSphere(float mass, float radius) {
        return 0.4f * mass * radius * radius;
    }

    static float calculateMomentOfInertiaHollowSphere(float mass, float radius) {
        return (2.0f / 3.0f) * mass * radius * radius;
    }

    static float calculateMomentOfInertiaSolidCylinder(float mass, float radius) {
        return 0.5f * mass * radius * radius;
    }

    static float calculateMomentOfInertiaHollowCylinder(float mass, float radius) {
        return mass * radius * radius;
    }

    static float calculateMomentOfInertiaBox(
        float mass,
        const glm::vec3& size
    ) {
        return (1.0f / 12.0f) * mass * (size.x * size.x + size.y * size.y + size.z * size.z);
    }

    static glm::vec3 calculateCenterOfMass(
        const std::vector<float>& masses,
        const std::vector<glm::vec3>& positions
    ) {
        if (masses.empty() || masses.size() != positions.size()) {
            return glm::vec3(0.0f);
        }

        glm::vec3 weightedSum(0.0f);
        float totalMass = 0.0f;

        for (size_t i = 0; i < masses.size(); ++i) {
            weightedSum += masses[i] * positions[i];
            totalMass += masses[i];
        }

        if (totalMass < 0.0001f) {
            return glm::vec3(0.0f);
        }

        return weightedSum / totalMass;
    }

    static bool isPointInSphere(
        const glm::vec3& point,
        const glm::vec3& sphereCenter,
        float sphereRadius
    ) {
        float distanceSquared = glm::dot(point - sphereCenter, point - sphereCenter);
        return distanceSquared <= sphereRadius * sphereRadius;
    }

    static bool isPointInBox(
        const glm::vec3& point,
        const glm::vec3& boxCenter,
        const glm::vec3& boxHalfExtents
    ) {
        glm::vec3 delta = point - boxCenter;
        return std::abs(delta.x) <= boxHalfExtents.x &&
               std::abs(delta.y) <= boxHalfExtents.y &&
               std::abs(delta.z) <= boxHalfExtents.z;
    }

    static float distanceToSphereSurface(
        const glm::vec3& point,
        const glm::vec3& sphereCenter,
        float sphereRadius
    ) {
        float distance = glm::length(point - sphereCenter);
        return distance - sphereRadius;
    }

    static glm::vec3 closestPointOnSphere(
        const glm::vec3& point,
        const glm::vec3& sphereCenter,
        float sphereRadius
    ) {
        glm::vec3 direction = point - sphereCenter;
        float distance = glm::length(direction);
        
        if (distance < 0.0001f) {
            return sphereCenter + glm::vec3(0.0f, sphereRadius, 0.0f);
        }
        
        return sphereCenter + glm::normalize(direction) * sphereRadius;
    }

    static glm::vec3 closestPointOnBox(
        const glm::vec3& point,
        const glm::vec3& boxCenter,
        const glm::vec3& boxHalfExtents
    ) {
        glm::vec3 result = point - boxCenter;
        
        result.x = glm::clamp(result.x, -boxHalfExtents.x, boxHalfExtents.x);
        result.y = glm::clamp(result.y, -boxHalfExtents.y, boxHalfExtents.y);
        result.z = glm::clamp(result.z, -boxHalfExtents.z, boxHalfExtents.z);
        
        return result + boxCenter;
    }

    static glm::vec3 calculateReflection(
        const glm::vec3& incident,
        const glm::vec3& normal
    ) {
        return incident - 2.0f * glm::dot(incident, normal) * normal;
    }

    static float calculateRestitutionVelocity(
        float incomingVelocity,
        float restitution
    ) {
        return -incomingVelocity * restitution;
    }

    static void resolveElasticCollision(
        glm::vec3& velocity1,
        float mass1,
        glm::vec3& velocity2,
        float mass2,
        const glm::vec3& collisionNormal,
        float restitution = 1.0f
    ) {
        float totalMass = mass1 + mass2;
        if (totalMass < 0.0001f) return;

        float relativeVelocity = glm::dot(velocity1 - velocity2, collisionNormal);
        if (relativeVelocity > 0.0f) return;

        float impulseMagnitude = -(1.0f + restitution) * relativeVelocity / totalMass;
        glm::vec3 impulse = impulseMagnitude * collisionNormal;

        velocity1 += impulse / mass1;
        velocity2 -= impulse / mass2;
    }

    static float calculateFrictionForce(
        float normalForce,
        float frictionCoefficient
    ) {
        return normalForce * frictionCoefficient;
    }

    static glm::vec3 applyFriction(
        const glm::vec3& velocity,
        const glm::vec3& normal,
        float normalForce,
        float frictionCoefficient,
        float deltaTime
    ) {
        glm::vec3 tangentVelocity = velocity - glm::dot(velocity, normal) * normal;
        float tangentSpeed = glm::length(tangentVelocity);
        
        if (tangentSpeed < 0.001f) return velocity;
        
        glm::vec3 tangentDirection = tangentVelocity / tangentSpeed;
        float frictionMagnitude = calculateFrictionForce(normalForce, frictionCoefficient);
        float speedChange = frictionMagnitude * deltaTime;
        
        if (speedChange >= tangentSpeed) {
            return velocity - tangentVelocity;
        }
        
        return velocity - tangentDirection * speedChange;
    }

    static glm::vec3 calculateTerminalVelocity(
        float mass,
        float dragCoefficient,
        float crossSectionalArea
    ) {
        float terminalSpeed = std::sqrt(
            (2.0f * mass * GRAVITY) /
            (AIR_DENSITY * dragCoefficient * crossSectionalArea)
        );
        return glm::vec3(0.0f, -terminalSpeed, 0.0f);
    }

    static float calculateOrbitalSpeed(
        float centralMass,
        float orbitalRadius,
        float gravitationalConstant = 6.67430e-11f
    ) {
        return std::sqrt((gravitationalConstant * centralMass) / orbitalRadius);
    }

    static float calculateEscapeVelocity(
        float mass,
        float radius,
        float gravitationalConstant = 6.67430e-11f
    ) {
        return std::sqrt((2.0f * gravitationalConstant * mass) / radius);
    }

    static glm::vec3 calculateGravitationalForce(
        float mass1,
        float mass2,
        const glm::vec3& position1,
        const glm::vec3& position2,
        float gravitationalConstant = 6.67430e-11f
    ) {
        glm::vec3 direction = position2 - position1;
        float distanceSquared = glm::dot(direction, direction);
        
        if (distanceSquared < 0.0001f) {
            return glm::vec3(0.0f);
        }
        
        float distance = std::sqrt(distanceSquared);
        float forceMagnitude = (gravitationalConstant * mass1 * mass2) / distanceSquared;
        
        return glm::normalize(direction) * forceMagnitude;
    }

    static bool raySphereIntersection(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDirection,
        const glm::vec3& sphereCenter,
        float sphereRadius,
        float& t
    ) {
        glm::vec3 oc = rayOrigin - sphereCenter;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(oc, rayDirection);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f) {
            return false;
        }

        t = (-b - std::sqrt(discriminant)) / (2.0f * a);
        return t >= 0.0f;
    }

    static bool rayBoxIntersection(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDirection,
        const glm::vec3& boxMin,
        const glm::vec3& boxMax,
        float& tMin,
        float& tMax
    ) {
        tMin = -std::numeric_limits<float>::infinity();
        tMax = std::numeric_limits<float>::infinity();

        for (int i = 0; i < 3; ++i) {
            if (std::abs(rayDirection[i]) < 0.0001f) {
                if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i]) {
                    return false;
                }
            } else {
                float t1 = (boxMin[i] - rayOrigin[i]) / rayDirection[i];
                float t2 = (boxMax[i] - rayOrigin[i]) / rayDirection[i];
                if (t1 > t2) std::swap(t1, t2);
                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);
                if (tMin > tMax) return false;
            }
        }

        return tMax >= 0.0f;
    }
};

} 
} 
