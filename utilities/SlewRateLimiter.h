#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_SLEW_RATE_LIMITER_

#ifndef AUTOMAT_SLEW_RATE_LIMITER_
#define AUTOMAT_SLEW_RATE_LIMITER_

#include "../utils.h"

namespace atmt {

    class SlewRateLimiter {
        public:
            SlewRateLimiter(double rate_limit); // Applies [+rate_limit, -rate_limit], units per second, Initial of 0
            SlewRateLimiter(double pos_rate_limit, double neg_rate_limit); // Applies [pos_rate_limit, neg_rate_limit], units per second, Initial of 0
            SlewRateLimiter(double pos_rate_limit, double neg_rate_limit, double initial_value); // Applies [pos_rate_limit, neg_rate_limit], units per second, Initial of initial_value
            SlewRateLimiter(double pos_rate_limit, double neg_rate_limit, double initial_value, double pos_bound, double neg_bound); // Applies [pos_rate_limit, neg_rate_limit], units per second, Initial of initial_value, output bounds of [pos_bound, neg_bound] units

            double calculateFiltered(double input); // Filters input based on rate_limit
            double getLastValue() const; // Returns last calculated value
            void resetValue(double value); // Resets, ignoring rate
            void setRateLimit(double rate_limit); // Applies [+rate_limit, -rate_limit], units per second
            void setRateLimit(double pos_rate_limit, double neg_rate_limit); // Applies [pos_rate_limit, neg_rate_limit], units per second
            void setBounds(double bounds); // Applies [+bounds, -bounds] to ouput, units
            void setBounds(double pos_bound, double neg_bound); // Applies [pos_bounds, neg_bounds] to ouput, units
            void removeBounds();
        private:
            double m_last_value; // Last calculated value
            Timestamp m_last_timestamp; // Time for interpolation of rate

            double m_pos_rate_limit;
            double m_neg_rate_limit;
            double m_pos_bound;
            double m_neg_bound;
            bool m_bounds_set;
    };
};

#endif

#else
#error "Enable ATMT_SUBMODULE_UTILITIES_SLEW_RATE_LIMITER_ in automat_submodules.h to use SlewRateLimiter"
#endif