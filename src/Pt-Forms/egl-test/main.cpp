#include <iostream>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "RenderScreen.h"

#ifdef WIN32
#include "DisplayWin32.h"
#else
#include "Display.h"
#endif

class Application
{
  public:
    Application()
      :screen( display.width(), display.height() )
    {
    }

    void render()
    {
      screen.Render();
      display.updateScreen();
    }
  private:
    Pt::Forms::Display       display;
    Pt::Forms::RenderScreen  screen;
};


int main(int argc, char* argv[])
{
  Application app;

  // main loop
  while( true )
  { 
    app.render();
  }

  return 0;
}