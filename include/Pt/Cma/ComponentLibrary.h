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
#ifndef PT_CMA_COMPONENTLIBRARY_H
#define PT_CMA_COMPONENTLIBRARY_H

#include <Pt/Cma/Api.h>
#include <Pt/Cma/IComponentBuilder.h>
#include <Pt/System/Library.h>
#include <Pt/System/Path.h>
#include <map>
#include <string>
#include <cstddef>

namespace Pt {

namespace Cma {

/**
  *  Class to handle dynamic libraries that contain components.
  */
class PT_EXPORT ComponentLibrary : protected Pt::System::Library
{
    public:
        typedef std::map<TypeId,IComponentBuilder*> BuilderMap;

        /**
          * Constructor
          * @param file the name of the dynamic library
          */
        ComponentLibrary(const System::Path& file);

        /**
          * Test whether this library is used (referenced) or not.
          * @return true if library is used, false otherwise
          */
        bool isUsed();

        /**
          * Get the IComponentBuilder of the specified component.
          * @return the found %ComponentBuilder, 0 if none found.
          */
        IComponentBuilder* getBuilder(const TypeId& typeId);

        /**
          * Get the IComponentBuilder of the first component.
          * @return the found %ComponentBuilder, 0 if none found.
          */
        IComponentBuilder* getBuilder();

        /**
          * Get the number of component builders in this library
          * @return the number of IComponentBuilder
          */
        std::size_t size() const;

        /**
          * Get the library file.
          * @return the library file.
          */
        const System::Path& path() const;

    protected:
        BuilderMap _builders;
};

}

}

#endif
