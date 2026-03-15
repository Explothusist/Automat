
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

#ifdef AUTOMAT_VEX_
    int m_line_on_screen = 0;
#endif
    
    void platform_clear_screen() {
#ifdef AUTOMAT_VEX_
        m_brain.Screen.clearScreen();
        m_brain.Screen.setCursor(1, 1);
        m_line_on_screen = 0;
#endif
#ifdef AUTOMAT_ESP32_
#endif
    };
    void platform_print(std::string stuff) {
#ifdef AUTOMAT_VEX_
        m_brain.Screen.print(stuff.c_str());
#endif
#ifdef AUTOMAT_ESP32_
        Serial.println(stuff.c_str());
#endif
    };
    void platform_println(std::string stuff) {
#ifdef AUTOMAT_VEX_
        m_line_on_screen += 1;
        if (m_line_on_screen > 12) { // Nevermind, this is FAR worse
            m_line_on_screen = 1;
            m_brain.Screen.clearScreen();
            m_brain.Screen.setCursor(1, 1);
        }

        m_brain.Screen.print(stuff.c_str());
        m_brain.Screen.newLine(); // This is horrendous...
#endif
#ifdef AUTOMAT_ESP32_
        Serial.println((stuff + "\n").c_str());
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
    double Timestamp::getTimeDifference(Timestamp timestamp) {
        return static_cast<double>(std::abs(getTimeMS() - timestamp.getTimeMS())) * 0.001;
    };
    int Timestamp::getTimeMS() {
#ifdef AUTOMAT_VEX_
        return m_milliseconds;
#endif
#ifdef AUTOMAT_ESP32_
        return static_cast<int>(m_milliseconds);
#endif
    };
    double Timestamp::getTime() {
        return static_cast<double>(getTimeMS()) * 0.001;
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

    
    double getProportional(double value, double input_min, double input_max, double output_min, double output_max) {
        double abs_value = std::abs(value);
        double sign = (value >= 0) ? 1 : -1;
        double output = 0.0;
        if (abs_value < input_min) {
            output = 0.0;
        }else if (abs_value > input_max) {
            output = output_max;
        }else {
            double proportion = (abs_value - input_min) / (input_max - input_min);
            output = output_min + (proportion * (output_max - output_min));
        }
        return output * sign;
    };

};