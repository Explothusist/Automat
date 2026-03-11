#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_KINEMATIC_PREDICTOR_

#ifndef AUTOMAT_KINEMATIC_PREDICTOR_
#define AUTOMAT_KINEMATIC_PREDICTOR_

#include "../utils.h"

namespace atmt {

    // Calculates position, velocity, and acceleration in order to predict future position
    // Input the value as a double each time it changes, then get the predicted whenever needed
    // Tracks time internally, so no need to input
    class KinematicPredictor {
        public:
            KinematicPredictor();
            KinematicPredictor(double start_position, double start_velocity, double start_accel);
            KinematicPredictor(double start_position, double start_velocity, double start_accel, double velocity_timeconst, double accel_timeconst);

            double getPredictedPosition();
            double getPredictedPosition(Timestamp time);
            double getPredictedChange();
            double getPredictedChange(Timestamp time);
            void updateLocation(double position);
            void updateLocation(double position, Timestamp time);
            void reset();

            double getPosition(); // units (WARNING: This method is NOT get_predicted())
            double getVelocity(); // units / second
            double getAcceleration(); // units / second^2
        private:
            double m_position;
            double m_velocity;
            double m_acceleration;

            bool m_has_been_reset;
            Timestamp m_timestamp;

            double m_velocity_timeconst; // 1, 3, 8 range
            double m_acceleration_timeconst; // 2, 5, 12 range
    };
};

#endif

#else
#error "Enable ATMT_SUBMODULE_UTILITIES_KINEMATIC_PREDICTOR_ in automat_submodules.h to use KinematicPredictor"
#endif