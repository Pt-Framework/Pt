#ifndef PT_CMA_TYPEID_H
#define PT_CMA_TYPEID_H

#include <string>


namespace Pt {

namespace Cma  {

    /**
    *  Class representing a type identifier.
    *
    *  Type identifier class that is used throughout the PTV COMA
    *  framework. It basically encapsulates a string.
    */
    class TypeId
    {
        public:
            /** Construct a %TypeId out of a string */
            TypeId(const char* typeName)
            {
                _typeName = typeName;
            }

            /** Construct a %TypeId out of a string */
            TypeId(const std::string& typeName)
            {
                _typeName = typeName;
            }

            /** Construct a %TypeId out of another %TypeId */
            TypeId(const TypeId& typeId)
            {
                _typeName = typeId._typeName;
            }

            /** Overloaded assignment operator */
            TypeId& operator=(const TypeId& typeId)
            {
                _typeName = typeId._typeName;
                return *this;
            }

            /** Overloaded "less than" operator */
            bool operator<(const TypeId& typeId) const
            {
            return _typeName < typeId._typeName;
            }

            /** Get the type ID as char array
            *  @return the ID as char array
            */
            const char* name() const
            {
                return _typeName.c_str();
            }

        private:
            std::string _typeName;
    };

}

}

#endif
