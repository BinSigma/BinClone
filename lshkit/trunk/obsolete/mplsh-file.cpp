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
#include <map>
#include <boost/bind.hpp>
#include <threadpool.hpp>
#include <lshkit/common.h>
#include <lshkit/metric.h>
#include <lshkit/mplsh-file.h>

namespace lshkit
{

    const std::string MultiProbeLshFile::PARAM_EXT(".param");

    class ProbeCallback
    {
            const std::vector<const float *> &queries_;
            std::vector<Topk<uint64_t> > *topks_;
            boost::mutex *mutexes_;
            const std::vector<unsigned> &indices_;
            const metric::l2<float> &l2_;
        public:
            ProbeCallback(
                const std::vector<const float *> &queries,
                std::vector<Topk<uint64_t> > *topks,
                boost::mutex *mutexes,
                const std::vector<unsigned> &indices,
                const metric::l2<float> &l2)
                : queries_(queries), topks_(topks), mutexes_(mutexes),
                indices_(indices), l2_(l2)
            {
            }

            void operator () (uint64_t key, const float *obj)
            {
    //            std::cerr << "Q:" << indices_.size()  << std::endl;
                for (unsigned i = 0; i < indices_.size(); ++i)
                {
//                    std::cerr << key << std::endl;
                    unsigned q = indices_[i];
                    Topk<uint64_t>::Element elem(key, l2_(queries_[q],
                                obj));
            //        if (elem < topks_->at(q).back())
                    {
                        boost::unique_lock<boost::mutex> lock(mutexes_[q]);
                        topks_->at(q) << elem;
                    }
                }
            }
    };

    class Show 
    {
        unsigned l_, h_;
    public:
        Show (unsigned l, unsigned h): l_(l), h_(h) {}
        void operator() (uint64_t key, const float *obj)
        {
            std::cout << l_ << ' ' << h_ << ' ' << key << ' ' << obj[10] << std::endl;
        }
    };

    void MultiProbeLshFile::batchQuery (std::vector<const float *> &queries,
               std::vector<Topk<uint64_t> > *topks, unsigned K, unsigned T,
               unsigned thread)
    {
        assert(queries.size() == topks->size());

        typedef std::map< std::pair<unsigned, unsigned>, std::vector<unsigned> > Queue;
        Queue queue;

        boost::mutex *mutexes = new boost::mutex[queries.size()];

        std::vector<unsigned> seq;
        std::cout << "Generating probe sequences...";
        for (unsigned i = 0; i < queries.size(); ++i)
        {
            topks->at(i).reset(K);
            for (unsigned j = 0; j < param_.L; ++j)
            {
                lshs_[j].genProbeSequence(queries[i], seq, T);
                for (unsigned k = 0; k < seq.size(); ++k)
                {
                    queue[std::pair<unsigned,unsigned>(j, seq[k])].push_back(i);
                }
            }
        }
        std::cout << "done." << std::endl;
        std::cout << "Probing..." << std::endl;

        boost::threadpool::thread_pool<> tp(thread);

        metric::l2<float> l2(param_.dim);
        for (Queue::const_iterator it = queue.begin(); it != queue.end(); ++it)
        {
            tp.schedule(boost::bind(&LshFile<FloatVectorAdaptor>::scan<ProbeCallback>,
                        boost::ref(file_), it->first.first, it->first.second, 
                        ProbeCallback(queries, topks, mutexes, it->second, l2)));
        }
        tp.wait();
        std::cout << "done." << std::endl;

        delete[] mutexes;
    }

    void MultiProbeLshFile::show ()
    {

        for (unsigned i = 0; i < param_.L; ++i)
            for (unsigned j = 0; j < param_.H; ++j)
            {
                file_.scan<Show>(i, j, Show(i, j));
            }
    }

}
