#ifndef Pt_Xml_CData_h
#define Pt_Xml_CData_h

#include <Pt/Api.h>
#include <Pt/Text/String.h>
#include <Pt/Xml/Node.h>

namespace Pt {

	namespace Xml {

		class PT_EXPORT CData : public Node {
			public:
				CData(const String& content = String());

				~CData();

				CData* clone() const
				{ return new CData(*this); }

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
