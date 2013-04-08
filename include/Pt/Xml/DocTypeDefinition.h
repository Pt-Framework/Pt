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
#include <Pt/Xml/QName.h>
#include <Pt/Xml/Entity.h>
#include <Pt/Xml/Notation.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <vector>

namespace Pt {

namespace Xml {

class ContentModel;
class AttributeListModel;
class ContentParticle;
class LeafParticle;
class SplitParticle;
class PcDataParticle;
class MatchParticle;

class PT_XML_API DocTypeDefinition : private NonCopyable
{
    public:
        DocTypeDefinition();

        ~DocTypeDefinition();

        void clear();

        bool isDefined() const;

        const QName& rootName() const;

        QName& rootName();

        Entity* declareEntity(const Pt::String& name);

        const Entity* findEntity(const Pt::String& name) const;

        Entity* declareParamEntity(const Pt::String& name);

        const Entity* findParamEntity(const Pt::String& name) const;

        Notation* declareNotation(const Pt::String& name);

        const Notation* findNotation(const Pt::String& name) const;

    public:
        //! @internal
        ContentModel* declareElement(const QName& name);

        //! @internal
        ContentModel* findElement(const QName& name);

        //! @internal
        AttributeListModel& declareAttributeList(const QName& name);

        //! @internal
        AttributeListModel* findAttributeList(const QName& name);

        //! @internal use allocator
        LeafParticle& getLabel(const Pt::String& name);

        //! @internal use allocator
        SplitParticle& getSplit(ContentParticle& to);

        //! @internal use allocator
        PcDataParticle& getPcData();

        //! @internal use allocator
        MatchParticle& getMatch();

    private:
        // TODO: vector<ContentModel*>, move QName to ContentModel
        // make separate class DocumentModel, where _pool and Builder
        // functionality is placed.
        typedef std::vector< std::pair<QName, ContentModel*> > DocumentModel;
        typedef std::map<String, Notation> Notations;
        typedef std::map<String, Entity> Entities;

        QName _rootName;
        DocumentModel _docModel;
        Entities _entities;
        Entities _paramEntities;
        Notations _notations;

        // TODO: ContentModelPool
        std::vector<ContentParticle*> _pool;
};

} // namespace Xml

} // namespace Pt

#endif
