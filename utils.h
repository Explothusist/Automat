
#ifndef AUTOMAT_UTILS_
#define AUTOMAT_UTILS_

#include <string>

namespace atmt {
    
#ifdef AUTOMAT_VEX_
    vex::competition m_competition
    vex::brain m_brain;
#endif

    class Vector_2D;
    class Vector_Polar;

    class Vector_2D { // This and platform prints and template
        public:
            Vector_2D(double x, double y);
            ~Vector_2D();

            double getX();
            double getY();

            void translate(Vector_2D vector);
            void translate(double x, double y);

            void rotate(double theta);

            void normalize();

            Vector_Polar toPolar();
        private:
            double m_x;
            double m_y;
    };

    class Vector_Polar {
        public:
            Vector_Polar(double r, double theta);
            ~Vector_Polar();

            double getR();
            double getTheta();

            void rotate(Vector_Polar vector);
            void rotate(double theta);

            void normalize();

            Vector_2D toCartesian();
        private:
            double m_r;
            double m_theta; // In radians
    };

    void print(std::string stuff);

};

#endif