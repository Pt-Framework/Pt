/* Copyright (C) 2015 Marc Boris Duerner
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_HMI_WINDOWMANAGER_H
#define PT_HMI_WINDOWMANAGER_H

#include <Pt/Hmi/Visual.h>
#include <Pt/Hmi/Window.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Hmi {

class Window;
class WindowFrame;

class WindowManager : public Visual
{
    friend class Window;

    public:
        WindowManager();

        virtual ~WindowManager();

        Pt::Signal<Window&>& surfaceChanged()
        {
          return _surfaceChanged;
        }

        // TODO:

        //void addWindow(Window& w)
        //{
        //    w.setParent(*this);
        //}

        //void removeWindow(Window& w)
        //{
        //    if(w.windowManager() == this)
        //        w.unparent();
        //}

        //std::vector<Pt::Hmi::Window*>& windows()
        //{
        //  return _windows;
        //}

        //Pt::Signal<Window&>& windowAdded()
        //{
        //  return _windowAdded;
        //}

        //Pt::Signal<Window&>& windowRemoved()
        //{
        //  return _windowRemoved;
        //}

    protected:
        virtual WindowFrame* onAttach(Window& w) = 0;

        virtual void onDetach(WindowFrame& w) = 0;

        virtual void onInit(WindowFrame& w) = 0;

        virtual void onRelease(WindowFrame& w) = 0;

    private:
      Pt::Signal<Window&> _surfaceChanged;

      std::vector<Pt::Hmi::Window*> _windows;
      Pt::Signal<Window&>           _windowAdded;
      Pt::Signal<Window&>           _windowRemoved;
};

} // namespace

} // namespace

#endif
