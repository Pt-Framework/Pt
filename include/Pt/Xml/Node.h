#ifndef Pt_Xml_Node_h
#define Pt_Xml_Node_h

#include <Pt/Api.h>
#include <Pt/Clonable.h>


namespace Pt {

	namespace Xml {
		
		class PT_EXPORT Node : public Clonable<Node> {
			public:
				enum Type {
					Unknown,
					XmlDeclaration,
					DocType,
					EndDocument,
					StartElement,
					EndElement,
					Characters,
					CData,
					Comment,
					ProcessingInstruction
				};

			public:
				Node(Type type)
				: _type(type)
				{ }

				virtual ~Node()
				{ }

				Type type() const
				{return _type;}

				virtual bool operator==(const Node& node) const
				{ return false; }

			private:
				Type _type;
		};
		
	}

}

#endif
