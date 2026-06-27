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

    class EventHandler;

    // typedef struct {
    //     uint8_t prefix;
    //     uint8_t data[kMaxPacketSize - 1];
    // } PrefixedSerial;

#ifdef AUTOMAT_ESP32_ARDUINO_
    typedef enum {
        Interface_Serial0,
        Interface_Serial1,
        Interface_Serial2//,
        // Interface_Serial3
    } SerialInterface;
#endif

    void SetReadSerialEvents(bool to_read);

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

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            void bindToMessage(Trigger* trigger, Command* command);
            void bindAutoTrigger(Trigger* trigger);
            void bindTeleopTrigger(Trigger* trigger);

            void triggerEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length, int id);
#endif
        
            // Expose through protocol
            bool availableMessages();
            bool popNextMessage();
            bool popNextMessage(uint8_t output[], uint8_t &length);
            bool popNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender);
            bool popNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length);
            bool popNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekNextMessage(uint8_t output[], uint8_t &length);
            bool peekNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length);
            bool peekNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekNextMessagePrefix(uint8_t &prefix);
            
            // Expose through protocol
            bool getNextMessageId(int &id);
            bool getMessageId(int index, int &id);
            int availableMessagesCount();

            // Expose through protocol
            bool popMessage(int id);
            bool popMessage(int id, uint8_t output[], uint8_t &length);
            bool popMessage(int id, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool popMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length);
            bool popMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekMessage(int id, uint8_t output[], uint8_t &length);
            bool peekMessage(int id, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length);
            bool peekMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekMessagePrefix(int id, uint8_t &prefix);

            // Expose through protocol
            bool sendMessage(uint8_t recipient_code, uint8_t message, int copies = 1);
            bool sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length, int copies = 1);
            bool sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message, int copies = 1);
            bool sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length, int copies = 1);
            bool sendMessageAll(uint8_t message, int copies = 1);
            bool sendMessageAll(uint8_t message[], uint8_t length, int copies = 1);
            bool sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message, int copies = 1);
            bool sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message[], uint8_t length, int copies = 1);

            // Expose through protocol
            void sendByte(uint8_t byte);
            void flushMessages();

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
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            std::vector<Trigger_Event*> m_triggers;
            std::vector<Trigger_Event*> m_temp_triggers;
            RobotState* m_robot_state;
            // TimedRobot* m_robot;
            EventHandler* m_event_handler;
#endif
            PacketHandler m_packet_handler;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif