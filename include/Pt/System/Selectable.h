#ifndef PT_SELECTABLE_H
#define PT_SELECTABLE_H

#include <Pt/Types.h>
#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/IOMonitor.h>
#include <limits>

namespace Pt {

class IOMonitor;
class SelectableImpl;

class Selectable : protected NonCopyable
{
    friend class IOMonitor;

    public:
        static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

    public:
        //! @brief Destructor
        virtual ~Selectable()
        {
            try
            {
                destroyed.send(*this);
            }
            catch(...) {}
        }

        void setParent(IOMonitor* parent)
        { _parent = parent; }

        IOMonitor* parent()
        { return _parent; }

        const IOMonitor* parent() const
        { return _parent; }

        //! @brief Closes the I/O device
        /*!
           Frees any resources associated with this object, like I/O handles.
        */
        void close()
        {
            if( this->valid() )
            {
                this->onClose();
                _valid = false;
                closed.send(*this);
            }
        }

        bool wait(unsigned int msecs = WaitInfinite)
        { return this->onWait(msecs); }

        //! @brief Test if the I/O device object is valid
        /*!
            Test if the I/O device object is valid i.e. open and ready
            to perform I/O operations

            \return true if the I/O device is usable, false otherwise.
        */
        bool valid() const
        { return _valid; }

        Signal<Selectable&> closed;

        Signal<Selectable&> destroyed;

        virtual SelectableImpl& simpl() = 0;

    protected:
        //! @brief Default Constructor
        Selectable()
        : _parent(0)
        , _valid(false)
        { }

        //! @brief Closes the Selector
        virtual void onClose() = 0;

        virtual bool onWait(unsigned int msecs) = 0;

        //! @brief Sets or unsets the device invalid
        void setValid(bool v)
        {
            if(_parent)
            {
                _parent->setDirty(*this);
            }
            _valid = v;
        }

        virtual void onAdd(IOMonitor&)
        {}

        virtual void onRemove(IOMonitor&)
        {}

    private:
        IOMonitor* _parent;
        bool _valid;
};

} // namespace Pt

#endif
