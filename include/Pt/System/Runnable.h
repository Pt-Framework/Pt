/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/
#if !defined(PT_Runnable_H)
#define PT_Runnable_H

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
    class PT_API Runnable : public NonCopyable
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

} // namespace System

} // namespace Pt

#endif // PT_Runnable_H
