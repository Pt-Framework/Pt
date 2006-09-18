#ifndef Pt_Xml_ProcessingInstruction_h
#define Pt_Xml_ProcessingInstruction_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {
	
		class PT_EXPORT ProcessingInstruction : public Node {
			public:
				ProcessingInstruction();

				~ProcessingInstruction();

				ProcessingInstruction* clone() const
				{return new ProcessingInstruction(*this);}

				const String& target() const;

				void setTarget(const String& target);

				const String& data() const;

				void setData(const String& data);
	
			private:
				String _target;
				String _data;
		};
	
	}

}

#endif
