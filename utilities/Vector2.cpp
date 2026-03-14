#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_VECTOR_

#include "Vector2.h"

#include <cmath>
#include "../utils.h"

namespace atmt {

    Vector2::Vector2():
        Vector2(0, 0)
    {

    };
    Vector2::Vector2(double x, double y):
        m_x{ x },
        m_y{ y }
    {

    };
    // Vector2(double r, double theta, double phi); Must have different parameters

    Vector2 Vector2::operator+(const Vector2 &vector) const {
        return Vector2(m_x + vector.m_x, m_y + vector.m_y);
    };
    Vector2 Vector2::operator-(const Vector2 &vector) const {
        return Vector2(m_x - vector.m_x, m_y - vector.m_y);
    };
    Vector2 Vector2::operator*(double scalar) const {
        return Vector2(m_x * scalar, m_y * scalar);
    };
    Vector2 operator*(double scalar, const Vector2 &vector) {
        return vector * scalar; // Swaps order to avoid error
    };
    Vector2 Vector2::operator/(double scalar) const {
        return Vector2(m_x / scalar, m_y / scalar);
    };
    Vector2& Vector2::operator+=(const Vector2 &vector) {
        m_x += vector.m_x;
        m_y += vector.m_y;
        return *this;
    };
    Vector2& Vector2::operator-=(const Vector2 &vector) {
        m_x -= vector.m_x;
        m_y -= vector.m_y;
        return *this;
    };
    Vector2& Vector2::operator*=(double scalar) {
        m_x *= scalar;
        m_y *= scalar;
        return *this;
    };
    Vector2& Vector2::operator/=(double scalar) {
        if (std::abs(scalar) < kZeroEpsilon) {
            return *this;
        }
        m_x /= scalar;
        m_y /= scalar;
        return *this;
    };
    bool Vector2::operator==(const Vector2 &vector) const {
        return (std::abs(m_x - vector.m_x) <= kZeroEpsilon && std::abs(m_y - vector.m_y) <= kZeroEpsilon);
    };

    double Vector2::dot(const Vector2 &vector) const {
        return m_x * vector.m_x + m_y * vector.m_y;
    };
    Vector2 Vector2::normalize() const {
        return *this / getR();
    };

    void Vector2::fromCartesian(double x, double y) {
        m_x = x;
        m_y = y;
    };
    void Vector2::fromPolar(double r, double theta) { // Matches Physics convention
        m_x = r * std::cos(theta);
        m_y = r * std::sin(theta);
    };

    double Vector2::getX() const {
        return m_x;
    };
    double Vector2::getY() const {
        return m_y;
    };
    void Vector2::setX(double x) {
        m_x = x;
    };
    void Vector2::setY(double y) {
        m_y = y;
    };
    double Vector2::getR2() const { // Magnitude
        return m_x * m_x + m_y * m_y;
    };
    double Vector2::getR() const { // Magnitude
        return std::sqrt(getR2());
    };
    double Vector2::getTheta() const { // Angle to X+ (Radians), Matches Physics convention
        return std::atan2(m_y, m_x);
    };

    void Vector2::translate(double x, double y) {
        m_x += x;
        m_y += y;
    };
    void Vector2::rotate(double theta) {
        fromPolar(getR(), getTheta() + theta);
    };

};

#else
// #error "Enable ATMT_SUBMODULE_UTILITIES_VECTOR_ in automat_submodules.h to use Vector2"
#endif
