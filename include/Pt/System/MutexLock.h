#ifndef Pt_System_MutexLock_h
#define Pt_System_MutexLock_h

#include <Pt/System/Api.h>
#include <Pt/System/Mutex.h>

namespace Pt {

namespace System {

    /** @brief MutexLock class for Mutex.

        The MutexLock class adds functionality for scoped
        locking. In the constructor of a  MutexLock, the mutex is locked
        and in the destructor it is unlocked. This way if for example an
        exception occures in the protected section the Mutex will be unlocked
        during stack unwinding when the MutexLock is destructed.

        @code
             // example how to make a member function thread-safe
             #include <Pt/System/Mutex.h>

             class MyClass
             {
                 public:
                     void function()
                     {
                         MutexLock lock(_lock);

                         //
                         // protected operations
                         //

                        // dtor of MutexLock unlocks _lock
                     }

                 private:
                     Pt::System::Mutex _lock;
             };
        @endcode
    */
    class MutexLock {
        public:
            //! @brief Constructor
            /**
                Construct a MutexLock object and lock the enclosing mutex.

                \param m the enclosing Mutex object
            */
            MutexLock(Mutex& m, bool doLock = true)
            : _mutex(m)
            , _isLocked(false)
            {
                if(doLock)
                    this->lock();
            }

            //! @brief Destructor
            /**
                The destructor unlocks the mutex.
             */
            ~MutexLock()
            {
                try
                {
                    if(_isLocked)
                        _mutex.unlock();
                }
                catch(...) {}
            }

            void lock()
            {
                if(!_isLocked)
                {
                    _mutex.lock();
                    _isLocked = true;
                }
            }

            //! @brief Unlock so that the destructor does not unlock
            void unlock()
            {
                if(_isLocked)
                {
                    _mutex.unlock();
                    _isLocked = false;
                }
            }

             //! @brief Get the mutex object
             /**
                 \return the enclosing Mutex object
              */
            Mutex& mutex()
            { return _mutex; }

             //! @brief Get the mutex object
             /**
                 \return the enclosing Mutex object
              */
            const Mutex& mutex() const
            { return _mutex; }

            private:
                Mutex& _mutex;
                bool _isLocked;
    };

} // !namespace System

} // !namespace Pt

#endif
