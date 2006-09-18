#ifndef Pt_Xml_EndElement_h
#define Pt_Xml_EndElement_h

#include <Pt/Xml/Node.h>
#include <Pt/Api.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {

		class PT_EXPORT EndElement : public Node {
			public:
				EndElement();

				EndElement(const String& name);

				~EndElement();

				EndElement* clone() const
				{return new EndElement(*this);}

				String& name();

				const String& name() const;

				void setName(const String name);

				virtual bool operator==(const Node& node) const;

			private:
				String _name;
		};
		
	}

}

#endif
