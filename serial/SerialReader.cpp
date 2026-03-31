#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_SERIAL_

#include "SerialReader.h"

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/EventHandler.h"
#endif

#include <cstring>

#ifdef AUTOMAT_ESP32_ARDUINO_
#include <Arduino.h>
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
#include "driver/uart.h"
#include "esp_log.h"
#endif

namespace atmt {
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    bool m_read_serial_events{ false };
    
    void SetReadSerialEvents(bool to_read) {
        m_read_serial_events = to_read;
    };
#endif

#ifdef AUTOMAT_VEX_
    SerialReader::SerialReader(uint8_t address_code, int port):
        m_fake_motor{ nullptr },
        m_port{ port },
        m_index{ 0 },
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
    SerialReader::SerialReader(uint8_t address_code):
        SerialReader(address_code, -1, -1)
    {

    };
    SerialReader::SerialReader(uint8_t address_code, int rx_pin, int tx_pin):
        m_rx_pin{ rx_pin < 0 ? kRXDefaultPin : rx_pin },
        m_tx_pin{ tx_pin < 0 ? kTXDefaultPin : tx_pin },
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
    SerialReader::SerialReader(uint8_t address_code):
        SerialReader(address_code, -1, -1)
    {

    };
    SerialReader::SerialReader(uint8_t address_code, int rx_pin, int tx_pin):
        SerialReader(address_code, rx_pin, tx_pin, -1)
    {

    };
    SerialReader::SerialReader(uint8_t address_code, int rx_pin, int tx_pin, int buffer_size):
        SerialReader(address_code, rx_pin, tx_pin, buffer_size, -1)
    {

    };
    SerialReader::SerialReader(uint8_t address_code, int rx_pin, int tx_pin, int buffer_size, int uart_port):
        m_rx_pin{ rx_pin < 0 ? kRXDefaultPin : rx_pin },
        m_tx_pin{ tx_pin < 0 ? kTXDefaultPin : tx_pin },
        m_buffer_size{ buffer_size < 0 ? kUARTDefaultBufferSize : buffer_size },
        m_uart_port{ uart_port < 0 ? kUARTDefaultPort : uart_port },
#endif
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
        m_triggers{ std::vector<Trigger_Event*>() },
        m_temp_triggers{ std::vector<Trigger_Event*>() },
        m_robot_state{ nullptr },
        m_event_handler{ nullptr },
#endif
        m_address_code{ address_code },
        m_raw_input{ },
        m_messages{ },
        m_to_send{ },

        m_last_message{ {0}, 0, 0 },

        m_part_has_start{ false },
        m_part_is_duplicate{ false },
        m_part_sender{ -1 },
        m_part_address{ -1 },
        m_part_length{ -1 },
        m_part_data{ },
        m_part_datas_input{ 0 },
        m_part_checksum{ -1 },
        m_part_has_end{ false },
        m_part_next_char_escaped{ false }
    {
        if (m_address_code == KSerialAddressSendAll) {
            platform_print("Cannot Set Address to 0xFF (Reserved for SendMessageAll)");
            m_address_code = 0;
        }
    };
    SerialReader::~SerialReader() {
#ifdef AUTOMAT_VEX_
        delete m_fake_motor;
        m_fake_motor = nullptr;
#endif
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
        for (Trigger_Event* trigger : m_triggers) {
            delete trigger;
        }
        m_triggers.clear();
        for (Trigger_Event* trigger : m_temp_triggers) {
            delete trigger;
        }
        m_temp_triggers.clear();
#endif
    };

    void SerialReader::init() {
#ifdef AUTOMAT_VEX_
        m_fake_motor = new vex::motor(m_port, vex::gearSetting::ratio18_1, false);
        m_index = m_fake_motor->index();
        vexGenericSerialEnable(m_index, 0);
        vexGenericSerialBaudrate(m_index, kBaudrate);
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        Serial2.begin(kBaudrate, SERIAL_8N1, m_rx_pin, m_tx_pin);
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
        uart_config_t uart_config = {};
        uart_config.baud_rate = kBaudrate;
        uart_config.data_bits = UART_DATA_8_BITS;
        uart_config.parity = UART_PARITY_DISABLE;
        uart_config.stop_bits = UART_STOP_BITS_1;
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        uart_config.source_clk = UART_SCLK_APB;

        // Configure UART parameters
        uart_param_config(m_uart_port, &uart_config);

        // Set UART pins
        uart_set_pin(m_uart_port, m_tx_pin, m_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

        // Install driver
        uart_driver_install(m_uart_port, m_buffer_size, m_buffer_size, 0, nullptr, 0);
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
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        int received_messages = 0;
        while (Serial2.available() && received_messages < kMaxMessagesPerFrame) {
            uint8_t raw = Serial2.read();
            m_raw_input.push(raw);
            received_messages += 1;
        }

        while (!m_to_send.empty() && Serial2.availableForWrite()) {
            Serial2.write(m_to_send.front());
            m_to_send.pop();
        }
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
        uint8_t buf[128];
        int len = uart_read_bytes(m_uart_port, buf, sizeof(buf), 0); // timeout 0 = non-blocking
        for (int i = 0; i < len; i++) {
            m_raw_input.push(buf[i]);
        }

        while (!m_to_send.empty()) {
            uint8_t b = static_cast<uint8_t>(m_to_send.front());

            int written = uart_write_bytes(m_uart_port, (const char*)&b, 1);
            if (written > 0) {
                m_to_send.pop();
            } else {
                break; // UART TX full
            }
        }
#endif

        interpretMessages();
    };
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void SerialReader::systemPeriodic() {
        periodic();
    };
    void SerialReader::disabledPeriodic() {};
    void SerialReader::autonomousPeriodic() {};
    void SerialReader::teleopPeriodic() {};
#endif
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void SerialReader::internal_init(RobotState* robot_state, EventHandler* event_handler) {
        m_event_handler = event_handler;
        m_robot_state = robot_state;
    };
#endif

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
            }else if (m_part_sender < 0) {
                if (!manageSpecial(m_raw_input.front())) {
                    m_part_sender = m_raw_input.front();
                }
                m_raw_input.pop();
            }else if (m_part_address < 0) {
                if (!manageSpecial(m_raw_input.front())) {
                    m_part_address = m_raw_input.front();
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
                    if (m_part_address == m_address_code || m_part_address == KSerialAddressSendAll) {
                        if (!m_part_is_duplicate) {
                            addInterpretedMessage(message);
                        }else {
                            // serial_message original = m_messages.back();
                            if (!checkIfMatching(message, m_last_message)) { // If it is not a duplicate of the last packet
                                addInterpretedMessage(message);
                            } // else: we did receive the original packet, so no need to recognize the duplicate
                        }
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
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
        triggerEvent(SerialReceive, message.sender, message.data, message.length);
#endif
    };
    void SerialReader::resetPartialMessage() {
        m_part_has_start = false;
        m_part_is_duplicate = false;
        m_part_sender = -1;
        m_part_address = -1;
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
        uint8_t sender = 0;
        return getNextMessage(output, length, sender);
    };
    bool SerialReader::getNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender) {
        if (availableMessages()) {
            const serial_message &message = m_messages.front();
            m_messages.pop();
            length = message.length;
            sender = message.sender;
            memcpy(output, message.data, length);
            return true;
        }else {
            return false;
        }
    };
    bool SerialReader::getNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length) {
        uint8_t sender = 0;
        return getNextMessagePrefixed(prefix, output, length, sender);
    };
    bool SerialReader::getNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        // uint8_t data[kMaxPacketSize];
        bool success = getNextMessage(output, length, sender);
        if (!success) {
            return false;
        }
        if (length == 0) {
            return false;
        }
        prefix = output[0];
        // output = output + 1;
        length -= 1;
        memmove(output, output + 1, length);
        return true;
    };
    // bool SerialReader::sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length) {
    //     return sendMessage(recipient_code, message, length, 1);
    // };
    bool SerialReader::sendMessage(uint8_t recipient_code, uint8_t message, int copies) {
        return sendMessageInternal(recipient_code, message, true, nullptr, 0, copies);
    };
    bool SerialReader::sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length, int copies) {
        return sendMessageInternal(recipient_code, 0, false, message, length, copies);
    };
    // bool SerialReader::sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length) {

    // };
    bool SerialReader::sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length, int copies) {
        return sendMessageInternal(recipient_code, message_prefix, true, message, length, copies);
    };
    // bool SerialReader::sendMessageAll(uint8_t message[], uint8_t length) {
    //     return sendMessage(KSerialAddressSendAll, message, length, 1);
    // };
    bool SerialReader::sendMessageAll(uint8_t message[], uint8_t length, int copies) {
        return sendMessageInternal(KSerialAddressSendAll, 0, false, message, length, copies);
        // return sendMessage(KSerialAddressSendAll, message, length, copies);
    };
    bool SerialReader::sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message[], uint8_t length, int copies) {
        return sendMessageInternal(KSerialAddressSendAll, message_prefix, true, message, length, copies);
        // return sendMessage(KSerialAddressSendAll, message, length, copies);
    };
    bool SerialReader::sendMessageInternal(uint8_t recipient_code, uint8_t message_prefix, bool with_prefix, uint8_t message[], uint8_t length, int copies) {
        if (length > kMaxPacketSize) {
            return false;
        }
        for (int i = 0; i < copies; i++) {
            if (i == 0) {
                m_to_send.push(static_cast<int>(SerialMessage::Start));
            }else {
                m_to_send.push(static_cast<int>(SerialMessage::StartDuplicate));
            }

            uint8_t checksum = 0;
            checksum += length;
            sendByte(length);
            checksum += m_address_code;
            sendByte(m_address_code);
            checksum += recipient_code;
            sendByte(recipient_code);
            
            if (with_prefix) {
                sendByte(message_prefix);
                checksum += message_prefix;
            }
            if (message) {
                for (int j = 0; j < length; j++) {
                    sendByte(message[j]);
                    checksum += message[j];
                }
            }
            sendByte(checksum);
            m_to_send.push(static_cast<int>(SerialMessage::End));
        }
        return true;
    };
    void SerialReader::sendByte(uint8_t byte) {
        if (isSpecial(byte)) {
            m_to_send.push(static_cast<uint8_t>(SerialMessage::Escape));
        }
        m_to_send.push(byte);
    };
    void SerialReader::flushMessages() {
        std::queue<serial_message> empty;
        std::swap( m_messages, empty );
    };

    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void SerialReader::triggerEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length) {
        if (!m_robot_state) { // Uninitialized
            return;
        }
        // for (size_t i = 0; i < m_temp_triggers.size(); i++) {
        for (size_t i = 0; i < m_temp_triggers.size(); ) {
            if (m_temp_triggers[i]->matchesEvent(event, sender, code, length, *m_robot_state)) {
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
            if (trigger->matchesEvent(event, sender, code, length, *m_robot_state)) {
                // interpretTrigger(trigger, true);
                Trigger_Event* temp_trigger = m_event_handler->interpretTrigger(trigger, true);
                if (temp_trigger) {
                    m_temp_triggers.push_back(temp_trigger);
                }
            }
        }
    };

    void SerialReader::bindToMessage(Trigger* trigger, Command* command) {
        m_triggers.push_back(new Trigger_Event(StartCommand, trigger, command));
    };
    void SerialReader::bindAutoTrigger(Trigger* trigger) {
        m_triggers.push_back(new Trigger_Event(StartAutonomous, (trigger)->inMode(ModeDisabled)));
    };
#endif

};

#else
// #error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif