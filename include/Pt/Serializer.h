#ifndef Pt_Serializer_h
#define Pt_Serializer_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <map>
#include <list>

namespace Pt {

class PT_API Serializer
{
    public:
        virtual ~Serializer();

        /** @brief Serialize an object to XML

            The serializer will serialize the object \a type as
            XML to the assigned stream. The string \a name will be used
            as the instance name of \a type and appear as the name of the
            XML element. The type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const std::string& name)
        {
            SerializationInfo& si = this->push(&type);
            si.setName(name);
            si <<= type;
        }

        /** @brief Serialize objects to XML

            Writes all serialized objects.
        */
        void finish();

    protected:
        Serializer();

        virtual void write(const SerializationInfo& si) = 0;

    private:
        SerializationInfo& push(const void* obj);

        //! @internal
        void fixdown(Pt::SerializationInfo& si);

        //! @internal
        std::list<Pt::SerializationInfo> _stack;

        //! @internal
        std::map<const void*, Pt::SerializationInfo*> _objects;
};

} // namespace Pt

#endif
