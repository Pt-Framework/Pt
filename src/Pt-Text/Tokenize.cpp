/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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

#include <Pt/Text/Tokenize.h>
using namespace std;


void Pt::Text::tokenize(vector<string> &tokens, const string &source, const string &delimiters)
{
    // Skip delimiters at beginning
    string::size_type lastPos = source.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter"
    string::size_type pos = source.find_first_of(delimiters, lastPos);

    // Clear the destination vector first
    tokens.clear();

    // Tokenize it
    while((pos != string::npos) || (lastPos != string::npos))
    {
        // Found a token, add it to the vector
        tokens.push_back(source.substr(lastPos, pos - lastPos));

        // Skip delimiters
        lastPos = source.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = source.find_first_of(delimiters, lastPos);
    }
}
