#ifndef PT_SELECTABLE_H
#define PT_SELECTABLE_H

#include <Pt/Types.h>
#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/Selector.h>
#include <limits>

namespace Pt {

namespace System {

class SelectableImpl;

class Selectable : protected NonCopyable
{
    public:
        static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

    public:
        //! @brief Destructor
        virtual ~Selectable()
        {
            try
            {
                if(_parent)
                {
                    _parent->onRemove(*this);
                }
            }
            catch(...) {}
        }

        void setSelector(SelectorBase* parent)
        {
            if(_parent)
            {
                _parent->onRemove(*this);
                this->onDetach(*_parent);
            }

            if(parent)
            {
                parent->onAdd(*this);
                this->onAttach(*parent);
            }

            _parent = parent;
        }

        SelectorBase* selector()
        { return _parent; }

        const SelectorBase* selector() const
        { return _parent; }

        //! @brief Closes the I/O device
        /*!
           Frees any resources associated with this object, like I/O handles.
        */
        void close()
        {
            if( this->enabled() )
            {
                this->setEnabled(false);
                this->onClose();
            }
        }

        bool wait(unsigned int msecs = WaitInfinite)
        { return this->onWait(msecs); }

        //! @brief Test if the I/O device object is enabled
        /*!
            Test if the I/O device object is enabled i.e. open and ready
            to perform I/O operations

            \return true if the I/O device is usable, false otherwise.
        */
        bool enabled() const
        { return _enabled; }

        virtual SelectableImpl& simpl() = 0;

    protected:
        //! @brief Default Constructor
        Selectable()
        : _parent(0)
        , _enabled(false)
        { }

        //! @brief Sets or unsets the device enabled
        void setEnabled(bool isEnabled)
        {
            if(_parent)
            {
                if(isEnabled)
                    _parent->onEnabled(*this);
                else
                    _parent->onDisabled(*this);
            }
            _enabled = isEnabled;
        }

        //! @brief Closes the Selector
        virtual void onClose() = 0;

        virtual bool onWait(unsigned int msecs) = 0;

        virtual void onAttach(SelectorBase&) = 0;

        virtual void onDetach(SelectorBase&) = 0;

    private:
        SelectorBase* _parent;
        bool _enabled;
};

} // namespace System

} // namespace Pt

#endif
