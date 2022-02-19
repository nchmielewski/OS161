#include <stdio.h>
#include "../include/unistd.h"
#include <string.h>
#include "../../kern/include/syscall.h"

void testprint() {
	int i;
	// loop to print integers 0-4
	for(i=0; i<5; i++) {
		printint(i);
		_exit(111);
	}
	return;
}

void testreverse() {
	reversestring("Hello there. This is a test!", 28);
	_exit(222);
	return;
}

int main () {
	testprint();
	testreverse();
	return 0;
}
