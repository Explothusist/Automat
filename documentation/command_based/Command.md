# command_based/Command

File contents:

- Class `atmt::Comand`

## Class `atmt::Command`

    class Command {
        public:
            Command();
            Command(const Command& command);
            virtual ~Command();
            virtual Command* cloneSelf() const = 0;

            virtual void initialize(); // User-made
            virtual void execute(); // User-made
            virtual void end(bool interrupted); // User-made
            virtual bool is_finished(); // User-made

            void usesSubsystem(Subsystem* subsystem);
            void usesSubsystems(std::initializer_list<Subsystem*> subsystems);

            Command* withTimeout(double seconds);
    };

This is a base class that describes a single, repeatable action with a defined beginning and end. It may be bound to a `Trigger` on a `Joystick` or `Joystick`-like.

The `Command` itself should primarily call `Subsystem` methods rather than having complex logic itself. For the duration of the `Command` ownership of the `Subsystem`(s) flagged by the constructor is guarenteed. If new `Command` using the same `Subsystem`(s) is scheduled, the old `Command` will be interrupted and end early.

The `CommandScheduler` runs the user-defined virtual methods in the following manner until `end()` returns or the command is interrupted:

    initialize() -> execute() -> is_finished() -(true)--> end()
                                               -(false)-> execute()

Usage example (taken from `TimedRobot-Kampf-Starter-Esp32/src/commands/ArmWeaponCommand.h` in [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)):

    class ArmWeaponCommand : public atmt::Command {
        public:
            ArmWeaponCommand(kmpf::WeaponMotor* weapon); // Put subsystems as parameters
            ArmWeaponCommand(ArmWeaponCommand& command); // Copy constructor
            ~ArmWeaponCommand();
            atmt::Command* cloneSelf() const override;

            void initialize() override; // User-made
            void execute() override; // User-made
            void end(bool interrupted) override; // User-made
            bool is_finished() override; // User-made
            
        private:
            kmpf::WeaponMotor* m_weapon;
    };

With the corresponding source code(taken from `TimedRobot-Kampf-Starter-Esp32/src/commands/ArmWeaponCommand.cpp` in [Explothusist/Automat-Example-Code](https://github.com/Explothusist/Automat-Example-Code)):

    ArmWeaponCommand::ArmWeaponCommand(kmpf::WeaponMotor* weapon):
        atmt::Command(),
        m_weapon{ weapon }
    {
        usesSubsystem(m_weapon);
    };
    ArmWeaponCommand::ArmWeaponCommand(ArmWeaponCommand& command):
        atmt::Command(command)
    {
        m_weapon = command.m_weapon;
    };
    ArmWeaponCommand::~ArmWeaponCommand() {
        // Will run ~Command() after this is complete
    };
    atmt::Command* ArmWeaponCommand::cloneSelf() const {
        return new ArmWeaponCommand(m_weapon);
    };

    void ArmWeaponCommand::initialize() {
        m_weapon->armMotor();
    };
    void ArmWeaponCommand::execute() {

    };
    void ArmWeaponCommand::end(bool interrupted) {
        
    };
    bool ArmWeaponCommand::is_finished() {
        return true;
    };

### Constructor

All `Subsytem` instances which the command uses should be passed as paramters and set as requirements by calling `usesSubsystem(atmt::Subsystem*)` or `usesSubsystems(std::initializer_list<Subsystem*>)`. Other values may be passed and stored as properties as well, but values passed in should be treated as constants to avoid perculiar behavior when the `CommandScheduler` copies the `Command`.

### Copy Constructor and `cloneSelf()`

Used by the `CommandScheduler` to copy the `Commmand` when the `Trigger` returns true. The copy is then scheduled. All values and `Subsystem` pointers which should persist betwen runs of the `Command` should therefore be copied.

### Method `initialize()`

    virtual void atmt::Command::initialize();

Called once immediately after the `Commmand` is scheduled.

### Method `execute()`

    virtual void atmt::Command::execute();

Called every loop of the `CommandScheduler` until the `Commmand` is interrupted or `is_finished()` returns `true`.

### Method `is_finished()`

    virtual bool atmt::Command::is_finished();

Called every loop of the `CommandScheduler`, immediately following `execute()`. If it returns `true`, then `Command` terminates, immediately calling `end(false)` before being removed from the `CommandScheduler`. If it returns `false`, the `Command` remains scheduled and will be called again on the next loop of the `CommandScheduler`.

### Method `end()`

    virtual void atmt::Command::end(bool interrupted);

Called once immediately before the `Command` is removed from the `CommandScheduler`. If the `Command` was interrupted by another `Command` being scheduled that uses the same `Subsystem`(s), `interrupted` will be `true`. If the `Command` finished normally via `is_finished()` returning `true`, `interrupted` will be `false`.

### Method `usesSubsystem()`

    void atmt::Command::usesSubsystem(Subsystem* subsystem);

Used in the constructor to specify individual `Subsystem` instances which this `Command` requires. Exclusive ownership of all required `Subsystem`(s) is guarenteed while the `Command` is running. When scheduled, all older `Command` instances using any of the same `Subsystem` instances will be interrupted.

### Method `usesSubsystems()`

    void atmt::Command::usesSubsystems(std::initializer_list<Subsystem*> subsystems);

Used in the constructor to specify a list of `Subsystem` instances which this `Command` requires. Exclusive ownership of all required `Subsystem`(s) is guarenteed while the `Command` is running. When scheduled, all older `Command` instances using any of the same `Subsystem` instances will be interrupted.

### Method `withTimeout()`

    Command* atmt::Command::withTimeout(double seconds);

Used to specify a maximum run time for an individual instance of a `Command`. If the `Command` has not terminated normally or been interrupted by the time the timeout has elapsed, the `Command` will be interrupted.