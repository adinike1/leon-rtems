##
## $Id$
##


rtems_tests_PROGRAMS = spfifo02
spfifo02_SOURCES = init.c

dist_rtems_tests_DATA = spfifo02.scn
dist_rtems_tests_DATA += spfifo02.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am

AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(spfifo02_OBJECTS)
LINK_LIBS = $(spfifo02_LDLIBS)

spfifo02$(EXEEXT): $(spfifo02_OBJECTS) $(spfifo02_DEPENDENCIES)
	@rm -f spfifo08$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
