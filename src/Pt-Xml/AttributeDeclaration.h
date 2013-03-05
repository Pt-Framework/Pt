/*
 * Copyright (C) 2012 by Marc Boris Duerner
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
#ifndef Pt_Xml_AttributeDeclaration_h
#define Pt_Xml_AttributeDeclaration_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <set>

namespace Pt {

namespace Xml {

class DocTypeValidator;

class AttributeDeclaration
{
    public:
        enum Mode
        {
            Required = 0,
            Implied = 1,
            Fixed = 2,
            Default = 3
        };

    public:
        AttributeDeclaration()
        : _mode(Default)
        {}

        virtual ~AttributeDeclaration()
        { }

        void setMode(Mode mode)
        { _mode = mode; }

        Mode mode() const
        { return _mode; }

        const Pt::String& name() const
        { return _name; }

        void setName(const Pt::String& name)
        { _name = name; }

        void setDefaultValue(const Pt::String& def)
        { _default = def; }

        const Pt::String& defaultValue() const
        { return _default; }

        bool validate(const Attribute& attr) const
        {
            if(mode() == Fixed && attr.value() != defaultValue() )
                return false;

            return onValidate(attr);
        }
      
        bool fixup(AttributeList& list) const
        {
            switch(_mode)
            {
                case Required:
                    return false;

                case Implied:
                    return true;

                case Fixed:
                case Default:
                    break;
            };

            Attribute attr;
            attr.setName(_name);
            attr.setValue(_default);
            
            bool valid = onValidate(attr);
            if(valid)
                list.add(attr);
            
            return valid;
        }

    protected:
        virtual bool onValidate(const Attribute& attr) const = 0;

    private:
        Mode _mode;
        Pt::String _name;
        Pt::String _default;
};


class CDataAttributeDeclaration : public AttributeDeclaration
{
    public:
        CDataAttributeDeclaration()
        : AttributeDeclaration()
        {}

        virtual bool onValidate(const Attribute& attr) const
        { 
            // TODO: check for non-CDATA characters in value           
            return true; 
        }
};


class NMTokenAttributeDeclaration : public AttributeDeclaration
{
    public:
        NMTokenAttributeDeclaration()
        : AttributeDeclaration()
        {}

        virtual bool onValidate(const Attribute& attr) const
        { 
            // TODO: check for non-CDATA characters in value           
            return true; 
        }
};


class NMTokensAttributeDeclaration : public AttributeDeclaration
{
    public:
        NMTokensAttributeDeclaration()
        : AttributeDeclaration()
        {}

        virtual bool onValidate(const Attribute& attr) const
        { 
            // TODO: check for non-CDATA characters in value           
            return true; 
        }
};


class EnumAttributeDeclaration : public AttributeDeclaration
{
    public:
        EnumAttributeDeclaration()
        : AttributeDeclaration()
        {}

        void addValue(const Pt::String& value)
        {
            _enumValues.insert(value);
        }

    protected:
        virtual bool onValidate(const Attribute& attr) const;

    private:
        std::set<Pt::String> _enumValues;
};


class IDAttributeDeclaration : public AttributeDeclaration
                             , private NonCopyable
{
    public:
        // TODO: pass DocTypeIdContext
        IDAttributeDeclaration(DocTypeValidator& validator)
        : AttributeDeclaration()
        , _validator(&validator)
        {}

        virtual bool onValidate(const Attribute& attr) const;

    private:
        DocTypeValidator* _validator;
};


class IDRefAttributeDeclaration : public AttributeDeclaration
                                , private NonCopyable
{
    public:
        IDRefAttributeDeclaration(DocTypeValidator& validator)
        : AttributeDeclaration()
        , _validator(&validator)
        {}

        virtual bool onValidate(const Attribute& attr) const;

    private:
        DocTypeValidator* _validator;
};


class IDRefsAttributeDeclaration : public AttributeDeclaration
                                 , private NonCopyable
{
    public:
        IDRefsAttributeDeclaration(DocTypeValidator& validator)
        : AttributeDeclaration()
        , _validator(&validator)
        {}

        virtual bool onValidate(const Attribute& attr) const;

    private:
        DocTypeValidator* _validator;
};

} // namespace Xml

} // namespace Pt

#endif
