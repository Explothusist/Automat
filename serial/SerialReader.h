#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_SERIAL_

#ifndef AUTOMAT_SERIAL_READER_
#define AUTOMAT_SERIAL_READER_

#include <queue>
#include <deque>
#include <cstdint>
#include "../automat_platform.h"
#include "../utils.h"
#include "../packet_handling/PacketHandler.h"
#include "../packet_handling/PacketEventHandler.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Subsystem.h"
#include "../command_based/Trigger.h"
#endif

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

/*
    See PacketHandler for packet format and details, dataflow outline, etc.
*/

namespace atmt {

#ifdef AUTOMAT_ESP32_ARDUINO_
    typedef enum {
        Interface_Serial0,
        Interface_Serial1,
        Interface_Serial2
    } SerialInterface;
#endif

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    class SerialReader : public Subsystem {
#else
    class SerialReader {
#endif
        public:
#ifdef AUTOMAT_VEX_
            SerialReader(uint8_t address_code, int port);
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            SerialReader(SerialInterface serial_interface, uint8_t address_code);
            SerialReader(SerialInterface serial_interface, uint8_t address_code, int rx_pin, int tx_pin);
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
            SerialReader(uint8_t address_code);
            SerialReader(uint8_t address_code, int rx_pin, int tx_pin);
            SerialReader(uint8_t address_code, int rx_pin, int tx_pin, int buffer_size);
            SerialReader(uint8_t address_code, int rx_pin, int tx_pin, int buffer_size, int uart_port);
#endif
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            ~SerialReader() override;

            void init() override;
            void systemPeriodic() override;
            void disabledPeriodic() override;
            void autonomousPeriodic() override;
            void teleopPeriodic() override;

            void internal_init(RobotState* robot_state, EventHandler* event_handler);
#else
            ~SerialReader();

            void init();
#endif
            void periodic();
        
            PacketHandler packet;
            PacketEventHandler event;
        private:
#ifdef AUTOMAT_VEX_
            vex::motor* m_fake_motor;
            int m_port;
            int m_index;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
            SerialInterface m_serial_interface;
            int m_rx_pin;
            int m_tx_pin;
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
            int m_rx_pin;
            int m_tx_pin;
            int m_buffer_size;
            int m_uart_port;
#endif
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif