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
#ifndef Pt_Xml_AttributeModel_h
#define Pt_Xml_AttributeModel_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/QName.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <set>
#include <vector>

namespace Pt {

namespace Xml {

class DocTypeDefinition;
class AttributeListModel;

class AttributeValidator : private NonCopyable
{
    public:
        AttributeValidator();

        void reset();

        bool validate(AttributeList& attrs, const AttributeListModel& decl);

        bool isValid() const;

        bool addId(const Pt::String& id);

        void addRef(const Pt::String& id);

    private:
        std::set<Pt::String> _ids;
        std::vector<Pt::String> _idrefs;
};

class AttributeModel
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
        AttributeModel(bool normalize)
        : _mode(Default)
        , _normalize(normalize)
        {}

        virtual ~AttributeModel()
        { }

        bool isNormalize() const
        { return _normalize; }

        void setMode(Mode mode)
        { _mode = mode; }

        Mode mode() const
        { return _mode; }

        const QName& qname() const
        { return _qname; }

        void setName(const QName& name)
        { _qname = name; }

        void setDefaultValue(const Pt::String& def)
        { _default = def; }

        const Pt::String& defaultValue() const
        { return _default; }

        bool validate(AttributeValidator& validator, const Attribute& attr) const;
      
        bool fixup(AttributeValidator& validator, AttributeList& list) const;

    protected:
        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const = 0;

    private:
        Mode _mode;
        bool _normalize;
        QName _qname;
        Pt::String _default;
};


class CDataAttributeModel : public AttributeModel
{
    public:
        CDataAttributeModel()
        : AttributeModel(false)
        {}

    protected:
        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;

        virtual void onNormalize(Attribute& attr) const;
};


class NMTokenAttributeModel : public AttributeModel
{
    public:
        NMTokenAttributeModel()
        : AttributeModel(true)
        {}

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;
};


class NMTokensAttributeModel : public AttributeModel
{
    public:
        NMTokensAttributeModel()
        : AttributeModel(true)
        {}

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;
};


class EnumAttributeModel : public AttributeModel
{
    public:
        EnumAttributeModel()
        : AttributeModel(true)
        {}

        void addValue(const Pt::String& value)
        {
            _enumValues.insert(value);
        }

    protected:
        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;

    private:
        std::set<Pt::String> _enumValues;
};


class IDAttributeModel : public AttributeModel
                             , private NonCopyable
{
    public:
        IDAttributeModel()
        : AttributeModel(true)
        {}

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;
};


class IDRefAttributeModel : public AttributeModel
                                , private NonCopyable
{
    public:
        IDRefAttributeModel()
        : AttributeModel(true)
        {}

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;
};


class IDRefsAttributeModel : public AttributeModel
                                 , private NonCopyable
{
    public:
        IDRefsAttributeModel()
        : AttributeModel(true)
        {}

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;
};


class EntityAttributeModel : public AttributeModel
                                 , private NonCopyable
{
    public:
        EntityAttributeModel(const DocTypeDefinition& dtd)
        : AttributeModel(true)
        , _dtd(&dtd)
        {}

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;

    private:
        const DocTypeDefinition* _dtd;
};


class EntitiesAttributeModel : public AttributeModel
                                   , private NonCopyable
{
    public:
        EntitiesAttributeModel(const DocTypeDefinition& dtd)
        : AttributeModel(true)
        , _dtd(&dtd)
        {}

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;

    private:
        const DocTypeDefinition* _dtd;
};


class NotationAttributeModel : public AttributeModel
                                   , private NonCopyable
{
    public:
        NotationAttributeModel(const DocTypeDefinition& dtd)
        : AttributeModel(true)
        , _dtd(&dtd)
        {}

        void addNotation(const Pt::String& notation)
        {
            _notations.insert(notation);
        }

        virtual bool onValidate(AttributeValidator& validator, const Attribute& attr) const;

    private:
        const DocTypeDefinition* _dtd;
        std::set<Pt::String> _notations;
};

} // namespace Xml

} // namespace Pt

#endif
