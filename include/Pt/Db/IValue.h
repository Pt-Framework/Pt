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

#ifndef PT_DB_IVALUE_H
#define PT_DB_IVALUE_H

#include <string>
#include <Pt/RefCounted.h>
#include <Pt/Blob.h>
#include <Pt/NonCopyable.h>
#include <Pt/Db/Api.h>


namespace Pt {

    class Date;
    class Time;
    class DateTime;
    class Variant;

namespace Db {

    /** \brief Interface for DB Values
        \see Db::Value
    */
    class PT_DB_API IValue : public RefCounted, private NonCopyable
    {
        public:
            virtual bool isNull() const = 0;
            virtual bool getBool() const = 0;
            virtual int getInt() const = 0;
            virtual unsigned getUnsigned() const = 0;
            virtual float getFloat() const = 0;
            virtual double getDouble() const = 0;
            virtual char getChar() const = 0;
            virtual void getString(std::string& stringdata) const = 0;
            virtual Date getDate() const = 0;
            virtual Time getTime() const = 0;
            virtual DateTime getDateTime() const = 0;

            //TODO: extra blob function needed??? ->    alternative: read all text values with blob sql function
            //virtual void getBlob(std::string& blobdata) const = 0;
            virtual void getBlob(Pt::Blob& blobdata) const = 0;
            virtual void getData(Pt::Variant& data) const = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_IVALUE_H

