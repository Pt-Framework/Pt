/*
 * Copyright (C) 2009-2010 by Bendri Batti
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_POOLALLOCATOR_H
#define PT_POOLALLOCATOR_H

#include <Pt/Api.h>
#include <Pt/Allocator.h>
#include <Pt/Types.h>
#include <Pt/SmartPtr.h>

class AllocatorTest;

namespace Pt {

class PoolFactory;
class Chunk;

/** @brief Manages a pool of fixed-size allocators.

    Designed to be a non-templated base class of AllocatorSingleton so that
    implementation details can be safely hidden in the source code file.
 */
class PT_API PoolAllocator : public Pt::Allocator
{
	friend class ::AllocatorTest;
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
    inline std::size_t getMaxObjectSize() const
    { return _maxObjectSize; }

    /**
     * @brief Returns # of bytes between allocation boundaries.
     * @return Returns # of bytes between allocation boundaries.
     */
    inline std::size_t getAlignment() const 
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

protected:
   
   /** @brief Proxy class for Chunk

    Use to test chunk class. This proxy class is needed under windows, because Chunk symbol
	is not exported.
	*/
	class PT_API ChunkProxy
	{
	public:
		ChunkProxy();
		~ChunkProxy();
		
		void init(std::size_t blockSize, Pt::uint8_t blocks);
		void* allocate( std::size_t blockSize );
		void deallocate(void* p, std::size_t blockSize);
		void reset( std::size_t blockSize, Pt::uint8_t blocks );    
		void release();
		bool operator== (const ChunkProxy& rhs);
	
#ifndef NDEBUG
		
		bool isCorrupt( Pt::uint8_t numBlocks, std::size_t blockSize, bool checkIndexes ) const;
		bool isBlockAvailable(void* p, Pt::uint8_t numBlocks, std::size_t blockSize) const;
#endif
		
		bool hasBlock( void* p, std::size_t chunkLength ) const;
		bool hasAvailable(Pt::uint8_t numBlocks ) const;
		bool isFilled() const;	
		const Pt::uint8_t blocksAvailable() const;
		const Pt::Chunk& chunk();

	private:
		Pt::SmartPtr<Pt::Chunk> _chunk;
	};


};

} // namespace Pt
#endif

