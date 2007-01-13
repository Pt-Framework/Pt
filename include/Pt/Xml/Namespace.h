#ifndef PTV_Xml_Namespace_h
#define PTV_Xml_Namespace_h

#include <Pt/Api.h>
#include <Pt/Text/String.h>

#include <iosfwd>


namespace Pt {

namespace Xml {

	/**
	 * @brief A Namespace element (Node) of an XML document.
	 *
	 * A namespace element stores a namespace uri which describes the namespace URI and a locally
	 * usable prefix which can be added before a tag name to specify that this particular tag
	 * is part of that namespace.
	 *
	 * Use namespaceUri() to get the namespace URI. Use prefix() to get the prefix.
	 *
	 * @see Node
	 * @see NamespaceContext
	 */
	class PT_API Namespace {
		public:
			/**
			 * @brief Constructs a new Namespace object with the given namespace URI and prefix.
			 *
			 * @param namespaceURI The unique URI of this namespace.
			 * @param prefix The namespace prefix which can be added to a tag name to specify that
			 * this tag belongs to that namespace.
			 */
			Namespace(const String& namespaceURI, const String& prefix);

			//! Empty destructor
			~Namespace();

			/**
			 * @brief Returns the prefix of this namespace.
			 *
			 * The namespace prefix can be added to a tag name to specify that this tag belongs
			 * to that namespace.
			 *
			 * @return The namespace prefix of this Namespace object.
			 */
			const String& prefix() const
			{ return _prefix; }

			/**
			 * @brief Sets the prefix of this namespace.
			 *
			 * The namespace prefix can be added to a tag name to specify that this tag belongs
			 * to that namespace.
			 *
			 * @param prefix The namespace prefix for this Namespace object.
			 */
			void setPrefix(const String& prefix)
			{ _prefix = prefix; }

			/**
			 * @brief Returns the URI of this namespace.
			 *
			 * The URI is unique and identifies the namespace.
			 *
			 * @return The namespace URI of this Namespace object.
			 */
			const String& namespaceUri() const
			{ return _namespaceUri; }

			/**
			 * @brief Sets the URI of this namespace.
			 *
			 * The URI is unique and identifies the namespace.
			 *
			 * @param namespaceUri The namespace URI for this Namespace object.
			 */
			void setNamespaceUri(const String& namespaceUri)
			{ _namespaceUri = namespaceUri; }

			/**
			 * @brief Returns $true$ if this is the default namespace in the current XML document. Otherwise
			 * $false$ is returned.
			 *
			 * @return $true$ if this is the default namespace; $false$ otherwise.
			 */
			bool isDefaultNamespaceDeclaration();

		private:
			//! The prefix of this namespace.
			String _prefix;

			//! The namespace URI of this namespace.
			String _namespaceUri;
	};

}

}

#endif
