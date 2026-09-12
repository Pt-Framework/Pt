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

#ifndef PT_FORMS_WINDOWMANAGER_H
#define PT_FORMS_WINDOWMANAGER_H

#include <Pt/Forms/Widget.h>
#include <Pt/Forms/Window.h>
#include <Pt/Gfx/Size.h>

namespace Pt {

namespace Forms {

class Window;
class WindowFrame;

/** @brief Shared window-host abstraction for screens and workspaces.

    A %WindowManager is a %Widget that attaches %Window objects to a visual
    host. %Screen provides the manager for top-level platform windows, while
    %Workspace uses a derived manager to present windows in its own bounds.
    Applications normally select a manager through a screen or workspace and
    attach windows through %Window; they do not derive from this class.

    Backend implementations derive from %WindowManager and implement the
    attachment and lifecycle hooks that create, initialize, release, and
    detach each internal %WindowFrame. %Window controls that lifecycle: it
    calls %onAttach(), initializes the frame and then calls %onInit(). During
    detachment it releases the frame, calls %onRelease() and %onDetach(), and
    finally deletes the frame. The returned frame is therefore owned by the
    window; a manager must not delete it.

    The default %onInit() and %onRelease() implementations connect and
    disconnect the frame and its window from the current screen. Derived
    managers must call the corresponding base implementation. The protected
    hooks are for backend implementations and are not an application
    customization API.

    @ingroup Pt-Forms-Application
*/
class WindowManager : public Widget
{
    friend class Window;

    public:
        typedef Widget Base;

    public:
        /** @brief Creates a disconnected host for window frames.
        */
        WindowManager();

        /** @brief Destroys the window host.
        */
        virtual ~WindowManager();

        /** @brief Requests a relayout of hosted windows.

            Forwards the request to %onRequestRelayout(). The base
            implementation does nothing.
        */
        void relayout();

        /** @brief Returns the signal sent after a window backing pixmap is synchronized.

            The signal argument identifies the painted window. It is sent
            after its %WindowFrame processes a paint event; it does not report
            attachment, visibility, or geometry changes.
        */
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

        //std::vector<Pt::Forms::Window*>& windows()
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
        /** @brief Connects this host to @a screen.

            Derived managers must call the base implementation and connect
            their existing frames when the host becomes connected.
        */
        virtual void onConnect(Screen& screen);

        /** @brief Disconnects this host from its screen.

            Derived managers must disconnect their frames and call the base
            implementation.
        */
        virtual void onDisconnect();

        /** @brief Handles a request to relayout hosted windows.

            The base implementation does nothing. Embedded managers normally
            forward the request to their containing widget.
        */
        virtual void onRequestRelayout();

    protected:
        /** @brief Processes a paint event through the %Widget event path.

            Embedded managers can override this hook to paint their hosted
            frames.
        */
        virtual void onProcessPaintEvent(const PaintEvent& ev);

        /** @brief Handles a paint event after standard processing.

            The base implementation forwards the event to %Widget.
        */
        virtual void onPaintEvent(const PaintEvent& ev);

    protected:
        /** @brief Creates and registers a frame for @a window.

            Return a non-null frame that remains valid until %onDetach() is
            called. Configure the responder relationship and manager
            bookkeeping, but do not delete the frame; %Window owns and
            deletes it.
        */
        virtual WindowFrame* onAttach(Window& window) = 0;

        /** @brief Unregisters @a frame from this host.

            Called after %onRelease(). Clear responder links and manager
            bookkeeping, but do not delete @a frame.
        */
        virtual void onDetach(WindowFrame& frame) = 0;

        /** @brief Initializes @a frame after the frame initializes its window.

            Prepare host-specific state such as scaling, then call the base
            implementation to connect the frame and its window to the current
            screen.
        */
        virtual void onInit(WindowFrame& frame) = 0;

        /** @brief Releases @a frame before it is detached and deleted.

            Call the base implementation to disconnect the frame and its
            window from the current screen.
        */
        virtual void onRelease(WindowFrame& frame) = 0;

        //virtual void onAutoCenter(WindowFrame& w, bool enable) = 0;

    private:
      Pt::Signal<Window&> _surfaceChanged;

      //std::vector<Pt::Forms::Window*> _windows;
      //Pt::Signal<Window&>           _windowAdded;
      //Pt::Signal<Window&>           _windowRemoved;
};

} // namespace

} // namespace

#endif
