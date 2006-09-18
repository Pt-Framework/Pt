#ifndef Pt_Xml_Characters_h
#define Pt_Xml_Characters_h

#include <Pt/Api.h>
#include <Pt/Text/String.h>
#include <Pt/Xml/Node.h>

namespace Pt {

	namespace Xml {

		class PT_EXPORT Characters : public Node {
			public:
				Characters( const String& content = String() );

				~Characters();

				Characters* clone() const
				{ return new Characters(*this); }

				bool empty() const;

				String& content();

				const String& content() const;

				void setContent(const String& content);

				virtual bool operator==(const Node& node) const;

			private:
				String _content;
		};

	}

}

#endif
