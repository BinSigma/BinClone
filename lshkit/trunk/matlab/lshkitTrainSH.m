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

function SHparam = lshkitTrainSH (train_path, hash_path, nbits)
%
% Input
%	train_path: path of training data
%	hash_path: path to output hash function
%	nbits: # bits in hash function
%
% To use this program, you need to download Y. Weiss' Spectral Hashing
% matlab code and unzip the code to somewhare matlab can find.
%
% http://www.cs.huji.ac.il/~yweiss/SpectralHashing/
%
% The input format is  LSHKIT matrix format.  The output file
% can be loaded by the lshkit::SpectralHash::serialze method.

data = lshkitLoadMatrix(train_path);

dim = size(data);

SHparam.nbits = nbits;
SHparam = trainSH(data, SHparam);

fid = fopen(hash_path, 'wb');


fwrite(fid, nbits, 'uint32');
for i=1:nbits
	fwrite(fid, dim(2), 'uint32');
	fwrite(fid, SHparam.pc(:,i), 'float32');
end

fwrite(fid, nbits, 'uint32');
fwrite(fid, SHparam.mn, 'float32');
fwrite(fid, nbits, 'uint32');

omega0=pi./(SHparam.mx-SHparam.mn);
omegas=SHparam.modes.*repmat(omega0, [nbits 1]);
omegas=omegas';

for i=1:nbits
	fwrite(fid, nbits, 'uint32');
	fwrite(fid, omegas(:,i), 'float32');
end

fclose(fid)
