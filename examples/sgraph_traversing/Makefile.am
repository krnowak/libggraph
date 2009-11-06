include $(top_srcdir)/examples/examples.am

#Build the executable, but don't install it.
noinst_PROGRAMS = sgraph_traversing

sgraph_traversing_SOURCES = sgraph_traversing.c
sgraph_traversing_CFLAGS = $(examples_include)
sgraph_traversing_LDADD = $(examples_ldadd)

dist_noinst_DATA = topology.dia topology.png

example_name = sgraph_traversing
example_src = $(example_name).c
include	$(top_srcdir)/examples/example-dist.am
