/***************************************************************************
 *   Copyright (C) 2006 by Tommi Maekitalo                                 *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Stefan Bueder                                   *
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

#ifndef PT_DB_VALUEIMPL_H
#define PT_DB_VALUEIMPL_H

#include <Pt/Blob.h>
#include <Pt/Db/IValue.h>
#include <Pt/Variant.h>


namespace Pt {

    class Date;
    class Time;
    class DateTime;

namespace Db {

    class ValueImpl : public IValue
    {
        private:
            bool _null;
            Variant _data;

        public:
            ValueImpl()
            : _null(true)
            { }

/*          ValueImpl(const std::string& d)
            : _null(false)
            , _data(d)
            { }
*/
            ValueImpl(const Variant& var)
            : _null(false)
            , _data(var)
            { }

            ValueImpl(const void* blob, int byteCount)
                : _null(false)
                , _data(blob, byteCount)
            { }

            virtual bool isNull() const;

            virtual void getData(Variant& v) const
            { v = _data; }

            virtual bool getBool() const;

            virtual int getInt() const;

            virtual unsigned getUnsigned() const;

            virtual float getFloat() const;

            virtual double getDouble() const;

            virtual char getChar() const;

            virtual void getString(std::string& stringdata) const;

            virtual Date getDate() const;

            virtual Time getTime() const;

            virtual DateTime getDateTime() const;

            static bool isTrue(char ch)
            {
                return ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y' || ch == '1';
            }

            //DoTo: extra blob function needed??? ->    alternative: read all text values with blob sql function
            //virtual void getBlob(std::string& blobdata) const;
            virtual void getBlob(Pt::Blob& blobdata) const;
    };

} // namespace Db

} // namepspace Pt

#endif // PT_DB_VALUEIMPL_H

