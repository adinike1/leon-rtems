##
## $Id$
##


rtems_tests_PROGRAMS = tm22
tm22_SOURCES = task1.c system.h ../include/timesys.h \
    ../../support/src/tmtests_empty_function.c

dist_rtems_tests_DATA = tm22.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


OPERATION_COUNT = @OPERATION_COUNT@
AM_CPPFLAGS += -I$(top_srcdir)/include -DOPERATION_COUNT=$(OPERATION_COUNT)
AM_CPPFLAGS += -I$(top_srcdir)/../support/include

LINK_OBJS = $(tm22_OBJECTS)
LINK_LIBS = $(tm22_LDLIBS)

tm22$(EXEEXT): $(tm22_OBJECTS) $(tm22_DEPENDENCIES)
	@rm -f tm22$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
