include $(top_srcdir)/examples/Makefile.am_fragment

#Build the executable, but don't install it.
noinst_PROGRAMS = simple_graph_construct

simple_graph_SOURCES = simple_graph_construct.c
simple_graph_CFLAGS = $(examples_include)
simple_graph_LDADD = $(examples_ldadd)

EXTRA_DIST = topology.dia topology.png
