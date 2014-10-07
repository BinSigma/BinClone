This procedure descrbies how to setup BinClone

Step 1: Install PostgreSQL

1.1 - Install PostgreSQl v8.4 (32-bit) in the PostgreSQL folder. This will create the "postgres" account. Give it a password. 

1.2 - Run pgAdmin III from Start --> PostgreSQL. Take a look at "Databases". You should have a default database called "postgres". 

1.3 - Run PostgreSQL\SetupDB.bat. You have to enter the password specified in Step 1.1 twice. This will create a database called "CSDataBase" and the required database tables. 

1.4 - Verify that the database called CSDataBase is created in pgAdminIII.

---------------

Step 2: Populate assembly files into CSDataBase

2.1 - Edit BinClone\AssemblyToDB.bat. Replace "<DB_PWD>" with the database password specified in Step 1.1.

2.2 - Execute BinClone\AssemblyToDB.bat to run a test case. By default, it will run TestCaseA. Alternatively, you can change the parameters as follows.
	
	2.2.1: <DB_PWD>: database password

	2.2.2: <ASSEMBLY_FOLDER_PATH>: absolute path to the folder where you store your assembly files, e.g., "C:\BinClone Installation\TestCases\TestCaseA(Small)\AssemblyFiles".

 	2.2.3: <NORMALIZATION_LEVEL>: Possible values are REG, TYPE, IDXPTR or BITS. Recommended value: REG, i.e., all the registers will be generalized to the symbol "REG". 

	2.2.4: <WINDOWSIZE>: Size of the sliding window. (Recommended value: 20).

	2.2.5: <STEPSIZE>: Step size of the sliding window. (Keep it as 1).


-- Note that you can populate one and only one set of files (a folder) to the database, but you may populate the same set of files multiple times with different combinations of parameters.
-- The current implementation does not support incrementally updating the database. If you need to add new assembly files to the database, then you will need to drop the current database tables, run DropDB.bat, run SetupDB.bat, and re-populate all the assembly files, together with the new files, into the database again.


---------------

Step 3: Run BinClone

3.1 - Run "BinClone\BinClone.exe", which is the GUI of BinClone. 

3.2 - You can choose one of the 3 modes from the File menu. 

	3.2.1: Search clones in file: First, you need to enter your database password. You have the option to choose either "Exact clones" or "Inexact clones" or both of them. If you choose "Inexact clones", you need to enter the "Min. co-occurrence" threshold as well. 
	       You need to select your list of database parameters in "Select parameters" section. These parameter are fetched from the ones you specified in Step 2.1.
	       Finally, you need to choose your target assembly file in the "TargetFile" folder of each test case. This is the file for which you want to search clones in the database. 

	3.2.2: Search clones in fragment: In the section, you can write your target code and find its clones in the database. Parameters selection is the same as in Step 3.2.1. Some sample code fragements are provided for each test case.

	3.2.3: Search token: You can search for a specific token (string, constant or import) from the files populated in the database. 



