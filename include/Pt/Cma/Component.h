#ifndef PT_CMA_COMPONENT_H
#define PT_CMA_COMPONENT_H

#include <Pt/Api.h>
#include <Pt/Cma/TypeId.h>
#include <Pt/Cma/IUnknown.h>
#include <Pt/Cma/IComponentBuilder.h>
#include <map>

namespace Pt {

namespace Cma {

    /**
     *  Base class of all components.
     *
     *  This is the base class fo all componets.
     *  All componets have to subclass ComponentBase.
     */
    class PT_EXPORT ComponentBase
    {
        public:
            typedef std::map<TypeId,IUnknown*> InterfaceMap;

            //! Default constructor
			ComponentBase()
			{}

            //! Default deconstructor
			virtual ~ComponentBase()
			{}

            /**
             *  Register an interface to this component.
             *
             *  Add an interface to the set of the component's services.
             *  @param impl the component that implements the interface <IfaceT>
             */
            template <typename IfaceT>
			void registerInterface(IfaceT* impl)
			{
				_interfaces[IfaceT::typeId()] = static_cast<IfaceT*>(impl);
			}

            /**
             *  Register an interface to this component
             *
             *  Add an interface to the set of the component's services.
             *  @param typeId the component type ID.
             *  @param iface the interface to register.
             */
            void registerInterface(const TypeId& typeId, IUnknown* iface)
            {
                _interfaces[typeId] = iface;
            }


		protected:
			InterfaceMap _interfaces;

    };

    /**
     *  Class type derived from any concrete compoment.
     *
     *  Component is derived from any concrete compoment that implement services.
     *  It is used as a linking class between the ComponentBuilder and the
     *  component implementation classes.
     */
	template< typename BaseT>
	class Component : public BaseT
	{
		public:
            typedef std::map<Pt::Cma::TypeId,IUnknown*> InterfaceMap;

            //! Default constructor
            Component()
			:_refs(0)
            ,_builder(0)
			{}

            Component(IComponentBuilder* builder)
			:_refs(0)
            ,_builder(builder)
			{}

            //! Default deconstructor
			virtual ~Component()
			{}

            /**
             * Looks up the registered interfaces for a specified service.
             *
             * Looks up a specific interface type in the set of registered
             * interfaces and increases the reference count if any found.
             *
             * @param typeId the interface type id to look up
             * @return pointer to the found component type (of class IUnknown)
             *         that implements the typeId Interface,
             *         0 if none was found.
             */
			IUnknown* queryInterface(const TypeId& typeId)
			{
                InterfaceMap::iterator iter = this->_interfaces.find(typeId);

				if(iter == this->_interfaces.end())
				{
				    return 0;
                }

				_refs++;
				return iter->second;
			}

            /**
             * Get the compoment implementation.
             *
             * @return the compoment implementation type, 0 if none interfaces
             *         are registered.
             */
            IUnknown* getComponent()
			{
				if( this->_interfaces.empty() )
					return 0;

				_refs++;
				/* When the interfaces were registered, the according component
				 * was saved in the "second" slot of the map. */
				return this->_interfaces.begin()->second;
			}


            /**
             * Decrement the reference count of this component.
             *
             * The components are reference counted and each time an interface
             * is queried, the reference counter is increased. If all interfaces
             * are released, the component is not referenced anymore and deletes
             * itself.
             */
            void release()
			{
				_refs--;
				if(_refs == 0)
                {
                    if(_builder)
                        _builder->release();

					delete this;

                }
			}

		private:
		    /** number of created interfaces accessing this component */
			int                _refs;

		    /** the builder that created this compoment */
            IComponentBuilder* _builder;
	};

}

}

#endif
