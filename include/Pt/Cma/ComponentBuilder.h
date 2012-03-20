/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
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
