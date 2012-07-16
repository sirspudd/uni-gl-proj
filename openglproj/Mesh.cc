#include <iostream>
#include <vector>
#include <fstream>

#include <Mesh.h>
#include <GL/gl.h>

#include <math.h>

using namespace std;

Polygon::Polygon (vector<int> v, vector<Point> texcrd)

{
  vertices = v;
  texcoords = texcrd;
}

Polygon::~Polygon ()

{
}

Vertex::Vertex (Point pn, Vector vn)

{
  v = pn;
  n = vn;
}
      
Vertex::~Vertex ()

{
}

Mesh::Mesh ()

{
}

Mesh::Mesh (string name)

  {
    listid = -1;
    
    int i;

    int numvertex;
    int numpolygon;
    int numedge;

    string filename = name + ".off";
    
    ifstream offFl (filename.c_str ());
    if (offFl.fail ())
      {
        cerr << "Unable to open file: " << filename << "\n";
        return;
      }

    string header;
    offFl >> header;
    offFl >> numvertex >> numpolygon >> numedge; 
    if (offFl.bad ())
      {
        cerr << "Unable to parse header of file: " << filename << "\n";
        return;
      }

    for (i = 0; i < numvertex; i++)
      {
        double x, y, z;
        offFl >> x >> y >> z;
        if (offFl.bad ())
          {
            cerr << "Error parsing vertex " << i << " of object " << filename << "\n";
            vertex.clear ();
            polygon.clear ();
            return;
          }
        setVertex (i, new Vertex (Point (x, y, z)));  
      }

    vector<int> v;
    vector<Point> tc;  
    for (i = 0; i < numpolygon; i++)
      {
        int numv;
        offFl >> numv;
        for (int j = 0; j < numv; j++)
          {
            int vn;
            offFl >> vn;
            v.push_back (vn);
            tc.push_back (Point ());
            if (offFl.bad ())
              {
                cerr << "Error parsing triangle " << i << " of object " << filename << "\n";
		vertex.clear ();
		polygon.clear ();
                return;
              }  
          }
            
        setPolygon (i, v, tc);
        v.clear ();
        tc.clear ();

/*        triangles[i].texx[0] = 0.0;
        triangles[i].texx[1] = 1.0;
        triangles[i].texx[2] = 0.0;
        triangles[i].texy[0] = 0.0;
        triangles[i].texy[1] = 0.0;
        triangles[i].texy[2] = 1.0;*/
      }
    offFl.close ();

    updateNormals ();
/*
    // try and read a file of texture coordinates.
    filename = name + ".tex";
    ifstream texFl (filename.c_str ());
    if (texFl.fail ())
      {
        cerr << "Unable to open file: " << filename << "\n";
        return;
      }

    int numtex;
    int numtri;
    texFl >> numtex >> numtri;
    if (texFl.bad ())
      {
        cerr << "Unable to parse header of file: " << filename << "\n";
        return;
      }
    if (numtri != numtriangle)
      {
        cerr << "Texture does not match object in : " << filename << "\n";
        return;
      }

    for (i = 0; i < numtex; i++)
      {
        string name;
        texFl >> name;
      }
    for (i = 0; i < numtriangle; i++)
      {
        double nf;
        double nt;
        double tx[3];
        double ty[3];
        texFl >> nf >> nt >> tx[0] >> ty[0] >> tx[1] >> ty[1] >> tx[2] >> ty[2];
        if (texFl.bad ())
          {
            cerr << "Error parsing texture coordinate " << i << " of texture " << filename << "\n";
            return;
          }
        triangles[i].texx[0] = tx[0];
        triangles[i].texy[0] = ty[0];
        triangles[i].texx[1] = tx[1];
        triangles[i].texy[1] = ty[1];
        triangles[i].texx[2] = tx[2];
        triangles[i].texy[2] = ty[2];
      }
    texFl.close ();
 */   
}

int Mesh::numberVertex ()

{
  return vertex.size ();
}

int Mesh::numberPolygon ()

{
  return polygon.size ();
}

void Mesh::setVertex (int i, Vertex * v)

{
  while (numberVertex () <= i)
    vertex.push_back (NULL);
  vertex[i] = v;  
}

Vertex * Mesh::getVertex (int i)

{
  if ((i >= 0) && (i < numberVertex ()))
    return vertex[i];
  else
    {
      cerr << "Vertex " << i << " out of valid range [0:" << numberVertex () << "] in getVertex\n";
      return NULL;
    }  
}
      
int Mesh::getPolygonNumberVertices (int i)

{
  if ((i >= 0) && (i < numberPolygon ()))
    {
      return polygon[i]->vertices.size ();
    }      
  else
    {
      cerr << "Polygon " << i << " out of valid range [0:" << numberPolygon () << "] in getNumberVertices\n";
      return 0;
    }  
}

Vertex * Mesh::getPolygonVertex (int i, int j)

{
  if ((i >= 0) && (i < numberPolygon ()))
    {
      if ((j >= 0) && (j < polygon[i]->vertices.size ()))
        {
          return vertex[polygon[i]->vertices[j]];
        }
      else
        {
          cerr << "Polygon vertex " << j << " out of valid range [0:" << polygon[i]->vertices.size () << "] in getPolygonVertex\n";
          return NULL;
        }  
    }      
  else
    {
      cerr << "Polygon " << i << " out of valid range [0:" << numberPolygon () << "] in getPolygonVertex\n";
          return new Vertex (Point ());
      return NULL;
    }  
}

int Mesh::getPolygonVertexIndex (int i, int j)

{
  if ((i >= 0) && (i < numberPolygon ()))
    {
      return polygon[i]->vertices[j];
    }      
  else
    {
      cerr << "Polygon " << i << " out of valid range [0:" << numberPolygon () << "] in getPolygonVertexIndex\n";
      return 0;
    }  
}

void Mesh::setPolygon (int i, vector <int> v, vector <Point> tc)

{
  while (numberPolygon () <= i)
    polygon.push_back (NULL);
    
  if ((i >= 0) && (i < numberPolygon ()))
    {
      Polygon * p = new Polygon (v, tc);
      polygon[i] = p;
    }      
  else
    {
      cerr << "Polygon " << i << " out of valid range [0:" << numberPolygon () << "] in setPolygon\n";
    }  
}

void Mesh::setPolygonTexCoord (int i, int j, Point tc)

{
  if ((i >= 0) && (i < numberPolygon ()))
    {
      if ((j >= 0) && (j < polygon[i]->texcoords.size ()))
        {
          polygon[i]->texcoords[j] = tc;
        }
      else
        {
          cerr << "Polygon texcoord " << j << " out of valid range [0:" << polygon[i]->vertices.size () << "] in setPolygonTexCoord\n";
        }  
    }      
  else
    {
      cerr << "Polygon " << i << " out of valid range [0:" << numberPolygon () << "] in setPolygonTexCoord\n";
    }  
}

void Mesh::updateNormals ()

{
  int i;

  for (i = 0; i < numberVertex (); i++)
    {
      vertex[i]->n = Vector (0.0, 0.0, 0.0);
    }

  for (i = 0; i < numberPolygon (); i++)
    {
      for (int j = 0; j < polygon[i]->vertices.size (); j++)
        {
          Vector side1 = vertex[polygon[i]->vertices[(j + 1) % polygon[i]->vertices.size ()]]->v - 
                         vertex[polygon[i]->vertices[j]]->v;
          Vector side2 = vertex[polygon[i]->vertices[(j + 2) % polygon[i]->vertices.size ()]]->v - 
                         vertex[polygon[i]->vertices[j]]->v;
          side1.normalize ();
          side2.normalize ();
          Vector facenormal = crossProduct (side1, side2);

          // weighting works with triangles, how well does it work with quads?
          double weight = asin (facenormal.length ());
          if (weight < 0)
            weight = weight + M_PI;
          facenormal.normalize ();
          vertex[polygon[i]->vertices[j]]->n = vertex[polygon[i]->vertices[j]]->n + weight * facenormal;
        }
    }

  for (i = 0; i < numberVertex (); i++)
    {
      vertex[i]->n.normalize ();
    }

}

void Mesh::render ()

{
  if (!glIsList (listid))
    {
      cout << "Creating Mesh\n";
      listid = glGenLists (1);
      glNewList (listid, GL_COMPILE);
      for (int i = 0; i < numberPolygon (); i++)
        {
          glBegin (GL_POLYGON);
          for (int j = 0; j < polygon[i]->vertices.size (); j++)
            {
              glTexCoord2f (polygon[i]->texcoords[j].coord[0], polygon[i]->texcoords[j].coord[1]);
              glNormal3f (vertex[polygon[i]->vertices[j]]->n.coord[0],
                          vertex[polygon[i]->vertices[j]]->n.coord[1],
                          vertex[polygon[i]->vertices[j]]->n.coord[2]);
              glVertex3f (vertex[polygon[i]->vertices[j]]->v.coord[0],
                          vertex[polygon[i]->vertices[j]]->v.coord[1],
                          vertex[polygon[i]->vertices[j]]->v.coord[2]);
            }  
          glEnd ();
        }
      glEndList ();  
    }
      
  glCallList (listid);
}

void Mesh::renderWireframe ()
  
{  
  glColor3f (1.0, 1.0, 1.0);
  for (int i = 0; i < numberPolygon (); i++)
    {
      glBegin (GL_LINES);
      for (int j = 0; j < polygon[i]->vertices.size (); j++)
        {
          glNormal3f (vertex[polygon[i]->vertices[j]]->n.coord[0],
                      vertex[polygon[i]->vertices[j]]->n.coord[1],
                      vertex[polygon[i]->vertices[j]]->n.coord[2]);
          glVertex3f (vertex[polygon[i]->vertices[j]]->v.coord[0],
                      vertex[polygon[i]->vertices[j]]->v.coord[1],
                      vertex[polygon[i]->vertices[j]]->v.coord[2]);
        }  
      glEnd ();
    }
}

void Mesh::renderNormals ()

{  
  glColor3f (1.0, 0.0, 0.0);
  for (int i = 0; i < numberPolygon (); i++)
    {
      for (int j = 0; j < polygon[i]->vertices.size (); j++)
        {
          glBegin (GL_LINES);
          glVertex3f (vertex[polygon[i]->vertices[j]]->v.coord[0] +
                      vertex[polygon[i]->vertices[j]]->n.coord[0],
                      vertex[polygon[i]->vertices[j]]->v.coord[1] +
                      vertex[polygon[i]->vertices[j]]->n.coord[1],
                      vertex[polygon[i]->vertices[j]]->v.coord[2] +
                      vertex[polygon[i]->vertices[j]]->n.coord[2]);
          glVertex3f (vertex[polygon[i]->vertices[j]]->v.coord[0],
                      vertex[polygon[i]->vertices[j]]->v.coord[1],
                      vertex[polygon[i]->vertices[j]]->v.coord[2]);
          glEnd ();
        }  
    }
}

