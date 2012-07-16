#include <Curve.h>
#include <BezierSpline.h>
#include <GL/gl.h>

void Curve::allocateCurve (int splineDegree, int controlPoints, int knotCount)

{
  degree = splineDegree;
  numControlPoints = controlPoints;
  numKnots = knotCount;
  
  if (numControlPoints > 0)
    points = new Point [numControlPoints];
  else
    points = NULL;
    
  if (numKnots > 0)
    knots = new double [numKnots];
  else
    knots = NULL;      
}

Vector Curve::derivativeOfCurveAt (double t)
{
  Point p = curveAt (t);
  Point np = curveAt (t + 0.01);
  Vector v = np - p;
  v.normalize ();
  return v;
}

Curve::~Curve ()
 {
   if (points != NULL)
     delete [] points;
   if (knots != NULL)
     delete [] knots;  
     
   degree = 0;
   numControlPoints = 0;
   numKnots = 0;  
 }

Curve * Curve::createCurve (istream & parameters)

{
  string type;
  parameters >> type;
  cout << "Creating curve of type: " << type << "\n";
  
  if (type == "BezierSpline")
    return new BezierSpline (parameters);

  cout << "Curve type unknown\n";      
  return NULL;
}

ostream & operator << (ostream & s, Curve & x)

{
  x.outputCurve (s);
  return s;
}

int Curve::splineDegree ()

{
  return degree;
}

int Curve::numberControlPoints ()

{
  return numControlPoints;
}

Point Curve::controlPoint (int i)

{
  if ((i >= 0) && (i < numControlPoints))
    return points[i];
  else  
    {  
      cerr << "Requested control point " << i << " out of range [0:" << numControlPoints << "]\n";
      return Point (0.0, 0.0, 0.0);
    }  
}
      
void Curve::setControlPoint (int i, Point p)

{
  if ((i >= 0) && (i < numControlPoints))
    points[i] = p;
  else  
    {  
      cerr << "Requested control point " << i << " out of range [0:" << numControlPoints << "] in set\n";
    }  
}

int Curve::numberKnots ()

{
  return numKnots;
}

double Curve::knot (int i)

{
  if ((i >= 0) && (i < numKnots))
    return knots[i];
  else  
    {  
      cerr << "Requested knot " << i << " out of range [0:" << numKnots << "]\n";
      return 0.0;
    }  
}

void Curve::setKnot (int i, double k)

{
  if ((i >= 0) && (i < numKnots))
    knots[i] = k;
  else  
    {  
      cerr << "Requested knot " << i << " out of range [0:" << numKnots << "] in set\n";
    }  
}

void Curve::render ()

{
  double time = 0.0;
  double delta = 0.01;
  Point p = curveAt (time);
  for (; time < 1.0; time += delta)
    {
      Point q = curveAt (time + delta);
      glBegin (GL_LINES);
      glNormal3f (1.0, 1.0, 1.0);
      glVertex3f (p.coord[0], p.coord[1], p.coord[2]);
      glVertex3f (q.coord[0], q.coord[1], q.coord[2]);
      glEnd ();
      p = q;
    }
}

void Curve::renderWireframe ()

{
  render ();
}

void Curve::renderNormals ()

{
  render ();
  
  double time = 0.0;
  double delta = 0.1;
  for (; time < 1.0; time += delta)
    {
      Point p = curveAt (time);
      Vector v = derivativeOfCurveAt (time);
      v.normalize ();
      glBegin (GL_LINES);
      glNormal3f (1.0, 1.0, 1.0);
      glVertex3f (p.coord[0], p.coord[1], p.coord[2]);
      glVertex3f (p.coord[0] + 100.0 * v.coord[0], p.coord[1] + 100.0 * v.coord[1], p.coord[2] + 100.0 * v.coord[2]);
      glEnd ();
    }

}
