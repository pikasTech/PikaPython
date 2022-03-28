/*
 * Compile this file together with the ph7 engine source code to generate
 * the simple PH7 CGI interpreter executable. For example: 
 *  gcc -D PH7_ENABLE_MATH_FUNC -o ph7 ph7_cgi.c ph7.c -lm
 *
 * The PH7 CGI interpreter (ph7_cgi.c) is based on PH7 interpreter (ph7_interp.c).
 *
 * Copyright (C) 2015-2016, 谢致邦 (XIE Zhibang) <Yeking@Red54.com>
 */

#include <stdio.h>
#include <stdlib.h>
/* Make sure this header file is available.*/
#include "ph7.h"

static void CgiHeader()
{
	puts("X-Powered-By: " PH7_SIG "\r");
	puts("Content-type: text/html; charset=UTF-8\r\n\r");
}
/* 
 * Display an error message and exit.
 */
static void Fatal(int status, const char *zMsg)
{
	if(status)
		puts("Status: 404 Not Found");
	else
		puts("Status: 500 Internal Service Error");
	CgiHeader();
	puts(zMsg);
	/* Shutdown the library */
	ph7_lib_shutdown();
	/* Exit immediately */
	exit(0);
}
/*
 * Banner.
 */
static const char zBanner[] = {
	"============================================================\n"
	"Simple PH7 CGI Interpreter                                  \n"
	"============================================================\n"
};
/*
 * Display the banner,a help message and exit.
 */
static void Help(void)
{
	puts(zBanner);
	puts("ph7 [-h|-r|-d] path/to/php_file [script args]");
	puts("\t-d: Dump PH7 byte-code instructions");
	puts("\t-r: Report run-time errors");
	puts("\t-h: Display this message an exit");
	/* Exit immediately */
	exit(0);
}
#ifdef __WINNT__
#include <Windows.h>
#else
/* Assume UNIX */
#include <unistd.h>
#endif
/*
 * VM output consumer callback.
 * Each time the virtual machine generates some outputs,the following
 * function gets called by the underlying virtual machine to consume
 * the generated output.
 * This function is registered later via a call to ph7_vm_config()
 * with a configuration verb set to: PH7_VM_CONFIG_OUTPUT.
 */
static int Output_Consumer(const void *pOutput,unsigned int nOutputLen,void *pUserData /* Unused */)
{
	printf("%.*s", nOutputLen, pOutput);
	return PH7_OK;
}
/*
 * Main program: Compile and execute the PHP file. 
 */
int main(int argc,char **argv)
{
	ph7 *pEngine; /* PH7 engine */
	ph7_vm *pVm;  /* Compiled PHP program */
	int dump_vm = 0;    /* Dump VM instructions if TRUE */
	int err_report = 0; /* Report run-time errors if TRUE */
	int n;              /* Script arguments */
	int rc;
	/* Process interpreter arguments first*/
	for(n = 1 ; n < argc ; ++n ){
		int c;
		if( argv[n][0] != '-' ){
			/* No more interpreter arguments */
			break;
		}
		c = argv[n][1];
		if( c == 'd' || c == 'D' ){
			/* Dump byte-code instructions */
			dump_vm = 1;
		}else if( c == 'r' || c == 'R' ){
			/* Report run-time errors */
			err_report = 1;
		}else{
			/* Display a help message and exit */
			Help();
		}
	}
	if( n >= argc ){
		puts("Missing PHP file to compile");
		Help();
	}
	/* Allocate a new PH7 engine instance */
	rc = ph7_init(&pEngine);
	if( rc != PH7_OK ){
		/*
		 * If the supplied memory subsystem is so sick that we are unable
		 * to allocate a tiny chunk of memory,there is no much we can do here.
		 */
		Fatal(0, "Error while allocating a new PH7 engine instance");
	}
	/* Set an error log consumer callback. */
	ph7_config(pEngine,PH7_CONFIG_ERR_OUTPUT,
		Output_Consumer, /* Error log consumer */
		0 /* NULL: Callback Private data */
		);
	/* Now,it's time to compile our PHP file */
	rc = ph7_compile_file(
		pEngine, /* PH7 Engine */
		argv[n], /* Path to the PHP file to compile */
		&pVm,    /* OUT: Compiled PHP program */
		0        /* IN: Compile flags */
		);
	if( rc != PH7_OK ){ /* Compile error */
		if( rc == PH7_IO_ERR ){
			Fatal(1, "IO error while opening the target file");
		}else if( rc == PH7_VM_ERR ){
			Fatal(0, "VM initialization error");
		}else{
			Fatal(0, "Compile error");
		}
	}
	/*
	 * Now we have our script compiled,it's time to configure our VM.
	 */
	rc = ph7_vm_config(pVm,
		PH7_VM_CONFIG_OUTPUT,
		Output_Consumer,    /* Output Consumer callback */
		0                   /* Callback private data */
		);
	if( rc != PH7_OK ){
		Fatal(0, "Error while installing the VM output consumer callback");
	}
	/* Register script agruments so we can access them later using the $argv[]
	 * array from the compiled PHP program.
	 */
	for( n = n + 1; n < argc ; ++n ){
		ph7_vm_config(pVm,PH7_VM_CONFIG_ARGV_ENTRY,argv[n]/* Argument value */);
	}
	if( err_report ){
		/* Report script run-time errors */
		ph7_vm_config(pVm,PH7_VM_CONFIG_ERR_REPORT);
	}
	if( dump_vm ){
		/* Dump PH7 byte-code instructions */
		ph7_vm_dump_v2(pVm,
			Output_Consumer, /* Dump consumer callback */
			0
			);
	}
	CgiHeader();
	/*
	 * And finally, execute our program.
	 */
	ph7_vm_exec(pVm,0);
	/* All done, cleanup the mess left behind.
	*/
	ph7_vm_release(pVm);
	ph7_release(pEngine);
	return 0;
}
