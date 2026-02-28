
#ifndef AUTOMAT_SERIAL_READER_
#define AUTOMAT_SERIAL_READER_

#include <queue>
#include <cstdint>
#include "automat_platform.h"
#include "Subsystem.h"
#include "Trigger.h"
#include "utils.h"

#ifdef AUTOMAT_VEX_
#include "vex.h"
#endif

/*
    Packet format:
        START -> LEN -> DATA ... DATA -> CHKSUM -> END
    
        START:  0xfd
        CHKSUM: DATA+DATA+...+DATA % 256
        END:    0xfc
*/

namespace atmt {

    class EventHandler;

    void SetReadSerialEvents(bool to_read);

    class SerialReader : public Subsystem {
        public:
#ifdef AUTOMAT_VEX_
            SerialReader(int port);
#endif
#ifdef AUTOMAT_ESP32_
            SerialReader();
#endif
            ~SerialReader() override;

            void init() override;
            void periodic() override;

            void internal_init(RobotState* robot_state, EventHandler* event_handler);

            void interpretMessages();
            void addInterpretedMessage(serial_message message);
            void resetPartialMessage();
            bool manageSpecial(uint8_t code);
            bool isSpecial(uint8_t code);
            bool checkIfMatching(const serial_message &duplicate, const serial_message &original);

            bool availableMessages();
            bool getNextMessage(uint8_t output[], uint8_t &length);
            // void destroyMessage(std::shared_ptr<uint8_t[]> output, uint8_t &length);
            bool sendMessage(uint8_t message[], uint8_t length);
            bool sendMessage(uint8_t message[], uint8_t length, int duplicates);
            void flushMessages();

            void bindToMessage(Trigger* trigger, Command* command);
            void bindAutoTrigger(Trigger* trigger);

            void triggerEvent(SerialEvent event, uint8_t code[], uint8_t length);

            std::vector<Command*> pollEvents();
            std::vector<int> pollEventTerminations();
            bool pollAutonomousTriggers();

        private:
            std::vector<Trigger_Event*> m_triggers;
            std::vector<Trigger_Event*> m_temp_triggers;

#ifdef AUTOMAT_VEX_
            vex::motor* m_fake_motor;
            int m_port;
            int m_index;
#endif
            std::queue<uint8_t> m_raw_input;
            // std::queue<uint8_t> m_partial_message;
            std::queue<serial_message> m_messages;
            std::queue<uint8_t> m_to_send;

            serial_message m_last_message;

            bool m_part_has_start;
            bool m_part_is_duplicate;
            int m_part_length;
            uint8_t m_part_data[kMaxPacketSize];
            int m_part_datas_input;
            int m_part_checksum;
            bool m_part_has_end;

            bool m_part_next_char_escaped;
            
            RobotState* m_robot_state;
            // TimedRobot* m_robot;
            EventHandler* m_event_handler;
    };

}

#endif