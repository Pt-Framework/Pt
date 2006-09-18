#ifndef Pt_Xml_Namespace_h
#define Pt_Xml_Namespace_h

#include <Pt/Api.h>
#include <Pt/Text/String.h>

#include <iosfwd>


namespace Pt {

namespace Xml {
	
	class PT_EXPORT Namespace {
		public:
			Namespace(const String& namespaceURI, const String& prefix);

			~Namespace();

			const String& prefix() const
			{ return _prefix; }

			void setPrefix(const String& prefix)
			{ _prefix = prefix; }

			const String& namespaceUri() const
			{ return _namespaceUri; }

			void setNamespaceUri(const String& namespaceUri)
			{ _namespaceUri = namespaceUri; }

			bool isDefaultNamespaceDeclaration();

		private:
			String _prefix;
			String _namespaceUri;
	};

}

}

#endif
