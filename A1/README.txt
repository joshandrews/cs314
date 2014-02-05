Windows:

The setup instructions for Windows assumes you are using Visual Studio 2012, which is freely available to all UBC students enrolled in a computer science course via Dreamspark. If you are using a different IDE, the installation should be similar but instruction will not be provided.

1) Download the assignment files from the course webpage

2) Create a folder somewhere for all the openGL stuff you're about to download (though probably NOT in your Assignment_1 folder since these libraries will be reused on future assignments). Create 2 subfolders inside, one called "include", the other "lib". I will refer to these as your "include" and "lib" folders.

3)
	a) Point your browser at http://glew.sourceforge.net/ and click on the link: Binaries Windows 32-bit and 64-bit. A download should start.

	b) Unpack the .zip file and open up the resulting folder. It should be called "glew-1.10.0". Inside are 3 important subfolders: bin, include, and lib. Let's start with bin. In glew-1.10.0>bin>Release>Win32 is a file called glew32.dll. DO NOT use glew-1.10.0>bin>Release>x64's glew32.dll, you must use the 32-bit version. If you are using 64-bit windows, copy this file to C:\Windows\SysWOW64. If you are using 32-bit windows, copy it to C:\Windows\System32.

	c) Open glew-1.10.0>include and copy the folder GL to your include folder you created in step 2).

	d) Open glew-1.10.0>lib>Release>Win32 and copy those 2 files (glew32.lib, glew32s.lib) to your lib folder you created in step 2).

4)
	a) Point your browser at http://www.transmissionzero.co.uk/software/freeglut-devel/ and click the link "Download freeglut 2.8.1-1 for MSVC" under the "freeglut 2.8.1 MSVC Package" heading. A download should start

	b) Unpack the .zip file and open up the resulting folder. It should be called "freeglut". Inside, as before, are 3 important subfolders: bin, include, and lib. In freeglut>bin (NOT freeglut>bin>x64), copy freeglut.dll to C:\Windows\SysWOW64 for 64-bit windows, C:\Windows\System32 for 32-bit.

	c) Open freeglut>include>GL and copy the 4 .h files into your include>GL folder

	d) Open freeglut>lib (NOT freeglut>lib>x64) and copy freeglut.lib to your lib folder.

5)
	a) Point your browser at http://sourceforge.net/projects/ogl-math/files/ and click the link: "Looking for the latest version?"

	b) Unpack the .zip file and open the resulting folder. It should be called "glm". Inside is only 1 important subfolder: glm. Copy this folder to your include folder.

6) Let's recap. You download glew and copy the dll to your SysWOW64/System32 directory, copy the include>GL folder to your include folder, and the .lib files to your lib folder. You download freeglut and copy the dll to your SysWOW64/System32 directory, the headers in freeglut>include>GL into your include>GL folder, and the .lib file in freeglut>lib into your lib folder. You download glm and put the glm>glm folder into your include folder. 

Your include folder should now have 2 subdirectories: GL (which contains freeglut and glew headers) and glm (which contains a bunch of math headers and some subfolders).

Your lib folder should now have freeglut.lib, glew32.lib, and glew32s.lib in it

Your SysWOW64 (or System32 for 32-bit windows) folder should now have freeglut.dll and glew32.dll in it

7) Now we just have to configure the project to look for your includes and libs. Open the Assignment_1.sln Visual Studio 2012 solution we provide.

	a) In the Solution Explorer, right click on the Assignment_1 project (not solution) and click "properties". Click on C/C++ (or C/C++->General) and edit the field for Additional Include Directories to be the path of your include folder. Note that if your path has spaces in any of the folder names, you'll have to surround the path with double quotes.

	b) Still in the properties dialog, click Linker (or Linker->General) and edit the field for Additional Library Directories to be the path of your lib folder.

8) Now cross your fingers and build your solution!



Linux:

The Linux instructions assume you're using the school machines, which already have freeglut and glew set up. If you are using Linux on your personal machine, you'll have to set up those libraries on your own. Their respective websites can be found in the Windows instructions above. Last time I checked, the school machines were still missing glm though, so you'll have to download that yourself:

Point your browser at http://sourceforge.net/projects/ogl-math/files/ and click the link: "Looking for the latest version?" Unpack the .zip file and open the resulting folder. It should be called "glm". Inside is only 1 important subfolder: also called "glm".

Open the Makefile provided with the assignment. You need to tell the compiler where to look for the glm headers, which you can do by editing the line LFLAGS to be:

LFLAGS = -lGL -lfreeglut -lGLEW -I path/to/glm/glm
