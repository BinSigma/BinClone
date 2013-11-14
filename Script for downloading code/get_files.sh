#!/bin/bash

paths=(/usr/local/google/other_disk/winfiles/home/thomasdullien/Desktop/win_files/win7/ /usr/local/google/other_disk/winfiles/home/thomasdullien/Desktop/win_files/win8/ /usr/local/google/other_disk/winfiles/home/thomasdullien/Desktop/win_files/winVista/ /usr/local/google/other_disk/winfiles/home/thomasdullien/Desktop/win_files/WinXP/)
suffices=('*.exe' '*.dll' '/drivers/*.sys')
#suffices=('ntdll.dll')

for path in "${paths[@]}"; do
	for suffix in "${suffices[@]}"; do
		file_paths=$path$suffix
		echo "Processing $file_paths\n"
		for file_path in $file_paths; do
			echo "--Processing $file_path\n"
			var=`md5sum $file_path`
			md5=`echo $var | cut -d'/' -f1 | cut -c1-32`
			echo $md5
			file_name=`echo $var | rev | cut -d'/' -f1 | rev`
			target_name=$file_name'-'$md5
			echo $target_name
			# Now get the PDB file using pdbdownloader.py
			python ~/Desktop/temp/pdbparse-read-only/examples/symchk.py -e $file_path
			#cabextract `echo $file_name | cut -d'.' -f1`.pd_
			#rm `echo $file_name | cut -d'.' -f1`.pd_
			mv ./`echo $file_name | cut -d'.' -f1 | tr '[A-Z]' '[a-z]'`.pdb ./$target_name'.pdb'
			mv ./`echo $file_name | cut -d'.' -f1`.pdb ./$target_name'.pdb'
			cp $file_path ./$target_name
		done
	done
done

	

		
#for filename in /home/thomasdullien/Desktop/VMShared/win7.base.4th.of.April.2011/*.exe
#do
#	var=`md5sum $filename`
#	fname=`echo $var | cut -d'/' -f7 | cut -d' ' -f1` 
#	md5=`echo $var | cut -d'/' -f1 | cut -c1-32`
#	if $fname='drivers' then
#		fname=`echo $var | cut -d'/' -f8 | cut -d' ' -f1` 
#	fi
#	outvar=$md5'_'$fname
#	echo 'cp '$filename' ./'$outvar
#	cp $filename ./$outvar
#done
