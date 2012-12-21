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

#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <boost/timer.hpp>
#include <lshkit/common.h>
#include <lshkit/composite.h>
#include <lshkit/lsh.h>
#include <lshkit/flat-index.h>
#include <lshkit/matrix.h>
#include <lshkit/eval.h>

using namespace std;
using namespace lshkit;
namespace po = boost::program_options; 

class MatrixAccessor
{
    const Matrix<float> &matrix_;
public:
    typedef unsigned Key;
    MatrixAccessor(const Matrix<float> &matrix)
        : matrix_(matrix) {}

    MatrixAccessor(const MatrixAccessor &accessor)
        : matrix_(accessor.matrix_) {}

    const float *operator () (unsigned key)
    {
        return matrix_[key];
    }
};

class L2 
{
    unsigned dim_;

    float sqr (float x) const { return x * x; }
public:
    L2 (unsigned dim): dim_(dim) {}

    L2 (const L2 &l2): dim_(l2.dim_) {}

    float operator () (const float *first1, const float *first2) const
    {
        float r = 0.0;
        for (unsigned i = 0; i < dim_; ++i)
        {
            r += sqr(first1[i] - first2[i]);
        }
        return sqrt(r);
    }
};

int main (int argc, char *argv[])
{
    string data_file;
    string benchmark;

    float W;
    unsigned M, L, H;
    unsigned Q, K;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message.")
		(",W", po::value<float>(&W)->default_value(1.0), "")
		(",M", po::value<unsigned>(&M)->default_value(1), "")
		(",L", po::value<unsigned>(&L)->default_value(1), "")
		(",H", po::value<unsigned>(&H)->default_value(1017881), "")
		(",Q", po::value<unsigned>(&Q)->default_value(1), "")
		(",K", po::value<unsigned>(&K)->default_value(1), "")
		("data,D", po::value<string>(&data_file), "")
		("benchmark,B", po::value<string>(&benchmark), "")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);	

	if (vm.count("help") || (vm.count("data") < 1) || (vm.count("benchmark") < 1))
	{
		cout << desc;
		return 0;
	}

    cout << "Loading data...";
    Matrix<float> data(data_file);
    cout << "done." << endl;

    Benchmark<> bench(K, Q);
    cout << "Loading benchmark...";
    bench.load(benchmark);
    cout << "done." << endl;

    for (unsigned i = 0; i < Q; ++i)
    {
        for (unsigned j = 0; j < K; ++j)
        {
            assert(bench.getAnswer(i)[j].key < data.getSize());
        }
    }

    cout << "Initializing index..." << endl;

    typedef Tail<RepeatHash<GaussianLsh> > Lsh;
    Lsh::Parameter param;

    param.first = H;
    param.second.first = M;
    param.second.second.W = W;
    param.second.second.dim = data.getDim();
    DefaultRng rng;

    MatrixAccessor accessor(data);
    L2 l2(data.getDim());

    FlatIndex<Lsh, MatrixAccessor, L2> index(param, rng, accessor, l2, L);

    cout << "done." << endl;

    cout << "Populating index..." << endl;

    {
        boost::progress_display progress(data.getSize());
        for (unsigned i = 0; i < data.getSize(); ++i)
        {
            index.insert(i);
            ++progress;
        }
    }

    cout << "Running queries..." << endl;

    Stat recall;
    Stat cost;
    boost::timer timer;
    timer.restart();
    Topk<unsigned> topk;

    {
        boost::progress_display progress(Q);
        for (unsigned i = 0; i < Q; ++i)
        {
            unsigned cnt;
            topk.reset(K+1);
            index.query(data[bench.getQuery(i)], topk, &cnt);
            recall << bench.getAnswer(i).recall(topk);
            cost << double(cnt)/double(data.getSize());
            ++progress;
        }
    }

    cout << "[RECALL] " << recall.getAvg() << " ± " << recall.getStd() << endl;
    cout << "[COST] " << cost.getAvg() << " ± " << cost.getStd() << endl;

    return 0;
}

