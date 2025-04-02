/* Copyright (C) 2013 Marc Boris Dürner
 * Copyright (C) 2013 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/

#ifndef Pt_Forms_ApplicationImpl_h
#define Pt_Forms_ApplicationImpl_h

#include "winrt/Selector.h"
#include <Pt/Forms/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Forms/Controller.h>
#include <vector>
#include <windows.h>

namespace Pt {
namespace Forms {

class ApplicationImpl;

ref class FrameworkView : public Windows::ApplicationModel::Core::IFrameworkView
{
public:	
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView );

    virtual void Uninitialize();

    virtual void SetWindow( Windows::Gfx::Core::CoreWindow^ window );

    virtual void Run();

    virtual void Load( Platform::String^ entryPoint ) ;

	Windows::Gfx::Core::CoreDispatcher^ dispatcher()
	{
		return _dispatcher;
	}
		

	void setApplicationImpl( long long  ptr)
	{
		_impl = (ApplicationImpl*)ptr;
	}
private:
	Windows::Gfx::Core::CoreWindow^ _window;
	Windows::Gfx::Core::CoreDispatcher^ _dispatcher;
	ApplicationImpl* _impl;

};

class ApplicationImpl : public Pt::System::EventLoop
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();
	
        void Closed(Windows::Gfx::Core::CoreWindow^ Sender, Windows::Gfx::Core::CoreWindowEventArgs^ Args);

		Windows::ApplicationModel::Core::IFrameworkView^ frameworkView()
		{
			return _frameworkView;
		}

		bool waitNext();

	public:
        void nextEvent();

        double toUnit(int value);
        Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value);
        Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value);

        int fromUnit(double value);
        Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value);
        Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value);

        double unitSizeInch() const;
        double unitSizeMm() const;

        void setResolution(double dpi);

        double resolutionDPI() const;
        
        void showConsole(bool show);
    
        inline Pt::Signal<Controller*, const PointingEvent&>& pointerEvent()
        {
            return _pointerEvent;
        }

        inline Pt::Signal<Controller*, const KeyEvent&>& keyDeviceEvent()
        {
            return _keyDeviceEvent;
        }

        Pt::System::Selector& selector()
        { 
            return _selector; 
        }

     protected:
        virtual void onAttachSelectable(System::Selectable&);

        virtual void onDetachSelectable(System::Selectable&);

        virtual void onCancel(System::Selectable& s);

        virtual void onReady(System::Selectable& s);

        virtual void onRun();

        virtual void onExit();

        virtual void onCommitEvent(const Pt::Event& event);

        virtual void onQueueEvent(const Pt::Event& event);

        virtual void onProcessEvents();

        virtual void onWake();

        virtual void onAttachTimer(System::Timer& timer);

        virtual void onDetachTimer(System::Timer& timer);

    protected:
        

    private:
        void getScreeResolution(int& horizontal, int& vertical);

    private:
        //! @brief Instance handle of this application
        bool _trackingMouseEvent;

        //Unit infos
        int _screenWidth;
        int _screenHeight;
        double _factorX;
        double _offsetX;
        double _factorY;
        double _offsetY;
        double _width;
        double _height;
        double _dpi;

        Pt::Signal<Controller*, const PointingEvent&> _pointerEvent;            
        Pt::Signal<Controller*, const KeyEvent&> _keyDeviceEvent;

    private:
        System::Mutex _mutex;
        System::TimerQueue _timerQueue;
        System::EventQueue _eventQueue;
        std::vector<System::Selectable*> _avail;
        System::Selector _selector;
		bool _isClosed;
        Pt::Signal<const Pt::Event&>* _event;
		FrameworkView^ _frameworkView;
};


ref class AppSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
    public:
	    AppSource(long long impl)
        : _impl((ApplicationImpl*)impl)
        {}

	    //Basic method that returns to us an instance of our user defined App class.
	    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	    {
		    return _impl->frameworkView();
	    }

    private:
        ApplicationImpl* _impl;

};

}

}

#endif

