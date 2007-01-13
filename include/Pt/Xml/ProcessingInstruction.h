#ifndef PTV_Xml_ProcessingInstruction_h
#define PTV_Xml_ProcessingInstruction_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {

		/**
		 * @brief A ProcessingInstruction (PI) element (Node) of an XML document.
		 *
		 * A processing instruction can be used to add instructions to an XML document which is needed
		 * and can be used by specific XML processing software. The data of a processing instruction
		 * has no particular format and can contain plain Text or XML-like attribute/value-associations.
		 *
		 * To access the target, which may for example be a identifier for a specific XML processor,
		 * the method target() can be used. To access the data for this processor the method data()
		 * can be used.
		 *
		 * @see Node
		 */
		class PT_API ProcessingInstruction : public Node {
			public:
				//! Constructs a new ProcessingInstruction.
				ProcessingInstruction();

				//! Empty destructor
				~ProcessingInstruction();

				/**
				 * @brief Clones this CData object by creating a duplicate on the heap and returning it.
				 * @return A cloned version of this CData object.
				 */
				ProcessingInstruction* clone() const
				{return new ProcessingInstruction(*this);}

				/**
				 * @brief Returns the processor instruction's target.
				 *
				 * The target may be the XML processor for which this PI was added to the XML document.
				 *
				 * @return The target of this processing instruction.
				 */
				const String& target() const;

				/**
				 * @brief Sets the processor instruction's target.
				 *
				 * @param target The target for this processing instruction.
				 */
				void setTarget(const String& target);

				/**
				 * @brief Returns the processor instruction's data.
				 *
				 * The precise nature of the PI data depends on the XML processor for which this PI
				 * was added to the XML document. It usually contains special instructions for this processor.
				 *
				 * @return The data of this processing instruction.
				 */
				const String& data() const;

				/**
				 * @brief Sets the processor instruction's data.
				 *
				 * @param data The data for this processing instruction.
				 */
				void setData(const String& data);

			private:
				//! The target of this processing instruction.
				String _target;

				//! The data of this processing instruction.
				String _data;
		};

	}

}

#endif
