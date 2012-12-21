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

#ifndef __LSHKIT_MPLSH_FILE__
#define __LSHKIT_MPLSH_FILE__

#include <fstream>
#include <lshkit/lsh-file.h>
#include <lshkit/adaptor.h>
#include <lshkit/mplsh.h>

namespace lshkit {

class MultiProbeLshFile
{
public:
    static const std::string PARAM_EXT;

    struct Parameter: public MultiProbeLsh::Parameter
    {
        unsigned L;
        unsigned S;
        uint64_t B;
        Parameter () {}
        Parameter(const Parameter &param)
        {
            H = param.H;
            M = param.M;
            dim = param.dim;
            W = param.W;
            L = param.L;
            S = param.S;
            B = param.B;
        }

        Parameter(const std::string &path)
        {
            std::ifstream is(path.c_str());
            is >> H >> M >> dim >> W >> L >> S;
        };

        void dump (const std::string &path)
        {
            std::ofstream os(path.c_str());
            os << H << ' ' << M << ' ' << dim << ' ' << W << ' ' << L << ' ' <<
                S << std::endl;
        }
    };
private: 
    Parameter param_;
    std::vector<MultiProbeLsh> lshs_;
    LshFile<FloatVectorAdaptor> file_;
public:
    MultiProbeLshFile (const std::string &path, uint64_t cacheSize)
        : param_(path + PARAM_EXT),
        lshs_(param_.L), 
        file_(path, cacheSize, FloatVectorAdaptor(param_.dim))
    {
        DefaultRng rng;
        for (unsigned i = 0; i < param_.L; ++i)
            lshs_[i].reset(param_, rng);
    }

    MultiProbeLshFile (const std::string &path, const Parameter &param, uint64_t cacheSize)
        : param_(param), lshs_(param_.L), file_(path, param_.L, param_.H,
                    param_.B, cacheSize, FloatVectorAdaptor(param_.dim))
    {
        DefaultRng rng;
        for (unsigned i = 0; i < param_.L; ++i)
            lshs_[i].reset(param_, rng);
    }

    ~MultiProbeLshFile ()
    {
        param_.dump(file_.getPath() + PARAM_EXT);
    }

    void flush () { file_.flush(); }

    void append (uint64_t key, const float *obj)
    {
        for (unsigned i = 0; i < param_.L; ++i)
        {
            unsigned hash = lshs_[i](obj);
            //std::cout << i << ' ' << hash << ' ' << key << ' ' << obj[10] << std::endl;
            //file_.append(i, lshs_[i](obj), key, obj);
            file_.append(i, hash, key, obj);
        }
    }

    void batchQuery (std::vector<const float *> &queries,
            std::vector<Topk<uint64_t> > *topks,
            unsigned K, unsigned T, unsigned thread = 1);

    void show ();
};

}

#endif
