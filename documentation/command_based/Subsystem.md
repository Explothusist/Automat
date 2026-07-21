# command_based/Subsystem

File contents:

- Class `atmt::Subsystem`

## Class `atmt::Subsystem`

    class Subsystem {
        public:
            Subsystem();
            virtual ~Subsystem();

            virtual void init(); // User-made
            virtual void systemPeriodic(); // User-made
            virtual void disabledPeriodic(); // User-made
            virtual void autonomousPeriodic(); // User-made
            virtual void teleopPeriodic(); // User-made

            void setDefaultCommand(Command* command);
            bool hasDefaultCommand();
    };

This is a base class that describes a discrete subset of the robot, typically either a single motor or a small set of motors. Every `Subsystem` should be able to operate independently. Only one `Command` can run per `Subsystem`, so splitting `Subsystems` as finely as possible is often helpful.

The bulk of motor control code and complex logic should be in `Subsystem` methods. The primary role of `Commands` is to call the appropriate `Subsystem` methods with the appropriate arguments at the appropriate times.

The `Subsystem` class has periodic functions which are run by the `CommandScheduler`. `systemPeriodic()` runs in all modes, while the other periodics only run in the specified mode. A default `Command` may also be specified, to be run whenever no other `Command` is running on the `Subsystem`.

All `Subsystem` instances must be registered with the `TimedRobot` in order for periodics and default `Command` to run, and in order for triggered `Command` instances to run properly using the `Subsystem`.

Usage example (taken from `WeaponMotor.h` in [Explothusist/Kampf-RIOT-Battlebots-Library](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Library)):

    class WeaponMotor : public atmt::Subsystem {
        public:
            WeaponMotor();
            WeaponMotor(int pin);
            WeaponMotor(int pin, double pwm_min_us, double pwm_max_us, double pwm_slew_rate_us);
            ~WeaponMotor() override;

            void init() override;
            void systemPeriodic() override;
            void disabledPeriodic() override;
            void autonomousPeriodic() override;
            void teleopPeriodic() override;

            void armMotor();
            void disarmMotor();
            void spinMotor(double percent_power);
            void spinDown();
            void cutoff();
        private:
            Servo m_motor;
            int m_pin;
            atmt::SlewRateLimiter m_current_pwm_width;
            double m_pwm_width_minimum_us; // For percent power
            double m_pwm_width_range_us;
            double m_pwm_target_width_us;

            bool m_is_armed;
            atmt::Timestamp m_last_command;
    };

With the corresponding source code(taken from `WeaponMotor.cpp` in [Explothusist/Kampf-RIOT-Battlebots-Library](https://github.com/Explothusist/Kampf-RIOT-Battlebots-Library)):

    WeaponMotor::WeaponMotor():
        WeaponMotor(
            consts::WeaponMotor::k_Pin
        )
    {
        
    }
    WeaponMotor::WeaponMotor(int pin):
        WeaponMotor(
            pin,
            consts::WeaponMotor::k_MinPWMPulseWidthUs,
            consts::WeaponMotor::k_MaxPWMPulseWidthUs,
            consts::WeaponMotor::k_MaxPWMPulseSlewRate
        )
    {
        
    }
    WeaponMotor::WeaponMotor(int pin, double pwm_min_us, double pwm_max_us, double pwm_slew_rate_us):
        m_motor{ },
        m_pin{ pin },
        m_current_pwm_width( pwm_slew_rate_us, -pwm_slew_rate_us, pwm_min_us, pwm_max_us, pwm_min_us ),
        m_pwm_width_minimum_us{ pwm_min_us },
        m_pwm_width_range_us{ pwm_max_us - pwm_min_us },
        m_pwm_target_width_us{ pwm_min_us },
        m_is_armed{ false },
        m_last_command{ atmt::Timestamp{0} }
    {
        
    }
    WeaponMotor::~WeaponMotor() {

    };

    void WeaponMotor::init() {
        pinMode(consts::LED::k_LED3_Armed, OUTPUT);

        digitalWrite(consts::LED::k_LED3_Armed, LOW);
    };
    void WeaponMotor::systemPeriodic() {
        if (!m_is_armed) return;

        if (m_last_command.getTimeDifference(atmt::getSystemTime()) > consts::WeaponMotor::k_SpindownTimeoutSec) {
            m_pwm_target_width_us = m_pwm_width_minimum_us;
        }
        m_motor.writeMicroseconds(m_current_pwm_width.calculateFiltered(m_pwm_target_width_us));
    };
    void WeaponMotor::disabledPeriodic() {
        disarmMotor();
    };
    void WeaponMotor::autonomousPeriodic() {};
    void WeaponMotor::teleopPeriodic() {};

    void WeaponMotor::armMotor() { // May need a delay in the arming sequence, depending on hardware?
        if (m_is_armed) return;

        digitalWrite(consts::LED::k_LED3_Armed, HIGH);
        atmt::platform_printf("Arming Weapon Motor\n");
        // m_motor.detach();
        m_motor.attach(m_pin);
        m_pwm_target_width_us = m_pwm_width_minimum_us;
        m_current_pwm_width.resetValue(m_pwm_width_minimum_us);
        m_motor.writeMicroseconds(m_pwm_width_minimum_us);
        m_is_armed = true;
        m_last_command = atmt::getSystemTime();
    };
    void WeaponMotor::disarmMotor() {
        if (!m_is_armed) return;

        digitalWrite(consts::LED::k_LED3_Armed, LOW);
        atmt::platform_printf("Disarming Weapon Motor\n");
        m_pwm_target_width_us = m_pwm_width_minimum_us;
        m_current_pwm_width.resetValue(m_pwm_width_minimum_us);
        m_motor.writeMicroseconds(m_pwm_width_minimum_us);
        m_motor.detach();
        m_is_armed = false;
        m_last_command = atmt::getSystemTime();
    };
    void WeaponMotor::spinMotor(double percent_power) {
        if (!m_is_armed) return;

        percent_power = std::max(0.0, std::min(percent_power, 1.0));
        m_pwm_target_width_us = m_pwm_width_minimum_us + m_pwm_width_range_us * percent_power;
        m_last_command = atmt::getSystemTime();
    };
    void WeaponMotor::spinDown() {
        if (!m_is_armed) return;

        spinMotor(0.0);
    };
    void WeaponMotor::cutoff() {
        if (!m_is_armed) return;

        spinMotor(0.0);
        m_current_pwm_width.resetValue(m_pwm_width_minimum_us);
    };

### Constructor

Constructor behavior is standard. No specific requirements.

### Destructor

Destructor behavior is standard. No specific requirements.

### Method `init()`

    virtual void atmt::Subsystem::init();

Called once immediately after `TimedRobot::environmentInit()` and `TimedRobot::robotInit()` are run.

### Method `systemPeriodic()`

    virtual void atmt::Subsystem::systemPeriodic();

Called every loop of the `CommandScheduler` in all `TimedRobot` modes.

### Method `disabledPeriodic()`

    virtual void atmt::Subsystem::disabledPeriodic();

Called every loop of the `CommandScheduler` when `TimedRobot` is in Disabled mode. A common use of this method is to stop all motor movement or whatever other safety features may be needed. **For safety reasons, this periodic should not cause motor movement**. That allows Disabled mode to be used as a configurable E-Stop.

### Method `autonomousPeriodic()`

    virtual void atmt::Subsystem::autonomousPeriodic();

Called every loop of the `CommandScheduler` when `TimedRobot` is in Autonomous mode.

### Method `teleopPeriodic()`

    virtual void atmt::Subsystem::teleopPeriodic();

Called every loop of the `CommandScheduler` when `TimedRobot` is in Teleop mode.

### Method `setDefaultCommand()`

    void atmt::Subsystem::setDefaultCommand(Command* command);

Sets a `Command` to be run whenever no other `Command` instances that use this `Subsystem` instance are running. The `Command` will be scheduled whenever a loop of the `CommandScheduler` runs and there is no other `Command` using the `Subsystem`. The `Command` will be interrupted whenever any other `Command` that uses the `Subsystem` is scheduled.

### Method `hasDefaultCommand()`

    bool atmt::Subsystem::hasDefaultCommand();

Returns `true` if a default `Command` has been specified via `setDefaultCommand()`, otherwise returns `false`.
