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
#include <lshkit/mplsh-file.h>
#include <lshkit/matrix.h>

using namespace std;
using namespace lshkit;
namespace po = boost::program_options; 

int main (int argc, char *argv[])
{
    string data_file;
    string lsh_file;

    MultiProbeLshFile::Parameter param;

    uint64_t cacheSize;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message.")
		(",W", po::value<float>(&param.W)->default_value(1.0), "")
		(",M", po::value<unsigned>(&param.M)->default_value(1), "")
		(",L", po::value<unsigned>(&param.L)->default_value(1), "")
		(",H", po::value<unsigned>(&param.H)->default_value(1017881), "")
		(",B", po::value<uint64_t>(&param.B)->default_value(4096), "")
		(",C", po::value<uint64_t>(&cacheSize)->default_value(4096*1024*10), "")
		("input,I", po::value<string>(&data_file), "")
		("output,O", po::value<string>(&lsh_file), "")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);	

	if (vm.count("help") || (vm.count("input") < 1) || (vm.count("output") < 1))
	{
		cout << desc;
		return 0;
	}

    cout << "Loading data...";
    Matrix<float> data(data_file);
    cout << "done." << endl;

    param.dim = data.getDim();
    param.S = 0;

    MultiProbeLshFile lsh(lsh_file, param, cacheSize);

    cout << "Importing..." << endl;
    boost::progress_display progress(data.getSize());
#pragma omp parallel for schedule(guided, 1) default(shared)
    for (int i = 0; i < data.getSize(); ++i)
    {
        lsh.append(i, data[i]);
#pragma omp critical
        ++progress;
    }
    cout << "Done." << endl;

    lsh.flush();

    return 0;
}

