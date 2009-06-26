include $(top_srcdir)/examples/Makefile.am_fragment

#Build the executable, but don't install it.
noinst_PROGRAMS = simple_graph

simple_graph_SOURCES = simple_graph.c
simple_graph_CFLAGS = $(examples_include)
simple_graph_LDADD = $(examples_ldadd)

EXTRA_DIST = topology.dia topology.png
