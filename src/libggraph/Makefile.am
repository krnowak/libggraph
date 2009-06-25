libggraph_c = ggraph.c ggrapharray.c ggraphedge.c ggraphedgearray.c gsgraph.c gsgrapharray.c
libggraph_h = $(libggraph_c:.c=.h)

libggraph_includedir = $(includedir)/@libggraph_libname@/@libggraph_name@
libggraph_include_HEADERS = $(libbgraph_h)

lib_LTLIBRARIES = libggraph-1.0.la
libggraph_1_0_la_SOURCES = $(libggraph_c)
libggraph_1_0_la_LDFLAGS = -version-info @libggraph_so_version@
libbgraph_1_0_la_LIBADD = $(glib_LIBS)
