#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_TIMEOUT_MANAGER_

#include "TimeoutManager.h"
#include "../utils.h"

namespace atmt {

    TimeoutManager::TimeoutManager(double min_timeout_sec):
        m_last_timestamp{ Timestamp(0) },
        m_timeout_unset{ true },
        m_min_timeout_sec{ min_timeout_sec }
    {
        
    };

    bool TimeoutManager::checkTimeout() {
        Timestamp now = getSystemTime();
        double elapsed = m_last_timestamp.getTimeDifference(now);
        if (elapsed > m_min_timeout_sec || m_timeout_unset) {
            m_last_timestamp = now;
            m_timeout_unset = false;
            return true;
        }else {
            return false;
        }
    };
    bool TimeoutManager::forceTimeout() {
        m_last_timestamp = getSystemTime();
        m_timeout_unset = false;
        return true;
    };
    void TimeoutManager::resetTimeout() {
        m_last_timestamp = getSystemTime();
        m_timeout_unset = false;
    };
    void TimeoutManager::setMinTimeout(double min_timeout_sec) {
        m_min_timeout_sec = min_timeout_sec;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_UTILITIES_SLEW_RATE_LIMITER_ in automat_submodules.h to use SlewRateLimiter"
#endif