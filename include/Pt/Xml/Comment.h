#ifndef Pt_Xml_Comment_h
#define Pt_Xml_Comment_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {
		
		class PT_EXPORT Comment : public Node {
			public:
				Comment(const String& text);

				~Comment();

				Comment* clone() const
				{return new Comment(*this);}

				String& text();

				const String& text() const;

				void setText(const String text);
	
			private:
				String _text;
		};
		
	}

}

#endif
