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

#include <cassert>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>

#include <lshkit/storage.h>

#include <iostream>

namespace lshkit
{
        uint64_t CachedBlockFile::Meta::timer = 0;

        char *CachedBlockFile::lock (uint32_t id, bool write, bool load)
        {
//            std::cout << "lock " << id << ' ' << write << ' ' << load << std::endl;
            Meta *meta = 0;
            {
                boost::unique_lock<boost::mutex> lock(mutex_);
                ++cnt;

                CacheSet::nth_index<0>::type::iterator it = cache_.find(id);
                // miss
                if (it == cache_.end())
                {
                    while (free_.empty())
                    {
                        CacheSet::nth_index<1>::type::iterator it1 =
                            cache_.get<1>().begin();
                        if ((it1 != cache_.get<1>().end()) && (it1->meta->ref ==
                                    0))
                        // we find an empty slot, swap it out
                        {
                            meta = it1->meta;
                            uint32_t id1 = it1->id;
                            cache_.get<1>().erase(it1);
                            if (meta->dirty)
                            {
                                lock.unlock();
                                File::write(uint64_t(id1)*blockSize_, blockSize_, meta->data);
                                lock.lock();
                            }
                            free_.push_back(meta);

                            break;
           //                 mem_.notify_one();
                        }
                        // start garbage collection
                        gc_.notify_all();
                        // wait for memory
                        mem_.wait(lock);
                    }

                    meta = free_.back();
                    meta->reset();
                    if (!cache_.insert(CacheItem(id, meta)).second) assert(0);

                    free_.pop_back();


                    if (!write) load = true;

                    // disable access to the block before data is loaded in
                    if (load) meta->mutex.lock();

                    it = cache_.find(id);
                    assert(it != cache_.end());

                }
                else
                {   // already loaded
                    assert(it->id == id);
                    ++hit;
                    load = false;
                    meta = it->meta;
                    ++meta->hit;
                }
                if (write) meta->dirty = true;
                meta->ref++;
                if (!cache_.modify(it, CacheItem::Sync())) assert(0);
            }

            if (load)
            {
                read(uint64_t(id) * blockSize_, blockSize_, meta->data);

                if (!write)
                {
                    meta->mutex.unlock();
                    meta->mutex.lock_shared();
                }
            }
            else
            {
                if (write) meta->mutex.lock();
                else meta->mutex.lock_shared();
            }

            return meta->data;
        }
        
        // modify for gc
        void CachedBlockFile::unlock (uint32_t id, bool write)
        {
//            std::cout << "unlock " << id << ' ' << write << std::endl;
            boost::unique_lock<boost::mutex> lock(mutex_);
            CacheSet::nth_index<0>::type::iterator it = cache_.find(id);
            assert(it->id == id);
            assert(it != cache_.end());
                // miss

            Meta *meta = it->meta;

            if (write) meta->mutex.unlock();
            else meta->mutex.unlock_shared();
            --meta->ref;
            cache_.modify(it, CacheItem::Sync());
        }

        char *CachedBlockFile::wlock_new (uint32_t *id)
        {
            uint32_t r;

            {
                boost::lock_guard<boost::mutex> lock(mutex_);
                r = *id = nextId_;
                if (nextId_ >= maxId_)
                {
                    maxId_ += inc_;
                    truncate(uint64_t(maxId_) * blockSize_);
                }
                ++nextId_;
            }

            return lock(r, true, false);
        }

        CachedBlockFile::CachedBlockFile (std::string path, uint64_t size,
                uint32_t blockSize, uint32_t inc)
            : File(path),
            blockSize_(blockSize), 
            nextId_(initSize() / blockSize_),
            maxId_(nextId_),
            metas_(size / blockSize_),
            data_(0), hit(0), cnt(0), inc_(inc)
        {
            assert(initSize() % blockSize_ == 0);
            size = metas_.size() * blockSize_;
            data_ = new char[size];
            assert(data_ != 0);
            for (unsigned i = 0; i < metas_.size(); ++i)
            {
                metas_[i].data = data_ + blockSize_ * i;
                free_.push_back(&metas_[i]);
            }
        }

        CachedBlockFile::~CachedBlockFile ()
        {
//            mutex_.lock();
            flush();
        //    assert(cache_.empty());
            // flush
            delete [] data_;
            std::cout << "HIT RATIO " << double(hit)/double(cnt) << std::endl;
        }

        void CachedBlockFile::flush ()
        {
            // FIXME
            boost::unique_lock<boost::mutex> lock(mutex_);
            for (CacheSet::nth_index<0>::type::iterator it = cache_.begin();
                    it != cache_.end(); ++it)
            {
                    Meta *meta = it->meta;
                    uint32_t id = it->id;
                    if (meta->dirty)
                    {
                        mutex_.unlock();
                        File::write(uint64_t(id)*blockSize_, blockSize_, meta->data);
                        mutex_.lock();
                        meta->dirty = false;
                    }
                    free_.push_back(meta);
            }
            File::truncate(uint64_t(nextId_) * blockSize_);
        }
}
