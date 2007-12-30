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
        typedef void (*Fixup)(void**, const std::type_info&, void*);

        virtual ~Deserializer();

        /** @brief Deserialize an object

            This method will deserialize the object \a type from an
            XML format. The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            SerializationInfo& si = this->get();
            si >>= type;
            this->markFixup(si, &type, &Deserializer::do_fixup<T>);
        }

        SerializationInfo& peek();

        void finish();

    protected:
        Deserializer();

        virtual void read(SerializationInfo& si) = 0;

    private:
        Pt::SerializationInfo& get();

        void markFixup(Pt::SerializationInfo& si, void* type, Fixup fixup);

        void fixup(const Pt::SerializationInfo& si);

    private:
        std::list<Pt::SerializationInfo> _stack;

        bool _peeking;

        std::map<std::string, void*> _objects;

        std::map<std::string, Fixup> _fixups;

        template <typename T>
        static void do_fixup(void** fixme, const std::type_info& fixmeInfo , void* obj)
        {
            if( fixmeInfo != typeid(T) )
                throw SerializationError("reference fixup failed, type mismatch", PT_SOURCEINFO);

            *( (T**)(fixme) ) = (T*)(obj);
        }
};

} // namespace Pt

#endif
