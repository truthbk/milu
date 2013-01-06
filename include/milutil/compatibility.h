#ifndef _COMPATIBILITY_H 
#define _COMPATIBILITY_H 

#ifdef __APPLE__
#include <machine/types.h>
#define __BITS_PER_LONG 32
#elif __GNUC__
#include <asm-generic/types.h>
#include <asm-generic/bitsperlong.h>
#endif

#endif
