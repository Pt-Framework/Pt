#ifndef Pt_Xml_Resolver_h
#define Pt_Xml_Resolver_h

#include <Pt/Text/String.h>
#include <map>


namespace Pt {

namespace Xml {

/**
 * @brief Entity resolver class which associates entities to resolved entity values.
 *
 * Entities can be added to this class using the method addEntity(). This method takes
 * the entity and the resolved entity value. To resolve the resolves value for an entity
 * the method resolveEntity() can be used.
 */
class PT_API Resolver {
	public:
		/**
		 * @brief Constructs a new Resolver object and initializes the entity list using the XML default entities.
		 *
		 * The constructor calls clear() which clears the entity list and adds the XML default entities.
		 */
		Resolver();

		//! Empty destructor.
		virtual ~Resolver();

		/**
		 * @brief Resets the entity list to the XML default entities.
		 *
		 * The default entities are &amp;lt; &amp;gt; &amp;amp; &amp;apos; &amp;quot;
		 */
		void clear();

		/**
		 * @brief Adds the given entity and the given resolved entity value (token) to the entity list.
		 *
		 * To determine the resolved entity value of a entity the method resolveEntity() can be used.
		 *
		 * @param entity A list entry for this entity is created and associated with the also given token.
		 * @param token The resolved entity value that is associated with the also given entity.
		 */
		void addEntity(const String& entity, const String& token);

		/**
		 * @brief Returns the resolved entity value (token) for the given entity.
		 *
		 * If the entity is not in the list an empty String is returned.
		 *
		 * @param entity The resolved entity value for this entity is returned.
		 * @return The resolved entity or an empty String if the entity is not in the list.
		 */
		String resolveEntity(const String& entity);

	private:
		//! Entity map containing entities which are associated to their resolved entity value.
		std::map<String, String> _entityMap;
};

}

}

#endif
