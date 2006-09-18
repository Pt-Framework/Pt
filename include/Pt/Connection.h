#ifndef Pt_Connection_h
#define Pt_Connection_h

#include <Pt/Api.h>
#include <Pt/Slot.h>
#include <Pt/Shared.h>
#include <Pt/SmartPtr.h>


namespace Pt {

	class Connectable;


	class PT_EXPORT ConnectionData : public Shared {
		public:
			ConnectionData(Connectable& sender, Slot* slot)
			: _refs(1)
			, _valid(true)
			, _slot(slot)
			, _sender(&sender)
			{ }

			~ConnectionData()
			{ delete _slot; }

			size_t ref()
			{ return ++_refs; }

			size_t unref()
			{ return --_refs; }

			size_t refs() const
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
			size_t _refs;
			bool _valid;
			Slot* _slot;
			Connectable* _sender;
	};


	class PT_EXPORT Connection
    {
		public:
			Connection(Connectable& sender, Slot* slot);

			Connection(const Connection& connection);

			~Connection();

			bool valid() const
			{ return _data->valid(); }

			const Connectable& sender() const
			{ return _data->sender(); }

			const Slot& slot() const
			{ return _data->slot(); }

			void close();

			Connection& operator=(const Connection& connection);

			bool operator==(const Connection& connection) const;

        private:
			ConnectionData* _data;
	};

} // namespace Pt

#endif
