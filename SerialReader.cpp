
#include "SerialReader.h"

namespace atmt {

    constexpr int kBaudrate = 115200;
    constexpr int kMaxMessagesPerFrame = 256;
    constexpr int kMaxPacketSize = 64;

    enum class SerialMessage : uint8_t {
        // Get_Status = 0xfb,
        // Status_Processing = 0xfc,
        // Status_Complete = 0xfd,
        Escape = 0xfd,
        End = 0xfe,
        Start = 0xff,
        // Invalid = 0xfe,
        // Error = 0xff
    };

#ifdef AUTOMAT_VEX_
    SerialReader::SerialReader(int port):
        m_fake_motor{ nullptr },
        m_port{ port }
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
        while (m_to_send.size() > 0 && vexGenericSerialWriteFree(m_index) > 0) {
            vexGenericSerialWriteChar(m_index, m_to_send.front());
            m_to_send.pop();
            // available_length -= 1;
        }

        interpretMessages();
#endif
#ifdef AUTOMAT_ESP32_
#endif
    };

    void SerialReader::interpretMessages() {
        while (m_raw_input.size() > 0) {
            if (!m_part_has_start) {
                if (m_raw_input.front() == static_cast<uint8_t>(SerialMessage::Start)) { // 0xfd
                    m_part_has_start = true;
                }
                m_raw_input.pop();
            }else if (m_part_length < 0) {
                if (!manageSpecial(m_raw_input.front())) {
                    m_part_length = m_raw_input.front();
                    m_raw_input.pop();
                    if (m_part_length > kMaxPacketSize) {
                        resetPartialMessage();
                    }else {
                        m_part_data = std::shared_ptr<uint8_t[]>(new uint8_t[m_part_length]);
                    }
                }
            }else if (m_part_datas_input < m_part_length) {
                if (!manageSpecial(m_raw_input.front())) {
                    (*m_part_data)[m_part_datas_input] = m_raw_input.front();
                    m_part_datas_input += 1;
                    m_raw_input.pop();
                }
            }else if (m_part_checksum < 0) {
                if (!manageSpecial(m_raw_input.front())) {
                    m_part_checksum = m_raw_input.front();
                    m_raw_input.pop();
                    uint8_t checksum = 0;
                    for (int i = 0; i < m_part_length; i++) {
                        checksum += (*m_part_data)[i]; // Rollover handled automatically
                    }
                    if (m_part_checksum != checksum) {
                        resetPartialMessage();
                    }
                }
            }else if (!m_part_has_end) {
                if (m_raw_input.front() == static_cast<uint8_t>(SerialMessage::End)) { // 0xfc
                    m_part_has_end = true;
                    m_raw_input.pop();
                    serial_message message { m_part_data, m_part_length };
                    m_messages.push(message);
                }
                resetPartialMessage();
            }else {
                // Should never happen
                resetPartialMessage();
            }
        }
    };
    void SerialReader::resetPartialMessage() {
        m_part_has_start = false;
        m_part_length = -1;
        // if (!m_part_has_end) { // Means that the message was corrupted or incomplete
        //     delete[] m_part_data;
        // }
        m_part_data = nullptr;
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
            || code == static_cast<uint8_t>(SerialMessage::End)
            || code == static_cast<uint8_t>(SerialMessage::Escape);
    };

    bool SerialReader::availableMessages() {
        return (m_messages.size() > 0);
    };
    bool SerialReader::getNextMessage(std::shared_ptr<uint8_t[]> &output, uint8_t &length) {
        if (availableMessages()) {
            serial_message message = m_messages.front();
            m_messages.pop();
            output = message.data;
            length = message.length;
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
    bool SerialReader::sendMessage(uint8_t* message, uint8_t length) {
        if (length > kMaxPacketSize) {
            return false;
        }
        m_to_send.push(static_cast<int>(SerialMessage::Start));
        if (isSpecial(length)) {
            m_to_send.push(static_cast<uint8_t>(SerialMessage::Escape));
        }
        m_to_send.push(length);
        uint8_t checksum = 0;
        for (int i = 0; i < length; i++) {
            if (isSpecial(message[i])) {
                m_to_send.push(static_cast<uint8_t>(SerialMessage::Escape));
            }
            m_to_send.push(message[i]);
            checksum += message[i];
        }
        if (isSpecial(checksum)) {
            m_to_send.push(static_cast<uint8_t>(SerialMessage::Escape));
        }
        m_to_send.push(checksum);
        m_to_send.push(static_cast<int>(SerialMessage::End));
        return true;
    };
    void SerialReader::flushMessages() {
        std::queue<serial_message> empty;
        std::swap( m_messages, empty );
    };

};
