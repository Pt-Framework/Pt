#ifndef Pt_XmlRpc_Service_h
#define Pt_XmlRpc_Service_h

namespace Pt {

namespace XmlRpc {

/** @brief XML-RPC service.

    The XML-RPC service provides a number of service procedures to its clients.
    Regular C++ functions or methods can be registered as service procedures,
    but in many cases a derived service simply registers member functions. The
    XML-RPC requests are then dispatched according to the name of the registered
    procedures. The arguments and return values must be serializable, for 
    example by supporting serialization to a SerializationInfo. The service
    supports synchronous and asynchronous procedures, where the latter ones
    are factories for %ActiveProcedure objects.
*/
class PT_XMLRPC_API Service : private NonCopyable
{
    public:
        /** @brief Constructor.
        */
        Service();

        /** @brief Destructor.
        */
        virtual ~Service();

        //! @internal
        ServiceProcedure* getProcedure(const std::string& name, Responder& resp);

        //! @internal
        void releaseProcedure(ServiceProcedure* proc);

        /** @brief Registers a function as a synchronous procedure.

            Registers the function \a func as a service procedure named \a
            name. Functions with up to ten arguments are supported. The 
            template parameter ARGS expands to the argument types of the
            funtcions signature.
        */
        template <typename R, typename... ARGS>
        void registerProcedure(const std::string& name, R (*func)(ARGS...));

        /** @brief Registers a member function as a synchronous procedure.

            Registers the method \a mth of the object instance \a obj as
            a service procedure named \a name. Methods with up to ten
            arguments are supported. The template parameter ARGS expands to
            the argument types of the methods signature.
        */
        template <typename R, class C, typename... ARGS>
        void registerProcedure(const std::string& name, C& obj, R (C::*mth)(ARGS...) );

        /** @brief Registers a generic callable as a synchronous procedure.
            
            Registers the callable \a cb as a service procedure named \a
            name. Callables with up to ten arguments are supported. The 
            template parameter ARGS expands to the argument types of the 
            signature.
        */
        template <typename R, typename... ARGS>
        void registerProcedure(const std::string& name, const Callable<R, ARGS...>& cb);

        /** @brief Registers a function as an asynchronous procedure.

            Registers the function \a func as an asynchronous service procedure
            named \a name. Asynchronous service procedures return an instance
            of an ActiveProcedure created with default new. The Responder
            needs to be passed to the constructed %ActiveProcedure.
        */
        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, A* (*func)(Responder&) );

        /** @brief Registers a member function as an asynchronous procedure.

            Registers the method \a mth of the object instance \a obj as an 
            asynchronous service procedure named \a name. Asynchronous service
            procedures return an instance of an ActiveProcedure created with
            default new. The Responder needs to be passed to the constructed
            %ActiveProcedure.
        */
        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, C& obj, A* (C::*mth)(Responder&) );

        /** @brief Registers a const member function as an asynchronous procedure.

            Registers the method \a mth of the object instance \a obj as an
            asynchronous service procedure named \a name. Asynchronous service
            procedures return an instance of an ActiveProcedure created with
            default new. The Responder needs to be passed to the constructed
            %ActiveProcedure.
        */
        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, C& obj, A* (C::*mth)(Responder&) const );
};

} // namespace XmlRpc

} // namespace Pt

#endif
