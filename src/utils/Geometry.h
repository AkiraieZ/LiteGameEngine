#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace LGE {
namespace Utils {
namespace Geometry {

struct Point2D {
    float x, y;

    Point2D() : x(0.0f), y(0.0f) {}
    Point2D(float x, float y) : x(x), y(y) {}

    Point2D operator+(const Point2D& other) const {
        return Point2D(x + other.x, y + other.y);
    }

    Point2D operator-(const Point2D& other) const {
        return Point2D(x - other.x, y - other.y);
    }

    Point2D operator*(float s) const {
        return Point2D(x * s, y * s);
    }

    Point2D operator/(float s) const {
        return Point2D(x / s, y / s);
    }

    float dot(const Point2D& other) const {
        return x * other.x + y * other.y;
    }

    float cross(const Point2D& other) const {
        return x * other.y - y * other.x;
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

    Point2D normalized() const {
        float len = length();
        if (len > 0.0001f) return *this / len;
        return Point2D(0.0f, 0.0f);
    }

    float distance(const Point2D& other) const {
        return (*this - other).length();
    }

    float distanceSquared(const Point2D& other) const {
        return (*this - other).lengthSquared();
    }

    Point2D lerp(const Point2D& other, float t) const {
        return *this * (1.0f - t) + other * t;
    }

    Point2D rotate(float angle) const {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return Point2D(x * c - y * s, x * s + y * c);
    }

    Point2D perpendicular() const {
        return Point2D(-y, x);
    }

    static Point2D zero() { return Point2D(0.0f, 0.0f); }
    static Point2D one() { return Point2D(1.0f, 1.0f); }
    static Point2D up() { return Point2D(0.0f, 1.0f); }
    static Point2D down() { return Point2D(0.0f, -1.0f); }
    static Point2D left() { return Point2D(-1.0f, 0.0f); }
    static Point2D right() { return Point2D(1.0f, 0.0f); }
};

struct Point3D {
    float x, y, z;

    Point3D() : x(0.0f), y(0.0f), z(0.0f) {}
    Point3D(float x, float y, float z) : x(x), y(y), z(z) {}
    Point3D(const Point2D& p, float z = 0.0f) : x(p.x), y(p.y), z(z) {}

    Point3D operator+(const Point3D& other) const {
        return Point3D(x + other.x, y + other.y, z + other.z);
    }

    Point3D operator-(const Point3D& other) const {
        return Point3D(x - other.x, y - other.y, z - other.z);
    }

    Point3D operator*(float s) const {
        return Point3D(x * s, y * s, z * s);
    }

    Point3D operator/(float s) const {
        return Point3D(x / s, y / s, z / s);
    }

    float dot(const Point3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Point3D cross(const Point3D& other) const {
        return Point3D(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float lengthSquared() const {
        return x * x + y * y + z * z;
    }

    Point3D normalized() const {
        float len = length();
        if (len > 0.0001f) return *this / len;
        return Point3D(0.0f, 0.0f, 0.0f);
    }

    float distance(const Point3D& other) const {
        return (*this - other).length();
    }

    float distanceSquared(const Point3D& other) const {
        return (*this - other).lengthSquared();
    }

    Point3D lerp(const Point3D& other, float t) const {
        return *this * (1.0f - t) + other * t;
    }

    Point2D toPoint2D() const { return Point2D(x, y); }

    static Point3D zero() { return Point3D(0.0f, 0.0f, 0.0f); }
    static Point3D one() { return Point3D(1.0f, 1.0f, 1.0f); }
    static Point3D up() { return Point3D(0.0f, 1.0f, 0.0f); }
    static Point3D down() { return Point3D(0.0f, -1.0f, 0.0f); }
    static Point3D left() { return Point3D(-1.0f, 0.0f, 0.0f); }
    static Point3D right() { return Point3D(1.0f, 0.0f, 0.0f); }
    static Point3D forward() { return Point3D(0.0f, 0.0f, -1.0f); }
    static Point3D back() { return Point3D(0.0f, 0.0f, 1.0f); }
};

struct Line2D {
    Point2D start, end;

    Line2D() = default;
    Line2D(const Point2D& start, const Point2D& end) : start(start), end(end) {}

    Point2D direction() const {
        return end - start;
    }

    Point2D normalizedDirection() const {
        return direction().normalized();
    }

    float length() const {
        return direction().length();
    }

    float lengthSquared() const {
        return direction().lengthSquared();
    }

    Point2D getPoint(float t) const {
        return start.lerp(end, t);
    }

    Point2D closestPoint(const Point2D& p) const {
        Point2D dir = direction();
        float lenSq = dir.lengthSquared();
        if (lenSq < 0.0001f) return start;
        
        float t = std::max(0.0f, std::min(1.0f, (p - start).dot(dir) / lenSq));
        return start + dir * t;
    }

    float distance(const Point2D& p) const {
        return p.distance(closestPoint(p));
    }

    float distanceSquared(const Point2D& p) const {
        return p.distanceSquared(closestPoint(p));
    }

    bool intersects(const Line2D& other, Point2D* intersection = nullptr) const {
        Point2D d1 = direction();
        Point2D d2 = other.direction();
        Point2D d = other.start - start;
        
        float cross = d1.cross(d2);
        float cross1 = d.cross(d1);
        float cross2 = d.cross(d2);
        
        if (std::fabs(cross) < 0.0001f) {
            return false;
        }
        
        float t = cross2 / cross;
        float u = cross1 / cross;
        
        if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
            if (intersection) *intersection = start + d1 * t;
            return true;
        }
        
        return false;
    }
};

struct Line3D {
    Point3D start, end;

    Line3D() = default;
    Line3D(const Point3D& start, const Point3D& end) : start(start), end(end) {}

    Point3D direction() const {
        return end - start;
    }

    Point3D normalizedDirection() const {
        return direction().normalized();
    }

    float length() const {
        return direction().length();
    }

    float lengthSquared() const {
        return direction().lengthSquared();
    }

    Point3D getPoint(float t) const {
        return start.lerp(end, t);
    }

    Point3D closestPoint(const Point3D& p) const {
        Point3D dir = direction();
        float lenSq = dir.lengthSquared();
        if (lenSq < 0.0001f) return start;
        
        float t = std::max(0.0f, std::min(1.0f, (p - start).dot(dir) / lenSq));
        return start + dir * t;
    }

    float distance(const Point3D& p) const {
        return p.distance(closestPoint(p));
    }

    float distanceSquared(const Point3D& p) const {
        return p.distanceSquared(closestPoint(p));
    }
};

struct Circle {
    Point2D center;
    float radius;

    Circle() : center(Point2D::zero()), radius(1.0f) {}
    Circle(const Point2D& center, float radius) : center(center), radius(radius) {}

    float circumference() const {
        return 2.0f * 3.14159265359f * radius;
    }

    float area() const {
        return 3.14159265359f * radius * radius;
    }

    bool contains(const Point2D& p) const {
        return center.distanceSquared(p) <= radius * radius;
    }

    bool intersects(const Circle& other) const {
        float d = center.distance(other.center);
        return d <= radius + other.radius && d >= std::fabs(radius - other.radius);
    }

    Point2D closestPoint(const Point2D& p) const {
        Point2D dir = p - center;
        float len = dir.length();
        if (len < 0.0001f) return center + Point2D(radius, 0.0f);
        return center + dir * (radius / len);
    }
};

struct Sphere {
    Point3D center;
    float radius;

    Sphere() : center(Point3D::zero()), radius(1.0f) {}
    Sphere(const Point3D& center, float radius) : center(center), radius(radius) {}

    float surfaceArea() const {
        return 4.0f * 3.14159265359f * radius * radius;
    }

    float volume() const {
        return 4.0f / 3.0f * 3.14159265359f * radius * radius * radius;
    }

    bool contains(const Point3D& p) const {
        return center.distanceSquared(p) <= radius * radius;
    }

    bool intersects(const Sphere& other) const {
        float d = center.distance(other.center);
        return d <= radius + other.radius && d >= std::fabs(radius - other.radius);
    }

    Point3D closestPoint(const Point3D& p) const {
        Point3D dir = p - center;
        float len = dir.length();
        if (len < 0.0001f) return center + Point3D(radius, 0.0f, 0.0f);
        return center + dir * (radius / len);
    }
};

struct AABB2D {
    Point2D min, max;

    AABB2D() : min(Point2D::zero()), max(Point2D::one()) {}
    AABB2D(const Point2D& min, const Point2D& max) : min(min), max(max) {}
    AABB2D(float minX, float minY, float maxX, float maxY) : min(minX, minY), max(maxX, maxY) {}

    Point2D center() const {
        return (min + max) * 0.5f;
    }

    Point2D size() const {
        return max - min;
    }

    float width() const {
        return max.x - min.x;
    }

    float height() const {
        return max.y - min.y;
    }

    float area() const {
        return width() * height();
    }

    void expand(const Point2D& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
    }

    void expand(const AABB2D& other) {
        min.x = std::min(min.x, other.min.x);
        min.y = std::min(min.y, other.min.y);
        max.x = std::max(max.x, other.max.x);
        max.y = std::max(max.y, other.max.y);
    }

    bool contains(const Point2D& p) const {
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y;
    }

    bool contains(const AABB2D& other) const {
        return contains(other.min) && contains(other.max);
    }

    bool intersects(const AABB2D& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y;
    }

    Point2D closestPoint(const Point2D& p) const {
        return Point2D(
            std::max(min.x, std::min(max.x, p.x)),
            std::max(min.y, std::min(max.y, p.y))
        );
    }

    static AABB2D fromPoints(const std::vector<Point2D>& points) {
        if (points.empty()) return AABB2D();
        AABB2D aabb(points[0], points[0]);
        for (size_t i = 1; i < points.size(); ++i) {
            aabb.expand(points[i]);
        }
        return aabb;
    }
};

struct AABB3D {
    Point3D min, max;

    AABB3D() : min(Point3D::zero()), max(Point3D::one()) {}
    AABB3D(const Point3D& min, const Point3D& max) : min(min), max(max) {}
    AABB3D(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) 
        : min(minX, minY, minZ), max(maxX, maxY, maxZ) {}

    Point3D center() const {
        return (min + max) * 0.5f;
    }

    Point3D size() const {
        return max - min;
    }

    float width() const {
        return max.x - min.x;
    }

    float height() const {
        return max.y - min.y;
    }

    float depth() const {
        return max.z - min.z;
    }

    float volume() const {
        return width() * height() * depth();
    }

    float surfaceArea() const {
        Point3D s = size();
        return 2.0f * (s.x * s.y + s.x * s.z + s.y * s.z);
    }

    void expand(const Point3D& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }

    void expand(const AABB3D& other) {
        min.x = std::min(min.x, other.min.x);
        min.y = std::min(min.y, other.min.y);
        min.z = std::min(min.z, other.min.z);
        max.x = std::max(max.x, other.max.x);
        max.y = std::max(max.y, other.max.y);
        max.z = std::max(max.z, other.max.z);
    }

    bool contains(const Point3D& p) const {
        return p.x >= min.x && p.x <= max.x &&
               p.y >= min.y && p.y <= max.y &&
               p.z >= min.z && p.z <= max.z;
    }

    bool contains(const AABB3D& other) const {
        return contains(other.min) && contains(other.max);
    }

    bool intersects(const AABB3D& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }

    Point3D closestPoint(const Point3D& p) const {
        return Point3D(
            std::max(min.x, std::min(max.x, p.x)),
            std::max(min.y, std::min(max.y, p.y)),
            std::max(min.z, std::min(max.z, p.z))
        );
    }

    static AABB3D fromPoints(const std::vector<Point3D>& points) {
        if (points.empty()) return AABB3D();
        AABB3D aabb(points[0], points[0]);
        for (size_t i = 1; i < points.size(); ++i) {
            aabb.expand(points[i]);
        }
        return aabb;
    }
};

struct Plane {
    Point3D normal;
    float distance;

    Plane() : normal(Point3D::up()), distance(0.0f) {}
    Plane(const Point3D& normal, float distance) : normal(normal.normalized()), distance(distance) {}
    Plane(const Point3D& a, const Point3D& b, const Point3D& c) {
        Point3D ab = b - a;
        Point3D ac = c - a;
        normal = ab.cross(ac).normalized();
        distance = -normal.dot(a);
    }

    float signedDistance(const Point3D& p) const {
        return normal.dot(p) + distance;
    }

    float distance(const Point3D& p) const {
        return std::fabs(signedDistance(p));
    }

    Point3D closestPoint(const Point3D& p) const {
        return p - normal * signedDistance(p);
    }

    bool isOnPositiveSide(const Point3D& p) const {
        return signedDistance(p) > 0.0001f;
    }

    bool isOnNegativeSide(const Point3D& p) const {
        return signedDistance(p) < -0.0001f;
    }

    bool intersects(const Line3D& line, Point3D* intersection = nullptr) const {
        float d1 = signedDistance(line.start);
        float d2 = signedDistance(line.end);
        
        if ((d1 > 0.0f && d2 > 0.0f) || (d1 < 0.0f && d2 < 0.0f)) {
            return false;
        }
        
        if (std::fabs(d1 - d2) < 0.0001f) {
            return false;
        }
        
        float t = -d1 / (d2 - d1);
        if (intersection) *intersection = line.getPoint(t);
        return true;
    }
};

struct Ray {
    Point3D origin;
    Point3D direction;

    Ray() : origin(Point3D::zero()), direction(Point3D::forward()) {}
    Ray(const Point3D& origin, const Point3D& direction) : origin(origin), direction(direction.normalized()) {}

    Point3D getPoint(float t) const {
        return origin + direction * t;
    }

    bool intersects(const Plane& plane, float* t = nullptr) const {
        float denom = plane.normal.dot(direction);
        if (std::fabs(denom) < 0.0001f) {
            return false;
        }
        
        float t_val = -(plane.normal.dot(origin) + plane.distance) / denom;
        if (t_val < 0.0f) {
            return false;
        }
        
        if (t) *t = t_val;
        return true;
    }

    bool intersects(const Sphere& sphere, float* t = nullptr) const {
        Point3D oc = origin - sphere.center;
        float a = direction.dot(direction);
        float b = 2.0f * oc.dot(direction);
        float c = oc.dot(oc) - sphere.radius * sphere.radius;
        float discriminant = b * b - 4.0f * a * c;
        
        if (discriminant < 0.0f) {
            return false;
        }
        
        float sqrtDisc = std::sqrt(discriminant);
        float t1 = (-b - sqrtDisc) / (2.0f * a);
        float t2 = (-b + sqrtDisc) / (2.0f * a);
        
        if (t1 >= 0.0f) {
            if (t) *t = t1;
            return true;
        }
        
        if (t2 >= 0.0f) {
            if (t) *t = t2;
            return true;
        }
        
        return false;
    }

    bool intersects(const AABB3D& aabb, float* tMin = nullptr, float* tMax = nullptr) const {
        float t1 = (aabb.min.x - origin.x) / direction.x;
        float t2 = (aabb.max.x - origin.x) / direction.x;
        float t3 = (aabb.min.y - origin.y) / direction.y;
        float t4 = (aabb.max.y - origin.y) / direction.y;
        float t5 = (aabb.min.z - origin.z) / direction.z;
        float t6 = (aabb.max.z - origin.z) / direction.z;
        
        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
        
        if (tmax < 0.0f) {
            return false;
        }
        
        if (tmin > tmax) {
            return false;
        }
        
        if (tMin) *tMin = tmin;
        if (tMax) *tMax = tmax;
        return true;
    }
};

struct Triangle {
    Point3D a, b, c;

    Triangle() = default;
    Triangle(const Point3D& a, const Point3D& b, const Point3D& c) : a(a), b(b), c(c) {}

    Point3D normal() const {
        Point3D ab = b - a;
        Point3D ac = c - a;
        return ab.cross(ac).normalized();
    }

    Point3D center() const {
        return (a + b + c) / 3.0f;
    }

    float area() const {
        Point3D ab = b - a;
        Point3D ac = c - a;
        return ab.cross(ac).length() * 0.5f;
    }

    Plane getPlane() const {
        return Plane(a, b, c);
    }

    bool contains(const Point3D& p) const {
        Plane plane = getPlane();
        if (plane.distance(p) > 0.0001f) {
            return false;
        }

        Point3D v0 = c - a;
        Point3D v1 = b - a;
        Point3D v2 = p - a;

        float dot00 = v0.dot(v0);
        float dot01 = v0.dot(v1);
        float dot02 = v0.dot(v2);
        float dot11 = v1.dot(v1);
        float dot12 = v1.dot(v2);

        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        return u >= 0.0f && v >= 0.0f && u + v <= 1.0f;
    }

    Point3D barycentric(const Point3D& p) const {
        Point3D v0 = c - a;
        Point3D v1 = b - a;
        Point3D v2 = p - a;

        float dot00 = v0.dot(v0);
        float dot01 = v0.dot(v1);
        float dot02 = v0.dot(v2);
        float dot11 = v1.dot(v1);
        float dot12 = v1.dot(v2);

        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        return Point3D(1.0f - u - v, v, u);
    }

    Point3D closestPoint(const Point3D& p) const {
        Point3D ab = b - a;
        Point3D ac = c - a;
        Point3D ap = p - a;

        float d1 = ab.dot(ap);
        float d2 = ac.dot(ap);
        if (d1 <= 0.0f && d2 <= 0.0f) return a;

        Point3D bp = p - b;
        float d3 = ab.dot(bp);
        float d4 = ac.dot(bp);
        if (d3 >= 0.0f && d4 <= d3) return b;

        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return a + ab * v;
        }

        Point3D cp = p - c;
        float d5 = ab.dot(cp);
        float d6 = ac.dot(cp);
        if (d6 >= 0.0f && d5 <= d6) return c;

        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6);
            return a + ac * w;
        }

        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + (c - b) * w;
        }

        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w;
    }
};

struct Frustum {
    Plane planes[6];

    Frustum() = default;

    enum PlaneIndex {
        Near = 0,
        Far = 1,
        Left = 2,
        Right = 3,
        Top = 4,
        Bottom = 5
    };

    bool contains(const Point3D& p) const {
        for (int i = 0; i < 6; ++i) {
            if (planes[i].isOnNegativeSide(p)) {
                return false;
            }
        }
        return true;
    }

    bool intersects(const Sphere& sphere) const {
        for (int i = 0; i < 6; ++i) {
            if (planes[i].signedDistance(sphere.center) < -sphere.radius) {
                return false;
            }
        }
        return true;
    }

    bool intersects(const AABB3D& aabb) const {
        for (int i = 0; i < 6; ++i) {
            Point3D normal = planes[i].normal;
            Point3D p(
                normal.x >= 0.0f ? aabb.max.x : aabb.min.x,
                normal.y >= 0.0f ? aabb.max.y : aabb.min.y,
                normal.z >= 0.0f ? aabb.max.z : aabb.min.z
            );
            
            if (planes[i].isOnNegativeSide(p)) {
                return false;
            }
        }
        return true;
    }
};

namespace Transform {

inline Point2D translate(const Point2D& p, const Point2D& offset) {
    return p + offset;
}

inline Point2D rotate(const Point2D& p, float angle, const Point2D& center = Point2D::zero()) {
    Point2D translated = p - center;
    Point2D rotated = translated.rotate(angle);
    return rotated + center;
}

inline Point2D scale(const Point2D& p, const Point2D& scale, const Point2D& center = Point2D::zero()) {
    Point2D translated = p - center;
    Point2D scaled = Point2D(translated.x * scale.x, translated.y * scale.y);
    return scaled + center;
}

inline Point2D scale(const Point2D& p, float scale, const Point2D& center = Point2D::zero()) {
    return Geometry::scale(p, Point2D(scale, scale), center);
}

inline Point3D translate(const Point3D& p, const Point3D& offset) {
    return p + offset;
}

inline Point3D rotateX(const Point3D& p, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Point3D(p.x, p.y * c - p.z * s, p.y * s + p.z * c);
}

inline Point3D rotateY(const Point3D& p, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Point3D(p.x * c + p.z * s, p.y, -p.x * s + p.z * c);
}

inline Point3D rotateZ(const Point3D& p, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return Point3D(p.x * c - p.y * s, p.x * s + p.y * c, p.z);
}

inline Point3D rotate(const Point3D& p, const Point3D& angles) {
    Point3D result = p;
    result = rotateX(result, angles.x);
    result = rotateY(result, angles.y);
    result = rotateZ(result, angles.z);
    return result;
}

inline Point3D scale(const Point3D& p, const Point3D& scale) {
    return Point3D(p.x * scale.x, p.y * scale.y, p.z * scale.z);
}

inline Point3D scale(const Point3D& p, float scale) {
    return Geometry::scale(p, Point3D(scale, scale, scale));
}

}

}