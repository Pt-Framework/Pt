/********************************************************************************
*                                                                               *
*                   U N I C O D E   C h a r a c t e r   I n f o                 *
*                                                                               *
*********************************************************************************
* Copyright (C) 2005,2006 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: fxunicode.h,v 1.4 2006/05/25 14:35:50 fox Exp $                          *
********************************************************************************/
#ifndef PT_TEXT_UNICODETABLE_H
#define PT_TEXT_UNICODETABLE_H

#include <Pt/Types.h>

namespace Pt {

namespace Text {

namespace Unicode {

// Get character category
int category(uint32_t ucs);

/*
/// Get character wide character direction
extern FXAPI FXuint charDirection(FXwchar ucs);

/// Get wide character decompose type
extern FXAPI FXuint decomposeType(FXwchar ucs);

/// Return number of wide characters in decomposition
extern FXAPI FXuint charNumDecompose(FXwchar ucs);

/// Return wide character decomposition
extern FXAPI const FXwchar* charDecompose(FXwchar ucs);

/// Return wide character composition from ucsa and ucsb
extern FXAPI FXwchar charCompose(FXwchar ucsa,FXwchar ucsb);

/// Get wide character joining
extern FXAPI FXuint joiningType(FXwchar ucs);

/// Get wide character symmetry
extern FXAPI FXuint isSymmetric(FXwchar ucs);

/// Get wide character combining type; zero means starter
extern FXAPI FXuint charCombining(FXwchar ucs);

/// Get numeric value of wide character (this includes hex value)
extern FXAPI FXint digitValue(FXwchar ucs);

/// Get linebreak type of wide character
extern FXAPI FXuint lineBreakType(FXwchar ucs);


/// Get mirror image of wide character or character itself
extern FXAPI FXwchar mirrorImage(FXwchar ucs);

/// Script type of wide character
extern FXAPI FXuint scriptType(FXwchar ucs);


/// Unicode flavor of common functions
extern FXAPI FXbool hasCase(FXwchar ucs);
extern FXAPI FXbool isUpper(FXwchar ucs);
extern FXAPI FXbool isLower(FXwchar ucs);
extern FXAPI FXbool isTitle(FXwchar ucs);
extern FXAPI FXbool isAscii(FXwchar ucs);
extern FXAPI FXbool isLetter(FXwchar ucs);
extern FXAPI FXbool isDigit(FXwchar ucs);
extern FXAPI FXbool isAlphaNumeric(FXwchar ucs);
extern FXAPI FXbool isControl(FXwchar ucs);
extern FXAPI FXbool isSpace(FXwchar ucs);
extern FXAPI FXbool isBlank(FXwchar ucs);
extern FXAPI FXbool isPunct(FXwchar ucs);
extern FXAPI FXbool isGraph(FXwchar ucs);
extern FXAPI FXbool isPrint(FXwchar ucs);
extern FXAPI FXbool isHexDigit(FXwchar ucs);
extern FXAPI FXbool isSymbol(FXwchar ucs);
extern FXAPI FXbool isMark(FXwchar ucs);
extern FXAPI FXbool isSep(FXwchar ucs);
*/

uint32_t toUpper(uint32_t ucs);

uint32_t toLower(uint32_t ucs);

uint32_t toTitle(uint32_t ucs);

}

}

}
#endif
