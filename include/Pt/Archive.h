/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Drner                       *
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
#ifndef Pt_Archive_h
#define Pt_Archive_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/SmartPtr.h>
#include <Pt/RefCounted.h>


namespace Pt {

class Archive;
class ArchiveValue;


class ArchiveNode
{
    public:
        virtual ~ArchiveNode()
        {}

        const Pt::String& name() const
        { return this->_name(); }

        bool operator< (const ArchiveNode& an) const
        { return this->name() < an.name(); }

        bool operator!= (const ArchiveNode& an) const
        { return this->name() != an.name(); }

        Archive* toArchive()
        { return this->_toArchive(); }

        const Archive* toArchive() const
        { return this->_toArchive(); }

        ArchiveValue* toValue()
        { return this->_toValue();  }

        const ArchiveValue* toValue() const
        { return this->_toValue(); }

    protected:
        ArchiveNode()
        {}

        virtual Archive* _toArchive() = 0;

        virtual const Archive* _toArchive() const = 0;

        virtual ArchiveValue* _toValue() = 0;

        virtual const ArchiveValue* _toValue() const = 0;

        virtual const Pt::String& _name() const = 0;
};


class ArchiveValue : public ArchiveNode
{
    public:
        const Pt::String& value() const
        { return this->_value(); }

    protected:
        ArchiveValue()
        {}

        virtual Archive* _toArchive()
        { return 0; }

        virtual const Archive* _toArchive() const
        { return 0; }

        virtual ArchiveValue* _toValue()
        { return this; }

        virtual const ArchiveValue* _toValue() const
        { return this; }

        virtual const Pt::String& _value() const = 0;
};


class Archive : public ArchiveNode
{
    public:
        class IteratorBase : public RefCounted
        {
            public:
                virtual ~IteratorBase()
                { }

                virtual bool advance() = 0;

                virtual ArchiveNode& current() const = 0;
        };

        class Iterator
        {
            public:
                Iterator(IteratorBase* base = 0)
                : _base(base)
                { }

                Iterator& operator++()
                {
                    if( ! _base->advance() )
                        _base = 0;

                    return *this;
                }

                ArchiveNode& operator*()
                { return _base->current(); }

                bool operator!= (const Iterator& other) const
                { return _base != other._base; }

            private:
                SmartPtr<IteratorBase, InternalRefCounted<IteratorBase> > _base;
        };

        class ConstIterator
        {
            public:
                ConstIterator(IteratorBase* base = 0)
                : _base(base)
                { }

                ConstIterator& operator++()
                {
                    if( ! _base->advance() )
                        _base = 0;

                    return *this;
                }

                const ArchiveNode& operator*() const
                { return _base->current(); }

                bool operator!= (const ConstIterator& other) const
                { return _base != other._base; }

            private:
                SmartPtr<IteratorBase, InternalRefCounted<IteratorBase> > _base;
        };

    public:
        virtual ~Archive()
        {}

        Iterator begin()
        { return this->_begin(); }

        Iterator end()
        { return Iterator(); }

        ConstIterator begin() const
        { return this->_begin(); }

        ConstIterator end() const
        { return ConstIterator(); }

        const ArchiveNode* getNode(const Pt::String& name) const
        { return this->_getNode(name); }

        const Pt::String* getValue(const Pt::String& name) const
        {
            const ArchiveNode* node = this->getNode(name);

            if( node && node->toValue() )
                return &( node->toValue()->value() );

            return 0;
        }

        void addValue(const Pt::String& name, const Pt::String& value)
        { this->_addValue(name, value); }

        const Archive* getArchive(const Pt::String& name) const
        {
            const ArchiveNode* node = this->getNode(name);
            if( node && node->toArchive() )
                return node->toArchive();

            return 0;
        }

        Archive& addArchive(const Pt::String& name)
        { return this->_addArchive(name); }

        template <typename T>
        void extract(T& type, const Pt::String& typeName)
        {
            const Archive* archive = this->getArchive(typeName);
            if(archive == 0)
                return;

            *archive >> type;
        }

    protected:
        Archive()
        {}

        virtual Archive* _toArchive()
        { return this; }

        virtual const Archive* _toArchive() const
        { return this; }

        virtual ArchiveValue* _toValue()
        { return 0; }

        virtual const ArchiveValue* _toValue() const
        { return 0; }

        /** @brief Returns the begin of the Archive contents

            The deriving class is suposed to return a pointer to its
            type of iterator created with new. If the archive is empty
            0 must be returned.
        */
        virtual IteratorBase* _begin() const = 0;

        virtual const ArchiveNode* _getNode(const Pt::String& name) const = 0;

        virtual void _addValue(const Pt::String& name, const Pt::String& value) = 0;

        virtual Archive& _addArchive(const Pt::String& name) = 0;
};

}

#endif
