#ifndef __ASM_I386_TYPES_H
#define __ASM_I386_TYPES_H

#ifndef __ASSEMBLY__

#ifdef __x86_64__
/*
 * This is used in dlmalloc. On X86_64 we need it to be
 * 64 bit
 */
#define INTERNAL_SIZE_T unsigned long

/*
 * This is a Kconfig variable in the Kernel, but we want to detect
 * this during compile time, so we set it here.
 */
#define CONFIG_PHYS_ADDR_T_64BIT

#endif

typedef unsigned short umode_t;

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

/*
 * These aren't exported outside the kernel to avoid name space clashes
 */
#ifdef __KERNEL__

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#ifdef __x86_64__
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif

/* Dma addresses are 32-bits wide.  */

typedef u32 dma_addr_t;

#endif /* __KERNEL__ */

#endif

#endif