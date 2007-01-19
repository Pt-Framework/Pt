#ifndef PTV_Xml_NamespaceContext_h
#define PTV_Xml_NamespaceContext_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Namespace.h>
#include <Pt/Text/String.h>
#include <map>


namespace Pt {

namespace Xml {

	/**
	 * @brief Manages all namespaces which are valid for a specific XML document.
	 *
	 * Namespaces can be added using the method addNamespace() and can be removed using
	 * the method removeNamespace().
	 *
	 * To get the namespace URI for a prefix the method namespaceUri() can be used. To
	 * determine the prefix for a namespace the method prefix() can be used.
	 *
	 * @see Namespace
	 */
	class PT_XML_API NamespaceContext {
		public:
			//! Creates a new NamespaceContext object which manages the namespaces of an XML document.
			NamespaceContext();

			//! Empty destructor
			~NamespaceContext();

			/**
			 * @brief Returns the namespace URI of the namespace which has the prefix that is passed to
			 * this method.
			 *
			 * If no namespace with the given prefix exists, an empty String is returned.
			 *
			 * @param prefix The namespace URI for the namespace with this prefix is returned.
			 * @return The namespace URI for the prefix or an empty String if the prefix was not found.
			 */
			const String& namespaceUri(const String& prefix) const;

			/**
			 * @brief Returns the prefix for the namespace which has the URI that is passed to this method.
			 *
			 * If no namespace with this URI exists, an empty String is returned.
			 *
			 * @param namespaceUri The prefix of the namespace with this namespace URI is returned.
			 * @return The namespace URI for the prefix or an empty String if the prefix was not found.
			 */
			const String& prefix(const String& namespaceUri) const ;

			/**
			 * @brief Associates the element name (elementName) with the given namespace (ns).
			 *
			 * The stored namespace can be retrieved by calling namespaceUri() or prefix(). To
			 * remove the association between the element name and namespace again, the method
			 * removeNamespace() can be used.
			 *
			 * @param elementName Associates this element name with the also given namespace (ns).
			 * @param ns Associates this namespace with the also given element name (elementName).
			 */
			void addNamespace(const String& elementName, const Namespace& ns);

			/**
			 * @brief Removes the associates of the given element name (elementName) to the namespace.
			 *
			 * @param elementName The associates for this element name is removed.
			 */
			void removeNamespace(const String& elementName);

		private:
			//! Multimap that stores the assocations between an element name and its namespace.
			std::multimap<String, Namespace> _namespaceScopes;
	};

}

}



#endif
