# command_based/Joystick

File contents:

- Function `atmt::SetReadJoystickEvents(bool to_read)` **(Internal use)**
- Enum `atmt::JoystickType` **(VEX only)**
- Struct `atmt::JoystickState`
- Function `atmt::setJoystickStateButton()`
- Function `atmt::getJoystickStateButton()`
- Enum `atmt::PollingMode`
- Enum `atmt::RangeComp`
- Class `atmt::Joystick`

## Function `atmt::SetReadJoystickEvents()` **(Internal use)**

    atmt::SetReadJoystickEvents(bool to_read)

Used internally by `TimedRobot` to prevent `Joystick` events from being processed before the environment is fully initialized and the `TimedRobot` loop is started. **Not recommended for manual use.**

## Enum `atmt::JoystickType` **(VEX only)**

    typedef enum {
        NotAVexJoystick,
        PrimaryJoystick,
        PartnerJoystick
    } atmt::JoystickType;

Used in VEX environments to specify the input of the `Joystick`. VEX V5 supports connecting up to two VEX controllers to the VEX brain. `PrimaryJoystick` and `PartnerJoystick` refer to those two controllers, respectively. If one of those is specified, the `Joystick` input is entirely handled by Automat. `NotAVEXJoystick` reverts to the standard `Joystick` type options available in other environments.

## Struct `atmt::JoystickState`

    struct __attribute__((packed)) atmt::JoystickState {
        uint16_t buttons; // A, B, X, Y, Up, Left, Down, Right, S, s, LB, RB
        float axes[6]; // RY, RX, LY, LX, RT, LT
        float axis_range[2]; // min, max (i.e. -100, 100)
    };

Maps to a standard modern controller with two joysticks, one D-pad, four buttons, start, select, two bumpers, and two triggers.

Used to manually control `Joystick` classes in `PollMode_Manual` and `PollMode_Continuous`. Compressed and optimized to be able to be sent over Serial, ESPNow, or other communication protocols. Has a size of 34 bytes in standard environments. Use `setJoystickStateButton` and `getJoystickStateButton` to reference and set the button states, as they are single bit flags. Use enum `AxisIndicator` to index `axes` (see `Trigger.md`). Use enum `RangeComp` to index the `axis_range`.

Usage example (taken from `src/RobotContainer.cpp` in [Explothusist/Kampf-RIOT-Battlebots-Codebase](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Codebase))

    atmt::JoystickState RobotContainer::getJoystickStateFromGPIO() {
        atmt::JoystickState new_state;

        atmt::setJoystickStateButton(new_state, atmt::AButton, digitalRead(kmpf::consts::Controller::GPIO::k_TopButton));
        atmt::setJoystickStateButton(new_state, atmt::BButton, digitalRead(kmpf::consts::Controller::GPIO::k_BottomButton));
        atmt::setJoystickStateButton(new_state, atmt::L1Button, digitalRead(kmpf::consts::Controller::GPIO::k_LeftTrigger));
        atmt::setJoystickStateButton(new_state, atmt::R1Button, digitalRead(kmpf::consts::Controller::GPIO::k_RightTrigger));

        new_state.axes[atmt::LXAxis] = analogRead(kmpf::consts::Controller::GPIO::k_AxisLX);
        new_state.axes[atmt::LYAxis] = analogRead(kmpf::consts::Controller::GPIO::k_AxisLY);
        new_state.axes[atmt::RXAxis] = analogRead(kmpf::consts::Controller::GPIO::k_AxisRX);
        new_state.axes[atmt::RYAxis] = analogRead(kmpf::consts::Controller::GPIO::k_AxisRY);

        new_state.axis_range[atmt::Range_Min] = 0;
        new_state.axis_range[atmt::Range_Max] = 4096;

        return new_state;
    };


## Function `atmt::setJoystickStateButton()`

    void setJoystickStateButton(JoystickState &state, ButtonIndicator button, bool value);

Used to set button states in `JoystickState` instances. See `Trigger.md` for enum `ButtonIndicator`.

## Function `atmt::getJoystickStateButton()`

    bool getJoystickStateButton(const JoystickState &state, ButtonIndicator button);

Used to reference button states in `JoystickState` instances. See `Trigger.md` for enum `ButtonIndicator`.

## Enum `atmt::PollingMode`

    typedef enum {
        PollMode_Manual,
        PollMode_Continuous
    } PollingMode;

Used to specify how a `Joystick` should expect to receive state updates. `PollMode_Manual` leaves state updates entirely in the hands of the developer, who is expected to call `updateState()` with a new `JoystickState` periodically. `PollMode_Continuous` on VEX platforms instructs the library to use the native VEX joysticks and specify `JoystickType` (see enum `atmt::JoystickType`). `PollMode_Continuous` on all other platforms requires that a function that returns a `JoystickState` is passed to the `Joystick`, which will be called by the library periodically.

## Enum `atmt::RangeComp`

    typedef enum {
        Range_Min,
        Range_Max
    } RangeComp;

Used to index axis range values in `JoystickState` instances. See usage example under struct `atmt::JoystickState` for usage details.

## Class `atmt::Joystick`

    class atmt::Joystick : public atmt::Subsystem {
        public:
            Joystick(atmt::PollingMode poll_mode);
    #ifdef AUTOMAT_VEX_
            Joystick(atmt::PollingMode poll_mode, atmt::JoystickType type);
    #else
            Joystick(atmt::PollingMode poll_mode, std::function<atmt::JoystickState()> state_function);
    #endif

            void updateState(atmt::JoystickState new_state);
            void setStatePollingMode(atmt::PollingMode poll_mode);
    #ifndef AUTOMAT_VEX_
            void setStateFunction(std::function<atmt::JoystickState()> state_function);
    #endif

            void bindKey(atmt::Trigger* trigger, atmt::Command* command);
            void bindAutoTrigger(atmt::Trigger* trigger);
            void bindTeleopTrigger(atmt::Trigger* trigger);

            atmt::StickEvent getStickState(atmt::StickIndicator stick);
            atmt::ButtonEvent getButtonState(atmt::ButtonIndicator button);
            double getRawAxis(atmt::AxisIndicator axis);
    };

A class that represents a physical or virtual controller. `Command` instances and robot state changes may be bound to the `Joystick` with `Trigger` instances.

All updates to the state of the controller come through `updateState()` in the form of `JoystickState` objects. See enum `atmt::PollingMode` for more details.

`PollMode_Manual` usage example (taken from `src/RobotContainer.cpp` in [Explothusist/Kampf-RIOT-Battlebots-Codebase](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Codebase)):

    RobotContainer::RobotContainer():
        m_joystick{ new atmt::Joystick(atmt::PollingMode::PollMode_Manual) },

With corresponding state updating code (taken from `ESPNowReciever.cpp` in [Explothusist/Kampf-RIOT-Battlebots-Library](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Library)):

    m_linked_espnow_handler->packet.popNextMessagePrefixed(prefix, buffer, length, sender);
    if (length != sizeof(atmt::JoystickState)) break;
    if (sender != m_connected_controller_address) break;
    const atmt::JoystickState& packet = *reinterpret_cast<atmt::JoystickState*>(buffer);

    m_linked_joystick->updateState(packet);
    m_last_packet_timestamp = atmt::getSystemTime();

VEX `PollMode_Continuous` usage example, which requires no state updating code (taken from `src/RobotContainer.cpp` in [Explothusist/Stormbot-Codebase](https://github.com/Explothusist/Stormbot-Codebase)):

    RobotContainer::RobotContainer():
        m_driver_controller{ new atmt::Joystick(atmt::PollMode_Continuous, atmt::PrimaryJoystick) },

### Constructor

        atmt::Joystick(atmt::PollingMode poll_mode);
    #ifdef AUTOMAT_VEX_
        atmt::Joystick(atmt::PollingMode poll_mode, atmt::JoystickType type);
    #else
        atmt::Joystick(atmt::PollingMode poll_mode, std::function<atmt::JoystickState()> state_function);
    #endif

For `PollingMode_Manual`, use the `Joystick(PollingMode poll_mode)` constructor. For `PollingMode_Continuous`, use the platform-specific constructor. See enums `atmt::PollingMode`, `atmt::JoystickType`, and `atmt::JoystickState` for more details.

### Method `updateState()`

    void atmt::Joystick::updateState(atmt::JoystickState new_state);

This method is for `PollMode_Manual` on non-VEX platforms. Updates the internally stored state of the `Joystick` to what is specified by the `JoystickState` object, immediately triggering events for all state changes. See `atmt::JoystickState` for more details.

### Method `setStatePollingMode()`

    void atmt::Joystick::setStatePollingMode(atmt::PollingMode poll_mode);

Used to change the `PollingMode` of the `Joystick`. See `atmt::PollingMode` for more details. Changing the `PollingMode` to `PollMode_Continuous` in non-VEX environments will require a call to `setStateFunction()` as well.

### Method `setStateFunction()` **Non-VEX only**

    void atmt::Joystick::setStateFunction(std::function<atmt::JoystickState()> state_function);

Used to change the `state_function` of the `Joystick` in `PollMode_Continuous` in non-VEX environments. The `state_function` is called periodically to update the `Joystick` state and trigger events. See `atmt::PollingMode` for more details.

### Method `bindKey()`

    void atmt::Joystick::bindKey(Trigger* trigger, Command* command);

Specifies a `Command` to be run every time the provided `Trigger` returns `true`. Create a new `Command` instance and `Trigger` instance for every call of `bindKey()`. The `Command` will be copied every time it is triggered, meaning the bind does not need recreated and will work repeatedly.

### Method `bindAutoTrigger()`

    void atmt::Joystick::bindAutoTrigger(atmt::Trigger* trigger);

Adds the `Trigger` to the list of `Trigger`(s) which cause `TimedRobot` to enter Autonomous mode from Disabled mode. When `TimedRobot` changes state, all commands are interrupted. When `TimedRobot` enters Autonomous, the specified Autonomous command is run. In Autonomous, standard `Trigger`s will not return `true`. These state change binding methods are the only ways to affect `TimedRobot` state.

### Method `bindTeleopTrigger()`

    void atmt::Joystick::bindAutoTrigger(atmt::Trigger* trigger);

Adds the `Trigger` to the list of `Trigger`(s) which cause `TimedRobot` to enter Teleop mode from Disabled mode or Autonomous mode. When `TimedRobot` changes state, all commands are interrupted. These state change binding methods are the only ways to affect `TimedRobot` state.

### Method `bindDisabledTrigger()`

    void atmt::Joystick::bindDisabledTrigger(atmt::Trigger* trigger);

Adds the `Trigger` to the list of `Trigger`(s) which cause `TimedRobot` to enter Disabled mode from Autonomous mode or Teleip mode. When `TimedRobot` changes state, all commands are interrupted. In Disabled, standard `Trigger`s will not return `true`. These state change binding methods are the only ways to affect `TimedRobot` state.

### Method `getStickState()`

    atmt::StickEvent atmt::Joystick::getStickState(atmt::StickIndicator stick);

Returns a `StickEvent` representing the current state of the joystick indicated by `StickIndicator` as specified by the most recent `JoystickState`. See enums `StickIndicator` and `StickEvent` in `Trigger.md` for more details.

### Method `getButtonState()`

    atmt::ButtonEvent atmt::Joystick::getButtonState(atmt::ButtonIndicator button);

Returns a `ButtonEvent` representing the current state of the button indicated by `ButtonIndicator` as specified by the most recent `JoystickState`. See enums `ButtonIndicator` and `ButtonEvent` in `Trigger.md` for more details.

### Method `getRawAxis()`

    double atmt::Joystick::getRawAxis(atmt::AxisIndicator axis);

Returns a `double` representing the current raw percentage value of the axis indicated by `AxisIndicator` as specified by the most recent `JoystickState`. See enum `AxisIndicator` in `Trigger.md` for more details.