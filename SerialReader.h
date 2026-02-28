
#ifndef AUTOMAT_SERIAL_READER_
#define AUTOMAT_SERIAL_READER_

#include <queue>
#include "automat_platform.h"
#include "Subsystem.h"
#include "Trigger.h"

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

    typedef struct {
        std::shared_ptr<uint8_t[]> data;
        uint8_t length;
    } serial_message;



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

            void interpretMessages();
            void resetPartialMessage();
            bool manageSpecial(uint8_t code);
            bool isSpecial(uint8_t code);

            bool availableMessages();
            bool getNextMessage(std::shared_ptr<uint8_t[]> &output, uint8_t &length);
            // void destroyMessage(std::shared_ptr<uint8_t[]> output, uint8_t &length);
            bool sendMessage(uint8_t* message, uint8_t length);
            void flushMessages();

            void bindToMessage(Trigger* trigger, Command* command);
            void bindAutoTrigger(Trigger* trigger);

            void interpretTrigger(Trigger_Event* trigger, bool is_stick);

            std::vector<Command*> pollEvents();
            std::vector<int> pollEventTerminations();
            bool pollAutonomousTriggers();

        private:
#ifdef AUTOMAT_VEX_
            vex::motor* m_fake_motor;
            int m_port;
            int m_index;
#endif
            std::queue<uint8_t> m_raw_input;
            // std::queue<uint8_t> m_partial_message;
            std::queue<serial_message> m_messages;
            std::queue<uint8_t> m_to_send;

            bool m_part_has_start;
            int m_part_length;
            std::shared_ptr<uint8_t[]> m_part_data;
            int m_part_datas_input;
            int m_part_checksum;
            bool m_part_has_end;

            bool m_part_next_char_escaped;
    };

}

#endif