#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_SERIAL_

#ifndef AUTOMAT_SERIAL_READER_
#define AUTOMAT_SERIAL_READER_

#include <queue>
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
            SerialReader(uint8_t address_code);
            SerialReader(uint8_t address_code, int rx_pin, int tx_pin);
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
            void periodic() override;

            void internal_init(RobotState* robot_state, EventHandler* event_handler);
#else
            ~SerialReader();

            void init();
            void periodic();
#endif

            void interpretMessages();
            void addInterpretedMessage(serial_message message);
            void resetPartialMessage();
            bool manageSpecial(uint8_t code);
            bool isSpecial(uint8_t code);
            bool checkIfMatching(const serial_message &duplicate, const serial_message &original);

            bool availableMessages();
            bool getNextMessage(uint8_t output[], uint8_t &length);
            bool getNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender);
            bool getNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length);
            bool getNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender);
            // void destroyMessage(std::shared_ptr<uint8_t[]> output, uint8_t &length);
            // bool sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length);
            bool sendMessage(uint8_t recipient_code, uint8_t message, int copies = 1);
            bool sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length, int copies = 1);
            // bool sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length);
            bool sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length, int copies = 1);
            // bool sendMessageAll(uint8_t message[], uint8_t length);
            bool sendMessageAll(uint8_t message[], uint8_t length, int duplicates = 1);
            bool sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message[], uint8_t length, int duplicates = 1);
            void sendByte(uint8_t byte);
            void flushMessages();

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            void bindToMessage(Trigger* trigger, Command* command);
            void bindAutoTrigger(Trigger* trigger);

            void triggerEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length);
#endif

        private:
#ifdef AUTOMAT_VEX_
            vex::motor* m_fake_motor;
            int m_port;
            int m_index;
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
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
            std::queue<serial_message> m_messages;
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
            
            bool sendMessageInternal(uint8_t recipient_code, uint8_t message_prefix, bool with_prefix, uint8_t message[], uint8_t length, int duplicates);
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif