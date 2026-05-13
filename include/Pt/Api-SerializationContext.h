#ifndef PT_API_SERIALIZATIONCONTEXT_H
#define PT_API_SERIALIZATIONCONTEXT_H

#include <Pt/Api.h>
#include <Pt/TypeInfo.h>
#include <Pt/FixupInfo.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationSurrogate.h>
#include <typeinfo>
#include <string>

namespace Pt {

class SerializationContextImpl;

/** @brief Context for the serialization of types.

    @headerfile SerializationContext.h <Pt/SerializationContext.h>
    @ingroup Serialization
*/
class PT_API SerializationContext
{
    public:
        //! @brief Default Constructor.
        SerializationContext();

        /** @brief Destructor.
        */
        virtual ~SerializationContext();

        //! @brief Enables recording of references.
        inline void enableReferencing(bool enabled)
        { _refsEnabled = enabled; }

        //! @brief Returns true if references are recorded.
        inline bool isReferencing() const
        { return _refsEnabled; }

        //! @brief Gets a %SerializationInfo object from the pool.
        SerializationInfo* get();

        //! @brief Pushes a %SerializationInfo object back on the pool.
        void push(SerializationInfo* si);

        //! @brief Register a serialization surrogate function pair.
        template <typename T>
        void registerSurrogate( const std::string& typeName,
                                void (*compose)(const Pt::SerializationInfo& si, T& type),
                                void (*decompose)(Pt::SerializationInfo& si, const T& type) );

        //! @brief Register a serialization surrogate function pair.
        template <typename T>
        void registerSurrogate( const char* typeName,
                                void (*compose)(const Pt::SerializationInfo& si, T& type),
                                void (*decompose)(Pt::SerializationInfo& si, const T& type) );

        //! @brief Returns true if the type could be composed with a surrogate.
        template <typename T>
        bool compose(const SerializationInfo& si, T& type) const;

        //! @brief Returns true if the type could be decomposed with a surrogate.
        template <typename T>
        bool decompose(SerializationInfo& si, const T& type);

        //! @brief Find a surrogate for a type.
        template <typename T>
        const BasicSerializationSurrogate<T>* getSurrogate() const;

    public:
        /** @brief Clears all content.
        */
        void clear()
        { onClear(); }

        /** @brief Begin saving a type.
        */
        bool beginSave(const void* p, const char* name)
        { return onBeginSave(p, name); }

        /** @brief Finish saving a type.
        */
        void finishSave()
        { onFinishSave(); }

        /** @brief Mark a type as being referenced.
        */
        void prepareId(const void* p)
        { onPrepareId(p); }

        /** @brief Get the ID for a reference.
        */
        const char* getId(const void* p)
        { return onGetId(p); }

        /** @brief Create an ID for a type, if referenced.
        */
        const char* makeId(const void* p)
        { return onMakeId(p); }

    public:     
        /** @brief Begin loading a type.
        */
        void beginLoad(void* obj, const std::type_info& ti,
                       const char* name, const char* id)
        { onBeginLoad(obj, ti, name, id); }
        
        /** @brief Finish loading a type.
        */
        void finishLoad()
        { onFinishLoad(); }

        /** @brief Rebind a ID to a referenced type.
        */
        void rebindTarget(const char* id, void* obj)
        { onRebindTarget(id, obj); }

        /** @brief Rebind reference to fixup for a ID.
        */
        void rebindFixup(const char* id, void* obj, void* prev)
        { onRebindFixup(id, obj, prev); }

        /** @brief Prepare a reference to fixup for an ID.
        */
        void prepareFixup(void* obj, const char* id, FixupInfo::FixupHandler fh, unsigned mid)
        { onPrepareFixup(obj, id, fh, mid); }

        /** @brief Fixup all prepared references to their targets.
        */
        void fixup()
        { onFixup(); }

    protected:
        /** @brief Clears all content.
        */
        virtual void onClear();

        /** @brief Begin saving a type.
        */
        virtual bool onBeginSave(const void* p, const char* name);

        /** @brief Finish saving a type.
        */
        virtual void onFinishSave();

        /** @brief Mark a type as being referenced.
        */
        virtual void onPrepareId(const void* p);

        /** @brief Get the ID for a reference.
        */
        virtual const char* onGetId(const void* p);

        /** @brief Create an ID for a type, if referenced.
        */
        virtual const char* onMakeId(const void* p);

        /** @brief Begin loading a type.
        */
        virtual void onBeginLoad(void* obj, const std::type_info& fixupInfo,
                                 const char* name, const char* id);

        /** @brief Finish loading a type.
        */
        virtual void onFinishLoad();

        /** @brief Rebind a ID to a referenced type.
        */
        virtual void onRebindTarget(const char* id, void* obj);

        /** @brief Rebind reference to fixup for a ID.
        */
        virtual void onRebindFixup(const char* id, void* obj, void* prev);

        /** @brief Prepare a reference to fixup for an ID.
        */
        virtual void onPrepareFixup(void* obj, const char* id, FixupInfo::FixupHandler fh, unsigned mid);

        /** @brief Fixup all prepared references to their targets.
        */
        virtual void onFixup();

    private:
        //! @internal
        void registerSurrogate(const std::type_info& ti, SerializationSurrogate* surrogate);

        //! @internal
        SerializationContext(const SerializationContext& si)
        {}

        //! @internal
        SerializationContext& operator=(const SerializationContext& si)
        { return *this; }

    private:
        SerializationContextImpl* _cache;
        bool _refsEnabled;
};

} // namespace Pt

#endif
