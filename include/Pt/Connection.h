#ifndef Pt_Connection_h
#define Pt_Connection_h

#include <Pt/Api.h>
#include <Pt/Slot.h>
#include <Pt/Atomicity.h>
#include <Pt/RefCounted.h>



namespace Pt {

    class Connectable;


    class PT_API ConnectionData : public RefCounted {
        public:
            ConnectionData()
            : _refs(1)
            , _valid(false)
            , _slot(0)
            , _sender(0)
            { }

            ConnectionData(Connectable& sender, Slot* slot)
            : _refs(1)
            , _valid(true)
            , _slot(slot)
            , _sender(&sender)
            { }

            ~ConnectionData()
            { delete _slot; }

            atomic_t ref()
            { return atomicIncrement(_refs); }

            atomic_t unref()
            { return atomicDecrement(_refs); }

            atomic_t refs() const
            { return _refs; }

            bool valid() const
            { return _valid; }

            void setValid(bool valid)
            { _valid = valid; }

            Connectable& sender()
            { return *_sender; }

            const Connectable& sender() const
            { return *_sender; }

            Slot& slot()
            { return *_slot; }

            const Slot& slot() const
            { return *_slot; }

        private:
            atomic_t _refs;
            bool _valid;
            Slot* _slot;
            Connectable* _sender;
    };


    class PT_API Connection
    {
        public:
            Connection();

            Connection(Connectable& sender, Slot* slot);

            Connection(const Connection& connection);

            ~Connection();

            bool valid() const
            { return _data->valid(); }

            const Connectable& sender() const
            { return _data->sender(); }

            const Slot& slot() const
            { return _data->slot(); }

            bool operator!() const
            { return this->valid() == false; }

            void close();

            Connection& operator=(const Connection& connection);

            bool operator==(const Connection& connection) const;

        private:
            ConnectionData* _data;
    };

} // namespace Pt

#endif
