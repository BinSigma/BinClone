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
#include <lshkit/common.h>
#include <lshkit/mplsh-file.h>
#include <lshkit/matrix.h>
#include <lshkit/eval.h>
#include <sys/time.h> 

class Timer
{
	struct	timeval	start; 
public:
    Timer () {}
    void tick ()
    {
	    gettimeofday(&start, 0); 
    }
    void tuck (const char *msg) const
    {
        struct timeval end;
	    float	diff; 
	    gettimeofday(&end, 0); 

	    diff = (end.tv_sec - start.tv_sec) 
	   			+ (end.tv_usec - start.tv_usec) * 0.000001; 
        std::cout << msg << ':' <<  diff << std::endl;
    }
};

using namespace std;
using namespace lshkit;
namespace po = boost::program_options; 


int main (int argc, char *argv[])
{
    Timer timer;
    string data_file;
    string query_file;
    string benchmark;

    unsigned K, Q, T, P;
    uint64_t cacheSize;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message.")
		(",C", po::value<uint64_t>(&cacheSize)->default_value(4096*1024*10), "")
		(",P", po::value<unsigned>(&P)->default_value(1), "")
		(",Q", po::value<unsigned>(&Q)->default_value(1), "")
		(",K", po::value<unsigned>(&K)->default_value(1), "")
		(",T", po::value<unsigned>(&T)->default_value(1), "")
		("data,D", po::value<string>(&data_file), "")
		("benchmark,B", po::value<string>(&query_file), "")
	    ("result,R", po::value<string>(&benchmark), "")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);	

	if (vm.count("help") || (vm.count("data") < 1) || (vm.count("benchmark") < 1) ||
            (vm.count("result") < 1) )
	{
		cout << desc;
		return 0;
	}

    timer.tick();
    Matrix<float> matrix(query_file);

    Benchmark<uint64_t> bench(K, Q);
    bench.load(benchmark);
    timer.tuck("Load benchmark");

    timer.tick();
    MultiProbeLshFile lsh(data_file, cacheSize);
    timer.tuck("Initialize LSH");

    vector<Topk<uint64_t> > topks(Q);
    vector<const float *> queries(Q);

    for (unsigned i = 0; i < Q; ++i)
        queries[i] = matrix[bench.getQuery(i)];

    timer.tick();
    lsh.batchQuery(queries, &topks, K, T, P);
    timer.tuck("Query");

    cout << "Evaluating results...";
    Stat recall;
    for (unsigned i = 0; i < Q; ++i)
    {
        recall << bench.getAnswer(i).recall(topks[i]);
    }
    cout << "Done." << endl;

    cout << "[RECALL] " << recall.getAvg() << " ± " << recall.getStd() << endl;

    return 0;
}

