
#ifndef AUTOMAT_UTILS_
#define AUTOMAT_UTILS_

#include "automat_platform.h"
#include <string>
#include <cstdint>
#include <vector>
#include <deque>

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

namespace atmt {

    typedef enum {
        Disabled = 0,
        Autonomous = 1,
        Teleop = 2
    } RobotState;

#ifdef AUTOMAT_VEX_
    extern vex::competition m_competition;
    extern vex::brain m_brain;
#endif

    void platform_clear_screen();
    void platform_print(std::string stuff);
    void platform_println(std::string stuff);
    void platform_print(int stuff);
    void platform_println(int stuff);
    void platform_print(double stuff);
    void platform_println(double stuff);

    double degreesToRadians(double angle);
    double radiansToDegrees(double angle);

    class Timestamp {
        public:
            Timestamp();
#ifdef AUTOMAT_VEX_
            Timestamp(int milliseconds); // vex::timer::system()
#endif
#ifdef AUTOMAT_ESP32_
            Timestamp(unsigned long int milliseconds);
#endif
            ~Timestamp();

            bool operator>(const Timestamp &timestamp) const;
            bool operator>=(const Timestamp &timestamp) const;
            bool operator<(const Timestamp &timestamp) const;
            bool operator<=(const Timestamp &timestamp) const;

            int getTimeDifferenceMS(Timestamp timestamp) const; // milliseconds
            double getTimeDifference(Timestamp timestamp) const; // seconds
            int getTimeMS() const; // milliseconds
            double getTime() const; // seconds

            Timestamp add(Timestamp timestamp);
            Timestamp addMS(int ms);
        private:
#ifdef AUTOMAT_VEX_
            int m_milliseconds;
#endif
#ifdef AUTOMAT_ESP32_
            unsigned long int m_milliseconds;
#endif
    };

    Timestamp getSystemTime();
    void systemWait(int milliseconds);

    double getProportional(double value, double input_min, double input_max, double output_min, double output_max);
    
    
    constexpr int kBaudrate = 115200;
    constexpr int kMaxMessagesPerFrame = 256;
    constexpr uint8_t kMaxPacketSize = 128;

#ifdef AUTOMAT_ESP32_
    constexpr int kRXDefaultPin = 16;
    constexpr int kTXDefaultPin = 17;
    constexpr int kUARTDefaultBufferSize = 1024;  // RX/TX buffer size
    constexpr int kUARTDefaultPort = 2;  // uart_port_t kUARTDefaultPort = UART_NUM_2
#endif

    typedef struct {
        uint8_t data[kMaxPacketSize];
        uint8_t length;
        uint8_t sender;
        int id;
    } serial_message;

    enum class SerialMessage : uint8_t {
        // Get_Status = 0xfb,
        // Status_Processing = 0xfc,
        // Status_Complete = 0xfd,
        StartDuplicate = 0xfc,
        Escape = 0xfd,
        End = 0xfe,
        Start = 0xff,
        // Invalid = 0xfe,
        // Error = 0xff
    };
    constexpr uint8_t KSerialAddressSendAll = 0xff;
    
    // Drop-in std::clamp replacement for C++11/C++14
    template <typename Type>
    constexpr const Type& clamp(const Type& value, const Type& low, const Type& high) {
        return (value < low) ? low : (high < value) ? high : value;
    }

    constexpr double kZeroEpsilon = 1e-12;

    typedef struct {
        std::string name;
        std::string data;
    } POSTInfo;

    typedef enum {
        Method_Get,
        Method_Post
    } atmtHTTPMethod;
    typedef enum {
        HTTP_OK,
        HTTP_FAIL
    } atmtHTTPError;

    std::vector<std::string> splitString(const std::string& string, const std::string& delineator);
    std::string substrUntil(const std::string& string, const std::string& delineator);
    std::string substrUntilFirstOf(const std::string& string, const std::string& delineator1, const std::string& delineator2);
    std::string substrAfter(const std::string& string, const std::string& delineator);
    std::string substrAfterFirstOf(const std::string& string, const std::string& delineator1, const std::string& delineator2);
    std::string substrBetween(const std::string& string, const std::string& delineator1, const std::string& delineator2);
    std::string trimWhitespace(const std::string& string);
    std::string trimTrailingCRLF(const std::string& string);
    inline int hexCharToInt(char c);
    std::string urlDecode(const std::string& string);

    /*
        Preserves ordering, O(n)
        If ordering does not matter, see vectorDeleteUnordered
    */
    template <typename Type>
    void vectorDelete(std::vector<Type> &vect, std::size_t i) {
        if (i < vect.size()) {
            if (i == vect.size()-1) {
                vect.pop_back();
            }else {
                vect.erase(vect.begin() + i);
            }
        }
    };
    /*
        Does not preserve ordering, O(1)
        If ordering matters, see vectorDelete
    */
    template <typename Type>
    void vectorDeleteUnordered(std::vector<Type> &vect, std::size_t i) {
        if (i < vect.size()) {
            if (i != vect.size()-1) {
                std::swap(vect[i], vect.back());
            }
            vect.pop_back();
        }
    };
    
    /*
        Preserves ordering, O(n)
        If ordering does not matter, see vectorDeleteUnordered
    */
    template <typename Type>
    void dequeDelete(std::deque<Type> &vect, std::size_t i) {
        if (i < vect.size()) {
            if (i == vect.size()-1) {
                vect.pop_back();
            }else {
                vect.erase(vect.begin() + i);
            }
        }
    };
    /*
        Does not preserve ordering, O(1)
        If ordering matters, see vectorDelete
    */
    template <typename Type>
    void dequeDeleteUnordered(std::deque<Type> &vect, std::size_t i) {
        if (i < vect.size()) {
            if (i != vect.size()-1) {
                std::swap(vect[i], vect.back());
            }
            vect.pop_back();
        }
    };

};

#endif