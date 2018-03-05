#include <features.h>
#ifdef __ELF__
# ifdef __GLIBC__
#  if __GLIBC__ >= 2
LIBC=gnu
#  else
LIBC=gnulibc1
#  endif
# else
LIBC=gnulibc1
# endif
#else
#ifdef __INTEL_COMPILER
LIBC=gnu
#else
LIBC=gnuaout
#endif
#endif
