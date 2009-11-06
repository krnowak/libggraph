include $(top_srcdir)/examples/examples.am

#Build the executable, but don't install it.
noinst_PROGRAMS = sgraph_construct

sgraph_construct_SOURCES = sgraph_construct.c
sgraph_construct_CFLAGS = $(examples_include)
sgraph_construct_LDADD = $(examples_ldadd)

dist_noinst_DATA = topology.dia topology.png

example_name = sgraph_construct
example_src = $(example_name).c
include $(top_srcdir)/examples/example-dist.am
