#ifndef __Curve_h
#define __Curve_h

#include <string>

#include <Geometry.h>
#include <vector>
#include <Renderable.h>

using namespace std;

class Curve : public Renderable
  {
    private:
      // degree of spline.
      int degree;

      // number of control points in the spline.
      int numControlPoints;
      // set of control points for the spline.
      Point * points;

      // number of knots;
      int numKnots;
      // set of knots;
      double * knots;
  
    protected:  
      // allocate storage for the given numbers of components.
      void allocateCurve (int degree, int controlpoints, int knotCount);
      
    public:
      static Curve * createCurve (istream & parameters);
      
      ~Curve ();

      int splineDegree ();
      
      int numberControlPoints ();
      Point controlPoint (int i);
      void setControlPoint (int i, Point p);
      
      int numberKnots ();
      double knot (int i); 
      void setKnot (int i, double k); 

      virtual Point curveAt (double t) = 0;
      
      virtual Vector derivativeOfCurveAt (double t);
      
      virtual void outputCurve (ostream & s) = 0;
      
      friend ostream & operator << (ostream & s, Curve & x);      
  
      virtual void render ();
      virtual void renderWireframe ();
      virtual void renderNormals ();
  };

#endif
// __Curve_h

