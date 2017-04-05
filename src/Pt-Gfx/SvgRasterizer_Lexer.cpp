/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2017-2017 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline fuelemtions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Ielem., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <stdio.h>
#include <algorithm>

#include "SvgRasterizer.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Private Member Functions =======================================================
// ======================================================================================

void SvgRasterizer::lexPathData(std::vector<std::string>& tokens, const std::string& str)
{
    // Supported commands and their expected number of parameters
    static const std::string supportedCommand = "MmZzLlHhVvCcSsQqTtAa";
    static const Pt::uint8_t numOfCmdParams[] = { 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 6, 6, 4, 4, 4, 4, 2, 2, 7, 7 };
                                              //  M  m  Z  z  L  l  H  h  V  v  C  c  S  s  Q  q  T  t  A  a

    // State variables
    char        curCmd     = 0;
    Pt::uint8_t curCmdNPar = 0;
    std::string token;

    // Clear first
    tokens.clear();

    // Walk thorugh the characters
    for(std::string::const_iterator it = str.begin(); it != str.end();) {
        // Get the character
        const char c = *it;
        // Start a new command?
        if(!curCmd) {
            // Check the command
            const size_t idx = supportedCommand.find(c);
            if(idx == std::string::npos)
                throw IOError(std::string("svg error: path data: invalid command '") + c + "'");
            // Update the state variables
            curCmd     = c;
            curCmdNPar = numOfCmdParams[idx];
            // Store the command
            tokens.push_back(supportedCommand.substr(idx, 1));
            // Check if the command actually expect no parameter
            if(!curCmdNPar) curCmd = 0;
            // Process the next character
            ++it;
            continue;
        }
        // Is the character is a '-'?
        if(c == '-') {
            // A '-' can only appear at the beginning of a number
            if(!token.empty())
                throw IOError("svg error: path data: unexpected location for character '-' in number definition");
            // Append the character to the token
            token += c;
            // Process the next character
            ++it;
            continue;
        }
        // Is the character is a '.'?
        if(c == '.') {
            // A '.' can only appear once in a number
            if(token.find('.') != std::string::npos)
                throw IOError("svg error: path data: unexpected location for character '.' in number definition");
            // Append the character to the token
            if(token.empty()) token += '0';
            token += c;
            // Process the next character
            ++it;
            continue;
        }
        // Is the character is a digit?
        if(::isdigit(c)) {
            // Append the character to the token
            token += c;
            // Process the next character
            ++it;
            continue;
        }
        // Other characters
        if(true) {
            // Store the parameter
            tokens.push_back(token);
            token.clear();
            // Decrement the number of parameter
            --curCmdNPar;
            // Check if all the parameters have been obtained
            if(!curCmdNPar) curCmd = 0;
            // If the character is a white-space or a ',' simply process the next character
            if(::isspace(c) || c == ',') ++it;
        }
    }

    // Process the last token (parameter)
    if(!token.empty()) {
        // Store the parameter
        tokens.push_back(token);
        // Decrement the number of parameter
        --curCmdNPar;
    }

    // Check if all the parameters have been obtained
    if(curCmdNPar)
        throw IOError("svg error: path data: not enough parameters specified for the last command");
}

void SvgRasterizer::lexStyleData(std::vector<std::string>& tokens, const std::string& str)
{
}


} // namespace
} // namespace
