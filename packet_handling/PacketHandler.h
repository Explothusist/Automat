#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_PACKET_HANDLING_

#ifndef AUTOMAT_PACKET_HANDLER_
#define AUTOMAT_PACKET_HANDLER_

#include <queue>
#include <deque>
#include <cstdint>
#include "../automat_platform.h"
#include "../utils.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/Subsystem.h"
#include "../command_based/Trigger.h"
#endif

/*
    Packet format:
        START -> LEN -> SENDER -> ADDRESS -> ID -> DATA ... DATA -> CHKSUM -> END
    
        START:  0xfd
        SENDER: Address of sender
        ADDRESS: Intended recipient
        ID: Id set by sender to identify duplicate packets (can also be used to order packets)
        CHKSUM: LEN+SENDER+ADDRESS+ID+DATA+DATA+...+DATA % 256
        END:    0xfc
*/

/*
    For use with a protocol:

        Outside API --- Protocol --- Packet Handler

           Send     -->  (Pass)  -->  Send Methods
                                           |
                        Protocol <--  Byte Stream
                           |
                     (Transmission)
                           |
                        Protocol --> Add Raw Input
                                           |
          Receive   -->  (Pass)  <--    Parsed

*/

namespace atmt {

    enum class ParsingState {
        FindStart,
        FindLength,
        FindSender,
        FindAddress,
        FindId,
        FindData,
        FindChecksum,
        FindEnd,
    };

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    class PacketHandler : public Subsystem {
#else
    class PacketHandler {
#endif
        public:
            PacketHandler(uint8_t address_code);
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
            ~PacketHandler() override;

            void init() override;
            void systemPeriodic() override;
            void disabledPeriodic() override;
            void autonomousPeriodic() override;
            void teleopPeriodic() override;

            void internal_init();
#else
            ~PacketHandler();

            void init();
#endif
            void periodic();

            // For use by protocol, bytes to send
            bool hasRawBytesToSend();
            bool getNextRawByteToSend(uint8_t &raw_byte);
            bool peekNextRawByteToSend(uint8_t &raw_byte);
            int getAllRawBytesToSend(uint8_t byte_buffer[], int buffer_length);

            // For use by protocol, bytes received
            void inputReceivedRawByte(uint8_t raw_byte);
            void inputReceivedRawBytes(uint8_t byte_buffer[], int buffer_length);

            // For internal use
            void interpretMessages();
            void addInterpretedMessage(SerialMessage message);
            void resetPartialMessage();
            bool manageSpecial(uint8_t code);
            bool isSpecial(uint8_t code);
            void updateLastFromSender(const SerialMessage &message);
            bool checkIfMatchesLastFromSender(const SerialMessage &message);
            bool checkIfMatching(const SerialMessage &duplicate, const SerialMessage &original);

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
            uint8_t m_address_code;

            std::queue<uint8_t> m_raw_input;
            // std::queue<uint8_t> m_partial_message;
            std::deque<SerialMessage> m_messages;
            std::queue<uint8_t> m_to_send;

            std::vector<SerialMessage> m_last_messages; // Las messages from each sender

            ParsingState m_parsing_state;
            // bool m_part_has_start;
            bool m_part_is_duplicate;
            uint8_t m_part_sender;
            uint8_t m_part_address;
            uint8_t m_part_id;
            uint8_t m_part_length;
            uint8_t m_part_data[kMaxPacketSize];
            uint8_t m_part_datas_input;
            uint8_t m_part_checksum;
            // bool m_part_has_end;

            bool m_part_next_char_escaped;

            uint8_t m_sent_message_id_counter;
            int m_received_message_id_counter;
            
            // Internal use
            bool sendMessageInternal(uint8_t recipient_code, uint8_t message_prefix, bool with_prefix, uint8_t message_singleton, bool with_singleton, uint8_t message[], uint8_t length, int copies);
            int findMessageIndex(int id);
            bool popMessageInternal(int index);
            bool peekMessageInternal(int index, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekMessagePrefixedInternal(int index, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender);
            bool peekMessagePrefixInternal(int index, uint8_t &prefix);

            // // Internal use
            // uint8_t computeChecksum(uint8_t length, uint8_t address, uint8_t sender, uint8_t sent_id, uint8_t data[]); // Unused right now
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_PACKET_HANDLING_ in automat_submodules.h to use PacketHandler"
#endif