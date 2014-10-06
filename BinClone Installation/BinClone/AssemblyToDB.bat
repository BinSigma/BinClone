@echo off
REM USAGE: .\AssemblyToDB.exe CSDataBase postgres <DB_PWD> <ABSOLUTE_ASSEMBLY_FOLDER_PATH> <NORMALIZATION_LEVEL(REG, TYPE, IDXPTR or BITS)> <WINDOWSIZE> <STEPSEIZE> > output.txt

@echo on
.\AssemblyToDB.exe CSDataBase postgres <DB_PWD> "C:\BinClone Installation\TestCases\TestCaseA(Small)\AssemblyFiles" REG 20 1 > output.txt
