/* Copyright (C) 2026 Marc Boris Duerner

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef Pt_Forms_ApplicationImpl_h
#define Pt_Forms_ApplicationImpl_h

#include "InputDevice.h"
#include "MouseDevice.h"
#include "DrmDevice.h"
#include "DrmDisplay.h"

#include <Pt/Forms/Widget.h>
#include <Pt/Forms/Cursor.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/System/MainLoop.h>
#include <Pt/DateTime.h>
#include <Pt/Timespan.h>

#include <vector>
#include <memory>
#include <string>

namespace Pt {

namespace Forms {

class ApplicationImpl : public Pt::System::MainLoop
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        const Cursor& cursor()
        { return _cursor; }

        void setCursor(const Cursor* cursor);

        Pt::Timespan inactivityTime() const;

        void sendKeyEvent(const KeyEvent& ev);

        void sendMouseEvent(const MouseEvent& ev);

        void nextEvent();

        DrmDisplay& drmDisplay()
        { return _drmDisplay; }

    private:
        void onMouseEvent(const MouseEvent& ev);

        void onScrollEvent(const ScrollEvent& ev);

        void onTouchEvent(const TouchEvent& ev);

        void onKeyEvent(const KeyEvent& ev);

        bool openConsole();

        void closeConsole();

        void showConsole(bool s);

        void openMouseDevice(const std::string& device);

        void openInputDevice(const std::string& device);

    private:
        DrmDevice                                   _drmDevice;
        DrmDisplay                                  _drmDisplay;
        std::vector<std::unique_ptr<InputDevice>>   _inputDevices;
        std::unique_ptr<MouseDevice>                _mouseDevice;
        Gfx::Transform                              _touchTransform;
        Cursor                                      _cursor;
        int                                         _consoleFd;
        bool                                        _consoleInGraphicsMode;
        std::string                                 _consoleDevice;
        Pt::DateTime                                _lastActivityTime;
};

} // namespace

} // namespace

#endif
