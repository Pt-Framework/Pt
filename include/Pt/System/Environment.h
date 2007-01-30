/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/
#if !defined(PT_Environment_H)
#define PT_Environment_H

#include "Pt/System/Api.h"
#include <string>

namespace Pt {

namespace System {

	//! @brief Environment class.
	/**
	*/

	class PT_SYSTEM_API Environment
	{
        friend class EnvironmentImpl;

		private:
			class EnvironmentImpl* _impl;

		protected:
			Environment();
			~Environment();

		public:
			//! @brief Returns the extension for a shared library on the current system.
			///
			static const std::string& sharedLibraryExtension();

			static const std::string& sharedLibraryPrefix();
	};

} // !namespace system

} // !namespace Pt

#endif // PT_Environment_H



