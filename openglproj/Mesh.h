#ifndef __Mesh_h
#define __Mesh_h

#include <Geometry.h>
#include <Renderable.h>

#include <GL/gl.h>

#include <string>
#include <vector>

using namespace std;

class Mesh;
class Vertex;

class Polygon
  {
    friend class Mesh;
    
    protected:
      vector<int> vertices;

      // just using the first two values of point
      // as the s and t texture coordinates.
      vector<Point> texcoords;
          
    public:
      Polygon (vector<int> v, vector<Point> texcrd);
      ~Polygon ();
  };

class Vertex
  {
    public:
      Point v;
      
      Vector n;

      Vertex (Point pn = Point (0.0, 0.0, 0.0), Vector vn = Vector (0.0, 1.0, 0.0));
      ~Vertex ();
  };  
  
// class to represent a set of polygons forming
// the surface of an object.

class Mesh : public Renderable
  {
    protected:
      vector<Vertex *> vertex;
      
      vector<Polygon *> polygon;

      GLuint listid;

    public:
      Mesh ();  
      // create a mesh by reading values in from an OFF file.
      Mesh (string name);

      int numberVertex ();
      int numberPolygon ();
            
      void updateNormals ();
      
      void setVertex (int i, Vertex * v);
      Vertex * getVertex (int i);
      
      void setPolygon (int i, vector<int> v, vector <Point> tc);
      void setPolygonTexCoord (int i, int j, Point tc);
      // return the number of vertices in polygon i.
      int getPolygonNumberVertices (int i);
      // return the jth vertex of polygon i.
      Vertex * getPolygonVertex (int i, int j);
      // get the index for the vertex, rather than the
      // actual coordinates - better for comparisons.
      int getPolygonVertexIndex (int i, int j);


      virtual void render ();
      virtual void renderWireframe ();
      virtual void renderNormals ();
  };
  
#endif
//__Mesh_h


