/*
 * Compile this file together with the ph7 engine source code to generate
 * the executable. For example: 
 *  gcc -W -Wall -O6 -o ph7_test ph7_func_intro.c ph7.c
 */
/*
 * This simple program is a quick introduction to the foreign functions and their related
 * interfaces.
 * For an introductory course to the [ph7_create_function()] interface and the foreign function
 * mechanism in general, please refer to:
 *  http://ph7.symisc.net/func_intro.html
 */
/* $SymiscID: ph7_func_intro.c v2.1 Win7 2012-09-06 23:30 stable <chm@symisc.net> $ */
/*
 * Make sure you have the latest release of the PH7 engine
 * from:
 *  http://ph7.symisc.net/downloads.html
 * Make sure this header file is available.
 */
#include "ph7.h"
/*
 * int shift_func(int $num)
 *  Right shift a number by one and return the result.
 * Description
 *  Our first function perform a simple right shift operation on a given number
 *  and return that number shifted by one.
 *  This function expects a single parameter which must be numeric (either integer or float
 *  or a string that looks like a number).
 * Parameter
 *  $num
 *   Number to shift by one.
 * Return value
 *   Integer: Given number shifted by one.
 * Usage example:
 * <?php
 *   echo shift_func(150); //return 300
 *   echo shift_func(50);  //return 100
 * ?>
 */
int shift_func(
	ph7_context *pCtx, /* Call Context */
	int argc,          /* Total number of arguments passed to the function */
	ph7_value **argv   /* Array of function arguments */
	)
{
	int num;
	/* Make sure there is at least one argument and is of the
	 * expected type [i.e: numeric].
	 */
	if( argc < 1 || !ph7_value_is_numeric(argv[0]) ){
		/*
		 * Missing/Invalid argument, throw a warning and return FALSE.
		 * Note that you do not need to log the function name, PH7 will
		 * automatically append the function name for you.
		 */
		ph7_context_throw_error(pCtx, PH7_CTX_WARNING, "Missing numeric argument");
		/* Return false */
		ph7_result_bool(pCtx, 0);
		return PH7_OK;
	}
	/* Extract the number */
	num = ph7_value_to_int(argv[0]);
	/* Shift by 1 */
	num <<= 1;
	/* Return the new value */
	ph7_result_int(pCtx, num);
	/* All done */
	return PH7_OK;
}
#include <time.h>
/*
 * string date_func(void)
 *  Return the current system date.
 * Description
 *  Our second function does not expects arguments and return the
 *  current system date.
 * Parameter
 *  None
 * Return value
 *   String: Current system date.
 * Usage example
 * <?php
 *   echo date_func(); //would return 2012-23-09 14:53:30
 * ?>
 */
int date_func(
	ph7_context *pCtx, /* Call Context */
	int argc,          /* Total number of arguments passed to the function */
	ph7_value **argv   /* Array of function arguments*/
	){
		time_t tt;
		struct tm *pNow;
		/* Get the current time */
		time(&tt);
		pNow = localtime(&tt);
		/* 
		 * Return the current date.
		 */
		ph7_result_string_format(pCtx, 
			"%04d-%02d-%02d %02d:%02d:%02d", /* printf() style format */
			pNow->tm_year + 1900, /* Year */
			pNow->tm_mday,        /* Day of the month */
			pNow->tm_mon + 1,     /* Month number */
			pNow->tm_hour, /* Hour */
			pNow->tm_min,  /* Minutes */
			pNow->tm_sec   /* Seconds */
			);
		/* All done */
		return PH7_OK;
}
/*
 * int64 sum_func(int $arg1, int $arg2, int $arg3, ...)
 *  Return the sum of the given arguments.
 * Description
 *  This function expects a variable number of arguments which must be of type
 *  numeric (either integer or float or a string that looks like a number) and
 *  returns the sum of the given numbers.
 * Parameter 
 *   int $n1, n2, ... (Variable number of arguments)
 * Return value
 *   Integer64: Sum of the given numbers.
 * Usage example
 * <?php
 *   echo sum_func(7, 8, 9, 10); //would return 34
 * ?>
 */
int sum_func(ph7_context *pCtx, int argc, ph7_value **argv)
{
	ph7_int64 iTotal = 0; /* Counter */
	int i;
	if( argc < 1 ){
		/*
		 * Missing arguments, throw a notice and return NULL.
		 * Note that you do not need to log the function name, PH7 will
		 * automatically append the function name for you.
		 */
		ph7_context_throw_error(pCtx, PH7_CTX_NOTICE, "Missing function arguments $arg1, $arg2, ..");
		/* Return null */
		ph7_result_null(pCtx);
		return PH7_OK;
	}
	/* Sum the arguments */
	for( i = 0; i < argc ; i++ ){
		ph7_value *pVal = argv[i];
		ph7_int64 n;
		/* Make sure we are dealing with a numeric argument */
		if( !ph7_value_is_numeric(pVal) ){
			/* Throw a notice and continue */
			ph7_context_throw_error_format(pCtx, PH7_CTX_NOTICE, 
				"Arg[%d]: Expecting a numeric value", /* printf() style format */
				i
				);
			/* Ignore */
			continue;
		}
		/* Get a 64-bit integer representation and increment the counter */
		n = ph7_value_to_int64(pVal);
		iTotal += n;
	}
	/* Return the count  */
	ph7_result_int64(pCtx, iTotal);
	/* All done */
	return PH7_OK;
}
/*
 * array array_time_func(void)
 *  Return the current system time in an array.
 * Description
 *  This function does not expects arguments and return the
 *  current system time in an array.
 * Parameter
 *  None
 * Return value
 *   Array holding the current system time.
 * Usage example
 * <?php
 *   var_dump( array_time_func() ); 
 * ?>
 * When running you should see something like that:
 * array(3) {
 *  [0] =>
 *   int(14)
 *  [1] =>
 *   int(53)
 *  [2] =>
 *   int(30)
 * }
 */
int array_time_func(ph7_context *pCtx, int argc, ph7_value **argv)
{
	ph7_value *pArray;    /* Our Array */
	ph7_value *pValue;    /* Array entries value */
	time_t tt;
	struct tm *pNow;
	/* Get the current time first */
	time(&tt);
	pNow = localtime(&tt);
	/* Create a new array */
	pArray = ph7_context_new_array(pCtx);
	/* Create a worker scalar value */
	pValue = ph7_context_new_scalar(pCtx);
	if( pArray == 0 || pValue == 0 ){
		/*
		 * If the supplied memory subsystem is so sick that we are unable
		 * to allocate a tiny chunk of memory, there is no much we can do here.
		 * Abort immediately.
		 */
		ph7_context_throw_error(pCtx, PH7_CTX_ERR, "Fatal, PH7 is running out of memory");
		/* emulate the die() construct */
		return PH7_ABORT; /* die('Fatal, PH7 is running out of memory'); */
	}
	/* Populate the array.
	 * Note that we will use the same worker scalar value (pValue) here rather than
	 * allocating a new value for each array entry. This is due to the fact
	 * that the populated array will make it's own private copy of the inserted
	 * key(if available) and it's associated value.
	 */
	
	ph7_value_int(pValue, pNow->tm_hour); /* Hour */
	/* Insert the hour at the first available index */
	ph7_array_add_elem(pArray, 0/* NULL: Assign an automatic index*/, pValue /* Will make it's own copy */);

	/* Overwrite the previous value */
	ph7_value_int(pValue, pNow->tm_min); /* Minutes */
	/* Insert minutes */
	ph7_array_add_elem(pArray, 0/* NULL: Assign an automatic index*/, pValue /* Will make it's own copy */);

	/* Overwrite the previous value */
	ph7_value_int(pValue, pNow->tm_sec); /* Seconds */
	/* Insert seconds */
	ph7_array_add_elem(pArray, 0/* NULL: Assign an automatic index*/, pValue /* Will make it's own copy */);

	/* Return the array as the function return value */
	ph7_result_value(pCtx, pArray);

	/* All done. Don't worry about freeing memory here, every
	 * allocated resource will be released automatically by the engine
	 * as soon we return from this foreign function.
	 */
	return PH7_OK;
}
/*
 * array array_date_func(void)
 *  Return a copy of the 'struct tm' structure in an associative array.
 * Description
 *  This function does not expects arguments and return a copy of
 *  the 'struct tm' structure found in the 'time.h' header file.
 *  This structure hold the current system date&time.
 * Parameter
 *  None
 * Return value
 *   Associative array holding a copy of the 'struct tm' structure.
 * Usage example
 * <?php
 *   var_dump( array_date_func() ); 
 * ?>
 * When running you should see something like that:
 * array(6) {
 * [tm_year] =>
 *  int(2012)
 * [tm_mon] =>
 *  int(9)
 * [tm_mday] =>
 *  int(23)
 * [tm_hour] =>
 *  int(15)
 * [tm_min] =>
 *  int(53)
 * [tm_sec] =>
 *  int(30)
 * }
 */
int array_date_func(ph7_context *pCtx, int argc, ph7_value **argv)
{
	ph7_value *pArray;    /* Our Array */
	ph7_value *pValue;    /* Array entries value */
	time_t tt;
	struct tm *pNow;
	/* Get the current time first */
	time(&tt);
	pNow = localtime(&tt);
	/* Create a new array */
	pArray = ph7_context_new_array(pCtx);
	/* Create a worker scalar value */
	pValue = ph7_context_new_scalar(pCtx);
	if( pArray == 0 || pValue == 0 ){
		/*
		 * If the supplied memory subsystem is so sick that we are unable
		 * to allocate a tiny chunk of memory, there is no much we can do here.
		 * Abort immediately.
		 */
		ph7_context_throw_error(pCtx, PH7_CTX_ERR, "Fatal, PH7 is running out of memory");
		/* emulate the die() construct */
		return PH7_ABORT; /* die('Fatal, PH7 is running out of memory'); */
	}
	/* Populate the array.
	 * Note that we will use the same worker scalar value (pValue) here rather than
	 * allocating a new value for each array entry. This is due to the fact
	 * that the populated array will make it's own private copy of the inserted
	 * key(if available) and it's associated value.
	 */
	
	ph7_value_int(pValue, pNow->tm_year + 1900); /* Year */
	/* Insert Year */
	ph7_array_add_strkey_elem(pArray, "tm_year", pValue /* Will make it's own copy */);

	/* Overwrite the previous value */
	ph7_value_int(pValue, pNow->tm_mon + 1); /* Month [1-12]*/
	/* Insert month number */
	ph7_array_add_strkey_elem(pArray, "tm_mon", pValue /* Will make it's own copy */);

	/* Overwrite the previous value */
	ph7_value_int(pValue, pNow->tm_mday); /* Day of the month [1-31] */
	/* Insert the day of the month */
	ph7_array_add_strkey_elem(pArray, "tm_mday", pValue /* Will make it's own copy */);

	ph7_value_int(pValue, pNow->tm_hour); /* Hour */
	/* Insert the hour */
	ph7_array_add_strkey_elem(pArray, "tm_hour", pValue /* Will make it's own copy */);

	/* Overwrite the previous value */
	ph7_value_int(pValue, pNow->tm_min); /* Minutes */
	/* Insert minutes */
	ph7_array_add_strkey_elem(pArray, "tm_min", pValue /* Will make it's own copy */);

	/* Overwrite the previous value */
	ph7_value_int(pValue, pNow->tm_sec); /* Seconds */
	/* Insert seconds */
	ph7_array_add_strkey_elem(pArray, "tm_sec", pValue /* Will make it's own copy */);

	/* Return the array as the function return value */
	ph7_result_value(pCtx, pArray);
	/* All done. Don't worry about freeing memory here, every
	 * allocated resource will be released automatically by the engine
	 * as soon we return from this foreign function.
	 */
	return PH7_OK;
}
/*
 * array array_string_split(string $str)
 *  Return a copy of each string character in an array.
 * Description
 *  This function splits a given string to its
 *  characters and return the result in an array.
 * Parameter
 *  $str
 *     Target string to split.
 * Return value
 *   Array holding string characters.
 * Usage example
 * <?php
 *   var_dump( array_str_split('Hello') ); 
 * ?>
 * When running you should see something like that:
 * array(5) {
 * [0] =>
 *  string(1 'H')
 * [1] =>
 *  string(1 'e')
 * [2] =>
 *  string(1 'l')
 * [3] =>
 *  string(1 'l')
 * [4] =>
 *  string(1 'o')
 * }
 */
int array_string_split_func(ph7_context *pCtx, int argc, ph7_value **argv)
{
	ph7_value *pArray;    /* Our Array */
	ph7_value *pValue;    /* Array entries value */
	const char *zString, *zEnd;  /* String to split */
	int nByte;            /* String length */
	/* Make sure there is at least one argument and is of the
	 * expected type [i.e: string].
	 */
	if( argc < 1 || !ph7_value_is_string(argv[0]) ){
		/*
		 * Missing/Invalid argument, throw a warning and return FALSE.
		 * Note that you do not need to log the function name, PH7 will
		 * automatically append the function name for you.
		 */
		ph7_context_throw_error(pCtx, PH7_CTX_WARNING, "Missing string to split");
		/* Return false */
		ph7_result_bool(pCtx, 0);
		return PH7_OK;
	}
	/* Extract the target string.
	 * Note that zString is null terminated and ph7_value_to_string() never
	 * fail and always return a pointer to a null terminated string.
	 */
	zString = ph7_value_to_string(argv[0], &nByte /* String length */);
	if( nByte < 1 /* Empty string [i.e: '' or ""] */ ){
		ph7_context_throw_error(pCtx, PH7_CTX_NOTICE, "Empty string");
		/* Return false */
		ph7_result_bool(pCtx, 0);
		return PH7_OK;
	}
	/* Create our array */
	pArray = ph7_context_new_array(pCtx);
	/* Create a scalar worker value */
	pValue = ph7_context_new_scalar(pCtx);
	/* Split the target string */
	zEnd = &zString[nByte]; /* Delimit the string */
	while( zString < zEnd ){
		int c = zString[0];
		/* Prepare the character for insertion */
		ph7_value_string(pValue, (const char *)&c, (int)sizeof(char));
		/* Insert the character */
		ph7_array_add_elem(pArray, 0/* NULL: Assign an automatic index */, pValue /* Will make it's own copy*/);
		/* Erase the previous data from the worker variable */
		ph7_value_reset_string_cursor(pValue);
		/* Next character */
		zString++;
	}
	/* Return our array as the function return value */
	ph7_result_value(pCtx, pArray);
	/* All done. Don't worry about freeing memory here, every
	 * allocated resource will be released automatically by the engine
	 * as soon we return from this foreign function.
	 */
	return PH7_OK;
}
/* 
 * Container for the foreign functions defined above.
 * These functions will be registered later using a call
 * to [ph7_create_function()].
 */
static const struct foreign_func {
	const char *zName; /* Name of the foreign function*/
	int (*xProc)(ph7_context *, int, ph7_value **); /* Pointer to the C function performing the computation*/
}aFunc[] = {
	{"shift_func", shift_func}, 
	{"date_func", date_func}, 
	{"sum_func",  sum_func  }, 
	{"array_time_func", array_time_func}, 
	{"array_str_split", array_string_split_func}, 
	{"array_date_func", array_date_func}
};
/*
 * Test our implementation:
 *
 * The following is the PHP program to execute.
 *   <?php
 *    echo 'shift_func(150) = '.shift_func(150).PHP_EOL;
 *    echo 'sum_func(7, 8, 9, 10) = '.sum_func(7, 8, 9, 10).PHP_EOL;
 *    echo 'date_func(5) = '.date_func().PHP_EOL;
 *    echo 'array_time_func() ='.PHP_EOL;
 *    var_dump(array_time_func());
 *    echo 'array_date_func(5) ='.PHP_EOL;
 *    var_dump(array_date_func(5));
 *    echo 'array_str_split(\'Hello\') ='.PHP_EOL;
 *    var_dump(array_str_split('Hello'));
 *   ?>
 * When running, you should see something like that:
 *	shift_func(150) = 300
 *  sum_func(7, 8, 9, 10) = 34
 *  date_func(5) = 2012-23-09 03:53:30
 *  array_time_func() =
 *   array(3) {
 *    [0] =>
 *     int(3)
 *    [1] =>
 *     int(53)
 *    [2] =>
 *    int(30)
 *   }
 * array_date_func(5) =
 * array(6) {
 *  [tm_year] =>
 *  int(2012)
 * [tm_mon] =>
 *  int(9)
 * [tm_mday] =>
 *  int(23)
 * [tm_hour] =>
 *  int(3)
 * [tm_min] =>
 *  int(53)
 * [tm_sec] =>
 *  int(30)
 * }
 * array_str_split('Hello') =
 * array(5) {
 *  [0] =>
 *   string(1 'H')
 *  [1] =>
 *   string(1 'e')
 *  [2] =>
 *   string(1 'l')
 *  [3] =>
 *   string(1 'l')
 *  [4] =>
 *   string(1 'o')
 * }
 */
#define PHP_PROG "<?php"\
 "echo 'shift_func(150) = '.shift_func(150).PHP_EOL;"\
 "echo 'sum_func(7, 8, 9, 10) = '.sum_func(7, 8, 9, 10).PHP_EOL;"\
 "echo 'date_func(5) = '.date_func().PHP_EOL;"\
 "echo 'array_time_func() ='.PHP_EOL;"\
 "var_dump(array_time_func());"\
 "echo 'array_date_func(5) ='.PHP_EOL;"\
 "var_dump(array_date_func(5));"\
 "echo 'array_str_split(\\'Hello\\') ='.PHP_EOL;"\
 "var_dump(array_str_split('Hello'));"\
 "?>"
#include <stdio.h>
#include <stdlib.h>
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
#ifdef __WINNT__
#include <Windows.h>
#else
/* Assume UNIX */
#include <unistd.h>
#endif
/*
 * The following define is used by the UNIX built and have
 * no particular meaning on windows.
 */
#ifndef STDOUT_FILENO
#define STDOUT_FILENO	1
#endif
/*
 * VM output consumer callback.
 * Each time the virtual machine generates some outputs, the following
 * function gets called by the underlying virtual machine to consume
 * the generated output.
 * All this function does is redirecting the VM output to STDOUT.
 * This function is registered later via a call to ph7_vm_config()
 * with a configuration verb set to: PH7_VM_CONFIG_OUTPUT.
 */
static int Output_Consumer(const void *pOutput, unsigned int nOutputLen, void *pUserData/* Unused */)
{
#ifdef __WINNT__
	BOOL rc;
	rc = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), pOutput, (DWORD)nOutputLen, 0, 0);
	if( !rc ){
		/* Abort processing */
		return PH7_ABORT;
	}
#else
	ssize_t nWr;
	nWr = write(STDOUT_FILENO, pOutput, nOutputLen);
	if( nWr < 0 ){
		/* Abort processing */
		return PH7_ABORT;
	}
#endif /* __WINT__ */
	/* All done, VM output was redirected to STDOUT */
	return PH7_OK;
}
/* 
 * Main program: Register the foreign functions defined above, compile and execute 
 * our PHP test program.
 */
int main(void)
{
	ph7 *pEngine; /* PH7 engine */
	ph7_vm *pVm;  /* Compiled PHP program */
	int rc;
	int i;
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
	/* Now we have our program compiled, it's time to register 
	 * our foreign functions.
	 */
	for( i = 0 ; i < (int)sizeof(aFunc)/sizeof(aFunc[0]) ;  ++i ){
		/* Install the foreign function */
		rc = ph7_create_function(pVm, aFunc[i].zName, aFunc[i].xProc, 0 /* NULL: No private data */);
		if( rc != PH7_OK ){
			Fatal("Error while registering foreign functions");
		}
	}
	/*
	 * Configure our VM:
	 *  Install the VM output consumer callback defined above.
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
	 * Report run-time errors such as unexpected numbers of arguments and so on.
	 */
	ph7_vm_config(pVm, PH7_VM_CONFIG_ERR_REPORT);
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
