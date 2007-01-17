/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/

#ifndef Pt_IOManager_h
#define Pt_IOManager_h

#include <Pt/IO/Api.h>
#include <Pt/Singleton.h>
#include <Pt/IO/IOHandler.h>
#include <Pt/IO/Url.h>


namespace Pt {

namespace IO {

	//! IOManager for protocol transparent IO.
	class PT_IO_API IOManager : public Pt::Singleton<IOManager> {
		friend class Pt::Singleton<IOManager>;

		public:
			template <typename HandlerT>
			class PT_API Register {
				public:
					Register(const std::string& protocol) {
						IOManager::instance().registerHandler(protocol, new HandlerT);
					}
			};

		public:
			~IOManager();

			GetTask* get(const Url& url) throw (IOError);

			PutTask* put(const Url& url) throw (IOError);

			UnlinkTask* unlink(const Url& url)
			{ throw IOError("Not implemented.", PT_SOURCEINFO); }

			MakeDirTask* mkdir(const Url& url) throw (IOError)
			{ throw IOError("Not implemented.", PT_SOURCEINFO); }

			RemoveDirTask* rmdir(const Url& url) throw (IOError)
			{ throw IOError("Not implemented.", PT_SOURCEINFO); }

			ListDirTask* list(const Url& url) throw (IOError)
			{ throw IOError("Not implemented.", PT_SOURCEINFO); }

			void registerHandler(const std::string& protocol, IOHandler* handler);

		protected:
			IOManager();

			IOHandler* findHandler(const std::string& protocol);

		private:
			std::map<std::string, IOHandler*> _handlerMap;
	};

} // namespace IO

} // namespace Pt

#endif
