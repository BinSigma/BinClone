/* 
    Copyright (C) 2008 Wei Dong <wdong@princeton.edu>. All Rights Reserved.
  
    This file is part of LSHKIT.
  
    LSHKIT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LSHKIT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LSHKIT.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __LSHKIT_STORAGE__
#define __LSHKIT_STORAGE__

#include <cassert>
#include <string>
#include <boost/multi_array.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition.hpp>
#include <stdint.h>


namespace lshkit {

    /// Basic file operations.  Sync. I/O, no buffering.
	class File
	{
		int fd_;
		std::string path_;
		uint64_t size_;
	public:

		File (std::string path);
		~File ();

		void read (uint64_t offset, uint64_t size, char *buf);
		void write (uint64_t offset, uint64_t size, char *buf);
        uint64_t initSize() const { return size_; }
        void truncate (uint64_t offset);
	};

    /// Cached, block-based I/O
    class CachedBlockFile: private File
    {

        /// Meta information of a block
        struct Meta
        {
            bool dirty;
            unsigned ref;
            unsigned hit;
            uint64_t time;
            boost::shared_mutex mutex;
            char *data;

            Meta () : ref(0), dirty(false), data(0), hit(0), time(0) {}

            // this is to make STL work. 
            Meta (const Meta &meta) { assert(meta.data == 0);}

            void reset () { dirty = false; ref = 0; hit = 0; time = timer++; }

            static uint64_t timer;
        };

        struct CacheItem
        {
            uint32_t id;
            Meta *meta;
            CacheItem (uint32_t id_, Meta *meta_)
                : id(id_), meta(meta_)
            {}
            class Sync
            {
            public:
                void operator() (CacheItem &c) const
                {}
                Sync() {}
            };
            bool operator < (const CacheItem &c) const
            {
                if (meta->ref < c.meta->ref) return true;
                if (meta->ref > c.meta->ref) return false;
                if ( meta->hit < c.meta->hit) return true;
                if ( meta->hit > c.meta->hit) return false;
                return (meta->timer < c.meta->timer);
            }
        };
        
        typedef std::map<uint32_t, Meta*> CacheMap;
        typedef boost::multi_index::multi_index_container <
            CacheItem,
            boost::multi_index::indexed_by<
                boost::multi_index::hashed_unique<boost::multi_index::member<CacheItem, uint32_t, &CacheItem::id> >,
                boost::multi_index::ordered_non_unique<boost::multi_index::identity<CacheItem> >
                > > CacheSet;

        CacheSet cache_;
        std::vector<Meta*> free_;

        uint64_t blockSize_;
        uint32_t nextId_;
        uint32_t maxId_;
        uint32_t inc_;
        std::vector<Meta> metas_;
        char *data_;
        

        /* mutex_ is to protect all the data structures including
           map_, free_, and the Meta information in map_, except for the
           data in the buffer (Meta::data), which are protected by Meta::mutex.
        */
        boost::mutex mutex_;

        boost::condition mem_; // memory available (free_ becomes non-empty
        boost::condition gc_; // start garbage collection

        // lock / unlock a block 
        char *lock (uint32_t id, bool write, bool load);
        void unlock (uint32_t id, bool write);


        unsigned cnt;
        unsigned hit;

    public:
        CachedBlockFile (std::string path, uint64_t size, uint32_t blockSize,
                uint32_t inc = 100);
        ~CachedBlockFile ();

        uint32_t getBlockSize () const { return blockSize_; }

        char *rlock (uint32_t id)
        {
            return lock(id, false, true);
        }

        void runlock (uint32_t id)
        {
            unlock(id, false);
        }

        char *wlock (uint32_t id)
        {
            return lock(id, true, true);
        }

        char *wlock_new (uint32_t *id);

        void wunlock (uint32_t id)
        {
            unlock(id, true);
        } 

        void flush ();
    };

}
#endif
