#ifndef PT_APPLICATION_H
#define PT_APPLICATION_H

#include <Pt/Api.h>
#include <Pt/Connectable.h>
#include <Pt/Event.h>
#include <Pt/Signal.h>

namespace Pt {


    class PT_EXPORT Application : public Connectable {
        public:
            Application()
            { }

            virtual ~Application()
            { }

            /**
             * \brief Starts this application and the underlying event loop.
             */
            virtual int run() = 0;

            /**
             * \brief Stops the execution of this application.
             */
            virtual void exit() = 0;

            /**
             * \brief Commits the given event to this application's EventLoop and wakes
             * the EventLoop so it delivers events.
             *
             * @param event The event object that will be added to the event queue.
             */
            virtual void commitEvent(const Event& event) = 0;

            /**
             * \brief Queues the given event to this application's EventLoop without
             * waking up the events so they may not be delivered immediately.
             *
             * @param event The event object that will be added to the event queue.
             */
            virtual void queueEvent(const Event& event) = 0;

            /**
             * \brief Delivers all events of this application's EventLoop which are
             * currently inside the event queue to the registered listeners.
             */
            virtual void processEvents() = 0;

            /**
             * \brief The signal to which slots can register themselves to listen for
             * any event that is committed to this application's event loop.
             */
            Signal<const Event&> event;
    };

} // namespace Pt

#endif
