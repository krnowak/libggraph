include $(top_srcdir)/examples/examples.am

#Build the executable, but don't install it.
noinst_PROGRAMS = simple_graph_construct

simple_graph_construct_SOURCES = simple_graph_construct.c
simple_graph_construct_CFLAGS = $(examples_include)
simple_graph_construct_LDADD = $(examples_ldadd)

dist_noinst_DATA = topology.dia topology.png
