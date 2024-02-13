#include <stdio.h>
#include <stdlib.h>
#include <kernel/kpanic.h>

__attribute__((__noreturn__))
void abort(void) {
#if defined(__is_libk)
	kpanic("Kernel panic: abort()");
#else
	// TODO: Abnormally terminate the process as if by SIGABRT.
	printf("abort()\n");
#endif
	while (1) { }
	__builtin_unreachable();
}
