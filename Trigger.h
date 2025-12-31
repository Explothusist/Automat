
#ifndef AUTOMAT_TRIGGER_
#define AUTOMAT_TRIGGER_

#include <vector>

#include "Command.h"

namespace atmt {

    // class Command; // Forward declare because it never actually uses it

    typedef enum {
        LeftStick,
        RightStick
    } StickIndicator;
    typedef enum {
        StickUp,
        StickDown,
        StickLeft,
        StickRight,
        StickCenter
    } StickEvent;
    typedef enum {
        AButton,
        BButton,
        XButton,
        YButton,
        UpButton,
        LeftButton,
        DownButton,
        RightButton,
        L1Button,
        L2Button,
        R1Button,
        R2Button
    } ButtonIndicator;
    typedef enum {
        ButtonPressed,
        ButtonReleased,
    } ButtonEvent;
    typedef enum {
        OnTrigger,
        WhileTrigger
    } TriggerType;
    typedef enum {
        Axis1,
        Axis2,
        Axis3,
        Axis4
    } AxisIndicator;

    typedef enum {
        StartCommand,
        EndCommand,
        StartAutonomous
    } TriggerEffect;

    class Trigger {
        public:
            Trigger(TriggerEffect effect);
            Trigger(TriggerEffect effect, TriggerType type, Command* command);
            Trigger(TriggerEffect effect, int m_command_id);
            virtual ~Trigger();

            virtual bool matchesEvent(StickIndicator stick, StickEvent event);
            virtual bool matchesEvent(ButtonIndicator stick, ButtonEvent event);
            TriggerEffect getTriggerEffect();

            Command* getCommand();
            TriggerType getTriggerType();
            int getCommandId();
        private:
            TriggerEffect m_effect;

            TriggerType m_type; // Possible properties, not all used everywhere
            Command* m_command;
            int m_command_id;
    };
    class StickTrigger : public Trigger {
        public:
            StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, Command* command); // StartCommand
            StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, TriggerType type, Command* command);
            StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, int command_id); // EndCommand
            StickTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event); // StartAutonomous
            ~StickTrigger();

            bool matchesEvent(StickIndicator stick, StickEvent event) override;
            bool matchesEvent(ButtonIndicator button, ButtonEvent event) override;

            StickIndicator m_stick;
            StickEvent m_event;
        private:
    };
    class ButtonTrigger : public Trigger {
        public:
            ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, Command* command); // StartCommand
            ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, TriggerType type, Command* command);
            ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, int command_id); // EndCommand
            ButtonTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event); // StartAutonomous
            ~ButtonTrigger();

            bool matchesEvent(StickIndicator stick, StickEvent event) override;
            bool matchesEvent(ButtonIndicator button, ButtonEvent event) override;
            
            ButtonIndicator m_button;
            ButtonEvent m_event;
        private:
    };


    // class EndingTrigger {
    //     public:
    //         EndingTrigger(int command_id);
    //         virtual ~EndingTrigger();

    //         virtual bool matchesEvent(StickIndicator stick, StickEvent event);
    //         virtual bool matchesEvent(ButtonIndicator stick, ButtonEvent event);
    //         int getCommandId();
    //     private:
    //         int m_command_id;
    // };
    class StickEndingTrigger : public StickTrigger{
        public:
            StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, Command* command); // StartCommand
            StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, TriggerType type, Command* command);
            StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event, int command_id); // EndCommand
            StickEndingTrigger(TriggerEffect effect, StickTrigger* trigger, int command_id); // EndCommand
            StickEndingTrigger(TriggerEffect effect, StickIndicator stick, StickEvent event); // StartAutonomous
            ~StickEndingTrigger();

            bool matchesEvent(StickIndicator stick, StickEvent event) override;
            bool matchesEvent(ButtonIndicator button, ButtonEvent event) override;
        private:
    };
    class ButtonEndingTrigger : public ButtonTrigger {
        public:
            ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, Command* command); // StartCommand
            ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, TriggerType type, Command* command);
            ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event, int command_id); // EndCommand
            ButtonEndingTrigger(TriggerEffect effect, ButtonTrigger* trigger, int command_id); // EndCommand
            ButtonEndingTrigger(TriggerEffect effect, ButtonIndicator button, ButtonEvent event); // StartAutonomous
            ~ButtonEndingTrigger();

            bool matchesEvent(StickIndicator stick, StickEvent event) override;
            bool matchesEvent(ButtonIndicator button, ButtonEvent event) override;
        private:
    };

}

#endif