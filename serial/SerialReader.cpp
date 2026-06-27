#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_SERIAL_

#include "SerialReader.h"

#ifdef ATMT_SUBMODULE_COMMAND_BASED_
#include "../command_based/EventHandler.h"
#endif

#include <cstring>
#include <string>
#ifdef AUTOMAT_VEX_
#include <vex.h>
#include "v5_apiuser.h"
#endif

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
    SerialReader::SerialReader(SerialInterface serial_interface, uint8_t address_code):
        SerialReader(serial_interface, address_code, -1, -1)
    {

    };
    SerialReader::SerialReader(SerialInterface serial_interface, uint8_t address_code, int rx_pin, int tx_pin):
        m_serial_interface{ serial_interface },
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
        m_uart_port{ uart_port < 0 ? kUARTDefaultPort : uart_port }
#endif
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
        m_triggers{ std::vector<Trigger_Event*>() },
        m_temp_triggers{ std::vector<Trigger_Event*>() },
        m_robot_state{ nullptr },
        m_event_handler{ nullptr },
#endif
        m_packet_handler{ PacketHandler(address_code) }
    {
        
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
        switch (m_serial_interface) {
            case Interface_Serial0:
                Serial.begin(kBaudrate);
                break;
            case Interface_Serial1:
                Serial1.begin(kBaudrate, SERIAL_8N1, m_rx_pin, m_tx_pin);
                break;
            case Interface_Serial2:
                Serial2.begin(kBaudrate, SERIAL_8N1, m_rx_pin, m_tx_pin);
                break;
            // case Interface_Serial3:
            //     Serial3.begin(kBaudrate, SERIAL_8N1, m_rx_pin, m_tx_pin);
            //     break;
        }
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
        int received_messages = 0;
        while (received_messages < kMaxMessagesPerFrame && vexGenericSerialReceiveAvail(m_index)) { // Will have a break;
            int32_t raw = vexGenericSerialReadChar(m_index);
            if (raw >= 0) { // i.e. != -1 // This has got to be one of the dumbest error codes because it maps to 255...
                uint8_t processed = static_cast<uint8_t>(raw);
                // if (message != static_cast<uint8_t>(SerialFlags::Invalid)) {
                // m_raw_input.push(processed);
                m_packet_handler.inputReceivedRawByte(processed);
                // }
                received_messages += 1;
            }else {
                break;
            }
        }

        // int32_t = available_length = vexGenericSerialWriteFree(m_index);
        // while (!m_to_send.empty() && vexGenericSerialWriteFree(m_index) > 0) {
        while (m_packet_handler.hasRawBytesToSend() && vexGenericSerialWriteFree(m_index) > 0) {
            uint8_t byte;
            m_packet_handler.getNextRawByteToSend(byte);
            vexGenericSerialWriteChar(m_index, byte);
            // m_to_send.pop();
            // available_length -= 1;
        }
#endif
#ifdef AUTOMAT_ESP32_ARDUINO_
        int received_messages = 0;
        while (received_messages < kMaxMessagesPerFrame) {
            bool serial_available = false;
            switch (m_serial_interface) {
                case Interface_Serial0:
                    serial_available = Serial.available();
                    break;
                case Interface_Serial1:
                    serial_available = Serial1.available();
                    break;
                case Interface_Serial2:
                    serial_available = Serial2.available();
                    break;
                // case Interface_Serial3:
                //     serial_available = Serial2.available();
                //     break;
            }
            if (!serial_available) break;

            uint8_t raw = 0;
            switch (m_serial_interface) {
                case Interface_Serial0:
                    raw = Serial.read();
                    break;
                case Interface_Serial1:
                    raw = Serial1.read();
                    break;
                case Interface_Serial2:
                    raw = Serial2.read();
                    break;
                // case Interface_Serial3:
                //     raw = Serial3.read();
                //     break;
            }
            // m_raw_input.push(raw);
            m_packet_handler.inputReceivedRawByte(raw);
            received_messages += 1;
        }

        // while (!m_to_send.empty() && Serial2.availableForWrite()) {
        // while (!m_to_send.empty()) {
        while (m_packet_handler.hasRawBytesToSend()) {
            bool serial_available = false;
            switch (m_serial_interface) {
                case Interface_Serial0:
                    // if (!Serial.availableForWrite()) break;
                    serial_available = Serial.availableForWrite();
                    break;
                case Interface_Serial1:
                    // if (!Serial1.availableForWrite()) break;
                    serial_available = Serial1.availableForWrite();
                    break;
                case Interface_Serial2:
                    // if (!Serial2.availableForWrite()) break;
                    serial_available = Serial2.availableForWrite();
                    break;
                // case Interface_Serial3:
                //     if (!Serial3.availableForWrite()) break;
                //     break;
            }
            if (!serial_available) break;
            // Serial1.write(0xf0);


            uint8_t byte;
            m_packet_handler.getNextRawByteToSend(byte);
            switch (m_serial_interface) {
                case Interface_Serial0:
                    Serial.write(byte);
                    break;
                case Interface_Serial1:
                    Serial1.write(byte);
                    break;
                case Interface_Serial2:
                    Serial2.write(byte);
                    break;
                // case Interface_Serial3:
                //     Serial3.write(byte);
                //     break;
            }
            // m_to_send.pop();
        }
#endif
#ifdef AUTOMAT_ESP32_ESPIDF_
        uint8_t buf[128];
        int len = uart_read_bytes(m_uart_port, buf, sizeof(buf), 0); // timeout 0 = non-blocking
        for (int i = 0; i < len; i++) {
            // m_raw_input.push(buf[i]);
            m_packet_handler.inputReceivedRawByte(buf[i]);
        }

        while (m_packet_handler.hasRawBytesToSend()) {
            uint8_t byte;
            m_packet_handler.peekNextRawByteToSend(byte);
            // uint8_t b = static_cast<uint8_t>(m_to_send.front());

            int written = uart_write_bytes(m_uart_port, (const char*)&byte, 1);
            if (written > 0) {
                // m_to_send.pop();
                m_packet_handler.getNextRawByteToSend(byte); // To pop
            } else {
                break; // UART TX full
            }
        }
#endif

        m_packet_handler.periodic();
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
    
#ifdef ATMT_SUBMODULE_COMMAND_BASED_
    void SerialReader::triggerEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length, int id) {
        if (!m_robot_state) { // Uninitialized
            return;
        }
        // for (size_t i = 0; i < m_temp_triggers.size(); i++) {
        for (size_t i = 0; i < m_temp_triggers.size(); ) {
            if (m_temp_triggers[i]->matchesEvent(event, sender, code, length, *m_robot_state)) {
                // interpretTrigger(m_temp_triggers[i], true);
                m_temp_triggers[i]->setSerialMessageId(id);
                Trigger_Event* temp_trigger = m_event_handler->interpretTrigger(m_temp_triggers[i], true);
                if (temp_trigger) {
                    m_temp_triggers.push_back(temp_trigger);
                }
                
                delete m_temp_triggers[i];
                vectorDeleteUnordered(m_temp_triggers, i);
                // m_temp_triggers.erase(m_temp_triggers.begin() + i);
                // i -= 1;
            }else {
                i += 1;
            }
        }

        for (Trigger_Event* trigger : m_triggers) {
            if (trigger->matchesEvent(event, sender, code, length, *m_robot_state)) {
                // interpretTrigger(trigger, true);
                trigger->setSerialMessageId(id);
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
        m_triggers.push_back(new Trigger_Event(StartAutonomous, trigger->inMode(ModeDisabled)));
    };
    void SerialReader::bindTeleopTrigger(Trigger* trigger) {
        m_triggers.push_back(new Trigger_Event(StartTeleop, trigger->inMode(ModeDisabledAndAuto)));
    };
#endif


    // Expose through protocol
    bool SerialReader::availableMessages() {
        return m_packet_handler.availableMessages();
    };
    bool SerialReader::popNextMessage() {
        return m_packet_handler.popNextMessage();
    };
    bool SerialReader::popNextMessage(uint8_t output[], uint8_t &length) {
        return m_packet_handler.popNextMessage(output, length);
    };
    bool SerialReader::popNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.popNextMessage(output, length, sender);
    };
    bool SerialReader::popNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length) {
        return m_packet_handler.popNextMessagePrefixed(prefix, output, length);
    };
    bool SerialReader::popNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.popNextMessagePrefixed(prefix, output, length, sender);
    };
    bool SerialReader::peekNextMessage(uint8_t output[], uint8_t &length) {
        return m_packet_handler.peekNextMessage(output, length);
    };
    bool SerialReader::peekNextMessage(uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.peekNextMessage(output, length, sender);
    };
    bool SerialReader::peekNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length) {
        return m_packet_handler.peekNextMessagePrefixed(prefix, output, length);
    };
    bool SerialReader::peekNextMessagePrefixed(uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.peekNextMessagePrefixed(prefix, output, length, sender);
    };
    bool SerialReader::peekNextMessagePrefix(uint8_t &prefix) {
        return m_packet_handler.peekNextMessagePrefix(prefix);
    }
    
    // Expose through protocol
    bool SerialReader::getNextMessageId(int &id) {
        return m_packet_handler.getNextMessageId(id);
    }
    bool SerialReader::getMessageId(int index, int &id) {
        return m_packet_handler.getMessageId(index, id);
    }
    int SerialReader::availableMessagesCount() {
        return m_packet_handler.availableMessagesCount();
    }

    // Expose through protocol
    bool SerialReader::popMessage(int id) {
        return m_packet_handler.popMessage(id);
    }
    bool SerialReader::popMessage(int id, uint8_t output[], uint8_t &length) {
        return m_packet_handler.popMessage(id, output, length);
    }
    bool SerialReader::popMessage(int id, uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.popMessage(id, output, length, sender);
    }
    bool SerialReader::popMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length) {
        return m_packet_handler.popMessagePrefixed(id, prefix, output, length);
    }
    bool SerialReader::popMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.popMessagePrefixed(id, prefix, output, length, sender);
    }
    bool SerialReader::peekMessage(int id, uint8_t output[], uint8_t &length) {
        return m_packet_handler.peekMessage(id, output, length);
    }
    bool SerialReader::peekMessage(int id, uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.peekMessage(id, output, length, sender);
    }
    bool SerialReader::peekMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length) {
        return m_packet_handler.peekMessagePrefixed(id, prefix, output, length);
    }
    bool SerialReader::peekMessagePrefixed(int id, uint8_t &prefix, uint8_t output[], uint8_t &length, uint8_t &sender) {
        return m_packet_handler.peekMessagePrefixed(id, prefix, output, length, sender);
    }
    bool SerialReader::peekMessagePrefix(int id, uint8_t &prefix) {
        return m_packet_handler.peekMessagePrefix(id, prefix);
    }

    // Expose through protocol
    bool SerialReader::sendMessage(uint8_t recipient_code, uint8_t message, int copies) {
        return m_packet_handler.sendMessage(recipient_code, message, copies);
    }
    bool SerialReader::sendMessage(uint8_t recipient_code, uint8_t message[], uint8_t length, int copies) {
        return m_packet_handler.sendMessage(recipient_code, message, length, copies);
    }
    bool SerialReader::sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message, int copies) {
        return m_packet_handler.sendMessagePrefixed(recipient_code, message_prefix, message, copies);
    }
    bool SerialReader::sendMessagePrefixed(uint8_t recipient_code, uint8_t message_prefix, uint8_t message[], uint8_t length, int copies) {
        return m_packet_handler.sendMessagePrefixed(recipient_code, message_prefix, message, length, copies);
    }
    bool SerialReader::sendMessageAll(uint8_t message, int copies) {
        return m_packet_handler.sendMessageAll(message, copies);
    }
    bool SerialReader::sendMessageAll(uint8_t message[], uint8_t length, int copies) {
        return m_packet_handler.sendMessageAll(message, length, copies);
    }
    bool SerialReader::sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message, int copies) {
        return m_packet_handler.sendMessagePrefixedAll(message_prefix, message, copies);
    }
    bool SerialReader::sendMessagePrefixedAll(uint8_t message_prefix, uint8_t message[], uint8_t length, int copies) {
        return m_packet_handler.sendMessagePrefixedAll(message_prefix, message, length, copies);
    }

    // Expose through protocol
    void SerialReader::sendByte(uint8_t byte) {
        return m_packet_handler.sendByte(byte);
    }
    void SerialReader::flushMessages() {
        return m_packet_handler.flushMessages();
    }
    
};

#else
// #error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif