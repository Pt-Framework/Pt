/*
 * Copyright (C) 2008 by Marc Boris Duerner
 * 
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
#ifndef Pt_Formatter_h
#define Pt_Formatter_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Date.h>
#include <string>
#include <map>

namespace Pt {

class Formatter
{
    public:
        virtual ~Formatter()
        { }

        virtual void addValue(const std::string& name, const std::string& type,
                              const Pt::String& value, const std::string& id) = 0;

        virtual void addBool(const std::string& name, bool value,
                             const std::string& id) = 0;

        virtual void addInt(const std::string& name, long value,
                            const std::string& id) = 0;

        virtual void addUInt(const std::string& name, unsigned long value,
                             const std::string& id) = 0;

        virtual void addFloat(const std::string& name, double value,
                              const std::string& id) = 0;

        virtual void addReference(const std::string& name, const std::string& refId) = 0;

        virtual void beginArray(const std::string& name, const std::string& type,
                                const std::string& id) = 0;

        virtual void beginElement(const std::string& type, const std::string& id) = 0;

        virtual void finishElement() = 0;

        virtual void finishArray() = 0;

        virtual void beginObject(const std::string& name, const std::string& type,
                                 const std::string& id) = 0;

        virtual void beginMember(const std::string& name, const std::string& type,
                                 const std::string& id) = 0;

        virtual void finishMember() = 0;

        virtual void finishObject() = 0;

        //virtual void setSurrogate(const std::string& typeName, Serialize s);

        virtual void transform(SerializationInfo& si) const {}

        // virtual void addAny(const std::string& name, const std::string& type,
        //                     const Any& value, const std::string& id)
        // {
        //     if(type == "Pt::Date")
        //     {
        //         const Pt::Date& date = any_cast<const Pt::Date&>(value);
        //     }
        // }

    protected:
        Formatter()
        {}
};

class FormatterSurrogate
{
    public:
        virtual ~FormatterSurrogate() {}

        void begin(const std::string& name)
        { _name = name; }

        const std::string& name() const
        { return _name; }

        virtual void onBool(const std::string& name, bool value) = 0;

        virtual void onInt(const std::string& name, long value) = 0;

        virtual void onUInt(const std::string& name, unsigned long value) = 0;

        virtual void onFloat(const std::string& name, double value) = 0;

        virtual void onValue(const std::string& name, const Pt::String& value) = 0;

        virtual void format(Formatter& f) const = 0;

    private:
        std::string _name;
};

class DateAsIsoString : public FormatterSurrogate
{
    public:
        virtual void onInt(const std::string& name, long value)
        {
            if(name == "year")
                y = value;
            else if(name == "month")
                m = value;
            else
                d = value;
        }

        virtual void onUInt(const std::string& name, unsigned long value)
        {
            if(name == "year")
                y = value;
            else if(name == "month")
                m = value;
            else
                d = value;
        }

        virtual void format(Formatter& f) const
        {
            char ret[10];
            unsigned short n = y;

            ret[3] = '0' + n % 10;
            n /= 10;
            ret[2] = '0' + n % 10;
            n /= 10;
            ret[1] = '0' + n % 10;
            n /= 10;
            ret[0] = '0' + n % 10;
            ret[4] = '-';
            ret[5] = '0' + m / 10;
            ret[6] = '0' + m % 10;
            ret[7] = '-';
            ret[8] = '0' + d / 10;
            ret[9] = '0' + d % 10;
            //Pt::String value = Pt::String::widen( Pt::Date(y, m, d).toIsoString() );
            //f.addValue(this->name(), "date", value, "");
        }

    private:
        long y;
        long m;
        long d;
};

} // namespace Pt

#endif
