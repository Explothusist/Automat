
#include "utils.h"

#include <cmath>
#include <string>
#include <cctype>
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

    std::vector<std::string> splitString(const std::string& string, const std::string& delineator) {
        std::vector<std::string> output;
        if (delineator == "") {
            for (size_t i = 0; i < string.size(); i++) {
                output.push_back(std::string(1, string[i]));
            }
        }else {
            size_t start = 0;
            size_t end = 0;
            // std::string remaining = string;
            while (true) {
                end = string.find(delineator, start);
                if (end != std::string::npos) {
                    output.push_back(string.substr(start, end - start));
                }else {
                    break;
                }
                start = end + delineator.length();
            }
            output.push_back(string.substr(start));
        }
        return output;
    };
    std::string substrUntil(const std::string& string, const std::string& delineator) {
        size_t delin_loc = string.find(delineator);
        return delin_loc != std::string::npos ? string.substr(0, delin_loc) : string;
    };
    std::string substrUntilFirstOf(const std::string& string, const std::string& delineator1, const std::string& delineator2) {
        size_t delin_loc = string.find(delineator1);
        delin_loc = std::min(delin_loc, string.find(delineator2));
        return delin_loc != std::string::npos ? string.substr(0, delin_loc) : string;
    };
    std::string substrAfter(const std::string& string, const std::string& delineator) {
        size_t delin_loc = string.find(delineator);
        return delin_loc != std::string::npos ? string.substr(delin_loc + delineator.length()) : "";
    };
    std::string substrAfterFirstOf(const std::string& string, const std::string& delineator1, const std::string& delineator2) {
        size_t delin_loc = string.find(delineator1);
        size_t delin_loc2 = string.find(delineator2);
        size_t delin_length = delineator1.length();
        if (delin_loc2 < delin_loc) {
            delin_loc = delin_loc2;
            delin_length = delineator2.length();
        }
        return delin_loc != std::string::npos ? string.substr(delin_loc + delin_length) : "";
    };
    std::string substrBetween(const std::string& string, const std::string& delineator1, const std::string& delineator2) {
        size_t start = string.find(delineator1);
        if (start == std::string::npos) {
            return "";
        }
        start += delineator1.length();
        size_t end = string.find(delineator2, start);
        if (end == std::string::npos) {
            // return "";
            end = string.length();
        }
        return string.substr(start, end - start);
    };
    std::string trimWhitespace(const std::string& string) {
        size_t start = 0;
        while (start < string.length() && isspace((unsigned char) string[start])) {
            start += 1;
        }
        size_t end = string.length() - 1;
        while (end > start && isspace((unsigned char) string[end - 1])) {
            end -= 1;
        }
        return string.substr(start, end - start);
    };
    std::string trimTrailingCRLF(const std::string& string) {
        // if (string.size() >= 2 && string[string.size() - 2] == '\r' && string[string.size() - 1] == '\n') {
        //     return string.substr(0, string.size() - 2);
        // }
        // if (string.size() >= 1 && string[string.size() - 1] == '\n') {
        //     return string.substr(0, string.size() - 1);
        // }
        size_t end = string.size();
        while (end > 0 && (string[end-1] == '\r' || string[end-1] == '\n')) {
            end -= 1;
        }
        return string.substr(0, end);
    };
    inline int hexCharToInt(char c) {
        // Convert a single hex character to its numeric value
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'A' && c <= 'F') {
            return 10 + (c - 'A');
        }
        if (c >= 'a' && c <= 'f') {
            return 10 + (c - 'a');
        }
        return -1; // invalid hex
    }
    std::string urlDecode(const std::string& string) {
        std::string result;
        result.reserve(string.length());
        int code;
        for (size_t i = 0; i < string.length(); i++) {
            if (string[i] == '+') {
                result += ' ';
            }else if (string[i] == '%' && i + 2 < string.length()) {
                int high = hexCharToInt(string[i + 1]);
                int low  = hexCharToInt(string[i + 2]);
                if (high >= 0 && low >= 0) {
                    result += static_cast<char>((high << 4) | low);
                    i += 2; // skip over hex digits
                } else {
                    // malformed %, just copy as-is
                    result += '%';
                }
            }else {
                result += string[i];
            }
        }
        return result;
    };

};