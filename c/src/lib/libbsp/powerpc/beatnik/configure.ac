## Process this file with autoconf to produce a configure script.
## 
## configure.ac,v 1.9.2.3 2003/08/11 14:37:22 ralf Exp

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-beatnik],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.11.1])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_TOOL([OBJCOPY],[objcopy])

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

AS=$CC
AM_PROG_AS

RTEMS_BSPOPTS_SET_DATA_CACHE_ENABLED([*],[1])
RTEMS_BSPOPTS_HELP_DATA_CACHE_ENABLED

RTEMS_BSPOPTS_SET_INSTRUCTION_CACHE_ENABLED([*],[1])
RTEMS_BSPOPTS_HELP_INSTRUCTION_CACHE_ENABLED

RTEMS_BSPOPTS_SET([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK], [*], [1])
RTEMS_BSPOPTS_HELP([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK],
[If defined then the BSP may reduce the available memory size
initially. This can be useful for debugging (reduce the core
size) or dynamic loading (std gcc text offsets/jumps are < +/-32M).
Note that the policy can still be defined by the application
(see sbrk.c, BSP_sbrk_policy). By undefining
CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK this feature is removed
and a little memory is saved.])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
