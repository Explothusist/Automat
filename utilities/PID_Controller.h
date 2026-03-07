#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_

#ifndef AUTOMAT_PID_CONTROLLER_
#define AUTOMAT_PID_CONTROLLER_

namespace atmt {

    class PID_Controller {
        public:
            PID_Controller();
            PID_Controller(double P, double I, double D, double timestep);
            PID_Controller(double P, double I, double D, double timestep, double FF, double AW, double min_out, double max_out, double max_rate);
            PID_Controller(double P, double I, double D, double timestep, double FF, double AW, double min_out, double max_out, double max_rate, double timeconst, double min_I, double max_I);

            double calculate_correction(double location, double setpoint);
            void reset();
            void locationReset();

            // void setSetpoint(double setpoint); // Changes too frequently

            void setP(double P);
            void setI(double I);
            void setD(double D);
            void setTimestep(double timestep);
            void setDTimeConst(double timeconst); // Set the length of the derivative's memory. 1T - light, 5T - moderate, 10T - strong filtering
            void setFF(double FF);
            void setAW(double AW); // Not just scaling factor, but tracking gain. Recommended as I or I/P
            void setMinOutput(double min_out);
            void setMaxOutput(double max_out);
            void setMaxRate(double max_rate);
            void setMinI(double min_I); // Only use when AW is small, integral is large for longr periods, or other certain cases
            void setMaxI(double max_I); // Only use when AW is small, integral is large for longr periods, or other certain cases

            void autoSetAW(bool flag);
            void autoSetTimeConst(bool flag);

            void calcAW();
            void calcTimeConst();
        private:
            // double m_setpoint; // Changes too frequently, so a parameter
            double m_P;
            double m_I;
            double m_D;
            double m_timestep;
            double m_FF;
            double m_AW; // Integral Anti-Windup
            double m_DTimeConst; // How long the derivative's memory is

            double m_Min_Out;
            double m_Max_Out;
            double m_Max_Rate;
            double m_Min_I; // Integral limits
            double m_Max_I;

            double m_integral; // Acculmulator
            double m_old_error;
            double m_old_derivative;
            double m_old_location;
            double m_old_output;
            double m_old_output_raw; // Not range limited 

            bool m_auto_AW;
            bool m_auto_timeconst;
    };
};

#endif

#else
#error "Enable ATMT_SUBMODULE_UTILITIES_PID_CONTROLLER_ in automat_submodules.h to use PID_Controller"
#endif