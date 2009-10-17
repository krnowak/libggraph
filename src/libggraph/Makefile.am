libggraph_c = 			\
	gsgraph.c		\
	gsgrapharray.c		\
	gsgraphdatapair.c

libggraph_h = 			\
	$(libggraph_c:.c=.h)	\
	ggraphtypedefs.h

libggraph_includedir = $(includedir)/@libggraph_libname@/@libggraph_name@
libggraph_include_HEADERS = $(libggraph_h)

lib_LTLIBRARIES = libggraph-1.0.la
libggraph_1_0_la_SOURCES = $(libggraph_c)
libggraph_1_0_la_LDFLAGS = -version-info @libggraph_so_version@
libggraph_1_0_la_LIBADD = $(glib_LIBS)
libggraph_1_0_la_CFLAGS = $(glib_CFLAGS) -D_LIB_G_GRAPH_COMPILATION_ -I$(top_srcdir)/src -I$(top_srcdir)/src/libggraph
