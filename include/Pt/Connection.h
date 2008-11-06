#ifndef Pt_Connection_h
#define Pt_Connection_h

#include <Pt/Api.h>
#include <Pt/Slot.h>
#include <Pt/RefCounted.h>

namespace Pt {

    class Connectable;
	class Connection;

    /** @internal
    */
    class ConnectionData {
        public:
            ConnectionData()
            : _refs(1)
            , _valid(false)
            , _slot(0)
            , _sender(0)
            { }

            ConnectionData(Connection& c, Connectable& sender, Slot* slot);

            ~ConnectionData()
            { delete _slot; }

            unsigned ref()
            { return ++_refs; }

            unsigned unref()
            { return --_refs; }

            unsigned refs() const
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
            unsigned _refs;
            bool _valid;
            Slot* _slot;
            Connectable* _sender;
    };

    /** @brief Represents a connection between a Signal/Delegate and a slot
        @ingroup sigslot
    */
    class Connection
    {
        public:
            Connection()
            {
                _data = new ConnectionData();
            }

            Connection(Connectable& sender, Slot* slot);

            Connection(const Connection& connection)
            {
                _data = connection._data;
                _data->ref();
            }

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

            bool operator==(const Connection& connection) const
            {
                return _data == connection._data;
            }

        private:
            ConnectionData* _data;
    };
	
inline Connection::Connection(Connectable& sender, Slot* slot)
: _data(0)
{
    _data = new ConnectionData(*this, sender, slot);
}
	
inline Connection::~Connection()
{
    if( _data->unref() > 0) {
        return;
    }

    // close the connection if its still valid
    if( this->valid() ) {
        this->close();
    }

    // delete the shared data
    delete _data;
    _data = 0;
}





inline Connection& Connection::operator=(const Connection& connection)
{
    if( 0 == _data->unref()) 
    {
        this->close();
        delete _data;
    }

    _data = connection._data;
    _data->ref();
    return (*this);
}
} // namespace Pt

#endif
