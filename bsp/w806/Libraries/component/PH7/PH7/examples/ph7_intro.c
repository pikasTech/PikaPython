/*
 * Compile this file together with the ph7 engine source code to generate
 * the executable. For example: 
 *  gcc -W -Wall -O6 -o ph7_test ph7_intro.c ph7.c
*/
/*
 * This simple program is a quick introduction on how to embed and start
 * experimenting with the PH7 engine without having to do a lot of tedious
 * reading and configuration.
 *
 * For an introduction to the PH7 C/C++ interface, please refer to this page
 *        http://ph7.symisc.net/api_intro.html
 * For the full C/C++ API reference guide, please refer to this page
 *        http://ph7.symisc.net/c_api.html
 */
/*
 * The following is the PHP program to execute.
 *   <?php
 *    echo 'Welcome guest'.PHP_EOL;
 *    echo 'Current system time is: '.date('Y-m-d H:i:s').PHP_EOL;
 *    echo 'and you are running '.php_uname();
 *   ?>
 * That is, this simple program when running should display a greeting
 * message, the current system time and the host operating system.
 * A typical output of this program would look like this:
 *
 *	Welcome guest
 *	Current system time is: 2012-09-14 02:08:44
 *	and you are running Microsoft Windows 7 localhost 6.1 build 7600 x86
 *
 */
#define PHP_PROG "<?php "\
                 "echo 'Welcome guest'.PHP_EOL;"\
                 "echo 'Current system time is: '.date('Y-m-d H:i:s').PHP_EOL;"\
                 "echo 'and you are running '.php_uname();"\
                 "?>"
/* Make sure you have the latest release of the PH7 engine
 * from:
 *  http://ph7.symisc.net/downloads.html
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
 * function gets called by the underlying virtual machine  to consume
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
	ph7_vm *pVm;  /* Compiled PHP program */
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
		pEngine,  /* PH7 engine */
		PHP_PROG, /* PHP test program */
		-1        /* Compute input length automatically*/, 
		&pVm,     /* OUT: Compiled PHP program */
		0         /* IN: Compile flags */
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
	 * We will install the VM output consumer callback defined above
	 * so that we can consume the VM output and redirect it to STDOUT.
	 */
	rc = ph7_vm_config(pVm, 
		PH7_VM_CONFIG_OUTPUT, 
		Output_Consumer,    /* Output Consumer callback */
		0                   /* Callback private data */
		);
	if( rc != PH7_OK ){
		Fatal("Error while installing the VM output consumer callback");
	}
	/*
	 * And finally, execute our program. Note that your output (STDOUT in our case)
	 * should display the result.
	 */
	ph7_vm_exec(pVm, 0);
	/* All done, cleanup the mess left behind.
	*/
	ph7_vm_release(pVm);
	ph7_release(pEngine);
	return 0;
}
