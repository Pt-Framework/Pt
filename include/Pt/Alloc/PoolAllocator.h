/***************************************************************************
 *   Copyright (C) 2008-2009 by Bendri Batti                               *
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

#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

#include <Pt/Alloc/Api.h>
#include <Pt/Allocator.h>

namespace Pt {
namespace Alloc {

class PoolFactory;

/**
 * @class PoolAllocator
 * Manages pool of fixed-size allocators.
 * Designed to be a non-templated base class of AllocatorSingleton so that
 * implementation details can be safely hidden in the source code file.
 */
class PT_ALLOC_API PoolAllocator : public Pt::Allocator
{
public:
    /** 
     * @brief The only available constructor needs certain parameters in order to
     *        initialize all the PoolFactory's.  
     * @param[in] pageSize # of bytes in a page of memory.
     * @param[in] maxObjectSize Max # of bytes which this may allocate.
     * @param[in] objectAlignSize # of bytes between alignment boundaries.
     */
    PoolAllocator(std::size_t pageSize, std::size_t maxObjectSize,
                      std::size_t objectAlignSize );

    /** 

     * @brief Destructor releases all blocks, all Chunks, and PoolFactory's.
     * 
     * Any outstanding blocks are unavailable, and should not be used after
     * this destructor is called.  The destructor is deliberately non-virtual
     * because it is protected, not public.
     */
    ~PoolAllocator();

    /** 
     * @brief Allocates a block of memory of requested size.  Complexity is often
     * constant-time, but might be O(C) where C is the number of Chunks in a
     * PoolFactory. 

     * @par Exception Safety Level
     * Provides either strong-exception safety, or no-throw exception-safety
     * level depending upon doThrow parameter.  The reason it provides two
     * levels of exception safety is because it is used by both the nothrow
     * and throwing new operators.  The underlying implementation will never
     * throw of its own accord, but this can decide to throw if it does not
     * allocate.  The only exception it should emit is std::bad_alloc.

     * @par Allocation Failure
     * If it does not allocate, it will call trimExcessMemory and attempt to
     * allocate again, before it decides to throw or return NULL.  Many
     * allocators loop through several new_handler functions, and terminate
     * if they can not allocate, but not this one.  It only makes one attempt
     * using its own implementation of the new_handler, and then returns NULL
     * or throws so that the program can decide what to do at a higher level.
     * (Side note: Even though the C++ Standard allows allocators and
     * new_handlers to terminate if they fail, the Pt allocator does not do
     * that since that policy is not polite to a host program.)

     * @param size # of bytes needed for allocation.
     * @param doThrow True if this should throw if unable to allocate, false
     * if it should provide no-throw exception safety level.
     * @return NULL if nothing allocated and doThrow is false.  Else the
     * pointer to an available block of memory.
     */
    void* allocate(std::size_t size);

    /** 
     * @brief Deallocates a block of memory at a given place and of a specific
     * size.  Complexity is almost always constant-time, and is O(C) only if
     * it has to search for which Chunk deallocates.  This never throws.
     */
    void deallocate(void* p, std::size_t size);


    /**
     * @brief Returns max # of bytes which this can allocate.
     *
     * @return Returns max # of bytes allocates by PoolFactorys.
     */
    std::size_t getMaxObjectSize() const
    { return _maxObjectSize; }

    /**
     * @brief Returns # of bytes between allocation boundaries.
     * @return Returns # of bytes between allocation boundaries.
     */
    std::size_t getAlignment() const 
    { return _objectAlignSize; }

    /** 
     * @brief Releases empty Chunks from memory.
     *
     * Complexity is O(F + C) where F is the count of PoolFactory's in the pool, and C is the number of
     * Chunks in all PoolFactory's.  This will never throw.
     * @return True if any memory released, or false if none released.
     */
    bool trim( void );

    /** 
     * @brief Returns true if anything in implementation is corrupt.
     *
     * Complexity is O(F + C + B) where F is the count of PoolFactory's in the pool,
     * C is the number of Chunks in all PoolFactory's, and B is the number
     * of blocks in all Chunks.  If it determines any data is corrupted, this
     * will return true in release version, but assert in debug version at
     * the line where it detects the corrupted data.  If it does not detect
     * any corrupted data, it returns false.
     */
    bool isCorrupt( void ) const;

private:
    /**
     * @brief Default-constructor is not implemented.
     */
    PoolAllocator(void);

    /** 
     * @brief Copy-constructor is not implemented.     
     */
    PoolAllocator(const PoolAllocator&);

    /** 
     * @brief Copy-assignment operator is not implemented.
     */
    PoolAllocator& operator = (const PoolAllocator&);

    /**
     * @brief Pointer to array of fixed-size allocators.
     */
    PoolFactory* _pool;

    /**
     * @brief Largest object size supported by allocators.
     */
    const std::size_t _maxObjectSize;

    /**
     * @brief Size of alignment boundaries.
     */
    const std::size_t _objectAlignSize;


};

} // namespace Alloc

} // namespace Pt
#endif

