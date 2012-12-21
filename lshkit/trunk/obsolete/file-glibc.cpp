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

#include <lshkit/common.h>
#include <lshkit/storage.h>

namespace lshkit
{

    File::File (std::string path)
        : path_(path)
    {
        fd_ = ::open(path.c_str(), O_CREAT | O_DIRECT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP);
        verify(fd_ >= 0);
        struct stat st;

        if (::fstat(fd_, &st) != 0) verify(0);

        size_ = st.st_size;
    }

    File::~File ()
    {
        close(fd_);
    }

    void File::read (uint64_t offset, uint64_t size, char *buf)
    {
        uint64_t r = ::pread(fd_, buf, size, offset);
        verify(r == size);
    }
    
    void File::write (uint64_t offset, uint64_t size, char *buf)
    {
        uint64_t r = ::pwrite(fd_, buf, size, offset);
        verify(r == size);
    }

    void File::truncate (uint64_t offset)
    {
        int ret = ::ftruncate(fd_, offset);
        verify(ret == 0);
    }

    /*
    void File::truncate (uint64_t offset)
    {
        ftruncate(fd_, offset);
        size_ = offset;
    }
    */
}



