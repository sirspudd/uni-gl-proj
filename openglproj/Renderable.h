#ifndef __Renderableh
#define __Renderableh

class Renderable
  {
    public:
      Renderable () 
      {}

      virtual void render () = 0;
      virtual void renderWireframe () = 0;
      virtual void renderNormals () = 0;
  };


#endif
// __Renderableh


