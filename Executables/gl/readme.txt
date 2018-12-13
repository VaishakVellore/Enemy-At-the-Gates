
1. Copy glut32.dll to somewhere in your PATH like c:\windows\system32.

2. The files in the include and lib directories should be moved to the usual 
   directories:

   <VISUAL STUDIO>/VC/include

   <VISUAL STUDIO>/VC/lib


3. In the "properties" of your project modify 
   "configuration properties" / "VC++ directories"
   Add in the "include directories" and the "library directories":

   <VISUAL STUDIO>/VC/include

   <VISUAL STUDIO>/VC/lib