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

    // Process the last token (parameter), if any
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

void SvgRasterizer::lexStyleData(std::vector<std::string>& tokens, const std::string& str_)
{
    // Convert to lower case
    const std::string& str = lcaseStdStr(str_);

    // State variables
    std::string curKey;
    std::string curVal;
    bool        getKey = true;

    // Clear first
    tokens.clear();

    // Walk thorugh the characters
    for(std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        // Get the character
        const char c = *it;
        // Is the character is a ';'?
        if(c == ';') {
            // Store the tokens
            if(!curKey.empty()) {
                tokens.push_back(lrtrimStdStr(curKey));
                tokens.push_back(lrtrimStdStr(curVal));
            }
            curKey.clear();
            curVal.clear();
            getKey = true;
            // Process the next character
            continue;
        }
        // Is the character is a ':'?
        if(c == ':') {
            // Check for invalid location the character ':'
            if(curKey.empty())
                throw IOError("svg error: style definition: a value without a key");
            if(!curVal.empty())
                throw IOError("svg error: style definition: multiple value specified");
            // Change flag
            getKey = false;
            // Process the next character
            continue;
        }
        // Store the character as a key or value
        if(getKey) curKey += c;
        else       curVal += c;
    }

    // Store the last tokens (key-value pair), if any
    if(!curKey.empty()) {
        tokens.push_back(lrtrimStdStr(curKey));
        tokens.push_back(lrtrimStdStr(curVal));
    }
}

void SvgRasterizer::lexTransformData(std::vector<std::string>& tokens, const std::string& str_)
{
    // Convert to lower case
    const std::string& str = lcaseStdStr(str_);

    // State variables
    std::string curCmd;
    std::string curPar;
    Pt::uint8_t numPar = 0;
    bool        getCmd = true;
    bool        gotAmp = false;

    // Clear first
    tokens.clear();

    // Walk thorugh the characters
    for(std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        // Get the character
        const char c = *it;
        // Is the character is a '('?
        if(c == '(') {
            // Check for invalid location the character '('
            if(curCmd.empty() || !curPar.empty())
                throw IOError("svg error: transform definition: unexpected location for character '('");
            // Check and shorten the command token
                 if(curCmd == "translate") curCmd = "t" ;
            else if(curCmd == "rotate"   ) curCmd = "r";
            else if(curCmd == "scale"    ) curCmd = "s";
            else if(curCmd == "skewx"    ) curCmd = "wx";
            else if(curCmd == "skewy"    ) curCmd = "wy";
            else if(curCmd == "matrix"   ) curCmd = "m";
            else throw IOError("svg error: transform definition: invalid command '" + curCmd + "'");
            // Store the command token
            tokens.push_back(lrtrimStdStr(curCmd));
            // Change flags
            getCmd = false;
            gotAmp = true;
            // Process the next character
            continue;
        }
        // Is the character is a ')'?
        if(c == ')') {
            // Check for invalid location the character ')'
            if(curCmd.empty() || curPar.empty())
                throw IOError("svg error: transform definition: unexpected location for character ')'");
            // Store the parameter token
            tokens.push_back(lrtrimStdStr(curPar));
            ++numPar;
            // Check the number of mandatory parameters and store the optional parameters
            if(curCmd == "t") {
                if(numPar != 1 && numPar != 2)
                    throw IOError("svg error: transform definition: invalid number of parameters for translate");
                if(numPar != 2) tokens.push_back("0");
            }
            else if(curCmd == "r") {
                if(numPar != 1 && numPar != 3)
                    throw IOError("svg error: transform definition: invalid number of parameters for rotate");
                if(numPar == 3) {
                    // Extract back the command
                    const std::string pary = tokens.back(); tokens.pop_back();
                    const std::string parx = tokens.back(); tokens.pop_back();
                    const std::string parr = tokens.back(); tokens.pop_back();
                    const std::string cmd  = tokens.back(); tokens.pop_back();
                    assert(cmd == "r");
                    // Put a translation
                    tokens.push_back("t");
                    tokens.push_back(parx);
                    tokens.push_back(pary);
                    // Put back the rotation
                    tokens.push_back("r");
                    tokens.push_back(parr);
                    // Put a reverse translation
                    tokens.push_back("t");
                    if(parx[0] == '-') tokens.push_back(parx.substr(1));
                    else               tokens.push_back("-" + parx    );
                    if(pary[0] == '-') tokens.push_back(pary.substr(1));
                    else               tokens.push_back("-" + pary    );
                }
            }
            else if(curCmd == "s") {
                if(numPar != 1 && numPar != 2)
                    throw IOError("svg error: transform definition: invalid number of parameters for scale");
                if(numPar != 2) tokens.push_back(tokens.back());
            }
            else if(curCmd == "wx") {
                if(numPar != 1)
                    throw IOError("svg error: transform definition: invalid number of parameters for skewX");
            }
            else if(curCmd == "wy") {
                if(numPar != 1)
                    throw IOError("svg error: transform definition: invalid number of parameters for skewY");
            }
            else if(curCmd == "m") {
                if(numPar != 6)
                    throw IOError("svg error: transform definition: invalid number of parameters for matrix");
            }
            // Clear the command and parameter tokens
            curCmd.clear();
            curPar.clear();
            // Change flags and reset the parameter count
            getCmd = true;
            gotAmp = false;
            numPar = 0;
            // Process the next character
            continue;
        }
        // Is the character is a ','?
        if(c == ',') {
            // Check for invalid location the character ','
            if(curCmd.empty() || curPar.empty())
                throw IOError("svg error: transform definition: unexpected location for character ','");
            // Store the parameter token
            tokens.push_back(lrtrimStdStr(curPar));
            ++numPar;
            // Clear the parameter token
            curPar.clear();
            // Process the next character
            continue;
        }
        // Is the character is a white-space or ';'?
        if(::isspace(c) || c == ';') {
            // Check for invalid location the character ';'
            if(gotAmp && c == ';')
                throw IOError("svg error: transform definition: unexpected location for character ';'");
            // A ' ' or ';' acts as a separator only if there is already a parameter defined
            if(!curPar.empty()) {
                // Store the parameter token
                tokens.push_back(lrtrimStdStr(curPar));
                ++numPar;
                // Clear the parameter token
                curPar.clear();
                // Consume the following white-space characters (if any)
                while(::isspace(*it)) ++it;
                // Consume the following ',' character (if any)
                if(*it == ',') ++it;
            }
            // Process the next character
            continue;
        }
        // Store the character as a command or parameter
        if(getCmd) curCmd += c;
        else       curPar += c;
    }

    // Check for an incomplete transform definition
    if(!curPar.empty() || !curPar.empty())
        throw IOError("svg error: transform definition: invalid/incomplete definition string");
}


} // namespace
} // namespace
