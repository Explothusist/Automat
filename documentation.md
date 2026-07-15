# Documentation Structure

Under `documentation/` there are files for each class, organized in the same file structure as the the library itself (i.e. the documentation for `command_based/TimedRobot.h` is found under `documentation/command_based/TimedRobot.md`).

## Examples and Starter Code

If you are new to WPILib, it is **highly recommended** to use starter code. Starter code for all environments and many examples may be found under [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code).

## Supported Environments

The code automatically detects the environment, but this feature is not exhaustively tested. The auto detection is tested in the environments listed below. If detection errors occur, manually specify environment in `automat_platform.h`.

### **VEX V5**

Tested in VEX V5 using the VSCode extension. VEXcode is not supported because it does not, to my knowledge, allow multiple source files. May work for other VEX environments, but not explicitly tested or supported.

The ESPNow and HTTP Server submodules is not available on VEX platforms. On VEX platforms, the Serial submodule uses RS485 via the Smart Cable ports (RJ12 with straight-through wiring). RS485 to TTL converters are available to interface with Automat devices on other platforms.

For examples, see:

- [Explothusist/StormBot-Codebase](https://github.com/Explothusist/StormBot-Codebase)
- [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)

### **ESPIDF** 

Tested in PlatformIO VSCode extension using the ESPIDF framework, run on an ESP32-S3.

The HTTP Server submodule natively supports asynchronous functionality in this framework. The Serial submodule uses UART in this framework.

For examples, see:

- [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)

### **Arduino**

Tested in PlatformIO VSCode extension using the Arduino framework, run on an ESP32-S3.

The HTTP Server submodule requires esphome/ESPAsyncWebServer-esphome and esphome/AsyncTCP-esphome for asynchronous functionality in the Arduino framework, as asynchronous HTTP is not supported by the baseline interface in the Arduino framework. The Serial submodule uses UART in this framework. ESPNow is only available on ESP32 devices.

For examples, see:

- [Explothusist/StormBot-Codebase](https://github.com/Explothusist/StormBot-Codebase)
- [Explothusist/StormBot-Esp32-Codebase](https://github.com/Explothusist/StormBot-Esp32-Codebase)
- [Explothusist/StormBot-Camera](https://github.com/Explothusist/StormBot-Camera)
- [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)
- [Explothusist/Kampf-RIOT-Battlebots-Codebase](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Codebase)

## Submodules

### **Command-based**

This is the part of the library derived from WPILib. This submodule creates a task handling system based around Commands and Subsystems. A Subsystem is a discrete set of motors or functionality of the robot. A Command is a single specific action with a defined start and end. Commands are set to require specific Subsystems and have exclusive control over those Subsystem while being executed. Joysticks provide event Triggers to bind Commands to, causing them to run on user input. A Robot Container encapsulates the Subsystems, while a Timed Robot class encapsulates the Robot Container and manages Subsystems and Command scheduling. A master Robot State allows for Disabled, Autonomous, and Teleoperated modes for safety. Timed Robot, Subsystems, and Commands all contain periodic functions which run on loop in designated Robot States and trigger functions which run when certain Robot State events happen.

For examples, see:

- [Explothusist/StormBot-Codebase](https://github.com/Explothusist/StormBot-Codebase)
- [Explothusist/StormBot-Esp32-Codebase](https://github.com/Explothusist/StormBot-Esp32-Codebase)
- [Explothusist/StormBot-Camera](https://github.com/Explothusist/StormBot-Camera)
- [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)
- [Explothusist/Kampf-RIOT-Battlebots-Codebase](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Codebase)

### **Packet Handling**

This submodule is used by the communication protocol submodules, like ESPNow and Serial, both to encode and parse packets and to integrate with the Command-based system with Joystick-like Triggers for Commands. Automat uses a custom packet format, which is used universally across all transmissions in all protocols. Users need no knowledge of the underlying format, as a all encoding and parsing is handled by the library.

These classes are exposed so that developers may build custom tools for other protocols if needed.

For examples, see:

- [Explothusist/Automat](https://github.com/Explothusist/Automat)
- [Explothusist/Kampf-RIOT-Battlebots-Library](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Library)

### **ESPNow**

This submodule is only available on ESP32 devices. This lightweight protocol allows such devices to communicate directly without need for a WiFi network or router. The submodule supports single-cast and multi-cast and allows for multiple endpoints from a single MAC address via the Packet Handling address system, with each handler instance able to be tied to send to and listen from a single MAC address. Uses the Packet Handling submodule to encode and parse packets and interface with the Command-based submodule with Joystick-like Triggers.

For examples, see:

- [Explothusist/Kampf-RIOT-Battlebots-Codebase](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Codebase)
- [Explothusist/Kampf-RIOT-Battlebots-Library](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Library)

### **Serial**

This submodule uses RS485 on VEX platforms and UART on all other platforms. This widely available protocol allows communication with other Automat devices with different platforms. The submodule supports single-cast and multi-case and allows for multiple endpoints on a single pin and a number of devices on a single bus via the Packet Handling address system. Uses the Packet Handling submodule to encode and parse packets and interface with the Command-based submodule with Joystick-like Triggers.

For examples, see:

- [Explothusist/StormBot-Codebase](https://github.com/Explothusist/StormBot-Codebase)
- [Explothusist/StormBot-Esp32-Codebase](https://github.com/Explothusist/StormBot-Esp32-Codebase)
- [Explothusist/StormBot-Camera](https://github.com/Explothusist/StormBot-Camera)

### **HTTP Server**

This submodule requires either ESPIDF or Arduino environments. The HTTP Server submodule requires esphome/ESPAsyncWebServer-esphome and esphome/AsyncTCP-esphome for asynchronous functionality in the Arduino framework, as asynchronous HTTP is not supported by the baseline interface in the Arduino framework. This submodule allows for the hosting of simple web servers for robot connection, diagnostics, camera streaming, etc.

For examples, see:

- [Explothusist/StormBot-Camera](https://github.com/Explothusist/StormBot-Camera)

### **Other Utilies**

Automat also includes a number of other utilities, including:

- **KinematicPredictor**: Wraps a simple algorithm to predict future movement of an object in 1D based on past movement. Use multiple instances for 2D, 3D predictions. Originally developed for ROI (Region of Interest) tracking for AprilTag tracking cameras.
- **PID_Controller**: Provides basic PID loop calculation for environments without other PID tools.
- **SlewRateLimiter**: Wraps a double to limit the maximum change over real time. Originally developed for ramping motor speeds over time.
- **ThreadsafeBuffer**: A templatted class which wraps a type to allow thread safety with one read thread and one write thread. For clarity, Thread-safe funtionality is not required in the Automat Command-based framework.
- **TimeoutManager**: A class which allows a test to see if enough time has passed to limit the frequency of tasks. As blocking delays are not acceptable in the Automat Command-based framework, this tool helps to fill that purpose in a non-blocking way.
- **Vector2**: A relatively standard 2D Vector class for environments without other Vector tools.
- **Vector3**: A relatively standard 3D Vector class for environments without other Vector tools.

## WPILib and Command-Based Architectures

WPILib: https://docs.wpilib.org/en/stable/docs/software/what-is-wpilib.html

Command-based architecture: https://docs.wpilib.org/en/stable/docs/software/commandbased/what-is-command-based.html

## Differences from WPILib

`TimedRobot`: `CommandScheduler` has been integrated into `TimedRobot`. A new lifecycle hook `environmentInit()` has bee added, which runs after the constructor but before `robotInit()`. In some enviroments, VEX especially, initialization code in the constructor can cause strange errors, so put that code in `environmentInit()`. All `Subsystem` and `Joystick` instances must be registered through `registerSubsystem` and `registerJoystick` to operate properly. The `TimedRobot` must be started by calling `startLoop()`. In ESP32 and Arduino environments, call `runLoop()` in in the `loop()` function. In VEX, `TimedRobot` may optionally use the `vex::competition` interface, which can be specified with `setUsesCompetition()`. Autonomous commands are handled more flexibly, with either `setAutonomousRoutineGetter()` setting a pair of functions to return an int and use the int to select a routine respectively, or with `setAutonomousCommand()` for more classic single routine functionality. Autonomous and Teleop are not triggered by the Driver Station of Field Management System, so `Joystick` and other `Joystick`-likes have `Joystick.bindAutoTrigger()` and `Joystick.bindTeleopTrigger()` to trigger those modes, respectively. Autonomous also has a configurable length before automatically switching to Teleop. See `documentation/command_based/TimedRobot.md` for more details.

`RobotContainer`: The standard usage of `RobotContainer` is mostly unchanged. Declare `Subsystem`, `Joystick`, etc. instances as public properties of `RobotContainer`. Additional methods are typically added to interface with the expanded autonomous routine framework. Methods for setting the Autonomous and Teleop triggers are typically added for organization and clarity.

`Subsystem`: The `Subsystem` class has added mode-specific periodics like `TimedRobot`. Must call `TimedRobot.registerSubsystem()` before the `Subsystem()` will function properly. See `documentation/command_based/Subsystem.md` for more details.

`Joystick`: Read the `Joystick` class carefully, as it is substantially different. Instead of retreiving a `Trigger` and binding the `Command` to that `Trigger`, use `bindKey()` to specify a `Command` and `Trigger` to link. Use `bindAutoTrigger()` and `bindTeleopTrigger()` to link a `Trigger` to a mode change (there is intentionally no way to directly modify robot state). A `Joystick` is a `Subsystem`, but uses a specific `TimedRobot.registerJoystick()` method instead of the standard `TimedRobot.registerSubsystem()`. Direct translation between a device and a `Joystick` class is only possible in VEX environments, so two other options exist. Either the `Joystick` may be manually fed `JoystickState` objects, or given a getter to obtain a `JoystickState` object which it will call periodically. For examples of how to use this functionality to connect controllers via protocols such as UART serial or ESPNow, see [Explothusist/Kampf-RIOT-Battlebots-Library](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Library). See `documentation/command_based/Joystick.md` for more details.

`Trigger`: Read the `Trigger` class carefully, as it is substantially different. `Trigger` objects are now created manually and fed to `Joystick` and other `Joystick`-likes. A number of methods may be strung to customize the exact circumstances which trigger the `Trigger`. See `documentation/command_based/Trigger.md` for more details.

`Command`: `Command` classes must specify which `Subsystem` instances they use with the `usesSubsystem()` or `usesSubsystems()` method in the `Command` constructor. See `documentation/command_based/Command.md` for more details.

Helpers: `WaitCommand`, `EmptyCommand`, `InstantCommand`, `SequentialCommandGroup`, `ParallelCommandGroup`, and `ParallelRaceCommandGroup` are all present with very similar functionality under `command_based/command_helpers`. Some other WPILib helpers are available among the Other Utilities mini-submodules.

## General Structure

The general structure for an Automat project looks something like the following:

+ src/
    + commands/
        + DriveCommand.h
        + DriveCommand.cpp
        + LiftCommand.h
        + LiftCommand.cpp
        + etc.
    + subsystems/
        + Drivebase.h
        + Drivebase.cpp
        + Arm.h
        + Arm.cpp
        + etc.
    + Constants.h
    + RobotContainer.h
    + RobotContainer.cpp
    + Robot.h
    + Robot.cpp
    + main.cpp

The `src/commands/` folder holds various custom command classes, all derived from `Command`. Each command performs a specific action, like lifting the arm or driving in a direction, using the methods defined by the subsystems in `src/subsystems/`. Some code will be in these files, but mostly it will be calling the various subsystem methods at the proper times.  
The `src/subsystems/` folder holds custom subsystem classes, derived from `Subsystem`. Each subsystem contains the low-level code for a motor or small collection of motors such as the drivetrain, robot arm, or claw. The subsystem has methods which are run by the commands in the `src/commands/` folder. The bulk of the code will be in these files.  
The `src/Constants.h` file is exactly what it sounds like, a central location for all of the semi-arbitrary but nonetheless vitally important numbers.  
The `src/RobotContainer.h` and `src/RobotContainer.cpp` file holds a custom `RobotContainer` class, not derived, which contains instances of the subsystems, joysticks, and commands. This setup allows those components to be referenced inside the various loop and trigger methods of the `TimedRobot` class. Very little code will be in these files, merely instantiating classes and setting keybindings.  
The `src/Robot.h` and `src/Robot.cpp` file holds a class derived from `TimedRobot`, which contains the main loops and functions of the robot. Very little code will be in this file.  
The `src/main.cpp` file instantiates the class in `src/Robot.h` and starts the robot's main loop.  

This setup is by no means binding, but seems to work well. For an actual implementation as an example, see:

- [Explothusist/StormBot-Codebase](https://github.com/Explothusist/StormBot-Codebase)
- [Explothusist/StormBot-Esp32-Codebase](https://github.com/Explothusist/StormBot-Esp32-Codebase)
- [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)
- [Explothusist/Kampf-RIOT-Battlebots-Codebase](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Codebase)