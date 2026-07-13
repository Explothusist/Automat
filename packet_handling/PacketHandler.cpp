#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_PACKET_HANDLING_

#include "PacketHandler.h"
#include "PacketEventHandler.h"

#include <cstring>
#include <string>

namespace atmt {

    PacketHandler::PacketHandler(uint8_t address_code):
        m_address_code{ address_code },
        m_raw_input{ },
        m_messages{ },
        m_to_send{ },

        // m_last_message{ {0}, 0, 0 },
        m_last_messages{ },

        m_parsing_state{ ParsingState::FindStart },
        // m_part_has_start{ false },
        m_part_is_duplicate{ false },
        m_part_sender{ 0 },
        m_part_address{ 0 },
        m_part_id{ 0 },
        m_part_length{ 0 },
        m_part_data{ },
        m_part_datas_input{ 0 },
        m_part_checksum{ 0 },
        // m_part_has_end{ false },
        m_part_next_char_escaped{ false },
        
        m_sent_message_id_counter{ 0 },
        m_received_message_id_counter{ 0 },
        m_packet_event_handler{ nullptr }
    {
        if (m_address_code == KSerialAddressSendAll) {
            platform_print("Cannot Set Address to 0xFF (Reserved for SendMessageAll)");
            m_address_code = 0;
        }
    };
    PacketHandler::~PacketHandler() {
        
    };

    void PacketHandler::init() {
        
    };
    void PacketHandler::periodic() {
        interpretMessages();
    };
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void PacketHandler::systemPeriodic() {
        periodic();
    };
    void PacketHandler::disabledPeriodic() {};
    void PacketHandler::autonomousPeriodic() {};
    void PacketHandler::teleopPeriodic() {};
#endif
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void PacketHandler::internal_init(PacketEventHandler* event_handler) {
        m_packet_event_handler = event_handler;
    };
#endif

    // For use by protocol, bytes to send
    bool PacketHandler::hasRawBytesToSend() {
        return !m_to_send.empty();
    };
    bool PacketHandler::getNextRawByteToSend(uint8_t &raw_byte) {
        if (!hasRawBytesToSend()) return false;
        raw_byte = m_to_send.front();
        m_to_send.pop();
        return true;
    };
    bool PacketHandler::peekNextRawByteToSend(uint8_t &raw_byte) {
        if (!hasRawBytesToSend()) return false;
        raw_byte = m_to_send.front();
        return true;
    };
    int PacketHandler::getAllRawBytesToSend(uint8_t byte_buffer[], int buffer_length) {
        int count = 0;
        while (hasRawBytesToSend() && count < buffer_length) {
            getNextRawByteToSend(byte_buffer[count]);
            count += 1;
        }
        return count;
    };
    int PacketHandler::peekAllRawBytesToSend(uint8_t byte_buffer[], int buffer_length) {
        int count = 0;
        std::queue<uint8_t> copy = m_to_send;
        while (!copy.empty() && count < buffer_length) {
            byte_buffer[count] = copy.front();
            copy.pop();
            count += 1;
        }
        return count;
    };

    // For use by protocol, bytes received
    void PacketHandler::inputReceivedRawByte(uint8_t raw_byte) {
        m_raw_input.push(raw_byte);
    };
    void PacketHandler::inputReceivedRawBytes(const uint8_t byte_buffer[], int buffer_length) {
        for (int i = 0; i < buffer_length; i++) {
            inputReceivedRawByte(byte_buffer[i]);
        }
    };

    void PacketHandler::interpretMessages() {
        while (!m_raw_input.empty()) {
            switch (m_parsing_state) {
                case ParsingState::FindStart:
                    if (m_raw_input.front() == static_cast<uint8_t>(SerialFlags::Start)) { // 0xff
                        m_parsing_state = ParsingState::FindLength;
                        m_part_is_duplicate = false;
                    }else if (m_raw_input.front() == static_cast<uint8_t>(SerialFlags::StartDuplicate)) { // 0xfc
                        m_parsing_state = ParsingState::FindLength;
                        m_part_is_duplicate = true;
                    }
                    m_raw_input.pop();
                    break;
                case ParsingState::FindLength:
                    if (!manageSpecial(m_raw_input.front())) {
                        m_part_length = m_raw_input.front();
                        m_parsing_state = ParsingState::FindSender;
                        if (m_part_length > kMaxPacketSize) {
                            resetPartialMessage();
                        }
                    }
                    m_raw_input.pop();
                    break;
                case ParsingState::FindSender:
                    if (!manageSpecial(m_raw_input.front())) {
                        m_part_sender = m_raw_input.front();
                        m_parsing_state = ParsingState::FindAddress;
                    }
                    m_raw_input.pop();
                    break;
                case ParsingState::FindAddress:
                    if (!manageSpecial(m_raw_input.front())) {
                        m_part_address = m_raw_input.front();
                        m_parsing_state = ParsingState::FindId;
                    }
                    m_raw_input.pop();
                    break;
                case ParsingState::FindId:
                    if (!manageSpecial(m_raw_input.front())) {
                        m_part_id = m_raw_input.front();
                        m_parsing_state = ParsingState::FindData;
                    }
                    m_raw_input.pop();
                    break;
                case ParsingState::FindData:
                    if (!manageSpecial(m_raw_input.front())) {
                        m_part_data[m_part_datas_input] = m_raw_input.front();
                        m_part_datas_input += 1;
                    }
                    if (m_part_datas_input >= m_part_length) {
                        m_parsing_state = ParsingState::FindChecksum;
                    }
                    m_raw_input.pop();
                    break;
                case ParsingState::FindChecksum:
                    if (!manageSpecial(m_raw_input.front())) {
                        m_part_checksum = m_raw_input.front();
                        m_parsing_state = ParsingState::FindEnd;
                        uint8_t checksum = 0;
                        for (int i = 0; i < m_part_length; i++) {
                            checksum += m_part_data[i]; // Rollover handled automatically
                        }
                        checksum += m_part_length;
                        checksum += m_part_sender;
                        checksum += m_part_address;
                        checksum += m_part_id;
                        if (m_part_checksum != checksum) {
                            resetPartialMessage();
                        }
                    }
                    m_raw_input.pop();
                    break;
                case ParsingState::FindEnd:
                    if (m_raw_input.front() == static_cast<uint8_t>(SerialFlags::End)) { // 0xfc
                        // m_part_has_end = true;
                        m_raw_input.pop();
                        SerialMessage message;
                        message.length = m_part_length;
                        message.sender = m_part_sender;
                        message.sent_id = m_part_id;
                        memcpy(message.data, m_part_data, m_part_length);
                        if (m_part_address == m_address_code || m_part_address == KSerialAddressSendAll) {
                            if (!m_part_is_duplicate) {
                                addInterpretedMessage(message);
                            }else {
                                // SerialMessage original = m_messages.back();
                                // if (!checkIfMatching(message, m_last_message)) { // If it is not a duplicate of the last packet
                                if (!checkIfMatchesLastFromSender(message)) { // If it is not a duplicate of the last packet
                                    addInterpretedMessage(message);
                                } // else: we did receive the original packet, so no need to recognize the duplicate
                            }
                        }
                    }
                    resetPartialMessage();
                    break;
                default: // Should never happen
                    resetPartialMessage();
                    break;
            }
        }
    };
    void PacketHandler::addInterpretedMessage(SerialMessage message) {
        message.id = m_received_message_id_counter;
        m_received_message_id_counter += 1;
        m_messages.push_back(message);
        // m_last_message = message;
        updateLastFromSender(message);
        if (m_packet_event_handler) {
            m_packet_event_handler->triggerEvent(SerialReceive, message.sender, message.data, message.length, message.id);
        }
    };
    void PacketHandler::resetPartialMessage() {
        m_parsing_state = ParsingState::FindStart;
        // // m_part_has_start = false;
        m_part_is_duplicate = false;
        // m_part_sender = -1;
        // m_part_address = -1;
        // m_part_length = -1;
        m_part_datas_input = 0;
        // m_part_checksum = -1;
        // // m_part_has_end = false;
        m_part_next_char_escaped = false;
    };
    bool PacketHandler::manageSpecial(uint8_t code) {
        if (m_part_next_char_escaped) {
            m_part_next_char_escaped = false;
            return false;
        }
        if (code == static_cast<uint8_t>(SerialFlags::Start)) { // 0xfd
            resetPartialMessage();
            // m_part_has_start = true;
            m_parsing_state = ParsingState::FindLength; // Because the byte will be eaten
            m_part_is_duplicate = false;
            return true;
        }else if (code == static_cast<uint8_t>(SerialFlags::StartDuplicate)) {
            resetPartialMessage();
            // m_part_has_start = true;
            m_parsing_state = ParsingState::FindLength; // Because the byte will be eaten
            m_part_is_duplicate = true;
            return true;
        }else if (code == static_cast<uint8_t>(SerialFlags::End)) {
            resetPartialMessage();
            return true;
        }else if (code == static_cast<uint8_t>(SerialFlags::Escape)) {
            m_part_next_char_escaped = true;
            return true;
        }
        return false;
    };
    bool PacketHandler::isSpecial(uint8_t code) {
        return code == static_cast<uint8_t>(SerialFlags::Start)
            || code == static_cast<uint8_t>(SerialFlags::StartDuplicate)
            || code == static_cast<uint8_t>(SerialFlags::End)
            || code == static_cast<uint8_t>(SerialFlags::Escape);
    };
    void PacketHandler::updateLastFromSender(const SerialMessage &message) {
        for (SerialMessage &old_message : m_last_messages) {
            if (old_message.sender == message.sender) {
                old_message = message;
                return;
            }
        }
        m_last_messages.push_back(message);
    };
    bool PacketHandler::checkIfMatchesLastFromSender(const SerialMessage &message) {
        for (const SerialMessage &old_message : m_last_messages) {
            if (old_message.sender == message.sender) {
                return checkIfMatching(message, old_message);
            }
        }
        return false;
    };
    bool PacketHandler::checkIfMatching(const SerialMessage &duplicate, const SerialMessage &original) {
        if (duplicate.sender != original.sender) {
            return false;
        }
        return duplicate.sent_id == original.sent_id;
    };

    bool PacketHandler::availableMessages() {
        return (!m_messages.empty());
    };
    bool PacketHandler::popNextMessage() {
        if (availableMessages()) {
            m_messages.pop_front();
            return true;
        }
        return false;
    };
    bool PacketHandler::popNextMessage(uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return popNextMessage(output, length, sender);
    };
    bool PacketHandler::popNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender) {
        bool success = peekNextMessage(output, length, sender);
        if (success) {
            return popNextMessage();
        }
        return false;
    };
    bool PacketHandler::popNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return popNextMessagePrefixed(prefix, output, length, sender);
    };
    bool PacketHandler::popNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        bool success = peekNextMessagePrefixed(prefix, output, length, sender);
        if (success) {
            return popNextMessage();
        }
        return false;
    };
    bool PacketHandler::peekNextMessage(uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return peekNextMessage(output, length, sender);
    };
    bool PacketHandler::peekNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender) {
        if (!availableMessages()) {
            return false;
        }
        const SerialMessage &message = m_messages.front();
        length = message.length;
        sender = message.sender;
        memcpy(output, message.data, length);
        return true;
    };
    bool PacketHandler::peekNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return peekNextMessagePrefixed(prefix, output, length, sender);
    };
    bool PacketHandler::peekNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        if (!availableMessages()) {
            return false;
        }
        const SerialMessage &message = m_messages.front();
        if (message.length == 0) {
            return false;
        }
        prefix = message.data[0];
        length = message.length - 1;
        sender = message.sender;
        memcpy(output, message.data + 1, length);
        return true;
        // // uint8_t data[kMaxPacketSize];
        // bool success = peekNextMessage(output, length, sender);
        // if (!success) {
        //     return false;
        // }
        // if (length == 0) {
        //     return false;
        // }
        // prefix = output[0];
        // // output = output + 1;
        // length -= 1;
        // memmove(output, output + 1, length);
        // return true;
    };
    bool PacketHandler::peekNextMessagePrefix(uint8_t &prefix) {
        if (!availableMessages()) {
            return false;
        }
        const SerialMessage &message = m_messages.front();
        if (message.length == 0) {
            return false;
        }
        prefix = message.data[0];
        return true;
    };

    bool PacketHandler::getNextMessageId(int &id) {
        if (m_messages.empty()) return false;
        id = m_messages.front().id;
        return true;
    };
    bool PacketHandler::getMessageId(int index, int &id) {
        if (m_messages.size() <= index || index < 0) return false;
        id = m_messages[index].id;
        return true;
    };
    int PacketHandler::availableMessagesCount() {
        return m_messages.size();
    };

    bool PacketHandler::popMessageInternal(int index) {
        if (index < 0 || index >= m_messages.size()) {
            return false;
        }
        dequeDelete(m_messages, index);
        return true;
    };
    bool PacketHandler::peekMessageInternal(int index, uint8_t output[], uint8_t &length, uint8_t &sender) {
        if (index >= m_messages.size() || index < 0) {
            return false;
        }
        const SerialMessage &message = m_messages[index];
        length = message.length;
        sender = message.sender;
        memcpy(output, message.data, length);
        return true;
    };
    bool PacketHandler::peekMessagePrefixedInternal(int index, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        if (index >= m_messages.size() || index < 0) {
            return false;
        }
        const SerialMessage &message = m_messages[index];
        if (message.length == 0) {
            return false;
        }
        prefix = message.data[0];
        length = message.length - 1;
        sender = message.sender;
        memcpy(output, message.data + 1, length);
        return true;
    };
    bool PacketHandler::peekMessagePrefixInternal(int index, uint8_t &prefix) {
        if (index >= m_messages.size() || index < 0) {
            return false;
        }
        const SerialMessage &message = m_messages[index];
        if (message.length == 0) {
            return false;
        }
        prefix = message.data[0];
        return true;
    };
    int PacketHandler::findMessageIndex(int id) {
        for (size_t i = 0; i < m_messages.size(); i++) {
            if (m_messages[i].id == id) {
                return i;
            }
        }
        return -1;
    };
    bool PacketHandler::popMessage(int id) {
        if (availableMessages()) {
            int i = findMessageIndex(id);
            if (i < 0) {
                return false;
            }
            popMessageInternal(i);
            return true;
        }
        return false;
    };
    bool PacketHandler::popMessage(int id, uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return popMessage(id, output, length, sender);
    };
    bool PacketHandler::popMessage(int id, uint8_t output[], uint8_t &length, uint8_t &sender) {
        int i = findMessageIndex(id);
        if (i < 0) {
            return false;
        }
        bool success = peekMessageInternal(i, output, length, sender);
        if (!success) {
            return false;
        }
        return popMessageInternal(i);
    };
    bool PacketHandler::popMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return popMessagePrefixed(id, prefix, output, length, sender);
    };
    bool PacketHandler::popMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        int i = findMessageIndex(id);
        if (i < 0) {
            return false;
        }
        bool success = peekMessagePrefixedInternal(i, prefix, output, length, sender);
        if (!success) {
            return false;
        }
        return popMessageInternal(i);
    };
    bool PacketHandler::peekMessage(int id, uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return peekMessage(id, output, length, sender);
    };
    bool PacketHandler::peekMessage(int id, uint8_t output[], uint8_t &length, uint8_t &sender) {
        int i = findMessageIndex(id);
        if (i < 0) {
            return false;
        }
        return peekMessageInternal(i, output, length, sender);
    };
    bool PacketHandler::peekMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return peekMessagePrefixed(id, prefix, output, length, sender);
    };
    bool PacketHandler::peekMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        int i = findMessageIndex(id);
        if (i < 0) {
            return false;
        }
        return peekMessagePrefixedInternal(i, prefix, output, length, sender);
    };
    bool PacketHandler::peekMessagePrefix(int id, uint8_t &prefix) {
        int i = findMessageIndex(id);
        if (i < 0) {
            return false;
        }
        return peekMessagePrefixInternal(i, prefix);
    };

    void PacketHandler::flushMessages() {
        std::deque<SerialMessage> empty;
        std::swap( m_messages, empty );
    };

    
    bool PacketHandler::sendMessage(uint8_t recipient_code, uint8_t message, int copies) {
        return sendMessageInternal(recipient_code, 0, false, message, true, nullptr, 0, copies);
    };
    bool PacketHandler::sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length, int copies) {
        return sendMessageInternal(recipient_code, 0, false, 0, false, message, length, copies);
    };
    bool PacketHandler::sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message, int copies) {
        return sendMessageInternal(recipient_code, message_prefix, true, message, true, nullptr, 0, copies);
    };
    bool PacketHandler::sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length, int copies) {
        return sendMessageInternal(recipient_code, message_prefix, true, 0, false, message, length, copies);
    };
    bool PacketHandler::sendMessageAll(uint8_t message, int copies) {
        return sendMessage(KSerialAddressSendAll, message, copies);
    };
    bool PacketHandler::sendMessageAll(uint8_t message[], uint8_t length, int copies) {
        return sendMessage(KSerialAddressSendAll, message, length, copies);
    };
    bool PacketHandler::sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message, int copies) {
        return sendMessagePrefixed(KSerialAddressSendAll, message_prefix, message, copies);
    };
    bool PacketHandler::sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message[], uint8_t length, int copies) {
        return sendMessagePrefixed(KSerialAddressSendAll, message_prefix, message, length, copies);
    };
    bool PacketHandler::sendMessageInternal(uint8_t recipient_code, uint8_t message_prefix, bool with_prefix, uint8_t message_singleton, bool with_singleton, uint8_t message[], uint8_t length, int copies) {
        uint8_t original_length = length;
        uint8_t full_length = length;
        if (with_prefix) {
            full_length += 1;
        }
        if (with_singleton) {
            full_length += 1;
        }
        // Also protect against length rollover
        if (full_length > kMaxPacketSize || full_length < original_length) {
            return false;
        }
        uint8_t message_id = m_sent_message_id_counter;
        m_sent_message_id_counter += 1;
        for (int i = 0; i < copies; i++) {
            if (i == 0) {
                m_to_send.push(static_cast<int>(SerialFlags::Start));
            }else {
                m_to_send.push(static_cast<int>(SerialFlags::StartDuplicate));
            }


            uint8_t checksum = 0;
            checksum += full_length;
            sendByte(full_length);
            checksum += m_address_code;
            sendByte(m_address_code);
            checksum += recipient_code;
            sendByte(recipient_code);
            checksum += message_id;
            sendByte(message_id);
            
            if (with_prefix) {
                sendByte(message_prefix);
                checksum += message_prefix;
            }
            if (with_singleton) {
                sendByte(message_singleton);
                checksum += message_singleton;
            }
            if (message) {
                for (int j = 0; j < original_length; j++) {
                    sendByte(message[j]);
                    checksum += message[j];
                }
            }
            sendByte(checksum);
            m_to_send.push(static_cast<int>(SerialFlags::End));
        }
        return true;
    };
    void PacketHandler::sendByte(uint8_t byte) {
        if (isSpecial(byte)) {
            m_to_send.push(static_cast<uint8_t>(SerialFlags::Escape));
        }
        m_to_send.push(byte);
    };
    
    // // Currently unused
    // uint8_t PacketHandler::computeChecksum(uint8_t length, uint8_t address, uint8_t sender, uint8_t sent_id, uint8_t data[]) {
    //     uint8_t checksum = 0;
    //     checksum += length;
    //     checksum += address;
    //     checksum += sender;
    //     checksum += sent_id;
    //     for (size_t i = 0; i < length; i++) {
    //         checksum += data[i];
    //     }
    //     return checksum;
    // };

};

#else
// #error "Enable ATMT_SUBMODULE_PACKET_HANDLING_ in automat_submodules.h to use PacketHandler"
#endif