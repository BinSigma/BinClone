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

#include <limits>
#include <iostream>
#include <lshkit/mplsh.h>
#include <boost/program_options.hpp>

using namespace std;
using namespace lshkit;
namespace po = boost::program_options; 

ostream &operator << (ostream &os, const Probe &p)
{
    os << '[';
    for (unsigned i = 0; i < 32; ++i)
    {
        if (p.mask & (1 << i))
        {
            os << i;
            if (p.shift & (1 << i))
            {
                os << '+';
            }
            else
            {
                os << '-';
            }
        }
    }
    os << "]\t";
    os << p.score;
    return os;
}

int main (int argc, char *argv[])
{
    unsigned M;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message.")
		(",M", po::value<unsigned>(&M)->default_value(5), "")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);	

	if (vm.count("help"))
	{
        cout << "This program prints the full template probe sequence." << endl;
		cout << desc;
		return 0;
	}


    ProbeSequence seq;
    GenProbeSequenceTemplate(seq, M, numeric_limits<unsigned>::max());
    for (ProbeSequence::const_iterator it = seq.begin(); it != seq.end(); ++it)
    {
        cout << *it << endl;
    }
    return 0;
}
