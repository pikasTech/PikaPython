PH7 - An Embedded Implementation of PHP (C Library).
===
[![Build Status](https://travis-ci.org/symisc/PH7.svg?branch=master)](https://travis-ci.org/symisc/PH7)

PH7 is a in-process software C library which implements a highly-efficient embeddable bytecode compiler and a virtual machine for the PHP programming language. In other words, PH7 is a PHP engine which allow the host application to compile and execute PHP scripts in-process. PH7 is to PHP what SQLite is to SQL.

PH7 implements most of the constructs introduced by the PHP 5.3 release such as heredoc, nowdoc, gotos, classes, anonymous functions, closures and so on and introduces very [powerful extensions](http://ph7.symisc.net/features.html) to the PHP programming language such as:


 * Function & Method Overloading.
 * Full Type Hinting.
 * Introducing comma expressions.
 * Introducing the eq and ne operators for strict string comparison.
 * Improved operators precedences.
 * Powerful OO subsystem.
 * Function arguments can take any complex expressions as their default values.
 * 64-bit integer arithmetic for all platforms.
 * Native UTF-8 support.
 * Written in ANSI C, thread-safe, full-reentrant; compile and run unmodified in any platform including restricted embedded devices with a C compiler.
 * Amalgamation: All C source code for PH7 are combined into a single source file.
 * Built with more 470 function including an XML parser (with namespace support), INI processor, CSV reader/writer, UTF-8 encoder/decoder, zip archive extractor, JSON encoder/decoder, random number/strings generator, native and efficient File IO for Windows and UNIX systems and many more without the need of any external library to link with.
 * PH7 is an Open-Source product.
 

As an embedded interpreter, it allows multiple interpreter states to coexist in the same program, without any interference between them. Programmatically, foreign functions in C/C++ can be added and values can be defined in the PHP environment. Being a quite small program, it is easy to comprehend, get to grips with, and use. 

PH7 History - Why PHP?
=========================

PH7 was developed in the early 2011 by a Tunisian startup for a French conglomerate which was looking for a way to power the web interface of their commercial routers.
Previously the technology used by the conglomerate was obsolete and based on a minimal HTTP server (micro-httpd written by Jef Poskanzer) and a hard-coded binary (the web interface) served via old CGI interface.
This poor combination shows its limitation when the company decided to switch to a user-configurable (self config) routers where everything must be done via the web interface. This involve a dynamic web UI that have to deal with many issues.
In order to keep the low level software layer of the router untouched, it was decided to write all the web interface in a dynamic web oriented programming language and the compilation result to be executed by a virtual machine (PH7) and thus leaving the low-level stack of the router untouched.


PH7 in 5 Minutes or Less
=========================
Here is what you do to start experimenting with the PH7 engine without having to do a lot of tedious reading and configuration:

Below is a simple C program that demonstrates how to use the C/C++ interface to PH7. This program compile and execute the following PHP script:
```php
<?php
 echo 'Welcome guest'.PHP_EOL;
 echo 'Current system time is: '.date('Y-m-d H:i:s').PHP_EOL;
 echo 'and you are running '.php_uname();
?>
```

That is, this simple PHP script when running should display a greeting message, the current system time and the host operating system. A typical output of this program would look like this:

     Welcome guest
     Current system time is: 2012-09-14 10:08:44
     and you are running Microsoft Windows 7 localhost 6.1 build 7600 x86

Here is the C code. Note that you can get a working version of this program [here](http://www.symisc.net/downloads/ph7_intro.c):

```c
/* Compile this file together with the ph7 engine source code to generate
* the executable. For example:
* gcc -W -Wall -O6 -o ph7_test ph7_intro.c ph7.c
*/
/*
* This simple program is a quick introduction on how to embed and start
* experimenting with the PH7 engine without having to do a lot of tedious
* reading and configuration.
*
* For an introduction to the PH7 C/C++ interface, please refer to this page
* http://ph7.symisc.net/api_intro.html
* For the full C/C++ API reference guide, please refer to this page
* http://ph7.symisc.net/c_api.html
*/
/*
* The following is the PHP program to execute.
* <?php
* echo 'Welcome guest'.PHP_EOL;
* echo 'Current system time is: '.date('Y-m-d H:i:s').PHP_EOL;
* echo 'and you are running '.php_uname();
* ?>
* That is, this simple program when running should display a greeting
* message, the current system time and the host operating system.
* A typical output of this program would look like this:
*
* Welcome guest
* Current system time is: 2012-09-14 02:08:44
* and you are running Microsoft Windows 7 localhost 6.1 build 7600 x86
*
*/
#define PHP_PROG "<?php "\
"echo 'Welcome guest'.PHP_EOL;"\
"echo 'Current system time is: '.date('Y-m-d H:i:s').PHP_EOL;"\
"echo 'and you are running '.php_uname();"\
"?>"
/* Make sure you have the latest release of the PH7 engine
* from:
* http://ph7.symisc.net/downloads.html
*/
#include <stdio.h>
#include <stdlib.h>
/* Make sure this header file is available.*/
#include "ph7.h"
/*
* Display an error message and exit.
*/
static void Fatal(const char *zMsg)
{
  puts(zMsg);
  /* Shutdown the library */
  ph7_lib_shutdown();
  /* Exit immediately */
   exit(0);
 }
/*
* VM output consumer callback.
* Each time the virtual machine generates some outputs, the following
* function gets called by the underlying virtual machine to consume
* the generated output.
* All this function does is redirecting the VM output to STDOUT.
* This function is registered later via a call to ph7_vm_config()
* with a configuration verb set to: PH7_VM_CONFIG_OUTPUT.
*/
static int Output_Consumer(const void *pOutput, unsigned int nOutputLen, void *pUserData /* Unused */)
{
  /*
   * Note that it's preferable to use the write() system call to display the output
   * rather than using the libc printf() which everybody now is extremely slow.
   */
  printf("%.*s",
      nOutputLen,
      (const char *)pOutput /* Not null terminated */
   );
    /* All done, VM output was redirected to STDOUT */
    return PH7_OK;
 }
/*
* Main program: Compile and execute the PHP program defined above.
*/
int main(void)
{
  ph7 *pEngine; /* PH7 engine */
  ph7_vm *pVm; /* Compiled PHP program */
  int rc;
  /* Allocate a new PH7 engine instance */
  rc = ph7_init(&pEngine);
  if( rc != PH7_OK ){
   /*
    * If the supplied memory subsystem is so sick that we are unable
    * to allocate a tiny chunk of memory, there is no much we can do here.
    */
   Fatal("Error while allocating a new PH7 engine instance");
  }
  /* Compile the PHP test program defined above */
  rc = ph7_compile_v2(
      pEngine, /* PH7 engine */
      PHP_PROG, /* PHP test program */
      -1 /* Compute input length automatically*/,
      &pVm, /* OUT: Compiled PHP program */
      0 /* IN: Compile flags */
   );
  if( rc != PH7_OK ){
    if( rc == PH7_COMPILE_ERR ){
      const char *zErrLog;
      int nLen;
     /* Extract error log */
     ph7_config(pEngine,
       PH7_CONFIG_ERR_LOG,
       &zErrLog,
       &nLen
     );
   if( nLen > 0 ){
     /* zErrLog is null terminated */
     puts(zErrLog);
    }
  }
  /* Exit */
  Fatal("Compile error");
}
/*
 * Now we have our script compiled, it's time to configure our VM.
 * We will install the output consumer callback defined above
 * so that we can consume and redirect the VM output to STDOUT.
 */
rc = ph7_vm_config(pVm,
      PH7_VM_CONFIG_OUTPUT,
      Output_Consumer, /* Output Consumer callback */
      0 /* Callback private data */
   );
  if( rc != PH7_OK ){
     Fatal("Error while installing the VM output consumer callback");
  }
/*
* And finally, execute our program. Note that your output (STDOUT in our case)
* should display the result.
*/
ph7_vm_exec(pVm,0);
/* All done, cleanup the mess left behind.
*/
ph7_vm_release(pVm);
ph7_release(pEngine);
return 0;
}
```


We create a new [PH7 engine instance](http://ph7.symisc.net/c_api_func.html#ph7_init) using a call to [ph7_init()](http://ph7.symisc.net/c_api_func.html#ph7_init) on line 86. This is often the first PH7 API call that an application makes and is a prerequisite in order to compile PHP code using one of the compile interfaces.


We compile our PHP test program on line 95 using the [ph7_compile_v2()](http://ph7.symisc.net/c_api_func.html#ph7_compile_v2) interface.


We [configure our Virtual Machine](http://ph7.symisc.net/c_api_func.html#ph7_vm_config) on line 125 by setting a [VM output consumer callback](http://ph7.symisc.net/output_consumer.html) named Output_Consumer(). All this callback does is redirecting the VM output to STDOUT using the libc printf() routine or the write() system call.


And finally we execute our PHP program on line 137 using a call to [ph7_vm_exec()](http://ph7.symisc.net/c_api_func.html#ph7_vm_exec). You should see now the greeting message, the current date and the host operating system.


Clean-up is done on line 140 and 141 respectively via calls to [ph7_vm_release()](http://ph7.symisc.net/c_api_func.html#ph7_vm_release) and [ph7_release()](http://ph7.symisc.net/c_api_func.html#ph7_release).

Now, Compile this C file together with the PH7 engine source code to generate the executable. For example:

**gcc -W -Wall -O6 -o ph7_test ph7_intro.c ph7.c**

When running [./ph7_test ] you should see the greeting message, the current system time and the host operating system.

The PH7 [download](http://ph7.symisc.net/downloads.html) page includes a simple stand-alone PHP interpreter named ph7 (or ph7.exe on windows) that allows the user to enter and execute PHP files against a PH7 engine. This utility is available in prebuilt binaries forms or can be compiled from source. You can get a copy of the PH7 interpreter from the download page.

To start the ph7 program, just type "ph7" followed by the name of the PHP file to compile and execute. That is, the first argument is to the interpreter, the rest are scripts arguments, press "Enter" and the PHP code will be executed.

If something goes wrong while processing the PHP script due to a compile-time error, your error output (STDOUT) should display the compile-time error messages.


Usage example of the ph7 interpreter:

Running the interpreter

**ph7 scripts/hello_world.php**

Running the interpreter with script arguments

**ph7 scripts/mp3_tag.php /usr/local/path/to/my_mp3s**

Useful links to start with
===========================

[Download](http://ph7.symisc.net/downloads.html) 	:	Get a copy of the last public release of the PH7 engine, start embedding and enjoy programming with.

[Distinctive Features](http://ph7.symisc.net/features.html): 	This document enumerates and describes some of the features and the powerfull extensions introduced by the PH7 engine.


[Frequently Asked Questions](http://ph7.symisc.net/faq.html): 	FAQ: The title of the document says all...


[Copyright/Licensing](http://ph7.symisc.net/licensing.html): 		PH7 is dual-licensed and is available free of charge for open source projects. Find more on the licensing situation there.

[Online Community Support](http://ph7.symisc.net/support.html): 	Need some help, join the PH7 online community.

PH7 Programming Interfaces
==========================

Documentation describing the APIs used to program PH7. Note that PH7 is very easy to learn, even for new programmer. Here is some useful links to start with:


[PH7 In 5 Minutes Or Less](http://ph7.symisc.net/intro.html):  Gives a high-level overview on the how to embed the PH7 engine in a host application.

[An Introduction To The PH7 C/C++ Interface](http://ph7.symisc.net/api_intro.html): Gives an overview and roadmap to the C/C++ interface to PH7.

[C/C++ API Reference Guide](http://ph7.symisc.net/c_api.html):  This document describes each API function in details.

[Foreign Function Implementation](http://ph7.symisc.net/func_intro.html): Is a how-to guide on how to install C functions and invoke them from your PHP script.

[Constant Expansion Mechanism](http://ph7.symisc.net/const_intro.html): 	Is a how-to guide on how to install foreign constants and expand their values from your PHP script.
