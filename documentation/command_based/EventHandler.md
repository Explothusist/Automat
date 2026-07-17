# command_based/EventHandler

File contents:

- Function `atmt::SetReadEvents()` **(Internal use)**
- Class `atmt::EventHandler` **(Internal use)**

## Function `atmt::SetReadEvents()` **(Internal use)**

    void atmt::SetReadEvents(bool to_read)

Used internally by `TimedRobot` to prevent events from being processed before the environment is fully initialized and the `TimedRobot` loop is started. **Not recommended for manual use.**

## Class `atmt::EventHandler()`

Used internally by `TimedRobot` to manage events from `Joystick` and `Joystick`-like classes. **Not recommended for manual use.**