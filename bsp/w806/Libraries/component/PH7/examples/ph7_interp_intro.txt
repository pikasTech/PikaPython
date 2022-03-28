-------------------------
Introduction
-------------------------

The PH7 interpreter is a simple,basic stand-alone PHP interpreter that allows
the user to enter and execute PHP files against a PH7 engine. 
To start the ph7 program, just type "ph7" followed by the name of the PHP file
to compile and execute. That is, the first argument is to the interpreter, the rest
are scripts arguments, press "Enter" and the PHP code will be executed.
If something goes wrong while processing the PHP script due to a compile-time
error,your error output (STDOUT) should display the compile-time error messages.

Usage example of the ph7 interpreter:

Running the interpreter
ph7 scripts/hello_world.php
Running the interpreter with script arguments
ph7 scripts/mp3_tag.php /usr/local/path/to/my_mp3s

The PH7 interpreter package includes more than 70 PHP scripts to test ranging from
simple hello world programs to XML processing,ZIP archive extracting, MP3 tag 
extracting, UUID generation, JSON encoding/decoding, INI processing,Base32 
encoding/decoding and many more. These scripts are available in the scripts directory
from the zip archive.

------------------------
 Compile from source
------------------------

To compile the PH7 interpreter from source,you need the latest release of the PH7 engine
from http://ph7.symisc.net/downloads.html
After that, invoke your favourite compiler and generate the executable as follows:
     gcc -W -Wall -O6 -o ph7 ph7_interp.c ph7.c -D PH7_ENABLE_MATH_FUNC -lm
Don't forget to compile the PH7 engine with built-in math functions enabled
[i.e: sqrt(), abs(), etc. ]using the PH7_ENABLE_MATH_FUNC compile-time directive.
Finally, don't forget to compile with full optimizations enabled.
