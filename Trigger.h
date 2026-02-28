
#ifndef AUTOMAT_TRIGGER_
#define AUTOMAT_TRIGGER_

#include <vector>
#include <memory>

#include "Command.h"
#include "utils.h"

namespace atmt {
    
    class Joystick;

    // class Command; // Forward declare because it never actually uses it

    typedef enum {
        NULLStick,
        LeftStick,
        RightStick
    } StickIndicator;
    typedef enum {
        StickNULL,
        StickUp,
        StickDown,
        StickLeft,
        StickRight,
        StickCenter
    } StickEvent;
    typedef enum {
        NULLButton,
        AButton,
        BButton,
        XButton,
        YButton,
        UpButton,
        LeftButton,
        DownButton,
        RightButton,
        StartButton, // ESP32 only
        SelectButton, // ESP32 only
        L1Button,
        L2Button,
        R1Button,
        R2Button
    } ButtonIndicator;
    typedef enum {
        ButtonNULL,
        ButtonPressed,
        ButtonReleased
    } ButtonEvent;
    typedef enum {
        NULLSerial,
        SerialReceive
    } SerialEvent;
    typedef enum {
        NULLTrigger,
        OnTrigger,
        WhileTrigger
    } TriggerType;
    typedef enum {
        ModeNULL,
        ModeTeleopOnly,
        ModeAutonomousOnly,
        ModeTeleopAndAuto
    } TriggerModes;
    typedef enum {
        NULLAxis,
        RYAxis,
        RXAxis,
        LYAxis,
        LXAxis,
        RTAxis, // ESP32 only
        LTAxis, // ESP32 only
    } AxisIndicator;

    typedef enum {
        StartCommand,
        EndCommand,
        StartAutonomous
    } TriggerEffect;

    class Trigger {
        public:
            Trigger();
            Trigger(StickIndicator stick, StickEvent event);
            // Trigger(StickIndicator stick, StickEvent event, TriggerType type);
            Trigger(ButtonIndicator button, ButtonEvent event);
            // Trigger(ButtonIndicator button, ButtonEvent event, TriggerType type);
            Trigger(SerialEvent event, uint8_t code[], uint8_t length);
            Trigger(const Trigger &trigger) = default;
            ~Trigger() = default;

            Trigger* setType(TriggerType type);
            Trigger* setCriteria(std::shared_ptr<Trigger> criteria);
            Trigger* inMode(TriggerModes modes);
            Trigger* invert();

            bool matchesEvent(StickIndicator stick, StickEvent event, RobotState state, Joystick* joystick);
            bool matchesEvent(ButtonIndicator button, ButtonEvent event, RobotState state, Joystick* joystick);
            bool matchesEvent(SerialEvent event, uint8_t code[], uint8_t length, RobotState state);
            bool serialCodeMatches(uint8_t code[], uint8_t length);

            bool checkMode(RobotState state);
            bool checkCriteria(Joystick* joystick);
            bool criteriaSatisfied(Joystick* joystick);
            
            TriggerType getTriggerType();

        private:
            StickIndicator m_stick;
            StickEvent m_stick_event;

            ButtonIndicator m_button;
            ButtonEvent m_button_event;

            SerialEvent m_serial_event;
            uint8_t m_serial_code[kMaxPacketSize];
            uint8_t m_serial_code_length;

            TriggerType m_type;
            TriggerModes m_modes;
            bool m_inverted;

            // std::shared_ptr<Trigger> m_criteria; // Linked list situation
            std::vector<std::shared_ptr<Trigger>> m_criteria; // Vector situation
    };

    class Trigger_Event {
        public:
            Trigger_Event(TriggerEffect effect, Trigger* trigger);
            Trigger_Event(TriggerEffect effect, Trigger* trigger, Command* command);
            Trigger_Event(TriggerEffect effect, Trigger* trigger, int m_command_id);
            ~Trigger_Event();

            bool matchesEvent(StickIndicator stick, StickEvent event, RobotState state, Joystick* joystick);
            bool matchesEvent(ButtonIndicator button, ButtonEvent event, RobotState state, Joystick* joystick);
            bool matchesEvent(SerialEvent event, uint8_t code[], uint8_t length, RobotState state);

            TriggerEffect getTriggerEffect();
            Command* getCommand();
            TriggerType getTriggerType();
            int getCommandId();
            Trigger* getTrigger();
        private:
            TriggerEffect m_effect;
            Trigger* m_trigger;

            Command* m_command; // Possible properties, not all used everywhere
            int m_command_id;
    };

}

#endif