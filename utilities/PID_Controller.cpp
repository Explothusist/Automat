#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_

#include "PID_Controller.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include "../utils.h"

namespace atmt {

    PID_Controller::PID_Controller():
        PID_Controller(
            1.0,
            0.0,
            0.0,
            1.0
        )
    {

    };
    PID_Controller::PID_Controller(double P, double I, double D, double timestep):
        PID_Controller(P, I, D, timestep,
            0.0,                    // FF
            P != 0.0 ? I / P : 0.0, // AW, Decent baseline

            -INFINITY,              // min_out
            INFINITY,               // max_out
            INFINITY                // max_rate
        )
    {
        m_auto_AW = true;
        m_auto_timeconst = true;
    };
    PID_Controller::PID_Controller(double P, double I, double D, double timestep, double FF, double AW, double min_out, double max_out, double max_rate):
        PID_Controller(P, I, D, timestep, FF, AW,
            min_out, max_out, max_rate,
            5 * timestep,
            -INFINITY,
            INFINITY
        )
    {
        m_auto_AW = false;
        m_auto_timeconst = true;
    };
    PID_Controller::PID_Controller(double P, double I, double D, double timestep, double FF, double AW, double min_out, double max_out, double max_rate, double timeconst, double min_I, double max_I):
        m_P{ P },
        m_I{ I },
        m_D{ D },
        m_timestep{ timestep },
        m_FF{ FF },
        m_AW{ AW },
        m_DTimeConst{ timeconst },

        m_Min_Out{ min_out },
        m_Max_Out{ max_out },
        m_Max_Rate{ std::abs(max_rate) },
        m_Min_I{ min_I },
        m_Max_I{ max_I },

        m_integral{ 0.0 },
        m_old_error{ 0.0 },
        m_old_derivative{ 0.0 },
        m_old_location{ 0.0 },
        m_old_output{ 0.0 },
        m_old_output_raw{ 0.0 },

        m_auto_AW{ false },
        m_auto_timeconst{ false }
    {

    };

    double PID_Controller::calculate_correction(double location, double setpoint) {
        double error = setpoint - location;

        // Integral Calculation
        m_integral += m_I * error * m_timestep; // Accumulator because integral
        m_integral += m_AW * (m_old_output - m_old_output_raw) * m_timestep; // to prevent integrator windup
        // The second piece kicks in when the output is rate limited or min/max bounded
        m_integral = clamp(m_integral, m_Min_I, m_Max_I); // Clamp to I range

        // Derivative Calculation (Filtered Derivative method)
        // double derivative = (error - m_old_error) / m_timestep; // Basic, but to smooth the output, we use the other
        // double derivative = (error - m_old_error + m_DTimeConst * m_old_derivative) / (m_timestep + m_DTimeConst);
        double derivative = -(location - m_old_location + m_DTimeConst * m_old_derivative) / (m_timestep + m_DTimeConst);
        // The filtered derivative algorithm remembers derivatives for a time determined by m_DTimeConst
        // Filtered Derivative is a simple formula derived from and a part of Fourier Analysis
        // Deriving based on location prevents massive D term punch after setpoint change

        double output_raw = m_FF * setpoint + m_P * error + m_integral + m_D * derivative;
        // double output = std::min(m_Max_Out, std::max(m_Min_Out, output_raw)); // Range limits
        double output = clamp(output_raw, m_Min_Out, m_Max_Out); // Same, except clamp
        double max_change = m_Max_Rate * m_timestep; // Calculate once instead of four times
        // output = (output > (m_old_output + max_change) ? (m_old_output + max_change) : output); // Upward rate limits
        // output = (output < (m_old_output - max_change) ? (m_old_output - max_change) : output); // Downward rate limits
        output = clamp(output, m_old_output - max_change, m_old_output + max_change); // Clamp upward and downward limits

        m_old_error = error;
        m_old_derivative = derivative;
        m_old_location = location;
        m_old_output = output;
        m_old_output_raw = output_raw;

        return output;
    };
    void PID_Controller::reset() {
        m_integral = 0.0;
        m_old_error = 0.0;
        m_old_derivative = 0.0;
        // m_old_location = 0.0; // Prevent derivative spike
        m_old_output = 0.0;
        m_old_output_raw = 0.0;
    };
    void PID_Controller::locationReset() {
        reset();
        m_old_location = 0.0;
    };

    void PID_Controller::setP(double P) {
        m_P = P;
        if (m_auto_AW) {
            calcAW();
        }
    };
    void PID_Controller::setI(double I) {
        m_I = I;
        if (m_auto_AW) {
            calcAW();
        }
    };
    void PID_Controller::setD(double D) {
        m_D = D;
    };
    void PID_Controller::setTimestep(double timestep) {
        m_timestep = timestep;
        if (m_auto_timeconst) {
            calcTimeConst();
        }
    };
    void PID_Controller::setDTimeConst(double timeconst) {
        m_DTimeConst = timeconst;
        m_auto_timeconst = false;
    };
    void PID_Controller::setFF(double FF) {
        m_FF = FF;
    };
    void PID_Controller::setAW(double AW) {
        m_AW = AW;
        m_auto_AW = false;
    };
    void PID_Controller::setMinOutput(double min_out) {
        m_Min_Out = min_out;
    };
    void PID_Controller::setMaxOutput(double max_out) {
        m_Max_Out = max_out;
    };
    void PID_Controller::setMaxRate(double max_rate) {
        m_Max_Rate = std::abs(max_rate);
    };
    void PID_Controller::setMinI(double min_I) {
        m_Min_I = min_I;
    };
    void PID_Controller::setMaxI(double max_I) {
        m_Max_I = max_I;
    };

    void PID_Controller::autoSetAW(bool flag) {
        m_auto_AW = flag;
        if (m_auto_AW) {
            calcAW();
        }
    };
    void PID_Controller::autoSetTimeConst(bool flag) {
        m_auto_timeconst = flag;
        if (m_auto_timeconst) {
            calcTimeConst();
        }
    };

    void PID_Controller::calcAW() {
        m_AW = m_P != 0.0 ? m_I / m_P : 0.0;
    };
    void PID_Controller::calcTimeConst() {
        m_DTimeConst = 5 * m_timestep;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_ in automat_submodules.h to use PID_Controller"
#endif