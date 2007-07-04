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
#ifndef PT_DB_ISTATEMENT_H
#define PT_DB_ISTATEMENT_H


#include <Pt/RefCounted.h>
#include <Pt/Db/Api.h>
#include <Pt/Blob.h>
#include <string>


namespace Pt {

    class Date;
    class Time;
    class DateTime;

namespace Db {

    class Result;
    class Row;
    class Value;
    class ICursor;

    class PT_DB_API IStatement : public RefCounted
    {
    public:
        typedef size_t size_type;

        virtual void clear() = 0;


        virtual void setNull(const std::string& col) = 0;
        virtual void setBool(const std::string& col, bool data) = 0;
        virtual void setInt(const std::string& col, int data) = 0;
        virtual void setUnsigned(const std::string& col, unsigned data) = 0;
        virtual void setFloat(const std::string& col, float data) = 0;
        virtual void setDouble(const std::string& col, double data) = 0;
        virtual void setChar(const std::string& col, char data) = 0;
        virtual void setString(const std::string& col, const std::string& data) = 0;
        virtual void setBlob(const std::string& col, const Pt::Blob& data) = 0;
        virtual void setDate(const std::string& col, const Date& data) = 0;
        virtual void setTime(const std::string& col, const Time& data) = 0;
        virtual void setDatetime(const std::string& col, const DateTime& data) = 0;

        virtual size_type execute() = 0;
        virtual Result select() = 0;
        virtual Row selectRow() = 0;
        virtual Value selectValue() = 0;
        virtual ICursor* createCursor() = 0;
    };

} // namespace Db

} // namespace Pt

#endif // PTV_DB_ISTATEMENT_H

