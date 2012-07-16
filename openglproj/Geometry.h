#ifndef __Geometryh
#define __Geometryh

#include <iostream>

#define MAXSTRING 10000
#define NumCoord 3   // number of dimensions in a point.

class Vector;

class Point
  {
    public:
      double coord[NumCoord];

    Point ();
    Point (double x, double y, double z);

    // transform point by given matrix, stored
    // in OpenGL's format.
    Point transform (double m[16]);

    // accumulate the coordinates of another point with this one.
    // not an operator, since not a typesafe operation.
    void addPoint (Point p);
    
    // scale the coordinates of the point by the given factor.
    // not an operator, since not a typesafe operation.
    void scalePoint (double s);
    
    friend Point operator + (Point a, Vector b);
  
    friend std::ostream &  operator << (std::ostream & s, Point & x);
    friend std::istream &  operator >> (std::istream & s, Point & x);
  };

class Vector
  {
    public:
      double coord[NumCoord];

    Vector ();
    Vector (double x, double y, double z);
    double length ();
    void normalize ();
    friend std::ostream &  operator << (std::ostream & s, Vector & x);
    friend std::istream &  operator >> (std::istream & s, Vector & x);
    friend Vector operator - (Point a, Point b);
    friend Vector operator + (Vector a, Vector b);
    friend Vector operator * (double a, Vector b);
    friend double dotProduct (Vector & a, Vector & b);
    friend Vector crossProduct (Vector & a, Vector & b);
  };

class Quaternion
  {
    public:
      Vector v;
      double cang;

    public:
      /** create a unit quaternion. */
      Quaternion ();
      /** Create a quaternion, using a vector, and an angle about the
          vector (in radians). */
      Quaternion (double theta, Vector direction);
      /** create a quaternion, given three euler angles (in radians). */
      Quaternion (double xa, double ya, double za);
      ~Quaternion ();

      /// return the inverse of the quaternion.
      Quaternion inverse();
      /// normalise the quaternion.
      void normalize();
      /// find the Euler angles corresponding to a quaternion.
      void EulerAngles (double & xa, double & ya, double & za);
      void EulerAnglesZXY (double & xa, double & ya, double & za);
      /** convert a quaternion to a rotation matrix. */
      void RotationMatrix (double mat [3][3]);

      /// multiply a by  b.
      friend Quaternion operator *(Quaternion a, Quaternion b);
      /// multiply a by the inverse of b.
      friend Quaternion operator /(Quaternion a, Quaternion b);

      friend Quaternion operator / (Vector a, Vector b);
      friend Vector operator * (Quaternion a, Vector b);
      friend Point operator * (Quaternion a, Point b);
      friend Quaternion InterpolateOrientation (double ratio, Quaternion a, Quaternion b);
      friend Vector Log (Quaternion a);
      friend Quaternion Exp (Vector a);

      /** print some useful form of a quaternion. */
      friend std::ostream &  operator << (std::ostream & s, Quaternion & x);
      friend std::istream &  operator >> (std::istream & s, Quaternion & x);
  };

#endif
// __Geometryh


