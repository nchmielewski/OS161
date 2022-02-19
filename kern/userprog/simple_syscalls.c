/*
#include <thread.h>
#include <curthread.h>
#include <syscall.h>
#include <types.h>
#include <kern/errno.h>
#include <kern/callno.h>
#include <kern/unistd.h>
//#include <stdio.h>
//#include <errno.h>
//#include <string.h>
#include <lib.h>
#include <synch.h>
#include <test.h>
#include <machine/trapframe.h>
*/

void _exit(int exitCode) {
        kprintf("%d\n", exitCode);
        thread_exit();
}

int printint(int c) {
	kprintf("%d\n", c);	// print int
	if(c%3 == 0) {
		return 1;	// if int divisible by 3, if int%3 == 0
	}

	else {
		return 0;
	}
}

int reversestring(const char *str, int len) {
	// print reverse of string
	char temp[len];	// temporary string bc str is of constant type and therefore immutable
	int i;
	for (i=0; i<len; i++) {
		temp[i] = str[len-i-1];
	}	
	kprintf("%s\n", temp);

	// return 0 if strlen is %3, else return 1
	if(len%3 == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

