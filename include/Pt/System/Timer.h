#ifndef Pt_System_Timer_h
#define Pt_System_Timer_h

#include <Pt/Signal.h>
#include <Pt/System/Api.h>
#include <vector>


namespace Pt {

namespace System {

    /** @brief Notifies receivers in constant intervals
    */
    class PT_SYSTEM_API Timer
    {
        public:
            Timer();

            ~Timer();

            bool active() const;

            size_t interval() const;

            void setInterval(size_t msecs);

            void start(unsigned interval);

            void stop();

            bool update();

            size_t remaining() const;

            Signal<> timeout;

            Signal<Timer&> destroyed;

        private:
            bool     _active;
            unsigned _started;
            unsigned _interval;
            unsigned _elapsed;
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
