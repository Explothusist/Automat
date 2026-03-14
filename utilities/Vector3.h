#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_VECTOR_

#ifndef AUTOMAT_PID_CONTROLLER_
#define AUTOMAT_PID_CONTROLLER_

namespace atmt {

    class Vector3 {
        public:
            Vector3();
            Vector3(double x, double y, double z);
            // Vector3(double r, double theta, double phi); Must have different parameters

            Vector3(const Vector3& other) = default; // copy constructor
            Vector3& operator=(const Vector3& other) = default; // copy assignment
            ~Vector3() = default;

            Vector3 operator+(const Vector3 &vector) const;
            Vector3 operator-(const Vector3 &vector) const;
            Vector3 operator*(double scalar) const;
            Vector3 operator/(double scalar) const;
            Vector3& operator+=(const Vector3 &vector);
            Vector3& operator-=(const Vector3 &vector);
            Vector3& operator*=(double scalar);
            Vector3& operator/=(double scalar);
            bool operator==(const Vector3 &vector) const;

            double dot(const Vector3 &vector) const;
            Vector3 cross(const Vector3 &vector) const;
            Vector3 normalize() const;

            void fromCartesian(double x, double y, double z);
            void fromSpherical(double r, double theta, double phi);

            double getX() const;
            double getY() const;
            double getZ() const;
            void setX(double x);
            void setY(double y);
            void setZ(double z);
            double getR2() const; // for efficiency
            double getR() const; // Magnitude
            double getTheta() const; // Angle to Z+ (Radians)
            double getPhi() const; // Angle to X+ (Radians)

            void translate(double x, double y, double z);
            void rotate(double theta, double phi);

        private:
            double m_x;
            double m_y;
            double m_z;
    };

    Vector3 operator*(double scalar, const Vector3 &vector); // Overload the baseline

};

#endif

#else
#error "Enable ATMT_SUBMODULE_UTILITIES_VECTOR_ in automat_submodules.h to use Vector3"
#endif