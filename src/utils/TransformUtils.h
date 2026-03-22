#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace LGE {
namespace Utils {

class TransformUtils {
public:
    static glm::mat4 createTranslationMatrix(const glm::vec3& translation) {
        return glm::translate(glm::mat4(1.0f), translation);
    }

    static glm::mat4 createRotationMatrix(float angle, const glm::vec3& axis) {
        return glm::rotate(glm::mat4(1.0f), angle, axis);
    }

    static glm::mat4 createRotationMatrix(const glm::quat& quaternion) {
        return glm::toMat4(quaternion);
    }

    static glm::mat4 createScaleMatrix(const glm::vec3& scale) {
        return glm::scale(glm::mat4(1.0f), scale);
    }

    static glm::mat4 createTransformMatrix(
        const glm::vec3& translation,
        const glm::quat& rotation,
        const glm::vec3& scale
    ) {
        glm::mat4 result = glm::mat4(1.0f);
        result = glm::translate(result, translation);
        result *= glm::toMat4(rotation);
        result = glm::scale(result, scale);
        return result;
    }

    static glm::vec3 extractTranslation(const glm::mat4& matrix) {
        return glm::vec3(matrix[3]);
    }

    static glm::quat extractRotation(const glm::mat4& matrix) {
        return glm::quat_cast(glm::mat3(matrix));
    }

    static glm::vec3 extractScale(const glm::mat4& matrix) {
        glm::vec3 scale;
        scale.x = glm::length(glm::vec3(matrix[0]));
        scale.y = glm::length(glm::vec3(matrix[1]));
        scale.z = glm::length(glm::vec3(matrix[2]));
        return scale;
    }

    static void decomposeMatrix(
        const glm::mat4& matrix,
        glm::vec3& translation,
        glm::quat& rotation,
        glm::vec3& scale
    ) {
        translation = extractTranslation(matrix);
        scale = extractScale(matrix);
        
        glm::mat3 rotationMatrix(glm::vec3(matrix[0]) / scale.x,
                                   glm::vec3(matrix[1]) / scale.y,
                                   glm::vec3(matrix[2]) / scale.z);
        rotation = glm::quat_cast(rotationMatrix);
    }

    static glm::quat eulerToQuaternion(const glm::vec3& eulerAngles) {
        return glm::quat(eulerAngles);
    }

    static glm::vec3 quaternionToEuler(const glm::quat& quaternion) {
        return glm::eulerAngles(quaternion);
    }

    static glm::quat slerp(const glm::quat& q1, const glm::quat& q2, float t) {
        return glm::slerp(q1, q2, t);
    }

    static glm::vec3 lerp(const glm::vec3& v1, const glm::vec3& v2, float t) {
        return glm::mix(v1, v2, t);
    }

    static glm::vec3 worldToLocal(
        const glm::vec3& worldPoint,
        const glm::mat4& worldTransform
    ) {
        glm::mat4 inverseTransform = glm::inverse(worldTransform);
        return glm::vec3(inverseTransform * glm::vec4(worldPoint, 1.0f));
    }

    static glm::vec3 localToWorld(
        const glm::vec3& localPoint,
        const glm::mat4& worldTransform
    ) {
        return glm::vec3(worldTransform * glm::vec4(localPoint, 1.0f));
    }

    static glm::vec3 directionWorldToLocal(
        const glm::vec3& worldDirection,
        const glm::mat4& worldTransform
    ) {
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(worldTransform)));
        return glm::normalize(normalMatrix * worldDirection);
    }

    static glm::vec3 directionLocalToWorld(
        const glm::vec3& localDirection,
        const glm::mat4& worldTransform
    ) {
        return glm::normalize(glm::mat3(worldTransform) * localDirection);
    }

    static glm::mat4 lookAt(
        const glm::vec3& eye,
        const glm::vec3& target,
        const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)
    ) {
        return glm::lookAt(eye, target, up);
    }

    static glm::quat lookAtQuaternion(
        const glm::vec3& eye,
        const glm::vec3& target,
        const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)
    ) {
        glm::mat4 lookAtMatrix = lookAt(eye, target, up);
        return glm::quat_cast(glm::inverse(lookAtMatrix));
    }

    static bool isIdentity(const glm::mat4& matrix, float epsilon = 1e-6f) {
        glm::mat4 identity = glm::mat4(1.0f);
        return glm::all(glm::epsilonEqual(matrix, identity, epsilon));
    }

    static bool isOrthogonal(const glm::mat3& matrix, float epsilon = 1e-6f) {
        glm::mat3 transposed = glm::transpose(matrix);
        glm::mat3 product = matrix * transposed;
        return isIdentity(glm::mat4(product), epsilon);
    }

    static bool isOrthonormal(const glm::mat3& matrix, float epsilon = 1e-6f) {
        return isOrthogonal(matrix, epsilon) &&
               glm::abs(glm::determinant(matrix) - 1.0f) < epsilon;
    }

    static glm::mat4 rotateAround(
        const glm::mat4& transform,
        const glm::vec3& point,
        const glm::vec3& axis,
        float angle
    ) {
        glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), -point);
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle, axis);
        glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), point);
        return translateBack * rotate * translateToOrigin * transform;
    }

    static glm::vec3 transformPoint(const glm::mat4& matrix, const glm::vec3& point) {
        return glm::vec3(matrix * glm::vec4(point, 1.0f));
    }

    static glm::vec3 transformVector(const glm::mat4& matrix, const glm::vec3& vector) {
        return glm::vec3(matrix * glm::vec4(vector, 0.0f));
    }

    static glm::vec3 transformNormal(const glm::mat4& matrix, const glm::vec3& normal) {
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(matrix)));
        return glm::normalize(normalMatrix * normal);
    }

    static glm::mat4 interpolateTransform(
        const glm::mat4& transform1,
        const glm::mat4& transform2,
        float t
    ) {
        glm::vec3 translation1, translation2;
        glm::quat rotation1, rotation2;
        glm::vec3 scale1, scale2;
        
        decomposeMatrix(transform1, translation1, rotation1, scale1);
        decomposeMatrix(transform2, translation2, rotation2, scale2);
        
        glm::vec3 interpolatedTranslation = lerp(translation1, translation2, t);
        glm::quat interpolatedRotation = slerp(rotation1, rotation2, t);
        glm::vec3 interpolatedScale = lerp(scale1, scale2, t);
        
        return createTransformMatrix(interpolatedTranslation, interpolatedRotation, interpolatedScale);
    }

    static glm::mat4 invertTransform(const glm::mat4& transform) {
        return glm::inverse(transform);
    }

    static glm::mat4 transposeTransform(const glm::mat4& transform) {
        return glm::transpose(transform);
    }

    static glm::quat rotateTowards(
        const glm::quat& from,
        const glm::quat& to,
        float maxDegreesDelta
    ) {
        float angle = glm::angle(from, to);
        if (angle <= maxDegreesDelta) {
            return to;
        }
        float t = maxDegreesDelta / angle;
        return slerp(from, to, t);
    }

    static glm::vec3 rotateVector(const glm::quat& rotation, const glm::vec3& vector) {
        return rotation * vector;
    }

    static glm::quat fromAxisAngle(const glm::vec3& axis, float angle) {
        return glm::angleAxis(angle, axis);
    }

    static void toAxisAngle(const glm::quat& quaternion, glm::vec3& axis, float& angle) {
        angle = glm::angle(quaternion);
        axis = glm::axis(quaternion);
    }

    static glm::quat multiplyQuaternions(const glm::quat& q1, const glm::quat& q2) {
        return q1 * q2;
    }

    static glm::quat normalizeQuaternion(const glm::quat& quaternion) {
        return glm::normalize(quaternion);
    }

    static glm::quat conjugateQuaternion(const glm::quat& quaternion) {
        return glm::conjugate(quaternion);
    }

    static glm::quat inverseQuaternion(const glm::quat& quaternion) {
        return glm::inverse(quaternion);
    }

    static float quaternionDot(const glm::quat& q1, const glm::quat& q2) {
        return glm::dot(q1, q2);
    }

    static float quaternionLength(const glm::quat& quaternion) {
        return glm::length(quaternion);
    }

    static bool quaternionsAreEqual(const glm::quat& q1, const glm::quat& q2, float epsilon = 1e-6f) {
        return glm::abs(quaternionDot(q1, q2)) >= 1.0f - epsilon;
    }
};

} 
} 
