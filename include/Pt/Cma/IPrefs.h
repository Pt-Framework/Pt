#ifndef PT_CMA_IPREFS_H
#define PT_CMA_IPREFS_H

#include <Pt/Api.h>
#include <Pt/System/File.h>

#include <Pt/Cma/TypeId.h>
#include <Pt/Cma/IUnknown.h>


namespace Pt {

namespace Cma {

    class IPrefs : public IUnknown
    {
        public:
            virtual void loadPrefs(const Pt::System::File& file) = 0;

            virtual void savePrefs(const Pt::System::File& file) = 0;

            static TypeId typeId()
            {
                static TypeId _typeId("IPrefs");
                return _typeId;
            }

        protected:
            //! Default destructor
            virtual ~IPrefs(void)
            {}
    };

} // namespace coma

} // namespace ptv

#endif
