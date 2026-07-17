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

## Function `atmt::setJoystickStateButton()`

    void setJoystickStateButton(JoystickState &state, ButtonIndicator button, bool value);

Used to set button states in `JoystickState` instances. See `Trigger.md` for enum `ButtonIndicator`.

## Function `atmt::getJoystickStateButton()`

    bool getJoystickStateButton(const JoystickState &state, ButtonIndicator button);

Used to reference button states in `JoystickState` instances. See `Trigger.md` for enum `ButtonIndicator`.

## Enum `atmt::PollingMode` ***WORKING HERE***