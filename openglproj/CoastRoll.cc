//Distant scapes : Doanld Carr
//base code supplied by Shaun Bangay

#include <iostream>
#include <sstream>
#include <math.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qgl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <Mesh.h>
#include <Curve.h>
#include <BezierSpline.h>


class TextureMap
  {
#define PictureCoords(x,y) (((y) * xsize + (x)) * 3)

    protected:
      int xsize;
      int ysize;

      GLubyte * colours;

      GLuint textureid;

    public:
      TextureMap ()
        {
	  xsize = 0;
          ysize = 0;
        }
      ~TextureMap ()
        {
          glDeleteTextures (1, &textureid);
          delete [] colours;
        }
      TextureMap (char * filename)
        {
          std::cout << "Loading texture: " << filename << "\n";

          FILE * file;
          if (strstr (filename, "ppm") == NULL)
            {
              char line [MAXSTRING];
              sprintf (line, "pngtopnm %s | pnmflip -tb >tmptexture.pnm", filename);
              system (line);

              file = fopen ("tmptexture.pnm", "r");
            }
          else
            file = fopen (filename, "r");

          if (file == NULL)
            {
              std::cerr << "Could not open pnm file " << filename << "\n";
              exit (1);
            }

          int c;
          if (fscanf (file, "P6\n%d %d\n%d\n", &xsize, &ysize, &c) != 3)
            {
              std::cerr << "Unable to read file header of converted " << filename << "\n";
              exit (1);
            }

          colours = new GLubyte [xsize * ysize * 3];
          fread (colours, xsize * 3, ysize, file);
          fclose (file);

          // set up OpenGL texturing.
          glGenTextures (1, &textureid);

          glBindTexture (GL_TEXTURE_2D, textureid);

          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
          glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //GL_DECAL //Modulate
          glTexImage2D (GL_TEXTURE_2D, 0, 3, xsize, ysize, 0, GL_RGB,
                        GL_UNSIGNED_BYTE, colours);
         
	}
      // make this texture active.
      void activate ()
        {
          glBindTexture (GL_TEXTURE_2D, textureid);
        }
      int textureID ()
        {
          return textureid;
        }
  };

class Light
  {
    public:
      Point         position;
      GLfloat       colour [4];
      
    Light (Point p, double r, double g, double b)
      {
        position = p;
        colour[0] = r;
        colour[1] = g;
        colour[2] = b;
        colour[3] = 1.0;
      }  
  };

class OpenGLPaintableWidget : public QGLWidget
  {
    Q_OBJECT
      protected:
        Mesh * terrain;
        Mesh * terrain2;
	Mesh * terrain3;
	Mesh * terrain4;

        int mouseox;
        int mouseoy;

        double scale;
        double rotx;
        double roty;
        double offsetx;
        double offsety;
    	Curve * camerapath;
    
        vector <Light *> lights;
        TextureMap * terraintexture;
	TextureMap * startexture;
	TextureMap * meteortexture;
	GLuint railid;
        GLuint starid;
	GLuint meteorid;
	            
    public:
      /* random number between 0 and 1 */
      double unitrand ()
        {
          return (((double) (random () % 10000)) / 10000.0);
        }
      
      void setTextureCoords (Mesh * m)
        {
          // set the t texture coord based on the height y of the mesh.
          // set the s coord randomly
          double ymin;
          double ymax;
          int started = 0;
          for (int i = 0; i < m->numberVertex (); i++)
            {
              if (!started)
                {
                  ymin = m->getVertex (i)->v.coord[1];
                  ymax = m->getVertex (i)->v.coord[1];
                  started = 1;
                }
              if (m->getVertex (i)->v.coord[1] < ymin)
                ymin = m->getVertex (i)->v.coord[1];
              if (m->getVertex (i)->v.coord[1] > ymax)
                ymax = m->getVertex (i)->v.coord[1];
            }
            
          for (int i = 0; i < m->numberPolygon (); i++)
            {
              for (int j = 0; j < m->getPolygonNumberVertices (i); j++)
                {
                  double h = m->getPolygonVertex (i, j)->v.coord[1];
                  cout << h << " - " << ymin << " .. " << ymax << "\n";
                  Point texc = Point (unitrand (), (h - ymin) / (ymax - ymin), 0.0);
                  m->setPolygonTexCoord (i, j, texc);
                }  
            }
        }
    
      OpenGLPaintableWidget (Mesh * terr, Mesh * spine, Mesh * canyon, Mesh * warp,QWidget *parent, const char *name) : QGLWidget (parent, name)
        {
          terrain = terr;
	  terrain2 = spine;
	  terrain3 = canyon;
	  terrain4 = warp;
	  
          mouseox = -1;
          mouseoy = -1;

          scale = 1.0;
          rotx = 0.0;
          roty = 0.0;
          offsetx = 0.0;
          offsety = 0.0;
	            
           string curvedefn ("14 [ {-60.0,12.0,-42.0} {-40.0,12.0,-43.0}{-20.0,12.0,-42.0}{0.0,12.0,-43.0}{0.0,12.0,-45.0}{0.0,18.0,-45.0}{0.0,18.0,-45.0}{0.0,18.0,-45.0}{20.0,-40.0,-40.0} {-60.0,50.0,-60.0} {+60.0,-99.8,-60.0} {+60.0,-5.8,+60.0} {-60.0,2.8,+60.0} {-60.0,2.8,-60.0} ]");
	  istringstream * str = new istringstream (curvedefn);  
          camerapath = new BezierSpline (*str);
//         
          lights.push_back (new Light (Point (37.0, 5.0, 0.0), 1, 1, 1));
          lights.push_back (new Light (Point (15.0, 12.0, 15.0), 1, 1, 1));
          lights.push_back (new Light (Point (5.0, 15.0, -8.0), 0.25, 0.25, 0.65));
          lights.push_back (new Light (Point (0.0, 10.0, 0.0), 0.58, 0.15, 0.15));
          lights.push_back (new Light (Point (37.0, 5.0, 0.0), 1, 1, 1));
          

          setMinimumSize (640, 480);
          setMaximumSize (640, 480);
          this->show ();
 
          setTextureCoords (terr);          
        }

      /* random number between -1 and 1 */
      double biunitrand ()
        {
          return (((double) (random () % 10000)) / 5000.0) - 1.0;
        }
	 
      void paintGL ()
        { GLUquadricObj * quadric;
          GLfloat noColour[] = { 0, 0, 0, 1 };
	  static double ang = 0.0;
	  static double t = 0.0; // position along the spline.
	  static double s = 0.005; //speed along the spline
	  ang += 5.0;
	  t += s;
          if (t > 1.0)
            t = 0.0;
	  
          glClearColor(0.0, 0.0, 0.0, 0.0);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          /* sets the x and y coordinates to run from -1 to +1 */
          glColor3f(1.0, 1.0, 1.0);
          glMatrixMode (GL_PROJECTION);
          glLoadIdentity ();
          //glFrustum(-2.0, 2.0, -2.0, 2.0, 1, 20.0);   
          glFrustum(-2.0, 2.0, -2.0, 2.0, 2, 100.0);   
          // don't do your transformations in the projection
          // matrix. It may seem to work, but there are nasty
          // side effects.
          glMatrixMode (GL_MODELVIEW);
          glLoadIdentity ();
	  //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

          int light = GL_LIGHT0;
          for (vector<Light *>::iterator i = lights.begin (); i != lights.end (); i++)
            {
              glLightf (light, GL_SPOT_CUTOFF, 360.0);
              glLightf (light, GL_CONSTANT_ATTENUATION, 1.0);
              glLightf (light, GL_LINEAR_ATTENUATION, 0.2);
              glLightf (light, GL_QUADRATIC_ATTENUATION, 0.0);
          
              GLfloat position [4];
              position[0] = (*i)->position.coord[0];
              position[1] = (*i)->position.coord[1];
              position[2] = (*i)->position.coord[2];
              position[3] = 1.0;
              glLightfv (light, GL_POSITION, position);
              glLightfv (light, GL_AMBIENT, (*i)->colour);
              glLightfv (light, GL_DIFFUSE, (*i)->colour);
              glLightfv (light, GL_SPECULAR, (*i)->colour);
              glEnable (light);

              light++;
            } 
	      Vector eyedir = Vector (0.0, 0.0, -1.0);
              Point p = camerapath->curveAt (t);
              // to put the camera on the path we would translate from origin to p.
              // in the view transformation, we translate world so that p ends up at
              // the origin.
              Vector tangent = camerapath->derivativeOfCurveAt (t);
              // we currently face [0,0,-1] we want to face in direction t.
              // find a perpendicular vector to rotate around.
              Vector n = crossProduct (eyedir, tangent);
              double angle = (180.0 / M_PI) * acos (-tangent.coord[2]); // the dot product.
             
	      glRotatef (roty, 1.0, 0.0, 0.0);
	     
	      glRotatef (-angle + 180, n.coord[0], n.coord[1], n.coord[2]);
              glRotatef (rotx, 0.0, 1.0, 0.0);
	     
	      glTranslatef (p.coord[0], p.coord[1], p.coord[2]);
             
	     
	      glTranslatef (0.0, -2.0, 0.0);
	      if (!glIsList (railid))
		{
			cout << "Creating rail list\n";
			railid = glGenLists (1);
			glNewList (railid, GL_COMPILE);
			quadric=gluNewQuadric();
			gluQuadricNormals(quadric,GLU_SMOOTH);
			gluQuadricTexture(quadric,GL_FALSE);
			for(double railT = 0; railT < 1;railT = railT + 0.003)
			{	//Exact same as viewing code only precognisant
				glPushMatrix();
				Point railP = camerapath->curveAt (railT);
				Vector railTangent = camerapath->derivativeOfCurveAt (railT);
				Vector railN = crossProduct (eyedir, railTangent);
				double railAngle = (180.0 / M_PI) * acos (-railTangent.coord[2]); // the dot product.
				glTranslatef (-railP.coord[0], -railP.coord[1], -railP.coord[2]);
				glRotatef (railAngle + 90, railN.coord[0], railN.coord[1], railN.coord[2]);
				gluCylinder(quadric,0.1,0.1,1,3,1);
				glPopMatrix();
			}    
			gluDeleteQuadric(quadric);
			glEndList ();  
		}
	  	glCallList (railid);
	  
	  // move the world down slightly so we see the terrain from the top.
          glTranslatef (0.0, -8.0, 0.0);
          //Create sphere surrounding landscape with star texture within it
	  //I create lists throughout in the hope of efficiency.	  
	  if (!glIsList (starid))
		{
			cout << "Creating star list\n";
			starid = glGenLists (1);
			glNewList (starid, GL_COMPILE);
			quadric=gluNewQuadric();
			gluQuadricOrientation(quadric, GLU_INSIDE);
              		gluQuadricNormals(quadric,GLU_SMOOTH);
			GLfloat colour [4];
			colour[0] = 0.6;
			colour[1] = 0.6;
			colour[2] = 0.6;
			colour[3] = 1;
			glPushMatrix();
			gluQuadricTexture(quadric,GL_TRUE);
			glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, colour);            
          		glTranslatef(18,0,18);
			glEnable( GL_TEXTURE_2D );
			startexture->activate();
        	 	glEnable(GL_TEXTURE_GEN_S); 	
	  		glEnable(GL_TEXTURE_GEN_T);
          		gluSphere(quadric,90,20,20);
			glDisable(GL_TEXTURE_GEN_S); 	
			glDisable(GL_TEXTURE_GEN_T);
			gluDeleteQuadric(quadric);
			glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, noColour);            
          		glPopMatrix();
			glEndList ();  
		}
	  glCallList (starid);
	  
	  //terrains are all meshes, and optomised by the base code
          //Original terrain. mildy tweaked
	  GLfloat colour [4];
          colour[0] = 0.7;
          colour[1] = 1.0;
          colour[2] = 0.7;
          colour[3] = 1.0;
          glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, colour);            
          glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, colour);            
          colour[0] = 1.0;
          colour[1] = 0.0;
          colour[2] = 0.;
          colour[3] = 1.0;
          glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, colour);            
	  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
	  glEnable (GL_TEXTURE_2D);
          terraintexture->activate();	  
          terrain->render ();
	  glDisable (GL_TEXTURE_2D);
          
	  //spiny terrain extension
	  glPushMatrix();
	  colour[0] = 1;
          colour[1] = 1;
          colour[2] = 1;
          colour[3] = 1;
          glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, noColour);            
          glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, colour);            
          colour[0] = 1.0;
          colour[1] = 1.0;
          colour[2] = 1.0;
          colour[3] = 1.0;
          glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, colour);            
	  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
	  glTranslatef(37,0,0);
	  glColor3f(1,1,1);
	  terrain2->render();
	  glPopMatrix();
	  
	  //channel terrain extension
	  
	  glPushMatrix();
	  colour[0] = 1;
          colour[1] = 1;
          colour[2] = 1;
          colour[3] = 1;
          glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, noColour);            
          glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, colour);            
          colour[0] = 1.0;
          colour[1] = 1.0;
          colour[2] = 1.0;
          colour[3] = 1.0;
          glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, colour);            
	  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
	  glTranslatef(37,-10,37);
	  terrain3->render();
	  glPopMatrix();
	  
	  //warp extension
	  glPushMatrix();
	  colour[0] = 1;
          colour[1] = 1;
          colour[2] = 1;
          colour[3] = 1;
          glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, noColour);            
          glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, colour);            
          colour[0] = 1.0;
          colour[1] = 1.0;
          colour[2] = 1.0;
          colour[3] = 1.0;
          glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, colour);            
	  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
	  glTranslatef(0,-10,37);
	  terrain4->render();
	  glPopMatrix();
          
	if (!glIsList (meteorid))
		{
			cout << "Creating meteor list\n";
			meteorid = glGenLists (1);
			glNewList (meteorid, GL_COMPILE);
			quadric=gluNewQuadric();
			gluQuadricOrientation(quadric, GLU_OUTSIDE);
              		gluQuadricNormals(quadric,GLU_SMOOTH);
			glPushMatrix();
			gluQuadricTexture(quadric,GL_TRUE);
			colour[0] = 1;
			colour[1] = 1;
			colour[2] = 1;
			colour[3] = 1;
			glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, noColour);            
			glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, colour);            
			colour[0] = 1.0;
			colour[1] = 1.0;
			colour[2] = 1.0;
			colour[3] = 1.0;
			glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, colour);            
			glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 100.0);           
          		glTranslatef(18,0,18);
			glEnable( GL_TEXTURE_2D );
			meteortexture->activate();
        	 	glEnable(GL_TEXTURE_GEN_S); 	
	  		glEnable(GL_TEXTURE_GEN_T);
          		gluSphere(quadric,4,4,4);
			glDisable(GL_TEXTURE_GEN_S); 	
			glDisable(GL_TEXTURE_GEN_T);
			gluDeleteQuadric(quadric);
			glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, noColour);            
          		glPopMatrix();
			glEndList ();  
			
			
		}
	  for (int i = 0; i < 10; i++)
	    {
	      glPushMatrix ();
	      srandom (i);
	      glTranslatef (30.0 * biunitrand (), 10 * biunitrand () + 15, 30.0 * biunitrand ());
	      glRotatef (ang, biunitrand (), 1.0, biunitrand ());
	      glTranslatef (0.5, 1.2, 0.0);
	      glCallList (meteorid);
	      glPopMatrix ();
	    }
	  glFlush();
        }
      void resizeGL (int w, int h)
        {
          glViewport (0, 0, (GLint)w, (GLint)h);
        }

      void mouseMoveEvent (QMouseEvent * ev)
        {
          int x = ev->x ();
          int y = ev->y ();

          if (mouseox < 0)
            {
              mouseox = x;
              mouseoy = y;
              return;
            }

          double diffx = (double) (x - mouseox) / (double) (width ());
          double diffy = (double) (y - mouseoy) / (double) (height ());
	  
          mouseox = x;
          mouseoy = y;

          switch (ev->state ())
            {
              case Qt::LeftButton:
                rotx += (diffx * 360.0);
                roty += (diffy * 360.0);
                break;
              case Qt::MidButton:
                scale *= pow (2.0, diffx * 5.0);
                break;
              case Qt::RightButton:
                //reset viewpoint
		diffx = diffy = 0;
		rotx = roty = 0;
		break;
            }
          repaint ();
        }
      
      void mouseReleaseEvent (QMouseEvent * ev)
        {
          mouseox = -1;
          mouseoy = -1;
        }
      void initializeGL ()
        { 
	  glDepthFunc(GL_LEQUAL);
          glEnable (GL_DEPTH_TEST);
          glEnable(GL_FOG);
	  glHint(GL_FOG_HINT,GL_NICEST);
          glShadeModel (GL_SMOOTH);
	  glEnable (GL_DEPTH_TEST);
	  glEnable (GL_NORMALIZE);          
          glEnable (GL_LIGHTING);
	  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
          GLfloat global_ambient[] = { 0.0, 0.1, 0.0, 1.0 };
          glLightModelfv (GL_LIGHT_MODEL_AMBIENT, global_ambient);
          glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
          
	  terraintexture = new TextureMap ("terrain.png");
	  startexture = new TextureMap ("starscape.png");
	  meteortexture = new TextureMap ("meteor.png");
	  glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	  glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	  
	  railid = -1;
	  starid = -1;
	  meteorid = -1;
	  
	  //Fog
	  //glEnable (GL_BLEND);
	  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);          
          GLfloat fogColor[4] = {0, 0, 0, 1};
	  glFogi (GL_FOG_MODE, GL_LINEAR);
          glFogfv (GL_FOG_COLOR, fogColor);
          glFogf (GL_FOG_DENSITY, 0.02);
	  glFogf (GL_FOG_START, 10.0f);
	  glFogf (GL_FOG_END, 100.0f);
	  //glDisable (GL_BLEND);  
        }
  };

int main (int argc, char * argv [])

{
  QApplication a (argc, argv);

  Mesh * m1 = new Mesh ("terrain");  
  Mesh * m2 = new Mesh ("spiny");
  Mesh * m3 = new Mesh ("spiny2");
  Mesh * m4 = new Mesh ("spiny3");
         
  OpenGLPaintableWidget w (m1, m2, m3, m4,NULL, "canvas");
  w.show();

  while (1)
    {
      a.processEvents ();
      w.repaint ();
    }
    
}

#include "CoastRoll.moc"
