#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_SLEW_RATE_LIMITER_

#include "SlewRateLimiter.h"
#include "../utils.h"

namespace atmt {

    SlewRateLimiter::SlewRateLimiter(double rate_limit):
        SlewRateLimiter(rate_limit, -rate_limit, 0.0)
    {
        
    };
    SlewRateLimiter::SlewRateLimiter(double pos_rate_limit, double neg_rate_limit):
        SlewRateLimiter(pos_rate_limit, neg_rate_limit, 0.0)
    {
        
    };
    SlewRateLimiter::SlewRateLimiter(double pos_rate_limit, double neg_rate_limit, double initial_value):
        m_last_value{ initial_value },
        m_last_timestamp{ getSystemTime() },
        m_pos_rate_limit{ pos_rate_limit },
        m_neg_rate_limit{ neg_rate_limit }
    {

    };
    SlewRateLimiter::SlewRateLimiter(double pos_rate_limit, double neg_rate_limit, double initial_value, double pos_bound, double neg_bound):
        SlewRateLimiter(pos_rate_limit, neg_rate_limit, initial_value)
    {
        setBounds(pos_bound, neg_bound);
    };

    double SlewRateLimiter::calculateFiltered(double input) {
        Timestamp now = getSystemTime();
        double elapsed = m_last_timestamp.getTimeDifference(now);

        double output = std::min(input, m_last_value + (m_pos_rate_limit * elapsed));
        output = std::max(output, m_last_value + (m_neg_rate_limit * elapsed));

        if (m_bounds_set) {
            output = std::min(output, m_pos_bound);
            output = std::max(output, m_neg_bound);
        }

        m_last_timestamp = now;
        m_last_value = output;
        return output;
    };
    double SlewRateLimiter::getLastValue() const {
        return m_last_value;
    };
    void SlewRateLimiter::resetValue(double value) {
        m_last_value = value;
        m_last_timestamp = getSystemTime();
    };
    void SlewRateLimiter::setRateLimit(double rate_limit) {
        m_pos_rate_limit = rate_limit;
        m_neg_rate_limit = -rate_limit;
    };
    void SlewRateLimiter::setRateLimit(double pos_rate_limit, double neg_rate_limit) {
        m_pos_rate_limit = pos_rate_limit;
        m_neg_rate_limit = neg_rate_limit;
    };
    void SlewRateLimiter::setBounds(double bounds) {
        m_pos_bound = bounds;
        m_neg_bound = -bounds;
        m_bounds_set = true;
    };
    void SlewRateLimiter::setBounds(double pos_bound, double neg_bound) {
        m_pos_bound = pos_bound;
        m_neg_bound = neg_bound;
        m_bounds_set = true;
    };
    void SlewRateLimiter::removeBounds() {
        m_bounds_set = false;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_UTILITIES_SLEW_RATE_LIMITER_ in automat_submodules.h to use SlewRateLimiter"
#endif