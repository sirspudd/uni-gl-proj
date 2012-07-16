#include <Geometry.h>

#include <math.h>

// numerical error value.
#define Tolerance 0.000001

Point::Point ()
  {
    coord[0] = 0.0;
    coord[1] = 0.0;
    coord[2] = 0.0;
  }

Point::Point (double x, double y, double z)
  {
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;
  }

Point Point::transform (double m[16])
  {
    double y[4];
    for (int i = 0; i < 4; i++)
      {
        y[i] = m[i + 0] * coord[0] + m[i + 4] * coord[1] + m[i + 8] * coord[2] + m[i + 12] * 1.0;
      }
    return Point (y[0] / y[3], y[1] / y[3], y[2] / y[3]);
  }
    
void Point::addPoint (Point p)
    
{
  for (int i = 0; i < NumCoord; i++)
    {
      coord[i] = coord[i] + p.coord[i];
    }
}

void Point::scalePoint (double s)

{
  for (int i = 0; i < NumCoord; i++)
    {
      coord[i] = coord[i] * s;
    }
}

Point operator + (Point a, Vector b)
  {
    Point c;
    for (int i = 0; i < NumCoord; i++)
      {
        c.coord[i] = a.coord[i] + b.coord[i];
      }
    return c;
  }

std::ostream &  operator << (std::ostream & s, Point & x)
  {
    s << "{" << x.coord[0] << "," << x.coord[1] << "," << x.coord[2] << "}";
    return s;
  }

std::istream &  operator >> (std::istream & s, Point & x)

{
  char c;
  s >> c >> x.coord[0] >> c >> x.coord[1] >> c >> x.coord[2] >> c;
  return s;
}

Vector::Vector ()
  {
    for (int i = 0; i < NumCoord; i++)
      {
        coord[i] = 0.0;
      }
  }

Vector::Vector (double x, double y, double z)
  {
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;
  }

std::ostream &  operator << (std::ostream & s, Vector & x)
  {
    s << "[" << x.coord[0] << "," << x.coord[1] << "," << x.coord[2] << "]";
    return s;
  }

std::istream &  operator >> (std::istream & s, Vector & x)

{
  char c;
  s >> c >> x.coord[0] >> c >> x.coord[1] >> c >> x.coord[2] >> c;
  return s;
}

double Vector::length ()
  {
    return sqrt (coord[0] * coord[0] +
                coord[1] * coord[1] +
                coord[2] * coord[2]);
  }

void Vector::normalize ()
  {
    double l = length ();
    if (l > 0.0)
      {
        coord[0] /= l;
        coord[1] /= l;
        coord[2] /= l;
      }	
  }

Vector operator - (Point a, Point b)
  {
    Vector c;
    for (int i = 0; i < NumCoord; i++)
      {
        c.coord[i] = a.coord[i] - b.coord[i];
      }
    return c;
  }

Vector operator + (Vector a, Vector b)
  {
    Vector c;
    for (int i = 0; i < NumCoord; i++)
      {
        c.coord[i] = a.coord[i] + b.coord[i];
      }
    return c;
  }

Vector operator * (double a, Vector b)
  {
    Vector c;
    for (int i = 0; i < NumCoord; i++)
      {
        c.coord[i] = a * b.coord[i];
      }
    return c;
  }

double dotProduct (Vector & a, Vector & b)
  {
    return a.coord[0] * b.coord[0] +
          a.coord[1] * b.coord[1] +
          a.coord[2] * b.coord[2];
  }

Vector crossProduct (Vector & a, Vector & b)
  {
    Vector result;

    result.coord[0] = a.coord[1] * b.coord[2] - a.coord[2] * b.coord[1];
    result.coord[1] = a.coord[2] * b.coord[0] - a.coord[0] * b.coord[2];
    result.coord[2] = a.coord[0] * b.coord[1] - a.coord[1] * b.coord[0];
    return result;
  }

Quaternion::Quaternion ()

{
  cang = 1.0;
  v = Vector (0.0, 0.0, 0.0);
}

Quaternion::Quaternion (double theta, Vector direction)

{
  double mag = direction.length ();
  double sintheta2 = sin (theta / 2.0);

  cang = cos (theta / 2.0);
  v = (sintheta2 / mag) * direction;
}

Quaternion::Quaternion (double xa, double ya, double za)

{
  float cx;
  float cy;
  float cz;
  float sx;
  float sy;
  float sz;

  cx = cos(xa / 2.0);  sx = sin(xa / 2.0);
  cy = cos(ya / 2.0);  sy = sin(ya / 2.0);
  cz = cos(za / 2.0);  sz = sin(za / 2.0);

  double a = cx * cy * cz + sx * sy * sz;
  double b = sx * cy * cz - cx * sy * sz;
  double c = cx * sy * cz + sx * cy * sz;
  double d = cx * cy * sz - sx * sy * cz;
  if (a < 0.0)
    {
      a = -a;
      b = -b;
      c = -c;
      d = -d;
    }
  cang = a;
  v = Vector (b, c, d);
}

Quaternion::~Quaternion ()

{
}

void Quaternion::RotationMatrix (double mat [3][3])

{
  double s;
  double xs;
  double ys;
  double zs;
  double wx;
  double wy;
  double wz;
  double xy;
  double xz;
  double yz;
  double xx;
  double yy;
  double zz;

  double a = cang;
  double b = v.coord[0];
  double c = v.coord[1];
  double d = v.coord[2];

  s = 2.0 / ((a * a) + (b * b) + (c * c) + (d * d));

  xs = b * s;  ys = c * s;  zs = d * s;
  wx = a * xs; wy = a * ys; wz = a * zs;
  xx = b * xs; xy = b * ys; xz = b * zs;
  yy = c * ys; yz = c * zs; zz = d * zs;

  mat[0][0] = 1.0 - yy - zz;
  mat[0][1] = xy - wz;
  mat[0][2] = xz + wy;
  mat[1][0] = xy + wz;
  mat[1][1] = 1.0 - xx - zz;
  mat[1][2] = yz - wx;
  mat[2][0] = xz - wy;
  mat[2][1] = yz + wx;
  mat[2][2] = 1.0 - xx - yy;
}

void Quaternion::EulerAngles (double & xa, double & ya, double & za)

{
  double mat [3][3];
  Quaternion qinv;

  qinv = inverse ();  /* why invert I don't know, but it seems to work. */
  qinv.RotationMatrix (mat);

  double sinya = -mat[0][2];
  double abscosya = sqrt (mat[1][2] * mat[1][2] + mat[2][2] * mat[2][2]);

  /* assume ya is between -PI/2 and +PI/2 */
  ya = atan2 (sinya, abscosya);

  xa = atan2 (mat[1][2], mat[2][2]);
  za = atan2 (mat[0][1], mat[0][0]);
}

void Quaternion::EulerAnglesZXY (double & xa, double & ya, double & za)
{
  // return the euler angles, assuming rotations are combined in the order
  // y, then x, then z.
  // 
  // Using the shorthand a2 for a / 2 (a = x,y,z), and
  // ca, sa for cosa, sina.
  //
  // The quaternions involved are combined as follows:
  //
  // (w=cz2, x=0, y=0, z=sz2) (w=cx2, x=sx2, y=0, z=0) (w=cy2,x= 0, y=sy2, z=0) 
  //
  // (w=cz2, x=0, y=0, z=sz2) (w=cx2 cy2,
  //                           x=sx2 cy2,
  //                           y=cx2 sy2, 
  //                           z=sx2 sy2)
  //
  // (w=cz2 cx2 cy2 - sz2 sx2 sy2,
  //  x=cz2 sx2 cy2 - sz2 cx2 sy2,
  //  y=cz2 cx2 sy2 + sz2 sx2 cy2, 
  //  z=cz2 sx2 sy2 + sz2 cx2 cy2 )
  //
  // Converts to rotation matrix
  // | 1-2y^2-2z^2          2xy+2wz           2xz-2wy      |
  // | 2xy-2wz              1-2x^2-2z^2       2yz+2wx      |
  // | 2xz+2wy              2yz-2wx           1-2x^2-2y^2  |
  //
  // 1-2y^2-2z^2 = 1 - 2 c2z2 c2x2 s2y2 - 4 cz2 cx2 sy2 sz2 sx2 cy2 - 2 s2z2 s2x2 c2y2 - 2 c2z2 s2x2 s2y2 - 4 cz2 sx2 sy2 sz2 cx2 cy2 - 2  s2z2 c2x2 c2y2
  //             = 1 - 2 c2z2 s2y2 (c2x2 + s2x2) - 2 s2z2 c2y2 (s2x2 + c2x2) - 8 cz2 cx2 cy2 sz2 sx2 sy2 
  //             = 1 - 2 c2z2 s2y2 - 2 s2z2 c2y2 - 8 cz2 cx2 cy2 sz2 sx2 sy2 
  //             = cz cy - sx sy sz
  //
  // 2xy+2wz     = 2 (cz2 sx2 cy2 - sz2 cx2 sy2) (cz2 cx2 sy2 + sz2 sx2 cy2) + 2 (cz2 cx2 cy2 - sz2 sx2 sy2) (cz2 sx2 sy2 + sz2 cx2 cy2)
  //             = 2 (c2z2 sx2 cy2 cx2 sy2 + cz2 s2x2 c2y2 sz2 - sz2 c2x2 s2y2 cz2 - s2z2 cx2 sy2 sx2 cy2 +
  //                  c2z2 cx2 cy2 sx2 sy2 - sz2 s2x2 s2y2 cz2 + cz2 c2x2 c2y2 sz2 - s2z2 sx2 sy2 cx2 cy2)
  //             = 2 (2 sx2 cy2 cx2 sy2 (c2z2 -s2z2) + cz2 sz2 (s2x2 c2y2 - c2x2 s2y2 - s2x2 s2y2 + c2x2 c2y2))
  //             =  sx sy cz + sz ((c2x2 + s2x2) (c2y2 - s2y2))
  //             =  sx sy cz + sz cy
  //
  // 2xz-2wy     = 2 (cz2 sx2 cy2 - sz2 cx2 sy2) (cz2 sx2 sy2 + sz2 cx2 cy2) - 2 (cz2 cx2 cy2 - sz2 sx2 sy2) (cz2 cx2 sy2 + sz2 sx2 cy2)
  //             = 2 (c2z2 s2x2 cy2 sy2 - sz2 cx2 s2y2 cz2 sx2 + cz2 sx2 c2y2 sz2 cx2 - s2z2 c2x2 sy2 cy2 - 
  //                  c2z2 c2x2 cy2 sy2 - sz2 sx2 s2y2 cz2 cx2 + cz2 cx2 c2y2 sz2 sx2 - s2z2 s2x2 sy2 cy2)
  //             = 2 ((c2z2 cy2 sy2 + s2z2 sy2 cy2) (s2x2 - c2x2) + sx2 cx2 sz2 cz2 (c2y2 - s2y2 + s2y2 - c2y2)
  //             = - sy cx
  //
  // 2xy-2wz     = 2 (cz2 sx2 cy2 - sz2 cx2 sy2) (cz2 cx2 sy2 + sz2 sx2 cy2) - 2 (cz2 cx2 cy2 - sz2 sx2 sy2) (cz2 sx2 sy2 + sz2 cx2 cy2)
  //             = 2 (c2z2 sx2 cy2 cx2 sy2 + cz2 s2x2 c2y2 sz2 - sz2 c2x2 s2y2 cz2 - s2z2 cx2 sy2 sx2 cy2 -
  //                  c2z2 cx2 cy2 sx2 sy2 - sz2 s2x2 s2y2 cz2 + cz2 c2x2 c2y2 sz2 - s2z2 sx2 sy2 cx2 cy2)
  //             = 2 (0 + cz2 sz2 (s2x2 c2y2 - c2x2 s2y2 + s2x2 s2y2 - c2x2 c2y2))
  //             =  sz ((s2x2 - c2x2) (s2y2 + c2y2))
  //             = - sz cx
  //
  // 1-2x^2-2z^2 = 1 - 2 c2z2 s2x2 c2y2 + 4 sz2 cx2 sy2 cz2 sx2 cy2 - 2 s2z2 c2x2 s2y2 - 2 c2z2 s2x2 s2y2 - 4 cz2 sx2 sy2 sz2 cx2 cy2 - 2  s2z2 c2x2 c2y2
  //             = 1 - 2 c2z2 s2x2 (c2y2 + s2y2) - 2 s2z2 c2x2 (s2y2 + c2y2)
  //             = 1 - 2 c2z2 s2x2 - 2 s2z2 c2x2
  //             = cz cx
  //
  // 2yz+2wx     = 2 (cz2 cx2 sy2 + sz2 sx2 cy2) (cz2 sx2 sy2 + sz2 cx2 cy2) + 2 (cz2 cx2 cy2 - sz2 sx2 sy2) (cz2 sx2 cy2 - sz2 cx2 sy2)
  //             = 2 (c2z2 cx2 s2y2 sx2 + sz2 s2x2 cy2 cz2 sy2 + cz2 c2x2 sy2 sz2 cy2 + s2z2 sx2 c2y2 cx2 +
  //                 (c2z2 cx2 c2y2 sx2 - sz2 s2x2 sy2 cz2 cy2 - cz2 c2x2 cy2 sz2 sy2 + s2z2 sx2 s2y2 cx2)
  //             = 2 (c2z2 cx2 sx2 + s2z2 sx2 cx2) + sz2 cy2 cz2 sy2 (s2x2 + c2x2 - (s2x2 + c2x2))
  //             = sx
  //
  // 2xz+2wy     = 2 (cz2 sx2 cy2 - sz2 cx2 sy2) (cz2 sx2 sy2 + sz2 cx2 cy2) + 2 (cz2 cx2 cy2 - sz2 sx2 sy2) (cz2 cx2 sy2 + sz2 sx2 cy2)
  //             = 2 (c2z2 s2x2 cy2 sy2 - sz2 cx2 s2y2 cz2 sx2 + cz2 sx2 c2y2 sz2 cx2 - s2z2 c2x2 sy2 cy2 + 
  //                  c2z2 c2x2 cy2 sy2 - sz2 sx2 s2y2 cz2 cx2 + cz2 cx2 c2y2 sz2 sx2 - s2z2 s2x2 sy2 cy2)
  //             = 2 ((c2z2 cy2 sy2 - s2z2 sy2 cy2) (s2x2 + c2x2) + sx2 cx2 sz2 cz2 (c2y2 - s2y2 - s2y2 + c2y2)
  //             = sy cz + sx sz cy
  //
  // 2yz-2wx     = 2 (cz2 cx2 sy2 + sz2 sx2 cy2) (cz2 sx2 sy2 + sz2 cx2 cy2) - 2 (cz2 cx2 cy2 - sz2 sx2 sy2) (cz2 sx2 cy2 - sz2 cx2 sy2)
  //             = 2 (c2z2 cx2 s2y2 sx2 + sz2 s2x2 cy2 cz2 sy2 + cz2 c2x2 sy2 sz2 cy2 + s2z2 sx2 c2y2 cx2 -
  //                 (c2z2 cx2 c2y2 sx2 - sz2 s2x2 sy2 cz2 cy2 - cz2 c2x2 cy2 sz2 sy2 + s2z2 sx2 s2y2 cx2)
  //             = 2 (c2z2 cx2 sx2 - s2z2 sx2 cx2) (s2y2 - c2y2) + sz2 cy2 cz2 sy2 (s2x2 + c2x2 + s2x2 + c2x2)
  //             = -cy sx cz + sz sy
  //
  // 1-2x^2-2y^2 = 1 - 2 c2z2 s2x2 c2y2 + 4 sz2 cx2 sy2 cz2 sx2 cy2 - 2 s2z2 c2x2 s2y2 - 2 c2z2 c2x2 s2y2 - 4 cz2 cx2 sy2 sz2 sx2 cy2 - 2 s2z2 s2x2 c2y2 
  //             = 1 - 2 s2x2 c2y2 - 2 c2x2 s2y2
  //             = cx cy
  //
  // Using:
  //  ca = ca2 ca2 - sa2 sa2 = c2a2 - s2a2
  //  sa = sa2 ca2 + ca2 sa2 = 2 ca2 sa2
  //
  //  ca cb = (c2a2 - s2a2) (c2b2 - s2b2) = (c2a2 c2b2 - c2a2 s2b2 - s2a2 c2b2 + s2a2 s2b2)
  //                                      = (1 - 1 + c2a2 c2b2 - c2a2 s2b2 - s2a2 c2b2 + s2a2 s2b2)
  //                                      = (1 - (c2a2 + s2a2) + c2a2 c2b2 - c2a2 s2b2 - s2a2 c2b2 + s2a2 s2b2)
  //                                      = (1 - c2a2 (1 - c2b2) - s2a2 (1 - s2b2) - c2a2 s2b2 - s2a2 c2b2)
  //                                      = (1 - c2a2 s2b2 - s2a2 c2b2 - c2a2 s2b2 - s2a2 c2b2)
  //                                      = (1 - 2 c2a2 s2b2 - 2 s2a2 c2b2)
  
  // mat[0][0] = cz cy - sx sy sz
  // mat[0][1] = sx sy cz + sz cy
  // mat[0][2] = - sy cx
  // mat[1][0] = - sz cx
  // mat[1][1] = cz cx
  // mat[1][2] = sx
  // mat[2][0] = sy cz + sx sz cy
  // mat[2][1] = -cy sx cz + sz sy
  // mat[2][2] = cx cy
  
  // x = atan2 (mat[1][2], sqrt (1.0 - mat[1][2]);
  // y = atan2 (-mat[0][2], mat[2][2]);
  // z = atan2 (-mat[1][0], mat[1][1])
  
 double mat [3][3];
  // | 1-2y^2-2z^2          2xy+2wz           2xz-2wy      |
  // | 2xy-2wz              1-2x^2-2z^2       2yz+2wx      |
  // | 2xz+2wy              2yz-2wx           1-2x^2-2y^2  |

  Quaternion q = *this;
  q.normalize ();  
  mat[0][0] = 1.0 - 2.0 * q.v.coord[1] * q.v.coord[1] - 2.0 * q.v.coord[2] * q.v.coord[2];
  mat[0][1] = 2.0 * q.v.coord[0] * q.v.coord[1] + 2.0 * q.cang * q.v.coord[2];
  mat[0][2] = 2.0 * q.v.coord[0] * q.v.coord[2] - 2.0 * q.cang * q.v.coord[1];
  mat[1][0] = 2.0 * q.v.coord[0] * q.v.coord[1] - 2.0 * q.cang * q.v.coord[2];
  mat[1][1] = 1.0 - 2.0 * q.v.coord[0] * q.v.coord[0] - 2.0 * q.v.coord[2] * q.v.coord[2];
  mat[1][2] = 2.0 * q.v.coord[1] * q.v.coord[2] + 2.0 * q.cang * q.v.coord[0];
  mat[2][0] = 2.0 * q.v.coord[0] * q.v.coord[2] + 2.0 * q.cang * q.v.coord[1];
  mat[2][1] = 2.0 * q.v.coord[1] * q.v.coord[2] - 2.0 * q.cang * q.v.coord[0];
  mat[2][2] = 1.0 - 2.0 * q.v.coord[0] * q.v.coord[0] - 2.0 * q.v.coord[1] * q.v.coord[1];

  // If cx = 0 then this rather stuffs things up.
  
  if ((fabs (mat[0][2]) < Tolerance) && (fabs (mat[2][2]) < Tolerance))
    {
      std::cout << *this << "\n";
      std::cout << "cx = 0, sx = " << mat[1][2] << " -- " << q << "\n";

      double sgn = mat[1][2];      
      xa = sgn * M_PI / 2.0;
      // (w=cz2, x=0, y=0, z=sz2) (w=p, x=+-p, y=0, z=0) (w=cy2,x= 0, y=sy2, z=0) 
      //
      // (w=p (cz2 cy2 +- sz2 sy2), = p cos (z2 -+ y2)
      //  x=p (+- cz2 cy2 - sz2 sy2), = p cos (z2 -+ y2)
      //  y=p (cz2 sy2 +- sz2 cy2), = p sin (z2 +- y2) 
      //  z=p (+- cz2 sy2 + sz2 cy2) = p sin (z2 +- y2))
      
      // looks like a gimbal lock, 
      ya = 0;
      za = 2.0 * atan2 (q.v.coord[1], q.cang);
    }
  else
    {
      // restricting xa to the range -pi/2 to pi/2 does not seem to be an
      // issue. The reconstructed quaternion matches (although may be
      // opposite angle about opposite vector.  
      xa = atan2 (mat[1][2], sqrt (1.0 - mat[1][2] * mat[1][2]));
      ya = atan2 (-mat[0][2], mat[2][2]);
      za = atan2 (-mat[1][0], mat[1][1]);
    }  
}

std::ostream &  operator << (std::ostream & s, Quaternion & x)

{
  s << "[" << x.cang << ", " << x.v << "]";
  return s;
}

std::istream &  operator >> (std::istream & s, Quaternion & x)

{
  char c;
  s >> c >> x.cang >> c >> x.v >> c;
  return s;
}

Quaternion operator / (Vector a, Vector b)

{
  Quaternion result;
  /* I assume one has to do this by finding the normal, scaling it
     by the difference between lengths of the vectors, and adding
     a rotation about this normal corresponding to the angle between
     them. Time for some new Vector functions. */
  double lena = a.length ();
  double lenb = b.length ();

  if (lenb == 0.0)
   { 
     std::cerr << "Error: Trying to divide by zero length vector\n";
     return result;
   //  exit (1);
   }
  Vector normal = crossProduct (a, b);  /* cross product */

  if (normal.length () == 0)
    {
      // interesting case. If same direction, null transform. In theory,
      // opposite directions, rotate 180 degrees about some normal axis. FIXME.
      std::cerr << "Error: Trying to divide parallel vectors, still to be implemented\n";
      return result;
    }
  normal.normalize ();

  normal = (lena / lenb) * normal;

  /** Need to take into account direction of rotation, cause
      dot producting only gives the smallest angle between vectors. */

  double angle = -acos (dotProduct (a, b) / (lena * lenb));

  result = Quaternion (angle, normal);

  return result;
}

/** No sensible result if all the components of the quaternion are zero. */
void Quaternion::normalize()

{
  Quaternion result;
  double qn;

  qn = sqrt ((cang * cang) + (v.length () * v.length ()));
  if (qn != 0.0)
    {
      cang = cang / qn;
      v = (1.0 / qn) * v;
    }
  else
    {
      std::cerr << "Warning: trying to normalize zero quaternion\n";
    }
}

/** No sensible result if all the components of the quaternion are zero. */
Quaternion Quaternion::inverse()

{
  Quaternion result;
  double qn;

  qn = ((cang * cang) + (v.length () * v.length ()));
  if (qn != 0.0)
    {
      result.cang =  cang / qn;
      result.v = (-1.0 / qn) * v;
    }
  else
    {
      std::cerr << "Warning: trying to invert zero quaternion\n";
    }
  return result;
}

Quaternion operator *(Quaternion a, Quaternion b)

{
  Quaternion result;

  (result).cang = a.cang * b.cang - (a.v.coord[0] * b.v.coord[0] + a.v.coord[1] * b.v.coord[1] + a.v.coord[2] * b.v.coord[2]);
  (result).v.coord[0] = a.cang * b.v.coord[0] + b.cang * a.v.coord[0] + a.v.coord[1] * b.v.coord[2] - b.v.coord[1] * a.v.coord[2];
  (result).v.coord[1] = a.cang * b.v.coord[1] + b.cang * a.v.coord[1] + a.v.coord[2] * b.v.coord[0] - b.v.coord[2] * a.v.coord[0];
  (result).v.coord[2] = a.cang * b.v.coord[2] + b.cang * a.v.coord[2] + a.v.coord[0] * b.v.coord[1] - b.v.coord[0] * a.v.coord[1];

  return result;
}

Quaternion operator / (Quaternion a, Quaternion b)

{
  Quaternion invb = b.inverse ();
  Quaternion result = a * invb;

  return result;
}

Vector operator * (Quaternion a, Vector b)

{
  Quaternion qb;
  qb.cang = 0.0;
  qb.v = b;
  Quaternion ainv = a.inverse ();

  Quaternion res = a * qb * ainv;

  return res.v;
}

Point operator * (Quaternion a, Point b)

{
  Quaternion qb;
  qb.cang = 0.0;
  qb.v = Vector (b.coord[0], b.coord[1], b.coord[2]);
  Quaternion ainv = a.inverse ();

  Quaternion res = a * qb * ainv;

  Point result (res.v.coord[0], res.v.coord[1], res.v.coord[2]);

  return result;
}

Quaternion InterpolateOrientation (double ratio, Quaternion a, Quaternion b)

{
  Quaternion result;

  result = a * Exp (ratio * Log (a.inverse () * b));

  result.normalize ();
  return result;
}

Vector Log (Quaternion a)

{
  Vector result;

  if (a.cang == 0.0)
    {
      result = (M_PI / 2.0) * a.v;
    }
  else
    {
      double norm = a.v.length ();
      double ang = atan2 (norm, a.cang);
      result = (ang / norm) * a.v;
    }
  return result;
}

Quaternion Exp (Vector a)

{
  Quaternion result;
  if (a.length () == 0.0)
    {
      result = Quaternion ();
    }
  else
    {
      double ang = a.length ();
      double norm = sin(ang);
      result.cang = cos (ang);
      result.v = (norm / ang) * a;
    }
  result.normalize ();
  return result;
}
