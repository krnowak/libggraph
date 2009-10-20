include $(top_srcdir)/examples/examples.am

#Build the executable, but don't install it.
noinst_PROGRAMS = simple_graph

simple_graph_SOURCES = simple_graph.c
simple_graph_CFLAGS = $(examples_include)
simple_graph_LDADD = $(examples_ldadd)

dist_noinst_DATA = topology.dia topology.png
