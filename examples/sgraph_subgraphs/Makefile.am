include $(top_srcdir)/examples/examples.am

#Build the executable, but don't install it.
noinst_PROGRAMS = sgraph_subgraphs

sgraph_subgraphs_SOURCES = sgraph_subgraphs.c
sgraph_subgraphs_CFLAGS = $(examples_include)
sgraph_subgraphs_LDADD = $(examples_ldadd)

dist_noinst_DATA = topology.dia topology.png

example_name = sgraph_subgraphs
example_src = $(example_name).c
include	$(top_srcdir)/examples/example-dist.am
