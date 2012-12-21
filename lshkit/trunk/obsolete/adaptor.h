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

#ifndef __LSHKIT_ADAPTOR__
#undef __LSHKIT_ADAPTOR__

#include <cstdlib>

namespace lshkit {

template <typename T>
class PodVectorAdaptor
{
    unsigned size_;
public:
    typedef const T *Domain;

    PodVectorAdaptor (unsigned dim) : size_(dim * sizeof(T)) {}
    PodVectorAdaptor (const PodVectorAdaptor &adap) : size_(adap.size_) {}

    Domain read (const uint8_t *buf, uint64_t *size)
    {
        if (*size < size_) return 0;
        *size = size_;
        return reinterpret_cast<Domain>(buf);
    }

    void write (uint8_t *buf, uint64_t *size, Domain obj)
    {
        if (*size > size_)
        {
            std::memcpy(buf, reinterpret_cast<const uint8_t*>(obj), size_);
        }
        *size = size_;
    }
};

typedef PodVectorAdaptor<float> FloatVectorAdaptor;


}

#endif

