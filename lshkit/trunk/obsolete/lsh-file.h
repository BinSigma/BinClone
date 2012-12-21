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

#ifndef __LSHKIT_INDEX__
#define __LSHKIT_INDEX__

#include <cassert>
#include <string>
#include <boost/multi_array.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <stdint.h>
#include <lshkit/common.h>
#include <lshkit/storage.h>

namespace lshkit {
	/* 
	class Adaptor
	{
		typedef ... Domain;
		const Domain *read(const uint8_t *buf, uint64_t *size);
		void write(uint8_t *buf, uint64_t *size, const Domain &);
        Adaptor (const Adaptor &);
	};
	*/
    template <typename ADAPTOR>
	class LshFile
	{
        static const std::string META_EXT;
        static const std::string BLOCK_EXT;

		struct Bin
		{
			uint32_t count;
            boost::shared_mutex mutex;
            std::vector<uint32_t> blocks;
            Bin (): count(0) {};
            Bin (const Bin &bin)
                : count(bin.count)
            {
                assert(count == 0);
            }
            Bin &operator = (const Bin &bin) { assert(0); }
		};

		typedef boost::multi_array<Bin, 2> Tables;

		struct Block
		{
			uint64_t size;
			uint8_t data[0];
		};

        struct Meta
        {
            uint64_t blockSize;
		    Tables tables;
            
            Meta (std::string path)
            {
                unsigned numTable, tableSize;
                std::ifstream is(path.c_str());
                is >> numTable >> tableSize >> blockSize;
                tables.resize(boost::extents[numTable][tableSize]);
                for (unsigned i = 0; i < numTable; ++i)
                    for (unsigned j = 0; j < tableSize; ++j)
                    {
                        unsigned size;
                        Bin &bin = tables[i][j];
                        is >> bin.count >> size;
                        bin.blocks.resize(size);
                        for (unsigned k = 0; k < size; ++k)
                        {
                            is >> bin.blocks[k];
                        }
                    }
            }
            Meta (unsigned numTable, unsigned tableSize, uint64_t blockSize_)
                : tables(boost::extents[numTable][tableSize]),blockSize(blockSize_) {}
            void dump (std::string path)
            {
                std::ofstream os(path.c_str());
                unsigned numTable = tables.size();
                unsigned tableSize = tables[0].size();
                os << numTable << ' ' << tableSize << ' ' << blockSize << std::endl;
                for (unsigned i = 0; i < numTable; ++i)
                    for (unsigned j = 0; j < tableSize; ++j)
                    {
                        Bin &bin = tables[i][j];
                        os << bin.count << ' ' << bin.blocks.size();
                        for (unsigned k = 0; k < bin.blocks.size(); ++k)
                        {
                            os << ' ' <<  bin.blocks[k];
                        }
                        os << std::endl;
                    }
            }
        };

        std::string path_;
        Meta meta_;
		CachedBlockFile file_;
        boost::shared_mutex mutex_;
        ADAPTOR adaptor_;
	public:
        typedef typename ADAPTOR::Domain Domain;

        const std::string &getPath () { return path_; }

        LshFile (std::string path, uint64_t cacheSize, ADAPTOR adaptor)
            :
              path_(path),
              meta_(path + META_EXT),
              file_(path + BLOCK_EXT, cacheSize, meta_.blockSize),
              adaptor_(adaptor)
        {
        }

		LshFile (std::string path, unsigned numTable, unsigned tableSize,
                uint64_t blockSize, uint64_t cacheSize, ADAPTOR adaptor)
			: 
              path_(path),
              meta_(numTable, tableSize, blockSize),
			  file_(path + BLOCK_EXT, cacheSize, blockSize),
              adaptor_(adaptor)
		{
		}

		~LshFile()
		{
            mutex_.lock();
		}

        void flush ()
        {
            file_.flush();
            meta_.dump(path_ + META_EXT);
        }

		void append (unsigned table, unsigned hash, uint64_t key, Domain object)
        {
            Bin &bin = meta_.tables[table][hash];
            bin.mutex.lock();
            Block *block = 0;
            bool done = false;
            if (!bin.blocks.empty())
            {
                block = reinterpret_cast<Block*>(file_.wlock(bin.blocks.back()));
                uint64_t size = meta_.blockSize - block->size;
                if (size > sizeof(uint64_t))
                {
                    uint64_t newsize = size = size - sizeof(uint64_t);
                    adaptor_.write(block->data + block->size + sizeof(uint64_t), &newsize, object);
                    if (newsize < size)
                    {
                        *reinterpret_cast<uint64_t *>(block->data + block->size) = key;
                        block->size += newsize + sizeof(uint64_t);
                        done = true;
                    }
                 }
                 file_.wunlock(bin.blocks.back());
            }
            if (!done)
            {
                uint32_t id;
                block = reinterpret_cast<Block*>(file_.wlock_new(&id));
                block->size = 0;
                bin.blocks.push_back(id);
                uint64_t size = meta_.blockSize - block->size;
                uint64_t newsize = size = size - sizeof(uint64_t);
                adaptor_.write(block->data + block->size + sizeof(uint64_t), &newsize, object);
                if (newsize < size)
                {
                    *reinterpret_cast<uint64_t *>(block->data + block->size) = key;
                    block->size += newsize + sizeof(uint64_t);
                    done = true;
                }
                file_.wunlock(bin.blocks.back());
            }
            verify(done);
            ++bin.count;
            bin.mutex.unlock();
        }

        template <typename CALLBACK>
        void scan (unsigned table, unsigned hash, CALLBACK callback)
        {
            Bin &bin = meta_.tables[table][hash];
            for (unsigned i = 0; i < bin.blocks.size(); ++i)
            {
//                std::cout << '.';
                uint32_t id = bin.blocks[i];
                Block *block = reinterpret_cast<Block*>(file_.rlock(id));
                uint64_t offset = 0;
                while (offset < block->size)
                {
                    uint64_t key = *reinterpret_cast<uint64_t *>(block->data +
                            offset);
                    offset += sizeof(uint64_t);
                    uint64_t s = meta_.blockSize - offset;

                    callback(key, adaptor_.read(block->data + offset, &s));
                    offset += s;
                }
                verify(offset == block->size);
                file_.runlock(id);
            }
        }
	};

    template <typename ADAPTOR>
    const std::string LshFile<ADAPTOR>::META_EXT(".meta");
    template <typename ADAPTOR>
    const std::string LshFile<ADAPTOR>::BLOCK_EXT(".block");
}


#endif
