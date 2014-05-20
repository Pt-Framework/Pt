/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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

#ifndef Pt_XmlRpc_SoapServiceDefinition_h
#define Pt_XmlRpc_SoapServiceDefinition_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/ServiceDefinition.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

class PT_XMLRPC_API ParameterDefinition : private NonCopyable
{
    public:
        ParameterDefinition();

        virtual ~ParameterDefinition();

        virtual const ParameterDefinition* parse(const Xml::Node& node, Composer*& composer) const = 0;

        ParameterDefinition* parent() const
        { return _parent; }

        void setParent(ParameterDefinition* p)
        { _parent = p; }

    private:
        ParameterDefinition* _parent;
};


class PT_XMLRPC_API IntegerParameter : public ParameterDefinition
{
    public:
        IntegerParameter();

        virtual ~IntegerParameter();

        virtual const ParameterDefinition* parse(const Xml::Node& node, Composer*& composer) const;
};


class PT_XMLRPC_API StructParameter : public ParameterDefinition
{
    public:
        StructParameter();

        virtual ~StructParameter();

        void addParameter(const std::string& name, ParameterDefinition* param);

        virtual const ParameterDefinition* parse(const Xml::Node& node, Composer*& composer) const;

    private:
        typedef std::map<std::string, ParameterDefinition*> ParameterMap;
        ParameterMap _params;
};


class PT_XMLRPC_API ArrayParameter : public ParameterDefinition
{
    public:
        ArrayParameter(ParameterDefinition* elem = 0);

        virtual ~ArrayParameter();

        void setElement(ParameterDefinition* param);

        virtual const ParameterDefinition* parse(const Xml::Node& node, Composer*& composer) const;

    private:
        ParameterDefinition* _elem;
};


class PT_XMLRPC_API ProcedureDefinition : private NonCopyable
{
    public:
        ProcedureDefinition();

        virtual ~ProcedureDefinition();

        void addParameter(const std::string& name, ParameterDefinition* param);

        const ParameterDefinition* getParameter(const std::string& name) const;

        virtual const ParameterDefinition* parse(Xml::Node& node, const std::string& procName);

    private:
        typedef std::map<std::string, ParameterDefinition*> ParameterMap;
        ParameterMap _params;
};


class PT_XMLRPC_API SoapServiceDefinition : public ServiceDefinition
{
    public:
        SoapServiceDefinition();

        virtual ~SoapServiceDefinition();

        void addDefinition(const std::string& name, ProcedureDefinition* procDef);

        const ProcedureDefinition* getDefinition(const std::string& name) const;

    private:
        typedef std::map<std::string, ProcedureDefinition*> ProcedureDefinitionMap;
        ProcedureDefinitionMap _procDefs;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_SoapServiceDefinition_h
