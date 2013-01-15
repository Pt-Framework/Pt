/*
 * Copyright (C) 2012 Marc Boris Duerner
 *
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
#ifndef Pt_Xml_DocType_h
#define Pt_Xml_DocType_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/String.h>
#include <vector>
#include <map>
#include <cassert>

namespace Pt {

namespace Xml {

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

        bool match(const Attribute& attr) const
        {
            if(mode() == Fixed)
                return attr.value() == defaultValue();

            return onMatch(attr);
        }
      
        bool validate(AttributeList& list) const
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
            list.add(attr);
            return true;
        }

    protected:
        virtual bool onMatch(const Attribute& attr) const = 0;

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

        virtual bool onMatch(const Attribute& attr) const
        { 
            // TODO: check for non-CDATA characters in value           
            return true; 
        }
};


class AttributeListDeclaration
{
    typedef std::vector<AttributeDeclaration*> AttrDecls;

    public:
        AttributeListDeclaration()
        {}

        ~AttributeListDeclaration()
        {
            AttrDecls::iterator it;
            for(it = _attrs.begin(); it != _attrs.end(); ++it)
            {
                delete *it;
            }
        }

        AttributeDeclaration& last()
        {
            return *_attrs.back();
        }

        void push(AttributeDeclaration* decl)
        { _attrs.push_back(decl); }

        bool validate(AttributeList& attrs) const
        {
            std::vector<AttributeDeclaration*> attrDecls = _attrs;

            //
            // match attributes against declarations, remove declarations
            // that match an attribute
            //
            AttributeList::ConstIterator attr;
            for(attr = attrs.begin(); attr != attrs.end(); ++attr)
            {
                AttrDecls::iterator it;
                 
                for(it = attrDecls.begin(); it != attrDecls.end(); ++it)
                {
                    if( (*it)->name() == attr->name() )
                    {
                        break;
                    }
                }

                if( it == attrDecls.end() )
                    return false;

                if( ! (*it)->match( *attr) )
                    return false;

                attrDecls.erase(it);
            }

            //
            // post process unmatched declarations e.g. get default values
            // and check for missing required attributes
            //
            AttrDecls::iterator decl;
            for(decl = attrDecls.begin(); decl != attrDecls.end(); ++decl)
            {
                if( ! (*decl)->validate(attrs) )
                    return false;
            }

            return true;
        }

    private:
        AttrDecls _attrs;
};

namespace ContentModel {

class Particle
{
    public:
        virtual ~Particle()
        { }

        //! @brief Gets this Particle and follows unlabelled transitions.
        virtual void get(std::vector<Particle*>& nodes) = 0;

        //! @brief Evaluate the XML node and get all following nodes.
        virtual void eval(Node& node, std::vector<Particle*>& states) = 0;

        //! @brief Returns true if the node represents a match state.
        virtual bool isValid() const
        { return false; }

        Particle* out()
        { return _out; }

        void setNext(Particle& state)
        { _out = &state; }

    protected:
        Particle()
        : _out(0)
        {}

    private:
        Particle* _out;
};

class Split : public Particle
{
    public:
        Split(Particle* to)
        : Particle()
        , _out1(to)
        { }

        virtual void eval(Node& node, std::vector<Particle*>& nodes)
        { }

        virtual void get(std::vector<Particle*>& nodes) 
        {
            assert( out() );
            out()->get(nodes);
            _out1->get(nodes);
        }

    private:
        Particle* _out1;
};

class Label : public Particle
{
    public:
        Label(const Pt::String& name)
        : Particle()
        , _name(name)
        { }

        virtual void eval(Node& node, std::vector<Particle*>& states)
        {
            StartElement* se = toStartElement(&node);
            if(se && se->name() == _name)
                out()->get(states);
        }

        virtual void get(std::vector<Particle*>& states) 
        {
            states.push_back(this);
        }

    private:
        Pt::String _name;
};

class PcData : public Particle
{
    public:
        PcData()
        : Particle()
        { }

        virtual void eval(Node& node, std::vector<Particle*>& states)
        {
            Characters* chars = toCharacters(&node);
            if(chars)
                out()->get(states);
        }

        virtual void get(std::vector<Particle*>& states) 
        {
            states.push_back(this);
        }
};

class Empty : public Particle
{
    public:
        Empty()
        : Particle()
        { }

        virtual void eval(Node& node, std::vector<Particle*>& states)
        { }
        
        virtual void get(std::vector<Particle*>& states) 
        { states.push_back(this); }

        virtual bool isValid() const
        { return true; }
};

class Match : public Particle
{
    public:
        Match()
        : Particle()
        { }

        virtual void eval(Node& node, std::vector<Particle*>& states)
        { }
        
        virtual void get(std::vector<Particle*>& states) 
        { states.push_back(this); }

        virtual bool isValid() const
        { return true; }
};

} // namespace ContentModel


class ElementDeclaration
{
    public:
        ElementDeclaration()
        {}

        void setContentModel(ContentModel::Particle& start)
        { _start = &start; }

        void start(std::vector<ContentModel::Particle*>& nodes)
        { 
            assert(_start);
            return _start->get(nodes); 
        }

        AttributeListDeclaration& attrListDecl()
        { return _attr; }

    private:
        ContentModel::Particle* _start;
        AttributeListDeclaration _attr;
};


class DocType;

class DocTypeContext : private NonCopyable
{
    public:
        DocTypeContext();

        ~DocTypeContext();

        void clear(DocType& dtd);

        ContentModel::Label& getLabel(const Pt::String& name);

        ContentModel::Split& getSplit(ContentModel::Particle& to);

        ContentModel::PcData& getPcData();

        ContentModel::Empty& getEmpty()
        { return _empty; }

        ContentModel::Particle& getMatch()
        { return _match; }

    private:
        std::vector<ContentModel::Particle*> _pool;
        ContentModel::Empty _empty;
        ContentModel::Match _match;
};


class DocType : public Node
              , private Pt::NonCopyable
{
    public:
        DocType(DocTypeContext& context)
        : Node(Node::DocType)
        , _ctx(&context)
        {}

        ~DocType()
        {
            _ctx->clear(*this);
        }

        DocTypeContext& context()
        { return *_ctx; }

        void clear()
        {
            _ctx->clear(*this);
            _rootName.clear();
            _elemDecls.clear();
        }

        bool isDefined() const
        { return ! _rootName.empty(); }

        const Pt::String& rootName() const
        { return _rootName; }

        Pt::String& rootName()
        { return _rootName; }

        ElementDeclaration& declareElement(const Pt::String& name)
        { 
            return _elemDecls[name]; 
        }

        ElementDeclaration* findElementDecl(const Pt::String& name)
        {
            ElementDeclaration* decl = 0;

            std::map<Pt::String, ElementDeclaration>::iterator it;
            it = _elemDecls.find(name);

            if(it != _elemDecls.end())
                decl = &it->second;

            return decl; 
        }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::DocType; }

    private:
        DocTypeContext* _ctx;

        // TODO: move this to ElementDeclaration
        Pt::String _rootName;

        // TODO: make this a sorted vector -> ElementDeclarations
        std::map<Pt::String, ElementDeclaration> _elemDecls;
};


inline DocType* toDocType(Node* node)
{
    return nodeCast<DocType>(node);
}


inline const DocType* toDocType(const Node* node)
{
    return nodeCast<DocType>(node);
}


inline DocType& toDocType(Node& node)
{
    return nodeCast<DocType>(node);
}


inline const DocType& toDocType(const Node& node)
{
    return nodeCast<DocType>(node);
}

} // namespace Xml

} // namespace Pt

#endif
