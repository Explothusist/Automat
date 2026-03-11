#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_KINEMATIC_PREDICTOR_

#include "KinematicPredictor.h"
#include <cmath>

namespace atmt {

    KinematicPredictor::KinematicPredictor():
        KinematicPredictor(
            0.0,
            0.0,
            0.0
        )
    {

    };
    KinematicPredictor::KinematicPredictor(double start_position, double start_velocity, double start_accel):
        KinematicPredictor(
            start_position,
            start_velocity,
            start_accel,
            0.0,
            2.0
        )
    {
        
    };
    KinematicPredictor::KinematicPredictor(double start_position, double start_velocity, double start_accel, double velocity_timeconst, double accel_timeconst):
        m_position{ start_position },
        m_velocity{ start_velocity },
        m_acceleration{ start_accel },
        m_has_been_reset{ true },
        m_timestamp{ Timestamp(0) },
        m_velocity_timeconst{ velocity_timeconst },
        m_acceleration_timeconst{ accel_timeconst }
    {
        
    };

    double KinematicPredictor::getPredictedPosition() {
        return getPredictedPosition(getSystemTime());
    };
    double KinematicPredictor::getPredictedPosition(Timestamp timestamp) {
        return m_position + getPredictedChange(timestamp);
    };
    double KinematicPredictor::getPredictedChange() {
        return getPredictedChange(getSystemTime());
    };
    double KinematicPredictor::getPredictedChange(Timestamp timestamp) {
        if (m_has_been_reset) {
            return 0.0;
        }else {
            double change_in_time = timestamp.getTimeDifference(m_timestamp);

            return m_velocity * change_in_time + 0.5 * m_acceleration * change_in_time * change_in_time;
            }
    };
    void KinematicPredictor::updateLocation(double position) {
        updateLocation(position, getSystemTime());
    };
    void KinematicPredictor::updateLocation(double position, Timestamp timestamp) {
        if (m_has_been_reset) {
            m_has_been_reset = false;
            m_position = position; // Do not calculate velocity after reset (to avoid the velocity spike as position changes from 0)
            m_timestamp = timestamp;
        }else {
            double old_position = m_position;
            double old_velocity = m_velocity;
            double old_acceleration = m_acceleration;
            Timestamp old_timestamp = m_timestamp;

            m_timestamp = timestamp;
            double change_in_time = std::max(m_timestamp.getTimeDifference(old_timestamp), kZeroEpsilon);

            m_position = position;
            double calculated_velocity = (position - old_position) / change_in_time;
            m_velocity = (calculated_velocity + old_velocity * m_velocity_timeconst) / (1 + m_velocity_timeconst);
            double calculated_acceleration = (calculated_velocity - old_velocity) / change_in_time; // Use calculated_velocity to prevent double filtering
            m_acceleration = (calculated_acceleration + old_acceleration * m_acceleration_timeconst) / (1 + m_acceleration_timeconst);
        }
    };
    void KinematicPredictor::reset() {
        m_position = 0.0;
        m_velocity = 0.0;
        m_acceleration = 0.0;
        m_has_been_reset = true;
        m_timestamp = Timestamp(0.0);
    };

    double KinematicPredictor::getPosition() {
        return m_position;
    };
    double KinematicPredictor::getVelocity() {
        return m_velocity;
    };
    double KinematicPredictor::getAcceleration() {
        return m_acceleration;
    };

};

#else
// #error "Enable ATMT_SUBMODULE_UTILITIES_KINEMATIC_PREDICTOR_ in automat_submodules.h to use KinematicPredictor"
#endif