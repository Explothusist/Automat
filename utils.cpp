
#include "utils.h"

#include <cmath>
#ifdef AUTOMAT_ESP32_
#include <Arduino.h>
#endif

namespace atmt {
    
#ifdef AUTOMAT_VEX_
    vex::competition m_competition;
    vex::brain m_brain;
#endif

    Vector_2D::Vector_2D(double x, double y):
        m_x{ x },
        m_y{ y }
    {

    };
    Vector_2D::~Vector_2D() {

    };

    double Vector_2D::getX() {
        return m_x;
    };
    double Vector_2D::getY() {
        return m_y;
    };

    void Vector_2D::translate(Vector_2D vector) {
        m_x += vector.getX();
        m_y += vector.getY();
    };
    void Vector_2D::translate(double x, double y) {
        m_x += x;
        m_y += y;
    };
    void Vector_2D::normalize() {
        Vector_Polar polar = toPolar();
        polar.normalize();
        Vector_2D new_vert = polar.toCartesian();
        m_x = new_vert.getX();
        m_y = new_vert.getY();
    };
    void Vector_2D::rotate(double theta) {
        Vector_Polar polar = toPolar();
        polar.rotate(theta);
        Vector_2D new_vert = polar.toCartesian();
        m_x = new_vert.getX();
        m_y = new_vert.getY();
    };

    Vector_Polar Vector_2D::toPolar() {
        return Vector_Polar(
            std::hypot(m_x, m_y), 
            std::atan2(m_y, m_x)
        );
    };
    

    Vector_Polar::Vector_Polar(double r, double theta):
        m_r{ r },
        m_theta{ theta }
    {

    };
    Vector_Polar::~Vector_Polar() {

    };

    double Vector_Polar::getR() {
        return m_r;
    };
    double Vector_Polar::getTheta() {
        return m_theta;
    };

    void Vector_Polar::rotate(Vector_Polar vector) {
        m_theta += vector.getTheta();
    };
    void Vector_Polar::rotate(double theta) {
        m_theta += theta;
    };
    void Vector_Polar::normalize() {
       m_r = 1;
    };

    Vector_2D Vector_Polar::toCartesian() {
        return Vector_2D(
            m_r * std::cos(m_theta), 
            m_r * std::sin(m_theta)
        );
    };

    
    void platform_print(std::string stuff) {
#ifdef AUTOMAT_VEX_
        m_brain.Screen.print(stuff.c_str());
#endif
#ifdef AUTOMAT_ESP32_
        Serial.println(stuff.c_str());
#endif
    };
    
    double degreesToRadians(double angle) {
        return (angle / 180.0) * M_PI;
    };
    double radiansToDegrees(double angle) {
        return (angle / M_PI) * 180.0;
    };

    Timestamp::Timestamp() {

    };
#ifdef AUTOMAT_VEX_
    Timestamp::Timestamp(int milliseconds):
        m_milliseconds{ milliseconds }
    {

    }
#endif
#ifdef AUTOMAT_ESP32_
    Timestamp::Timestamp(unsigned long int milliseconds): // Not sure yet
        m_milliseconds{ milliseconds }
    {

    }
#endif
    Timestamp::~Timestamp() {

    };

    int Timestamp::getTimeDifferenceMS(Timestamp timestamp) {
        return std::abs(getTimeMS() - timestamp.getTimeMS());
    };
    int Timestamp::getTimeMS() {
#ifdef AUTOMAT_VEX_
        return m_milliseconds;
#endif
#ifdef AUTOMAT_ESP32_
        return static_cast<int>(m_milliseconds);
#endif
    };

    Timestamp getSystemTime() {
#ifdef AUTOMAT_VEX_
        return Timestamp(vex::timer::system());
#endif
#ifdef AUTOMAT_ESP32_
        return Timestamp(millis());
#endif
    };
    void systemWait(int milliseconds) {
#ifdef AUTOMAT_VEX_
        vex::wait(milliseconds, vex::msec);
#endif
#ifdef AUTOMAT_ESP32_
        delay(milliseconds);
#endif
    };


};