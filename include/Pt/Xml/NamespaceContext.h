#ifndef Pt_Xml_NamespaceContext_h
#define Pt_Xml_NamespaceContext_h

#include <Pt/Xml/Namespace.h>
#include <Pt/Text/String.h>
#include <map>


namespace Pt {

namespace Xml {
	
	class PT_EXPORT NamespaceContext {
		public:
			NamespaceContext();

			~NamespaceContext();

			const String& namespaceUri(const String& prefix) const;

			const String& prefix(const String& namespaceUri) const ;

			void addNamespace(const String& elementName, const Namespace& ns);

			void removeNamespace(const String& elementName);

		private:
			std::multimap<String, Namespace> _namespaceScopes;
	};

}

}



#endif
