#ifndef PT_REMOTING_API_REMOTEPROCEDURE_H
#define PT_REMOTING_API_REMOTEPROCEDURE_H

namespace Pt {

namespace Remoting {

/** @brief Performs a remote procedure call.

    This class is a function object to perform remote procedure call. It is
    a class template where the template parameters imply the signature of the
    remote procedure. The first tempate parameter is the return value type, 
    followed by the argument types, if any. For example, a 
    %RemoteProcedure<double, int, int> calls a remote procedure which returns
    a double and takes two ints as arguments. A %RemoteProcedure<std::string>
    calls a remote procedure without arguments, only returning a std::string.

    Synchronous operation is supported by means of the operator() and the call()
    method. Asynchronous calls can be made by calling begin and connecting to
    the finished signal, which is sent when the result has been received.
    The finshed signal has one parameter of type Result<R>, which contains
    the return value.

    The %RemoteProcedure uses a Client for communication with the service. In
    many cases this will be a HttpClient, but other transport protocols can
    be supported by other types of clients. The Client must live longer than
    the remote procedures using it.

    @headerfile RemoteProcedure.h <Pt/Remoting/RemoteProcedure.h>
*/
template <typename R, typename... ARGUMENTS>
class RemoteProcedure
{
    public:
        /** @brief Construct with procedure name and client.
        */
        RemoteProcedure(Client& client, const std::string& name);

        /** @brief Destructor.
        */
        virtual ~RemoteProcedure();

        /** @brief Begins an asynchronous remote call.
        */
        void begin(const ARGUMENTS& args...);

        /** @brief Blocking remote call.
        */
        const R& call(const ARGUMENTS& args...);

        /** @brief Blocking remote call
        */
        const R& operator()(const ARGUMENTS& args...);

        /** @brief Returns the result of the call.
        */
        Result<R>& result();

        /** @brief Returns the result of the call.
        */
        const Result<R>& result() const;

        /** @brief Reports that the result has been received.
        */
        Signal< const Result<R>& >& finished();

        /** @brief Returns the client used for communication.
        */
        Client& client();

        /** @brief Returns the name of the procedure.
        */
        const String& name() const;

        /** @brief Returns true if the procedure failed.
        */
        bool isFailed() const;

        /** @brief Cancels the remote call.
        */
        void cancel();
};

}

}

#endif
