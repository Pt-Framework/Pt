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

#ifndef Pt_Xml_DocTypeDefinition_h
#define Pt_Xml_DocTypeDefinition_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/Xml/EntityMapping.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <vector>

namespace Pt {

namespace Xml {

class ElementDeclaration;
class AttributeDeclaration;
typedef std::vector<AttributeDeclaration*> AttributeDeclarationList;

class ContentParticle;
class LeafParticle;
class SplitParticle;
class PcDataParticle;
class MatchParticle;

class PT_XML_API DocTypeDefinition : public Node
                                   , private NonCopyable
{
    public:
        DocTypeDefinition();

        ~DocTypeDefinition();

        // TODO: this should declare and EMPTY element
        ElementDeclaration& declareElement(const Pt::String& name);

        ElementDeclaration* element(const Pt::String& name);

        AttributeDeclarationList& declareAttributeList(const Pt::String& name);

        AttributeDeclarationList* attributeList(const Pt::String& name);

        void clear();

        Entity* addEntity(const Pt::String& name);

        const Entity* resolveEntity(const Pt::String& name) const;

        Entity* addParamEntity(const Pt::String& name);

        const Entity* resolveParamEntity(const Pt::String& name) const;

        LeafParticle& getLabel(const Pt::String& name);

        SplitParticle& getSplit(ContentParticle& to);

        PcDataParticle& getPcData();

        MatchParticle& getMatch();

        //! @internal
        inline static Node::Type nodeId()
        { return Node::DocTypeDefinition; }

    private:
        typedef std::vector< std::pair<Pt::String, ElementDeclaration*> > ElementDeclarationList;
        
        ElementDeclarationList _elemDecls;
        EntityMapping _entities;
        EntityMapping _paramEntities;
        std::vector<ContentParticle*> _pool;
        MatchParticle* _match;
};


inline DocTypeDefinition* toDocTypeDefinition(Node* node)
{
    return nodeCast<DocTypeDefinition>(node);
}


inline const DocTypeDefinition* toDocTypeDefinition(const Node* node)
{
    return nodeCast<DocTypeDefinition>(node);
}


inline DocTypeDefinition& toDocTypeDefinition(Node& node)
{
    return nodeCast<DocTypeDefinition>(node);
}


inline const DocTypeDefinition& toDocTypeDefinition(const Node& node)
{
    return nodeCast<DocTypeDefinition>(node);
}

} // namespace Xml

} // namespace Pt

#endif
