#include <iostream>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "RenderScreen.h"
#include "Display.h"


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
    Pt::Hmi::Display       display;
    Pt::Hmi::RenderScreen  screen;
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