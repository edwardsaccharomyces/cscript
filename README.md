# cscript

7/23/2021: Thought I would upload this code that I enjoyed making. The purpose of it is to allow me to
merge the benefits of "scripting" with my enjoyment of c-style c++. It takes advantage of the Linux
trick that an executable text file whose first line begins with #!<path> will execute the file at
<path> and pass the text file as an argument. The file at <path> is the cscript bash file which just
looks for the hidden compiled version of this code, or compiles the code into that hidden file and then
runs it. This means I can write code and run it quickly, in a similar manner to how I would write
python code. Furthermore, the bash script includes some files of helper functions in the compilation,
which provide a rudamentary library of common functionality, again similar to python. Things like a 
simple "print()" function, string manipulation, and some other stuff.

I call it cscript, as in scripting in C.

Even thought I refer to it as a Just In Time compiler in the code, I understand that it isn't a proper
JIT. It just checks if there is a gcc based statically compiled version of the code that has a time
stamp greater than the input file, and compiles it if there isn't. Iterative coding edits are sped up
in that code is compiled and run in a single command. Running an already compiled file is as fast as
gcc -O2, which is pretty much the definition of fast.

