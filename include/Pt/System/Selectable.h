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

class PT_SYSTEM_API Selectable : protected NonCopyable
{
    public:
        // TODO: use size_t instread of unsigned
        static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

        enum State
        {
            Idle = 0,
            Busy = 1,
            Avail = 2
        };

    public:
        //! @brief Destructor
        virtual ~Selectable();

        void setSelector(SelectorBase* parent);

        SelectorBase* selector();

        const SelectorBase* selector() const;

        //! @brief Closes the I/O device
        /*!
           Frees any resources associated with this object, like I/O handles.
        */
        void close();

        bool wait(unsigned int msecs = WaitInfinite);

        //! @brief Test if the I/O device object is enabled
        /*!
            Test if the I/O device object is enabled i.e. open and ready
            to perform I/O operations

            \return true if the I/O device is usable, false otherwise.
        */
        bool enabled() const;

        bool idle() const;

        bool busy() const;

        bool avail() const;

        virtual SelectableImpl& simpl() = 0;

    protected:
        //! @brief Default Constructor
        Selectable();

        //! @brief Sets or unsets the device enabled
        void setEnabled(bool isEnabled);

        void setState(State state);

        //! @brief Closes the Selector
        virtual void onClose() = 0;

        virtual bool onWait(unsigned int msecs) = 0;

        virtual void onAttach(SelectorBase&) = 0;

        virtual void onDetach(SelectorBase&) = 0;

    private:
        SelectorBase* _parent;
        bool _enabled;
        State _state;
};

} // namespace System

} // namespace Pt

#endif
