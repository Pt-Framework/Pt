#ifndef Pt_System_Timer_h
#define Pt_System_Timer_h

#include <Pt/Signal.h>
#include <Pt/System/Api.h>
#include <vector>
#include <cstddef>


namespace Pt {

namespace System {

    /** @brief Notifies clients in constant intervals

        Timers can be used to be notified if a time interval expires. It
        usually works with a Selector or event loop, where the Timer
        needs to be registered. Timers send the timeout signal
        in given intervals, to which the interested clients connect. The
        interval can be changed at any time and timers
        can switch between an active and inactive state.
        The following code calls the function onTimer every second:
        @code
        void onTimer()
        {
            std::cerr << "Time out!\n";
        }

        int main()
        {
            Pt::System::Timer timer;
            connect(timer.timeout, onTimer);

            Pt::System::EventLoop loop;
            loop.addTimer(timer);
            timer.start(1000);

            loop.run();
            return 0;
        }
        @endcode
    */
    class PT_SYSTEM_API Timer
    {
        public:
            /** @brief Default constructor

                Contructs an inactive timer.
            */
            Timer();

            /** @brief Destructor

                The destructor sends the destroyed signal.
            */
            ~Timer();

            /** @brief Returs true if timer is active
            */
            bool active() const;

            /** @brief Returns the current timer interval

                Returns the current interval of the timer in milliseconds.
            */
            std::size_t interval() const;

            /** @brief Sets the timer interval

                This method will set the interval in which the timer notifies
                the connected clients by sending the timeout signal.

                @param msecs The timer inteval in milliseconds
            */
            void setInterval(unsigned msecs);

            /** @brief Starts the timer

                Start a timer from the moment this method is called. The
                Timer needs to be registered with a Selector or event loop,
                otherwise the timeout signal will not be sent.

                @param interval Timeout interval in milliseconds
            */
            void start(unsigned interval);

            /** @brief Stops the timer

                If the Timer is registered with a Selector or an event loop,
                the timout signal will not be sent anymore.
            */
            void stop();

            /** @brief Update the timer

                This method is supposed to be called by the Selector or an
                event loop. If the interval timeout is passed the Timer
                will send the timeout signal and return true, otherwise
                internal times are updated and false is returned.
            */
            bool update();

            /** @brief Returns the remaining time to the next timeout
            */
            std::size_t remaining() const;

            /** @brief Notifies about interval timeouts

                This signal is sent if the interval time has expired.
            */
            Signal<> timeout;

            /** @brief Notifies about object destruction

                This signal is sent if the Timer is destroyed. A
                reference to the destroyed timer is passed by the
                signal.
            */
            Signal<Timer&> destroyed;

        private:
            bool     _active;
            unsigned _started;
            unsigned _interval;
            unsigned _elapsed;
            static const unsigned InvalidTime;
    };

/*
class PT_SYSTEM_API Timer : public Runnable
{
public:
    Timer(void);
    virtual ~Timer(void);

    static size_t resolution();

    void start();
    void stop();

    void run();

    Signal<size_t> onTime;

private:
    Thread            _timerThread;
    static size_t    _resolution;
    size_t            _eventCounter;
    bool            _run;

};
*/

}

}

#endif
