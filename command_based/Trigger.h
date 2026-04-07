#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_COMMAND_BASED_

#ifndef AUTOMAT_TRIGGER_
#define AUTOMAT_TRIGGER_

#include <vector>
#include <memory>

#include "Command.h"
#include "../utils.h"

namespace atmt {
    
    class Joystick;

    // class Command; // Forward declare because it never actually uses it

    typedef enum {
        NULLStick = -1,
        LeftStick = 0,
        RightStick = 1
    } StickIndicator;
    typedef enum {
        StickNULL = -1,
        StickUp = 0,
        StickDown = 1,
        StickLeft = 2,
        StickRight = 3,
        StickCenter = 4
    } StickEvent;
    typedef enum {
        NULLButton = -1,
        AButton = 0,
        BButton = 1,
        XButton = 2,
        YButton = 3,
        UpButton = 4,
        LeftButton = 5,
        DownButton = 6,
        RightButton = 7,
        StartButton = 8, // ESP32 only
        SelectButton = 9, // ESP32 only
        L1Button = 10,
        L2Button = 11,
        R1Button = 12,
        R2Button = 13
    } ButtonIndicator;
    typedef enum {
        ButtonNULL = -1,
        ButtonPressed = 0,
        ButtonReleased = 1
    } ButtonEvent;
    typedef enum {
        NULLSerial = -1,
        SerialReceive = 0
    } SerialEvent;
    typedef enum {
        NULLTrigger = -1,
        OnTrigger = 0,
        WhileTrigger = 1
    } TriggerType;
    typedef enum {
        ModeNULL = -1,
        ModeTeleopOnly = 0,
        ModeAutonomousOnly = 1,
        ModeTeleopAndAuto = 2,
        ModeDisabled = 3, // ONLY FOR AUTONOMOUS TRIGGERS
        ModeDisabledAndAuto = 4, // ONLY FOR TELEOP TRIGGERS
        ModeAnyAndAll = 5 // DO NOT USE
    } TriggerModes;
    typedef enum {
        NULLAxis = -1,
        RYAxis = 0,
        RXAxis = 1,
        LYAxis = 2,
        LXAxis = 3,
        RTAxis = 4, // ESP32 only
        LTAxis = 5, // ESP32 only
    } AxisIndicator;

    typedef enum {
        StartCommand = 0,
        EndCommand = 1,
        StartAutonomous = 2,
        StartTeleop = 3
    } TriggerEffect;

    class Trigger {
        public:
            Trigger();
            Trigger(StickIndicator stick, StickEvent event);
            // Trigger(StickIndicator stick, StickEvent event, TriggerType type);
            Trigger(ButtonIndicator button, ButtonEvent event);
            // Trigger(ButtonIndicator button, ButtonEvent event, TriggerType type);
            Trigger(SerialEvent event, uint8_t code[], uint8_t length);
            Trigger(SerialEvent event, uint8_t code);
            Trigger(const Trigger &trigger) = default;
            ~Trigger() = default;

            Trigger* setType(TriggerType type);
            Trigger* setCriteria(std::shared_ptr<Trigger> criteria);
            Trigger* inMode(TriggerModes modes);
            Trigger* invert();
            Trigger* allowPartial(); // For Serial
            Trigger* fromSender(uint8_t sender); // For Serial

            bool matchesEvent(StickIndicator stick, StickEvent event, RobotState state, Joystick* joystick);
            bool matchesEvent(ButtonIndicator button, ButtonEvent event, RobotState state, Joystick* joystick);
            bool matchesEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length, RobotState state);
            bool serialCodeMatches(uint8_t sender, uint8_t code[], uint8_t length);

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
            bool m_allow_partial; // For Serial
            bool m_from_sender; // For Serial
            uint8_t m_sender; // For Serial

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
            bool matchesEvent(SerialEvent event, uint8_t sender, uint8_t code[], uint8_t length, RobotState state);

            TriggerEffect getTriggerEffect();
            Command* getCommand();
            TriggerType getTriggerType();
            int getCommandId();
            Trigger* getTrigger();

            void setSerialMessageId(int id);

        private:
            TriggerEffect m_effect;
            Trigger* m_trigger;

            Command* m_command; // Possible properties, not all used everywhere
            int m_command_id;
    };

}

#endif

#else
#error "Enable ATMT_SUBMODULE_COMMAND_BASED_ in automat_submodules.h to use Trigger, Trigger_Event"
#endif