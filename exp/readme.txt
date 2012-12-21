Usage: CloneDetector <assemblyFolder> <clonePairsFilePath> <window size> <stride> <maxKOperands> <maxDeltaDistance> <maxOverlapFraction> <bFindExactClones> <bFindInexactClones>

        Format: <assemblyFolder:string>:input folder with assembly files, e.g., C:\\temp\\sample
		<clonePairsFile:string>:file path of output XML file containing clone pairs, e.g., C:\\temp\\sample\\clonepairs.xml
	        <windowSize:integer>:window size, e.g., 5
        	<stride:integer>:step size, e.g., 1
	        <maxKOperands:integer>:maximum k threshold, e.g., 5
		<maxDeltaDistance:real>:maximum delta threshold, e.g., 2000
	        <maxOverlapFraction:real>:maximum overlap threshold, e.g., 0.5
	        <bFindExactClones:boolean>:find exact clones with 2 possible values: TRUE or FALSE
	        <bFindInexactClones:boolean>:find inexact clones with 2 possible values: TRUE or FALSE
	        <regNormLevel:boolean>:register normalization level with 4 possible values: REG, TYPE, IDXPTR, or BITS
    