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

#ifndef Pt_Xml_ContentModel_h
#define Pt_Xml_ContentModel_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/String.h>
#include <vector>

namespace Pt {

namespace Xml {

class ValidationContext;

class ContentParticle
{
    public:
        virtual ~ContentParticle()
        { }

        //! @brief Gets this Particle and follows unlabelled transitions.
        virtual void get(ValidationContext& ctx) const = 0;

        //! @brief Evaluate the XML node and get all following nodes.
        virtual void eval(ValidationContext& ctx, Node& node) const = 0;

        //! @brief Returns true if the node represents a match state.
        virtual bool isValid() const
        { return false; }

        const ContentParticle* out() const
        { return _out; }

        void setNext(ContentParticle& state)
        { _out = &state; }

        void setId(unsigned id)
        { _id = id; }

        unsigned id() const
        { return _id; }

    protected:
        ContentParticle()
        : _out(0)
        , _id(0)
        {}

    private:
        ContentParticle* _out;
        unsigned _id;
};


class ValidationContext
{
    public:
        ValidationContext(std::size_t nodeCount = 0);

        void clear();

        bool setVisited(unsigned id);

        void addNext(const ContentParticle* p);
        
        const std::vector<const ContentParticle*>& next() const;

    private:
        unsigned _stepId;
        std::vector<unsigned> _nodes;
        std::vector<const ContentParticle*> _current;
};


class SplitParticle : public ContentParticle
{
    public:
        SplitParticle(ContentParticle* to)
        : ContentParticle()
        , _out1(to)
        { }

        virtual void eval(ValidationContext& ctx, Node& node) const;

        virtual void get(ValidationContext& ctx) const ;

    private:
        ContentParticle* _out1;
};


class LeafParticle : public ContentParticle
{
    public:
        LeafParticle(const Pt::String& name)
        : ContentParticle()
        , _name(name)
        { }

        virtual void eval(ValidationContext& ctx, Node& node) const;

        virtual void get(ValidationContext& ctx) const;

    private:
        Pt::String _name;
};


class PcDataParticle : public ContentParticle
{
    public:
        PcDataParticle()
        : ContentParticle()
        { }

        virtual void eval(ValidationContext& ctx, Node& node) const;

        virtual void get(ValidationContext& ctx) const;
};


class MatchParticle : public ContentParticle
{
    public:
        MatchParticle()
        : ContentParticle()
        { setId(0); }

        virtual void eval(ValidationContext& ctx, Node& node) const;
        
        virtual void get(ValidationContext& ctx) const;

        virtual bool isValid() const
        { return true; }
};

} // namespace Xml

} // namespace Pt

#endif
