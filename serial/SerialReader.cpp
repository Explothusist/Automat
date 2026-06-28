#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_SERIAL_

#include "SerialReader.h"

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
        packet{ address_code },
        event{ }
    {
        
    };
    SerialReader::~SerialReader() {
#ifdef AUTOMAT_VEX_
        delete m_fake_motor;
        m_fake_motor = nullptr;
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
                packet.inputReceivedRawByte(processed);
                // }
                received_messages += 1;
            }else {
                break;
            }
        }

        // int32_t = available_length = vexGenericSerialWriteFree(m_index);
        // while (!m_to_send.empty() && vexGenericSerialWriteFree(m_index) > 0) {
        while (packet.hasRawBytesToSend() && vexGenericSerialWriteFree(m_index) > 0) {
            uint8_t byte;
            packet.getNextRawByteToSend(byte);
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
            }
            // m_raw_input.push(raw);
            packet.inputReceivedRawByte(raw);
            received_messages += 1;
        }

        // while (!m_to_send.empty() && Serial2.availableForWrite()) {
        // while (!m_to_send.empty()) {
        while (packet.hasRawBytesToSend()) {
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
            packet.getNextRawByteToSend(byte);
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
            packet.inputReceivedRawByte(buf[i]);
        }

        while (packet.hasRawBytesToSend()) {
            uint8_t byte;
            packet.peekNextRawByteToSend(byte);
            // uint8_t b = static_cast<uint8_t>(m_to_send.front());

            int written = uart_write_bytes(m_uart_port, (const char*)&byte, 1);
            if (written > 0) {
                // m_to_send.pop();
                packet.getNextRawByteToSend(byte); // To pop
            } else {
                break; // UART TX full
            }
        }
#endif

        packet.periodic();
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
        // m_event_handler = event_handler;
        // m_robot_state = robot_state;
        event.internal_init(robot_state, event_handler);
        packet.internal_init(&event);
    };
#endif
    
};

#else
// #error "Enable ATMT_SUBMODULE_SERIAL_ in automat_submodules.h to use SerialReader"
#endif