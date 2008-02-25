/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_System_IOResult_h
#define Pt_System_IOResult_h

#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/Selector.h>

namespace Pt {

namespace System {

    template <typename CharT>
    class BasicIODevice;
    class Selector;
    class IOResultImpl;

    //! Handle for asynchronous I/O operations
    class IOResult : protected NonCopyable
    {
        public:
            static const unsigned int WaitInfinite = static_cast<const unsigned int>(-1);

            //! @brief Default constructor
            IOResult()
            : _device(0)
            , _selector(0)
            {}

            //! @brief Removes this object from its selector if it was added.
            virtual ~IOResult()
            {
                if(_selector)
                {
                    _selector->remove(*this);
                    _selector = 0;
                }
            }

            //! @brief Returns the parent IODevice or 0 if it has none
            BasicIODevice<char>* device() const
            { return _device; }

            //! @brief Sets the parent IODevice
            void setDevice(BasicIODevice<char>* device)
            {
                _device = device;
            }

            //! @brief Returns the parent selector or 0 if it has none
            Selector* selector() const
            { return _selector; }

            //! @brief Sets the parent selector
            void setSelector(Selector* selector)
            {
                _selector = selector;
            }

            //! @brief Wait for the result to complete
            bool wait(unsigned int msecs = WaitInfinite)
            {
                return this->_wait(msecs);
            }

            //! @brief Returns the OS specific implementation
            virtual IOResultImpl* impl() = 0;

        protected:
            //! @internal
            virtual bool _wait(unsigned int msecs) = 0;

        private:
            //! @internal
            BasicIODevice<char>* _device;

            //! @internal
            Selector* _selector;
    };

} // namespace System

} // namespace Pt

#endif
