
/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris D�rner                          *
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

#ifndef CHUNK_TEST_H
#define CHUNK_TEST_H

#undef PT_API_EXPORT

#include <Pt/Unit/TestSuite.h>

class ChunkTest : public Pt::Unit::TestSuite
{
public:
    ChunkTest() : Pt::Unit::TestSuite("ChunkTest")
    {
        Pt::Unit::TestSuite::registerMethod( "Test chunk initialization", *this, &ChunkTest::initTest );
        Pt::Unit::TestSuite::registerMethod( "Test chunk complex test for allocate and deallocate method", *this, &ChunkTest::complexAllocateDeallocateTest );
        Pt::Unit::TestSuite::registerMethod( "Test chunk allocate deallocate method", *this, &ChunkTest::allocateDeallocate );
        //Pt::Unit::TestSuite::registerMethod( "Test chunk deallocate method", *this, &ChunkTest::deallocate );
    }

protected:
    void initTest();    
    void complexAllocateDeallocateTest();
    void allocateDeallocate();
    //void deallocate();

};

#endif
