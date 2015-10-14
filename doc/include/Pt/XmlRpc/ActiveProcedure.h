#ifndef PT_REMOTING_ACTIVEPROCEDURE_H
#define PT_REMOTING_ACTIVEPROCEDURE_H

namespace Pt {

namespace Remoting {

/** @brief Asynchronous service procedure.

    Instances derived from this class are returned by the asynchronous service
    procedures registered in a Service. The template parameters imply the
    signature of the procedure. The first tempate parameter is the return
    value type, followed by the argument types, if any. For example, a 
    %ActiveProcedure<double, int, int> is a service procedure which returns
    a double and takes two ints as arguments. A %ActiveProcedure<std::string>
    is a service procedure without arguments, only returning a std::string.
    
    The two methods onInvoke() and onResult() need to be implemented. When a 
    RPC request was received and dispatched to an asynchronous procedure,
    onInvoke() is called with the deserialized arguments and an EventLoop to
    execute further asynchronous operations such as I/O. It is the task of
    the implementer to advance progress of the procedure until a result is
    ready. By calling setReady(), the Responder will be notified of the 
    completion and call onResult() to get the result and serialize it. Both
    methods may throw exceptions of type Fault to indicate failure.

*/
template < typename R, typename... ARGS>
class ActiveProcedure : public ServiceProcedure
{
    public:
        /** @brief Constructs with Responder.
        */
        ActiveProcedure(Responder& resp);

        /** @brief Destructor.
        */
        ~ActiveProcedure();

    protected:
        // inherit doc
        virtual IComposer** onBeginArgs();

        // inherit doc
        virtual void onBeginCall(System::EventLoop& loop);

        // inherit doc
        virtual IDecomposer* onEndCall();

    protected:
        /** @brief Start an asynchronous procedure.
        */
        virtual void onInvoke(System::EventLoop& loop, const ARGS...& args) = 0;

        /** @brief Return result of the asynchronous procedure.
        */
        virtual const R& onResult() = 0;
};

} // namespace Remoting

} // namespace Pt

#endif // PT_REMOTING_ACTIVEPROCEDURE_H
