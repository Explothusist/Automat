#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_SERIAL_

#ifndef AUTOMAT_SERIAL_READER_
#define AUTOMAT_SERIAL_READER_

#include <queue>
#include <deque>
#include <cstdint>
#include "../automat_platform.h"
#include "../utils.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Subsystem.h"
#include "../command_based/Trigger.h"
#endif

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

/*
    Packet format:
        START -> LEN -> SENDER -> ADDRESS -> DATA ... DATA -> CHKSUM -> END
    
        START:  0xfd
        SENDER: Address of sender
        ADDRESS: Intended recipient
        CHKSUM: LEN+SENDER+ADDRESS+DATA+DATA+...+DATA % 256
        END:    0xfc
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

            void interpretMessages();
            void addInterpretedMessage(serial_message message);
            void resetPartialMessage();
            bool manageSpecial(uint8_t code);
            bool isSpecial(uint8_t code);
            bool checkIfMatching(const serial_message &duplicate, const serial_message &original);

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
            
            int getNextMessageId();
            int getMessageId(int index);
            int availableMessagesCount();

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

            bool sendMessage(uint8_t recipient_code, uint8_t message, int copies = 1);
            bool sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length, int copies = 1);
            bool sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message, int copies = 1);
            bool sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length, int copies = 1);
            bool sendMessageAll(uint8_t message[], uint8_t length, int copies = 1);
            bool sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message[], uint8_t length, int copies = 1);

            void sendByte(uint8_t byte);
            void flushMessages();

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            void bindToMessage(Trigger* trigger, Command* command);
            void bindAutoTrigger(Trigger* trigger);
            void bindTeleopTrigger(Trigger* trigger);

            void triggerEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length, int id);
#endif

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
            uint8_t m_address_code;

            std::queue<uint8_t> m_raw_input;
            // std::queue<uint8_t> m_partial_message;
            std::deque<serial_message> m_messages;
            std::queue<uint8_t> m_to_send;

            serial_message m_last_message;

            bool m_part_has_start;
            bool m_part_is_duplicate;
            int m_part_sender;
            int m_part_address;
            int m_part_length;
            uint8_t m_part_data[kMaxPacketSize];
            int m_part_datas_input;
            int m_part_checksum;
            bool m_part_has_end;

            bool m_part_next_char_escaped;

            int m_message_id_counter;
            
            bool sendMessageInternal(uint8_t recipient_code, uint8_t message_prefix, bool with_prefix, uint8_t message_singleton, bool with_singleton, uint8_t message[], uint8_t length, int copies);
            int findMessageIndex(int id);
            bool popMessageInternal(int index);
            bool peekMessageInternal(int index, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekMessagePrefixedInternal(int index, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekMessagePrefixInternal(int index, uint8_t &prefix);

            uint8_t computeChecksum(uint8_t length, uint8_t address, uint8_t sender, uint8_t data[]); // Unused right now
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif