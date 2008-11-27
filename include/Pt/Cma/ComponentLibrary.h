#ifndef PT_CMA_COMPONENTLIBRARY_H
#define PT_CMA_COMPONENTLIBRARY_H

#include <Pt/Api.h>
#include <Pt/System/Library.h>
#include <Pt/Cma/IComponentBuilder.h>
#include <map>
#include <string>

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
             * @param library the name of the dynamic library
             */
            ComponentLibrary(const std::string& file);

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
            size_t size() const;

            /**
             * Get the library file.
             * @return the library file.
             */
            const std::string& path() const;

        protected:
            BuilderMap _builders;
    };

}

}

#endif
