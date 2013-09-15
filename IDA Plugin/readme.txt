1.Install IDA SDK (version 6 or +)
2.There will be a plugins directory created (like C:\Program Files (x86)\IDA\plugins). This is the place to put your plugin binaries.
3.Open the CloneSearcher plugin project from VS
4.Update the project properties to reflect your IDA pro SDK installation directory:
	a.Right click the CloneSearcher plugin project and select Properties;
	b.Linker->General: Update the path to the SDK VC library “vc.w32” to Additional Library Directories. 
		e.g.  “C:\idp pro\idasdk60a\idasdk60\lib\vc.w32” in my case.
5.Build the project.
6.Copy the CloneSearcher.plw from Release to your IDA Pro plugin directory as mentioned in #2.
7.Start IDA Pro and open any binary file as wish.
8.After IDA Pro has finished processing the binary, click on any function from the left-hand side “Functions window”.
9.From the IDA main window, click on any view tab “like IDA View-A” and right click to change to “Text view” if it is on the “Graph view”.
10.Launch our plugin from the menu->Edit->Plugins->Clone Searcher. 


Here you go.
Happy IDAing...