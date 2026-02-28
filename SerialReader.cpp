
#include "SerialReader.h"

#include "EventHandler.h"

namespace atmt {
    
    bool m_read_serial_events{ false };
    
    void SetReadSerialEvents(bool to_read) {
        m_read_serial_events = to_read;
    };

#ifdef AUTOMAT_VEX_
    SerialReader::SerialReader(int port):
        m_triggers{ std::vector<Trigger_Event*>() },
        m_temp_triggers{ std::vector<Trigger_Event*>() },
        m_fake_motor{ nullptr },
        m_port{ port },
        m_index{ 0 },
        m_raw_input{ },
        m_messages{ },
        m_to_send{ },

        m_last_message{ 0, 0 },

        m_part_has_start{ false },
        m_part_is_duplicate{ false },
        m_part_length{ -1 },
        m_part_data{ },
        m_part_datas_input{ 0 },
        m_part_checksum{ -1 },
        m_part_has_end{ false },
        m_part_next_char_escaped{ false },

        m_robot_state{ nullptr },
        m_event_handler{ nullptr }
    {

    };
#endif
#ifdef AUTOMAT_ESP32_
    SerialReader::SerialReader()
    {

    };
#endif
    SerialReader::~SerialReader() {
        delete m_fake_motor;
        m_fake_motor = nullptr;
        for (Trigger_Event* trigger : m_triggers) {
            delete trigger;
        }
        m_triggers.clear();
        for (Trigger_Event* trigger : m_temp_triggers) {
            delete trigger;
        }
        m_temp_triggers.clear();
    };

    void SerialReader::init() {
#ifdef AUTOMAT_VEX_
        m_fake_motor = new vex::motor(m_port, vex::gearSetting::ratio18_1, false);
        m_index = m_fake_motor->index();
        vexGenericSerialEnable(m_index, 0);
        vexGenericSerialBaudrate(m_index, kBaudrate);
#endif
#ifdef AUTOMAT_ESP32_
#endif
    };
    void SerialReader::periodic() {
#ifdef AUTOMAT_VEX_
        // int32_t available_length = vexGenericSerialReceiveAvail(m_index);
        // for (int i = 0; i < available_length; i++) {
        //     m_messages.push(vexGenericSerialReadChar(m_index));
        // }
        // while (vexGenericSerialReceiveAvail(m_index) > 0) {
        int received_messages = 0;
        while (received_messages < kMaxMessagesPerFrame) { // Will have a break;
            int32_t raw = vexGenericSerialReadChar(m_index);
            if (raw >= 0) { // i.e. != -1 // This has got to be one of the dumbest error codes because it maps to 255...
                uint8_t processed = static_cast<uint8_t>(raw);
                // if (message != static_cast<uint8_t>(SerialMessage::Invalid)) {
                    m_raw_input.push(processed);
                // }
                received_messages += 1;
            }else {
                break;
            }
        }

        // int32_t = available_length = vexGenericSerialWriteFree(m_index);
        while (!m_to_send.empty() && vexGenericSerialWriteFree(m_index) > 0) {
            vexGenericSerialWriteChar(m_index, m_to_send.front());
            m_to_send.pop();
            // available_length -= 1;
        }

        interpretMessages();
#endif
#ifdef AUTOMAT_ESP32_
#endif
    };
    
    void SerialReader::internal_init(RobotState* robot_state, EventHandler* event_handler) {
        m_event_handler = event_handler;
        m_robot_state = robot_state;
    };

    void SerialReader::interpretMessages() {
        while (!m_raw_input.empty()) {
            if (!m_part_has_start) {
                if (m_raw_input.front() == static_cast<uint8_t>(SerialMessage::Start)) { // 0xff
                    m_part_has_start = true;
                    m_part_is_duplicate = false;
                }else if (m_raw_input.front() == static_cast<uint8_t>(SerialMessage::StartDuplicate)) { // 0xfc
                    m_part_has_start = true;
                    m_part_is_duplicate = true;
                }
                m_raw_input.pop();
            }else if (m_part_length < 0) {
                if (!manageSpecial(m_raw_input.front())) {
                    m_part_length = m_raw_input.front();
                    if (m_part_length > kMaxPacketSize) {
                        resetPartialMessage();
                    // }else {
                        // m_part_data = uint8_t[kMaxPacketSize];
                    }
                }
                m_raw_input.pop();
            }else if (m_part_datas_input < m_part_length) {
                if (!manageSpecial(m_raw_input.front())) {
                    m_part_data[m_part_datas_input] = m_raw_input.front();
                    m_part_datas_input += 1;
                }
                m_raw_input.pop();
            }else if (m_part_checksum < 0) {
                if (!manageSpecial(m_raw_input.front())) {
                    m_part_checksum = m_raw_input.front();
                    uint8_t checksum = 0;
                    for (int i = 0; i < m_part_length; i++) {
                        checksum += m_part_data[i]; // Rollover handled automatically
                    }
                    if (m_part_checksum != checksum) {
                        resetPartialMessage();
                    }
                }
                m_raw_input.pop();
            }else if (!m_part_has_end) {
                if (m_raw_input.front() == static_cast<uint8_t>(SerialMessage::End)) { // 0xfc
                    m_part_has_end = true;
                    m_raw_input.pop();
                    serial_message message;
                    message.length = m_part_length;
                    // for (int i = 0; i < m_part_length; i++) {
                    //     message.data[i] = m_part_data[i];
                    // }
                    memcpy(message.data, m_part_data, m_part_length);
                    if (!m_part_is_duplicate) {
                        addInterpretedMessage(message);
                    }else {
                        // serial_message original = m_messages.back();
                        if (!checkIfMatching(message, m_last_message)) { // If it is not a duplicate of the last packet
                            addInterpretedMessage(message);
                        } // else: we did receive the original packet, so no need to recognize the duplicate
                    }
                }
                resetPartialMessage();
            }else {
                // Should never happen
                resetPartialMessage();
            }
        }
    };
    void SerialReader::addInterpretedMessage(serial_message message) {
        m_messages.push(message);
        m_last_message = message;
        triggerEvent(SerialReceive, message.data, message.length);
    };
    void SerialReader::resetPartialMessage() {
        m_part_has_start = false;
        m_part_is_duplicate = false;
        m_part_length = -1;
        // if (!m_part_has_end) { // Means that the message was corrupted or incomplete
        //     delete[] m_part_data;
        // }
        // m_part_data = nullptr; // Could clear it, but don't need to
        m_part_datas_input = 0;
        m_part_checksum = -1;
        m_part_has_end = false;
        m_part_next_char_escaped = false;
    };
    bool SerialReader::manageSpecial(uint8_t code) {
        if (m_part_next_char_escaped) {
            m_part_next_char_escaped = false;
            return false;
        }
        if (code == static_cast<uint8_t>(SerialMessage::Start)) { // 0xfd
            resetPartialMessage();
            m_part_has_start = true; // Because the byte will be eaten
            m_part_is_duplicate = false;
            return true;
        }else if (code == static_cast<uint8_t>(SerialMessage::StartDuplicate)) {
            resetPartialMessage();
            m_part_has_start = true; // Because the byte will be eaten
            m_part_is_duplicate = true;
            return true;
        }else if (code == static_cast<uint8_t>(SerialMessage::End)) {
            resetPartialMessage();
            return true;
        }else if (code == static_cast<uint8_t>(SerialMessage::Escape)) {
            m_part_next_char_escaped = true;
            return true;
        }
        return false;
    };
    bool SerialReader::isSpecial(uint8_t code) {
        return code == static_cast<uint8_t>(SerialMessage::Start)
            || code == static_cast<uint8_t>(SerialMessage::StartDuplicate)
            || code == static_cast<uint8_t>(SerialMessage::End)
            || code == static_cast<uint8_t>(SerialMessage::Escape);
    };
    bool SerialReader::checkIfMatching(const serial_message &duplicate, const serial_message &original) {
        if (duplicate.length != original.length) {
            return false;
        }
        for (int i = 0; i < duplicate.length; i++) {
            if (duplicate.data[i] != original.data[i]) {
                return false;
            }
        }
        return true;
    };

    bool SerialReader::availableMessages() {
        return (!m_messages.empty());
    };
    bool SerialReader::getNextMessage(uint8_t output[], uint8_t &length) {
        if (availableMessages()) {
            serial_message message = m_messages.front();
            m_messages.pop();
            length = message.length;
            // for (int i = 0; i < length; i++) {
            //     output[i] = message.data[i];
            // }
            memcpy(output, message.data, length);
            return true;
        }else {
            // return static_cast<uint8_t>(SerialMessage::Error);
            return false;
        }
    };
    // void SerialReader::destroyMessage(uint8_t* output, uint8_t &length) {
    //     delete[] output;
    //     output = nullptr;
    //     length = 0;
    // };
    bool SerialReader::sendMessage(uint8_t message[], uint8_t length) {
        return sendMessage(message, length, 1);
    };
    bool SerialReader::sendMessage(uint8_t message[], uint8_t length, int duplicates) {
        if (length > kMaxPacketSize) {
            return false;
        }
        for (int i = 0; i < duplicates; i++) {
            if (i == 0) {
                m_to_send.push(static_cast<int>(SerialMessage::Start));
            }else {
                m_to_send.push(static_cast<int>(SerialMessage::StartDuplicate));
            }

            if (isSpecial(length)) {
                m_to_send.push(static_cast<uint8_t>(SerialMessage::Escape));
            }
            m_to_send.push(length);
            uint8_t checksum = 0;
            for (int j = 0; j < length; j++) {
                if (isSpecial(message[j])) {
                    m_to_send.push(static_cast<uint8_t>(SerialMessage::Escape));
                }
                m_to_send.push(message[j]);
                checksum += message[j];
            }
            if (isSpecial(checksum)) {
                m_to_send.push(static_cast<uint8_t>(SerialMessage::Escape));
            }
            m_to_send.push(checksum);
            m_to_send.push(static_cast<int>(SerialMessage::End));
        }
        return true;
    };
    void SerialReader::flushMessages() {
        std::queue<serial_message> empty;
        std::swap( m_messages, empty );
    };

    
    void SerialReader::triggerEvent(SerialEvent event, uint8_t code[], uint8_t length) {
        if (!m_robot_state) { // Uninitialized
            return;
        }
        // for (size_t i = 0; i < m_temp_triggers.size(); i++) {
        for (size_t i = 0; i < m_temp_triggers.size(); ) {
            if (m_temp_triggers[i]->matchesEvent(event, code, length, *m_robot_state)) {
                // interpretTrigger(m_temp_triggers[i], true);
                Trigger_Event* temp_trigger = m_event_handler->interpretTrigger(m_temp_triggers[i], true);
                if (temp_trigger) {
                    m_temp_triggers.push_back(temp_trigger);
                }
                
                delete m_temp_triggers[i];
                m_temp_triggers.erase(m_temp_triggers.begin() + i);
                // i -= 1;
            }else {
                i += 1;
            }
        }

        for (Trigger_Event* trigger : m_triggers) {
            if (trigger->matchesEvent(event, code, length, *m_robot_state)) {
                // interpretTrigger(trigger, true);
                Trigger_Event* temp_trigger = m_event_handler->interpretTrigger(trigger, true);
                if (temp_trigger) {
                    m_temp_triggers.push_back(temp_trigger);
                }
            }
        }
    };

};
