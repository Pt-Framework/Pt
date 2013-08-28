#ifndef Pt_Hmi_ApplicationImpl_h
#define Pt_Hmi_ApplicationImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Hmi/Controller.h>
#include "win32/Selector.h"
#include <vector>
#include <windows.h>

namespace Pt {
namespace Hmi {


class Selector : public System::Selector
{
    public:
        Selector();
        
        ~Selector();

        void processMessage();

    protected:
        virtual DWORD waitFor(DWORD numHandles, const HANDLE *handles, DWORD msecs, bool& isTimeout);
};


class ApplicationImpl : public Pt::System::EventLoop
{
    public:
        ApplicationImpl();

        virtual ~ApplicationImpl();

        Pt::System::Selector& selector()
        { 
			return _selector; 
		}

        HINSTANCE getInstanceHandle()
        { 
			return _instanceHandle; 
		}

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

    protected:
        static long CALLBACK wndProc(HWND hwnd, unsigned int message, unsigned int wParam, long lParam);
		static LRESULT CALLBACK mouseProc (int nCode, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK keyboardProc(int code, WPARAM wParam, LPARAM lParam);

		LRESULT dispatchGDIEvent(HWND hwnd, unsigned int message, unsigned int wParam, long lParam);

     protected:
        virtual void onAttachSelectable(System::Selectable&);

        virtual void onDetachSelectable(System::Selectable&);

        virtual void onIdle(System::Selectable& s);

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
        bool waitNext();

    private:
        //! @brief Registers the top level and child window classes with Windows for later use.
        void registerWindowClasses();

        //! @brief Unregisters the top level and child window classes.
        void unregisterWindowClasses();

		void getScreeResolution(int& horizontal, int& vertical);

	public:		
		Pt::Signal<HWND, unsigned int, unsigned int, long, bool&> WindowEvent;

    private:
        //! @brief Instance handle of this application
        HINSTANCE _instanceHandle;
        bool _trackingMouseEvent;
		static HHOOK _mouseHook;
		static HHOOK _keyboardHook;

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
        Pt::Hmi::Selector _selector;
        Pt::Signal<const Pt::Event&>* _event;
        std::vector<System::Selectable*> _avail;
};

} // namespace Hmi

} // namespace Pt

#endif

