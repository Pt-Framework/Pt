#ifndef Pt_System_Directory_h
#define Pt_System_Directory_h

#include <Pt/Exception.h>

#include <Pt/System/FileSystemNode.h>

#include <string>
#include <list>
#include <iostream>

namespace Pt {

namespace System {

	class FileSystemNode;


	class PT_EXPORT Directory : public FileSystemNode {
		public:
			typedef std::string value_type;

		public:
			class PT_EXPORT Iterator {
				public:
					Iterator();

					Iterator(const char* path);

					Iterator(const Iterator& it);

					~Iterator();

					Iterator& operator++();

					Iterator& operator=(const Iterator& it);

					bool operator==(const Iterator& it) const;

					bool operator!=(const Iterator& it) const;

					//FileSystemNode& node();

					FileSystemNode& operator*() const;

				private:
					class DirectoryIteratorImpl* _impl;
			};

		public:
			Directory(const char* path)
			: _path(path)
			{ /* check if Dir is valid? */ }

			~Directory() throw()
			{ }

			const char* path() const
			{ return _path.c_str(); }

			virtual std::size_t size() const
			{ return 0; }

			virtual void resize(std::size_t length)
			{ /* do nothing on purpose */ }

			//! Returns an iterator to the node in the directory.
			Iterator begin() const
			{ return Iterator( _path.c_str() ); }

			//! Returns an iterator to the end of the directory.
			Iterator end() const
			{
				static Iterator _end;
				return _end;
			}

		public:
			static Directory create(const char* dirpath);

			static void remove(const char* dirpath);

			static Directory system();

			static Directory current();

			static void changeCurrent(const char* dirpath);

		private:
			std::string _path;
	};

}

}

#endif
