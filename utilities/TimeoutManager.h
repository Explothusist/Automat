#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_TIMEOUT_MANAGER_

#ifndef AUTOMAT_TIMEOUT_MANAGER_
#define AUTOMAT_TIMEOUT_MANAGER_

#include "../utils.h"

namespace atmt {

    class TimeoutManager {
        public:
            TimeoutManager(double min_timeout_sec);

            bool checkTimeout(); // Checks whether enough time has passed, reseting if enough has passed
            bool checkTimeoutWithoutReset(); // Checks whether enough time has passed, not reseting if enough has passed
            bool forceTimeout(); // Checks, ignoring how much time has passed
            void resetTimeout(); // Resets how much time has passed
            void setMinTimeout(double min_timeout_sec); // Sets minimum timeout
        private:
            Timestamp m_last_timestamp;
            bool m_timeout_unset;
            double m_min_timeout_sec;
    };
};

#endif

#else
#error "Enable ATMT_SUBMODULE_UTILITIES_SLEW_RATE_LIMITER_ in automat_submodules.h to use SlewRateLimiter"
#endif