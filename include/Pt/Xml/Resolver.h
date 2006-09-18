#ifndef Pt_Xml_Resolver_h
#define Pt_Xml_Resolver_h

#include <Pt/Text/String.h>
#include <map>


namespace Pt {

namespace Xml {

class PT_EXPORT Resolver {
	public:
		Resolver();

		virtual ~Resolver();

		void clear();

		void addEntity(const String& entity, const String& token);

		String resolveEntity(const String& entity);

	private:
		std::map<String, String> _entityMap;
};

}

}

#endif
