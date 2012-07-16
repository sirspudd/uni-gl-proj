#include <BezierSpline.h>

BezierSpline::BezierSpline (istream & parameters)

{
  char c;
  int degree;

  parameters >> degree;
  allocateCurve (degree, degree + 1, 0);

  parameters >> c; // [
  for (int i = 0; i < numberControlPoints (); i++)
    {
      Point p;
      parameters >> p;
      setControlPoint (i, p);
    }
  parameters >> c; // ]
}
 
void BezierSpline::outputCurve (ostream & s)

{
  s << "BezierSpline" << " ";
  s << splineDegree () << " ";
  s << "[";
  for (int i = 0; i < numberControlPoints (); i++)
    {
      Point p = controlPoint (i);
      s << p;
      if (i < numberControlPoints () - 1)
        s << " ";
    }    
  s << "]";
}
           
// calculate n!
double factorial (int n)

{
  if (n <= 1)
    return 1.0;
  else
    return (double) n * factorial (n - 1);  
}

// return n choose i. Implement a caching scheme on smaller values
// to reduce recalculation.
#define SMALLNUMBER 10

double choice (int i, int n)

{
  static double cache [SMALLNUMBER][SMALLNUMBER];
  static int initcache = false;
  
  if (!initcache)
    {
      for (int ii = 0; ii < SMALLNUMBER; ii++)
        {
          for (int nn = 0; nn < SMALLNUMBER; nn++)
            {
              cache[ii][nn] = 0.0;
            }
        }
      initcache = true;
    }
    
  if ((i >= 0) && (i < SMALLNUMBER) &&
      (n >= 0) && (n < SMALLNUMBER) &&  
      (cache[i][n] > 0.0))
    {  
      return cache[i][n];
    }  
    
  double result = factorial (n) / (factorial (i) * factorial (n - i));
  if ((i >= 0) && (i < SMALLNUMBER) &&
      (n >= 0) && (n < SMALLNUMBER))
    {  
      cache[i][n] = result;
    }
      
  return result;
}

double Bernstein (int i, int n, double t)

{
  double ti = 1.0;
  for (int k = 0; k < i; k++)
    ti *= t;
  
  double tim1 = 1.0;
  for (int k = 0; k < n - i; k++)
    tim1 *= (1.0 - t);
    
  double c = choice (i, n) * ti * tim1;
  return c;
}

Point BezierSpline::curveAt (double t)

{
  Point p (0.0, 0.0, 0.0);
  
  int n = splineDegree ();
  for (int i = 0; i < numberControlPoints (); i++)
    {
      double c = Bernstein (i, n, t);
      
      Point cp = controlPoint (i);
      Vector v (c * cp.coord[0],
                c * cp.coord[1],
                c * cp.coord[2]);
              
      p = p + v;            
    }
                
  return p;
}
