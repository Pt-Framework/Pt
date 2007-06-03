#ifndef PT_CMA_COMPONENTBUILDER_H
#define PT_CMA_COMPONENTBUILDER_H

#include <Pt/Api.h>
#include <Pt/Cma/TypeId.h>
#include <Pt/Cma/IUnknown.h>
#include <Pt/Cma/Component.h>
#include <Pt/Cma/IComponentBuilder.h>


namespace Pt {

namespace Cma {

/** Interface Builder.
 \param ComponentT Type of the Component the builder constructs.

 To achieve maximum exchangeability components are distributed as
 shared libraries but can also be part of a linked unit (static library).
 Components are not created as such, but rather by a special
 builder object, which is exported from a shared library.
 The implementer of a component instantiates  CComponentBuilder for his
 type of component for instance statically in the component shared library.
 An array of IComponentBuilder pointer is exported as shared library
 C symbol. This array must be named Ptv_ComponentList.

 !Example:
 \code
 static ComponentBuilder<TVComponent> tvComponent;

 extern "C"
 {
  IComponentBuilder* Ptv_ComponentList[] =
  {
 	&tv2Component, 0
  };
 }

\endcode

This array is 0-terminated and can contain an arbitrary number of ComponentBuilder.

*/
template<typename ComponentT>
class PT_EXPORT ComponentBuilder : public IComponentBuilder
{
    public:
        /** Constructor. */
        ComponentBuilder()
        :IComponentBuilder( ComponentT::typeId() )
        { }

        /** creates a component
        \return the interface object of the component
        */
        IUnknown* createComponent()
        {
            ++_refCount;
            /* create a new Component class that is derived from the
            * component implementation class
            */
            Component<ComponentT>* p = new Component<ComponentT>(this);
            /* now get the implementation class */
            return p->getComponent();
        }
};

}

}

#endif
