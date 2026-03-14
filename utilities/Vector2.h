#include "../automat_submodules.h"
#ifdef ATMT_SUBMODULE_UTILITIES_VECTOR_

#ifndef AUTOMAT_PID_CONTROLLER_
#define AUTOMAT_PID_CONTROLLER_

namespace atmt {

    class Vector2 {
        public:
            Vector2();
            Vector2(double x, double y);
            // Vector2(double r, double theta); Must have different parameters

            Vector2(const Vector2& other) = default; // copy constructor
            Vector2& operator=(const Vector2& other) = default; // copy assignment
            ~Vector2() = default;

            Vector2 operator+(const Vector2 &vector) const;
            Vector2 operator-(const Vector2 &vector) const;
            Vector2 operator*(double scalar) const;
            Vector2 operator/(double scalar) const;
            Vector2& operator+=(const Vector2 &vector);
            Vector2& operator-=(const Vector2 &vector);
            Vector2& operator*=(double scalar);
            Vector2& operator/=(double scalar);
            bool operator==(const Vector2 &vector) const;

            double dot(const Vector2 &vector) const;
            Vector2 normalize() const;

            void fromCartesian(double x, double y);
            void fromPolar(double r, double theta);

            double getX() const;
            double getY() const;
            void setX(double x);
            void setY(double y);
            double getR2() const; // for efficiency
            double getR() const; // Magnitude
            double getTheta() const; // Angle to Z+ (Radians)

            void translate(double x, double y);
            void rotate(double theta);

        private:
            double m_x;
            double m_y;
    };

    Vector2 operator*(double scalar, const Vector2 &vector); // Overload the baseline

};

#endif

#else
#error "Enable ATMT_SUBMODULE_UTILITIES_VECTOR_ in automat_submodules.h to use Vector2"
#endif