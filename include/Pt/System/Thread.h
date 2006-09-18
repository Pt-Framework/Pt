/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Dürner                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Thread_h
#define Pt_Thread_h

#include <Pt/Api.h>
#include <Pt/NonCopyable.h>


namespace Pt {

namespace System {


    //! @brief Runnable interface.
    /**
	*   The Runnable interface should be implemented by any class whose
    *   instances are intended to be executed by a thread. The class must
    *   define a method of no arguments called run. This interface is
    *   designed to provide a common protocol for objects that wish to
    *   execute code while they are active. In addition, Runnable provides
    *   the means for a class to be active while not subclassing Thread.
    *   A class that implements Runnable can run without subclassing Thread
    *   by instantiating a Thread instance and passing itself in as the target.
    */
    class PT_EXPORT Runnable : public NonCopyable
    {
        public:
	        //! @brief Thread function
			/** When an object implementing interface Runnable is used to
            *   create a thread, starting the thread causes the object's
            *   run method to be called in that separately executing thread.
            *   The general contract of the method run is that it may take any
            *   action whatsoever.
            */
            virtual void run() = 0;

        protected:
             //! @brief Default constructor
            Runnable(){};

            //! @brief Destructor
            virtual ~Runnable(){};
    };

	//! @brief Thread class.
	/**
    *    A Thread represents a separate thread of control within the program.
    *    It shares data with all the other threads within the process but
    *    executes independently in the way that a separate program does on a
    *    multitasking operating system. Threads can either run as Joinable,
    *    so you can wait for them, or be Detached, so they run indepentently.
    *    The execution of a thread starts either by calling its virtuals method
    *    Thread::run(), which can be reimplemented in a derived class.
    *    Alternatively, an instance of Runnable object can be passed to the
    *    constructor. To create your own threads, subclass Runnable and
    *    reimplement run().
    *    For example:
    *    @code
    *    class MyRunnable : public Runnable
    *    {
    *       public:
    *           void run();
    *    };
    *    @endcode
    *    To start the thread, a instance of a Thread class needs to be created
    *    from the derived Runnable class.
    *    For example:
    *    @code
    *    MyRunnable runnable;
    *    Thread     thread(runnable);
    *
    *    thread.start();
    *    @endcode
    *
    *    Each thread gets its own stack, which size is determinated by the
    *    operating system. A thread can be forced to terminate by calling
    *    Thread::terminate(), however, doing so is dangerous and discouraged.
    *    Thread also provides platform independent sleep function.
    *    Thread::start begins the execution by calling the reimplemented
    *    Thread::main member function. If the Thread is Joinable, it can be
    *    waited on by calling Thread::wait on it.
    *    A thread can give up CPU time either by calling Thread::yield() or
    *    Thread::sleep() to stop for a specified periode of time.
    */
	class PT_EXPORT Thread : public NonCopyable
    {
		friend class ThreadImpl;

		private:
			class ThreadImpl* _impl;

		public:
			enum State
            {
				Ready    = 0,
				Running  = 1,
				Finished = 2
			};

			enum Mode
            {
				Joinable = 0,
				Detached = 1
			};

		public:
			//! @brief Default Constructor.
			///
			/// Constructs a thread object. The Thread is not started on construction,
			/// but when Thread::start() is called. Threads can either be detached or
			/// joinable.
			///
            /// @param runnable specify a runnable object with a run methode
			/// @param mode Joinable or Detached
			Thread(Runnable& runnable, Mode mode = Joinable);

			//! @brief Destructor
			///
			/// If the thread is joinable Thread::wait() is called. Detached Threads
			/// do not depend on the Thread object lifetime.
			virtual ~Thread();

			//! @brief Returns the current mode of the thread.
			///
			/// @return the Thread mode, either Joinable or Detached
			Mode mode() const;

			//! @brief Returns the current state of the thread.
			///
			/// @return the Thread state
			State state() const;

			//! @brief Returns true if thread is detached.
			///
			/// @return true if the thread is detached
			bool detached() const;

			//! @brief Returns true if thread is joinable.
			///
			/// @return true if the thread is joinable
			bool joinable() const;

			//! @brief Starts the thread and calls Thread::main()
			///
			/// This starts the execution of the thread. This mean Thread::main()
			/// will be called, which needs to be overriden in derived classes.
			///
			/// @return a self reference for error checking
			virtual Thread& start();

			//! @brief Wait until a joinable thread has exited.
			///
			/// @return a self reference for error checking
			virtual Thread& wait();

			//! @brief Terminates the thread.
			///
			/// Terminates the thread without respect for any allocated resources.
			/// Use with caution.
			///
			/// @return a self reference for error checking
			virtual Thread& terminate();

			//! @brief Detaches a joinable thread.
			///
			/// Detached threads can not be waited on and become independent of the lifetime
			/// of the Thread object, since no resources need to be reclaimed for them.
			///
			/// @return a self reference for error checking
			Thread& detach();

			//! @brief Exits a joinable thread.
			///
			/// This function is meant to be called from within a thread. Thread::exit()
			/// is implicitly called whenThread::main() returns.
			static void exit();

			//! @brief Yield CPU time
			///
			/// This function is meant to be called from within a thread.
			static void yield();

			//! @brief Sleep for some time
			///
			/// @param ms millisecs to sleep
			static void sleep(unsigned int ms);

		protected:
            //! @brief Constructor
			///
		    /// @param mode Joinable or Detached
	        Thread(Mode mode = Joinable);

            //! @brief Thread entry point
			/**  This function needs to be overridden by derived classes.
            *    Starting the thread causes the object's run method to be called
            *    in that separately executing thread.
            */
            virtual void run();

        private:
            Runnable* _runnable;
	};

} // !namespace System

} // !namespace Pt

#endif
