#include <Pt/System/Api.h>
#include <Pt/Callable.h>
#include <thread>

namespace Pt {

namespace System {

class ThreadImpl 
{
    public:
        struct ThreadExit
        {};
    
    public:
        ThreadImpl()
        : _cb(0)
        , _thread(0)
		, _detach(false)
        {}

        ~ThreadImpl();

        void init(const Callable<void>& cb);

        void start();

        void detach();

        void join();

        static void exit();

        static void yield();

        static void sleep(unsigned int ms);

    private:
        const Callable<void>* _cb;
		bool _detach;
        std::thread* _thread;
  };

} // namespace System

} // namespace Pt



