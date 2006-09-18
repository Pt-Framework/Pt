/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Duerner                              *
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
 ***************************************************************************/

#include "Pt/Text/String.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace Pt::Text;


int main (int argc, char* argv[])
{
	try {
		{
			std::string narrowString;
			const Pt::Char text[] = {'H', 'e', 'l' , 'l', 'o', 'w', '\0'};

			String str1(text, 4);
			assert(str1.narrow() == "Hell");

			String str2(text);
			assert(str2.narrow() == "Hellow");

			str1.assign(text, 4);
			assert(str1.narrow() == "Hell");

			str1.assign(text);
			assert(str1.narrow() == "Hellow");

			str2.assign(str1, 3, 3);
			assert(str2.narrow() == "low");

			str2.assign(str1);
			assert(str1.narrow() == "Hellow");

			str1.assign(6, 'X');
			assert(str1.narrow() == "XXXXXX");

			//clock_t begin = clock();
			//for(int n = 0; n < 100000; ++n)
			{ // test performance
				String a(text);
				a.append(5, 'A');
				//std::cerr << a.narrow() << std::endl;
				assert(a.narrow() == "HellowAAAAA");

				String b(a);
				b.append(5, 'Z');
				assert(b.narrow() == "HellowAAAAAZZZZZ");
				assert(a.narrow() == "HellowAAAAA");
			}
			//std::clog << "clocks: " << clock() - begin << std::endl;

			{
				String a(text, 4);
				a.append(text, 4);
				assert(a.narrow() == "HellHell");

				String b(a);
				b.append(text, 4);
				assert(b.narrow() == "HellHellHell");
				assert(a.narrow() == "HellHell");

				b.append(text);
				assert(b.narrow() == "HellHellHellHellow");
			}

			{
				String a(text, 4);
				String b(text, 4);
				a.append(b);
				assert(a.narrow() == "HellHell");

				b.append(a, 4, 4);
				assert(b.narrow() == "HellHell");
			}

			clog << "insert(size_type, Pt::Char*)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char xxxxx[] = {'x', 'x', 'x', 'x', 'x', '\0'};
				String a(abcde);
				String b(a);
				a.insert(3, xxxxx);
				//std::cerr << a.narrow() << std::endl;
				assert(a.narrow() == "abcxxxxxde");

				b.insert(1, xxxxx);
				//std::cerr << b.narrow() << std::endl;
				assert(b.narrow() == "axxxxxbcde");
			}

			clog << "insert(size_type, size_type, Pt::Char)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				String a(abcde);
				String b(a);
				a.insert(3, 5, 'x');
				//std::cerr << a.narrow() << std::endl;
				assert(a.narrow() == "abcxxxxxde");

				b.insert(1, 5, 'x');
				//std::cerr << b.narrow() << std::endl;
				assert(b.narrow() == "axxxxxbcde");
			}

			clog << "insert(size_type, Pt::Char*, size_type, size_type)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				//const Pt::Char xxxxx[] = {'x', 'x', 'x', 'x', 'x', '\0'};
				String source = abcde;
				String a(abcde);
				String b(a);
				
				a.insert(3, source, 2, 2);
				//std::cerr << a.narrow() << std::endl;
				assert(a.narrow() == "abccdde");

				b.insert(1, source, 2, 2);
				//std::cerr << b.narrow() << std::endl;
				assert(b.narrow() == "acdbcde");
			}

			clog << "insert(iterator, size_type n, Pt::Char)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				String a(abcde);
				String b(a);

				String::iterator it = a.begin() + 3;
				a.insert(it, 5, 'x');
				//std::cerr << a.narrow() << std::endl;
				assert(a.narrow() == "abcxxxxxde");
				it = b.begin()+ 1;

				b.insert(it, 5, 'x');
				//std::cerr << b.narrow() << std::endl;
				assert(b.narrow() == "axxxxxbcde");
			}

			clog << "insert(iterator, Pt::Char)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				String a(abcde);
				String b(a);

				String::iterator it = a.begin()+ 3;
				a.insert(it, 'x');
				//std::cerr << a.narrow() << std::endl;
				assert(a.narrow() == "abcxde");
				it = b.begin()+ 1;

				b.insert(it, 'x');
				//std::cerr << b.narrow() << std::endl;
				assert(b.narrow() == "axbcde");
			}

			clog << "clear()" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				String a(abcde);
				String b(a);

				a.clear();
				assert(a.narrow() == "");

				b.clear();
				assert(b.narrow() == "");
			}

			clog << "erase(size_type pos, size_type n)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				String a(abcde);
				String b(a);

				a.erase( 0, a.length() );
				a.invariant();
				assert(a.narrow() == "");

				b.erase( 1, b.length()-2 );
				b.invariant();
				assert(b.narrow() == "ae");
			}

			clog << "erase(iterator)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				String a(abcde);
				String b(a);

				String::iterator it = a.begin();
				it = a.erase(it);
				a.invariant();
				assert(a.narrow() == "bcde");
				assert(*it == 'b');

				it = b.begin() + 1;
				it = b.erase( it );
				b.invariant();
				assert(b.narrow() == "acde");
				assert(*it == 'c');
			}

			clog << "erase(iterator first, iterator last)" << endl;
			{
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', '\0'};
				String a(abcde);
				String b(a);

				String::iterator it = a.begin();
				it = a.erase(it, it + a.length());
				a.invariant();
				assert(a.narrow() == "");
				assert(*it == '\0');

				it = b.begin() + 2;
				it = b.erase( it, b.begin() + b.length() - 2 );
				b.invariant();
				assert(b.narrow() == "abfg");
				assert(*it == 'f');
			}

			clog << "replace()" << endl;
			{
				const Pt::Char abcde[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', '\0'};
				const Pt::Char vwxyz[] = {'v', 'w', 'x', 'y', 'z', '\0'};
				String a(abcde);
				String b(a);

				a.replace(0, 6, vwxyz);
				//std::cerr << a.narrow() << std::endl;
			}

			clog << "replace()" << endl;
			{
				const Pt::Char abcde[] = {'A', 'B', 'C', 'D', 'E', 'F', '\0'};
				//const Pt::Char vwxyz[] = {'v', 'w', 'x', 'y', 'z', '\0'};
				String a(abcde);
				String b(a);

				a.replace(1, 4, 3, 'x');
				//std::cerr << a.narrow() << std::endl;
				a.invariant();
				//assert(a.narrow() == "AxxxF");

				b.replace(0, b.length(), 5, 'x');
				//std::cerr << b.narrow() << std::endl;
				b.invariant();
				assert(b.narrow() == "xxxxx");
			}

			clog << "compare(const basic_string& str)" << endl;
			{
				//const Pt::Char abcxyz[] = {'a', 'b', 'c', 'x', 'y', 'z', '\0'};
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char abc[] = {'a', 'b', 'c', '\0'};
				String s(abcde);
				String t(abc);

				int result = -5;
				result = s.compare(t);
				assert(result > 0);

				result = t.compare(s);
				assert(result < 0);

				result = s.compare(s);
				assert(result == 0);
			}

			clog << "compare(const Pt::Char* str)" << endl;
			{
				const Pt::Char abcxyz[] = {'a', 'b', 'c', 'x', 'y', 'z', '\0'};
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char abc[] = {'a', 'b', 'c', '\0'};
				String s(abcde);

				int result = -5;
				result = s.compare(abcde);
				assert(result == 0);

				result = s.compare(abcxyz);
				assert(result < 0);

				result = s.compare(abc);
				assert(result > 0);
			}

			clog << "compare(size_t pos, size_t n, const basic_string& str)" << endl;
			{
				//const Pt::Char abcxyz[] = {'a', 'b', 'c', 'x', 'y', 'z', '\0'};
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char abc[] = {'a', 'b', 'c', '\0'};
				String s(abcde);
				String t(abc);

				int result;
				result = s.compare(0, 3, t);
				assert(result == 0);

				result = s.compare(0, 2, t);
				assert(result < 0);

				result = s.compare(0, 4, t);
				assert(result > 0);
			}


			clog << "compare(size_t pos, size_t n, const basic_string& str)" << endl;
			{
				//const Pt::Char abcxyz[] = {'a', 'b', 'c', 'x', 'y', 'z', '\0'};
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char abc[] = {'a', 'b', 'c', '\0'};
				String s(abcde);
				String t(abc);

				int result;
				result = s.compare(0, 3, t);
				assert(result == 0);

				result = s.compare(0, 2, t);
				assert(result < 0);

				result = s.compare(0, 4, t);
				assert(result > 0);
			}


			clog << "compare(size_t pos, size_t n, const basic_string& str, size_t pos2, size_t n2)" << endl;
			{
				//const Pt::Char abcxyz[] = {'a', 'b', 'c', 'x', 'y', 'z', '\0'};
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char abc[] = {'a', 'b', 'c', '\0'};
				String s(abcde);
				String t(abc);

				int result;
				result = s.compare(1, 2, t, 1, 2);
				assert(result == 0);

				result = s.compare(1, 2, t, 1, 3);
				assert(result < 0);

				result = s.compare(1, 4, t, 1, 3);
				assert(result > 0);
			}


			clog << "compare(size_t pos, size_t n, const Pt::Char* str)" << endl;
			{
				const Pt::Char abcxyz[] = {'a', 'b', 'c', 'x', 'y', 'z', '\0'};
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char abc[] = {'a', 'b', 'c', '\0'};
				String s(abcde);

				int result;
				result = s.compare(0, 3, abc);
				assert(result == 0);

				result = s.compare(0, 3, abcxyz);
				assert(result < 0);

				result = s.compare(0, 4, abc);
				assert(result > 0);
			}


			clog << "compare(size_t pos, size_t n, const Pt::Char* str, size_t n)" << endl;
			{
				const Pt::Char abcxyz[] = {'a', 'b', 'c', 'x', 'y', 'z', '\0'};
				const Pt::Char abcde[] = {'a', 'b', 'c', 'd', 'e', '\0'};
				const Pt::Char abc[] = {'a', 'b', 'c', '\0'};
				String s(abcde);

				int result;
				result = s.compare(0, 3, abc, 3);
				assert(result == 0);

				result = s.compare(0, 3, abcxyz, 4);
				assert(result < 0);

				result = s.compare(0, 4, abc, 2);
				assert(result > 0);
			}

			/*for(BString::iterator it = str1.begin(); it != str1.end(); ++it) {
				assert(*it == 'X');
				*it = 'Y';
			}

			for(BString::const_iterator it = str1.begin(); it != str1.end(); ++it) {
				assert(*it == 'Y');
			}*/
			std::cerr << "OK." << std::endl;
		}
	}
	catch(std::exception& e) {
		std::cerr << "Failed: " << e.what() << std::endl;
	}

	return 0;
}
