#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_VECTOR_

#include "Vector3.h"

#include <cmath>

namespace atmt {

    // Drop-in std::clamp replacement for C++11/C++14
    template <typename T>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }
    constexpr double kZeroEpsilon = 1e-12;

    Vector3::Vector3():
        Vector3(0, 0, 0)
    {

    };
    Vector3::Vector3(double x, double y, double z):
        m_x{ x },
        m_y{ y },
        m_z{ z }
    {

    };
    // Vector3(double r, double theta, double phi); Must have different parameters

    Vector3 Vector3::operator+(const Vector3 &vector) const {
        return Vector3(m_x + vector.m_x, m_y + vector.m_y, m_z + vector.m_z);
    };
    Vector3 Vector3::operator-(const Vector3 &vector) const {
        return Vector3(m_x - vector.m_x, m_y - vector.m_y, m_z - vector.m_z);
    };
    Vector3 Vector3::operator*(double scalar) const {
        return Vector3(m_x * scalar, m_y * scalar, m_z * scalar);
    };
    Vector3 operator*(double scalar, const Vector3 &vector) {
        return vector * scalar; // Swaps order to avoid error
    };
    Vector3 Vector3::operator/(double scalar) const {
        return Vector3(m_x / scalar, m_y / scalar, m_z / scalar);
    };
    Vector3 operator/(double scalar, const Vector3 &vector) {
        return vector / scalar; // Swaps order to avoid error
    };
    Vector3& Vector3::operator+=(const Vector3 &vector) {
        m_x += vector.m_x;
        m_y += vector.m_y;
        m_z += vector.m_z;
        return *this;
    };
    Vector3& Vector3::operator-=(const Vector3 &vector) {
        m_x -= vector.m_x;
        m_y -= vector.m_y;
        m_z -= vector.m_z;
        return *this;
    };
    Vector3& Vector3::operator*=(double scalar) {
        m_x *= scalar;
        m_y *= scalar;
        m_z *= scalar;
        return *this;
    };
    Vector3& Vector3::operator/=(double scalar) {
        m_x /= scalar;
        m_y /= scalar;
        m_z /= scalar;
        return *this;
    };
    bool Vector3::operator==(const Vector3 &vector) const {
        return (std::abs(m_x - vector.m_x) <= kZeroEpsilon && std::abs(m_y - vector.m_y) <= kZeroEpsilon && std::abs(m_z - vector.m_z) <= kZeroEpsilon);
    };

    double Vector3::dot(const Vector3 &vector) const {
        return m_x * vector.m_x + m_y * vector.m_y + m_z * vector.m_z;
    };
    Vector3 Vector3::cross(const Vector3 &vector) const {
        return Vector3(
            m_y * vector.m_z - m_z * vector.m_y,
            m_z * vector.m_x - m_x * vector.m_z,
            m_x * vector.m_y - m_y * vector.m_x
        );
    };
    Vector3 Vector3::normalize() const {
        return *this / getR();
    };

    void Vector3::fromCartesian(double x, double y, double z) {
        m_x = x;
        m_y = y;
        m_z = z;
    };
    void Vector3::fromSpherical(double r, double theta, double phi) { // Matches Physics convention
        m_x = r * std::sin(theta) * std::cos(phi);
        m_y = r * std::sin(theta) * std::sin(phi);
        m_z = r * std::cos(theta);
    };

    double Vector3::getX() const {
        return m_x;
    };
    double Vector3::getY() const {
        return m_y;
    };
    double Vector3::getZ() const {
        return m_z;
    };
    void Vector3::setX(double x) {
        m_x = x;
    };
    void Vector3::setY(double y) {
        m_y = y;
    };
    void Vector3::setZ(double z) {
        m_z = z;
    };
    double Vector3::getR2() const { // Magnitude
        return m_x * m_x + m_y * m_y + m_z * m_z;
    };
    double Vector3::getR() const { // Magnitude
        return std::sqrt(getR2());
    };
    double Vector3::getTheta() const { // Angle to Z+ (Radians), Matches Physics convention
        double r = getR();
        if (r <= kZeroEpsilon) {
            return 0.0;
        }

        double value = m_z / r;
        value = clamp(value, -1.0, 1.0);
        return std::acos(value);
    };
    double Vector3::getPhi() const { // Angle to X+ (Radians), Matches Physics convention
        return std::atan2(m_y, m_x);
    };

    void Vector3::translate(double x, double y, double z) {
        m_x += x;
        m_y += y;
        m_z += z;
    };
    void Vector3::rotate(double theta, double phi) {
        fromSpherical(getR(), getTheta() + theta, getPhi() + phi);
    };

};

#else
#error "Enable ATMT_SUBMODULE_UTILITIES_VECTOR_ in automat_submodules.h to use Vector3"
#endif
