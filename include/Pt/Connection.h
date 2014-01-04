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
#ifndef Pt_Connection_h
#define Pt_Connection_h

#include <Pt/Api.h>
#include <Pt/Slot.h>
#include <Pt/NonCopyable.h>

namespace Pt {

class Connectable;
class Slot;

//! @internal
class ConnectionData 
{
    public:
        ConnectionData(Connectable& sender, Slot* slot)
        : _refs(1)
        , _valid(true)
        , _slot(slot)
        , _sender(&sender)
        { }

        ~ConnectionData()
        { delete _slot; }

        unsigned ref()
        { return ++_refs; }

        unsigned unref()
        { return --_refs; }

        unsigned refs() const
        { return _refs; }

        bool isValid() const
        { return _valid; }

        void setInvalid()
        { _valid = false; }

        Connectable* sender()
        { return _sender; }

        const Connectable* sender() const
        { return _sender; }

        Slot* slot()
        { return _slot; }

        const Slot* slot() const
        { return _slot; }

    private:
        unsigned _refs;
        bool _valid;
        Slot* _slot;
        Connectable* _sender;
};

/** @brief Represents a connection between a Signal/Delegate and a slot.

    @ingroup sigslot
*/
class PT_API Connection
{
    public:
        Connection();

        Connection(Connectable& sender, Slot* slot);

        Connection(const Connection& connection);

        ~Connection();

        bool isValid() const
        { return _data && _data->isValid(); }

        const Connectable* sender() const
        { return _data ? _data->sender() : 0; }

        const Slot* slot() const
        { return _data ? _data->slot() : 0; }

        bool operator!() const
        { return this->isValid() == false; }

        void close();

        Connection& operator=(const Connection& connection);

        //! @internal @brief Only for std::list::remove.
        bool operator==(const Connection& connection) const
        { return _data == connection._data; }

    private:
        ConnectionData* _data;
};

} // namespace Pt

#endif
