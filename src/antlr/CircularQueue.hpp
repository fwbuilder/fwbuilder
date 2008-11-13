#ifndef INC_CircularQueue_hpp__
#define INC_CircularQueue_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id$
 */

#include <antlr/config.hpp>
#include <antlr/Token.hpp>
#include <vector>
#include <cassert>
#include <stdexcept> // out_of_range exception

#include <iostream>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

// Resize every 5000 items
// -------------------- vk --------------------------------------------
// 11/12/2008: extend the size to make sure large policy files are
// parsed without resizing. Resizing causes crash because of call to
// elementAt with idx+m_offset = -1 (call comes from TokenBuffer::LT).
// I could not figure out why this happens so simpler fix is just to avoid
// resizing.
// -------------------- vk --------------------------------------------
#define OFFSET_MAX_RESIZE 1000000

template <class T>
class ANTLR_API CircularQueue {
public:
	CircularQueue()
	: storage()
	, m_offset(0)
	{
	}
	~CircularQueue()
	{
	}

	/// Clear the queue
	inline void clear( void )
	{
		m_offset = 0;
		storage.clear();
	}

	/// @todo this should use at or should have a check
        //
        // -------------------- vk --------------------------------------------
        // Fix for 64-bt systems. Originally argument was defined as
        // size_t idx; size_t is defined as unsigned integral type. This leads
        // to a crash when parser requests LT(0) at the very beginning of
        // the circular buffer because TokenBuffer::LT calls elementAt
        // with argument markerOffset+i-1, which is at that point equal
        // to -1. If idx is defined as size_t, it ends up equal to
        // 4294967295, which means we are looking past the end of the buffer.
        // -------------------- vk --------------------------------------------
	inline T elementAt( int idx ) const
	{
            return storage[idx+m_offset];
	}

	void removeFirst()
	{
		if (m_offset >= OFFSET_MAX_RESIZE)
		{
			storage.erase( storage.begin(), storage.begin() + m_offset + 1 );
			m_offset = 0;
		}
		else
			++m_offset;
	}

	inline void removeItems( size_t nb )
	{
		// it would be nice if we would not get called with nb > entries
		// (or to be precise when entries() == 0)
		// This case is possible when lexer/parser::recover() calls
		// consume+consumeUntil when the queue is empty.
		// In recover the consume says to prepare to read another
		// character/token. Then in the subsequent consumeUntil the
		// LA() call will trigger
		// syncConsume which calls this method *before* the same queue
		// has been sufficiently filled.
		if( nb > entries() )
			nb = entries();

		if (m_offset >= OFFSET_MAX_RESIZE)
		{
			storage.erase( storage.begin(), storage.begin() + m_offset + nb );
			m_offset = 0;
		}
		else
			m_offset += nb;
	}
	inline void append(const T& t)
	{
		storage.push_back(t);
	}
	inline size_t entries() const
	{
		return storage.size() - m_offset;
	}

private:
	ANTLR_USE_NAMESPACE(std)vector<T> storage;
	size_t m_offset;

	CircularQueue(const CircularQueue&);
	const CircularQueue& operator=(const CircularQueue&);
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_CircularQueue_hpp__
