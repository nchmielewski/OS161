#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);

// new code
void _exit(int code);
int printint(int c);
int reversestring(const char* str, int len);

#endif /* _SYSCALL_H_ */
