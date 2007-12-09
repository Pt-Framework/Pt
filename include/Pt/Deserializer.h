#ifndef Pt_Deserializer_h
#define Pt_Deserializer_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <map>
#include <list>

namespace Pt {

class PT_API Deserializer
{
    public:
        typedef void (*Fixup)(void**, void*);

        virtual ~Deserializer();

        /** @brief Deserialize an object

            This method will deserialize the object \a type from an
            XML format. The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            SerializationInfo& si = this->next();
            si >>= type;
            this->markFixup(si, &type, &Deserializer::do_fixup<T>);
        }

        SerializationInfo& peek();

        void fixup();

    protected:
        Deserializer();

        virtual void read(SerializationInfo& si) = 0;

    private:
        Pt::SerializationInfo& next();

        void markFixup(Pt::SerializationInfo& si, void* type, Fixup fixup);

        void fixup(const Pt::SerializationInfo& si);

    private:
        std::list<Pt::SerializationInfo> _stack;

        bool _peeking;

        std::map<std::string, void*> _objects;

        std::map<std::string, Fixup> _fixups;

        template <typename T>
        static void do_fixup(void** ref , void* val)
        {
            *( (T**)(ref) ) = (T*)(val);
        }
};

} // namespace Pt

#endif
