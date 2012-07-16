#include <Curve.h>

class BezierSpline : public Curve
  {
    public:
      BezierSpline (istream & parameters);

      Point curveAt (double t);
      
      void outputCurve (ostream & s);
  };

double Bernstein (int i, int n, double t);
