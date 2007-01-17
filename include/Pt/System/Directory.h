/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *                                                                         *
 ***************************************************************************/

#ifndef PTV_SYSTEM_DIRECTORY_H
#define PTV_SYSTEM_DIRECTORY_H

#include <Pt/System/System.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/FileSystemNode.h>
#include <Pt/System/Api.h>

#include <string>
#include <list>
#include <iostream>


namespace Pt {

namespace System {

	/** Cycling through Directories.
	 *
	 * You use the iterator as follows:
	\code
	Directory d("/usr");
	Directory::iterator it = d.begin();
	while (it != d.end())
	{
		std::cout << "name : " << (*it).path() << std::endl;
		++it;
	}
	\endcode
 	*/
	class PT_SYSTEM_API DirectoryIterator {
		public:
			DirectoryIterator();

			DirectoryIterator(const char* path);

			DirectoryIterator(const DirectoryIterator& it);

			~DirectoryIterator();

			DirectoryIterator& operator++();

			DirectoryIterator& operator=(const DirectoryIterator& it);

			bool operator==(const DirectoryIterator& it) const;

			bool operator!=(const DirectoryIterator& it) const;

			FileSystemNode& operator*() const;

		private:
			class DirectoryIteratorImpl* _impl;
	};

/** Directory Operations.
 This class contains methods to create, move, delete directories and gives to possibility to iterate over the contents of the directory.

!Iterator Example:

\code
Directory d("/usr");
Directory::iterator it = d.begin();
while (it != d.end())
{
	std::cout << "name : " << (*it).path() << std::endl;
	++it;
}
\endcode
*/
	class PT_SYSTEM_API Directory : public FileSystemNode {
		public:
			typedef DirectoryIterator Iterator;

			enum mode { Create, UseExisting};

			Directory(const std::string& path, mode m = UseExisting);

			~Directory()
			{ }

			bool exists() const;

			///< gives the system-specific path separator
			static char separator();

			virtual const std::string& path() const
			{ return _path; }

			virtual std::size_t size() const
			{ return 0; }

			//! Returns an iterator to the node in the directory.
			DirectoryIterator begin() const
			{ return DirectoryIterator( _path.c_str() ); }

			//! Returns an iterator to the end of the directory.
			DirectoryIterator end() const
			{
				static DirectoryIterator _end;
				return _end;
			}

			virtual void remove();

			virtual void move(const std::string& newname);

		public:
			// DEPRECATED
			static Directory system();

			// DEPRECATED
			static Directory current();

		private:
			std::string _path;
	};

}

}

#endif
