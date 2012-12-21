% 
%    Copyright (C) 2009 Wei Dong <wdong@princeton.edu>. All Rights Reserved.
%  
%    This file is part of LSHKIT.
%  
%    LSHKIT is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    LSHKIT is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with LSHKIT.  If not, see <http://www.gnu.org/licenses/>.
%

function m = lshkitLoadMatrix (path)
%
% Input
%	path: path to LSHKIT matrix.
%
% Warning: This program only works with float32 matrix.

fid = fopen(path, 'rb');

header = fread(fid, 3, 'uint32');

fseek(fid, 12, 'bof');

raw_data = fread(fid, [header(3), header(2)], 'float32');

fclose(fid);

m = raw_data';

