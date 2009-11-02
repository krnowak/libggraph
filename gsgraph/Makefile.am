gsgraph_c = 			\
	gsgraphnode.c		\
	gsgraphnodearray.c	\
	gsgraphdatapair.c

gsgraph_h = 			\
	$(gsgraph_c:.c=.h)	\
	gsgraphtypedefs.h	\
	gsgraph.h

gsegraph_c = 			\
	gsegraphnode.c		\
	gsegraphedge.c		\
	gsegraphwhole.c		\
	gsegraphdatatriplet.c

gsegraph_h = 			\
	$(gsegraph_c:.c=.h)	\
	gsegraphtypedefs.h	\
	gsegraphenums.h		\
	gsegraph.h

gsgraph_includedir = $(includedir)/@libggraph_libname@/gsgraph/
gsgraph_include_HEADERS = $(gsgraph_h) $(gsegraph_h)

lib_LTLIBRARIES = libgsgraph-@libggraph_api_version@.la
libgsgraph_@libggraph_api_version@_la_SOURCES = $(gsgraph_c) $(gsegraph_c)
libgsgraph_@libggraph_api_version@_la_LDFLAGS = -version-info $(libggraph_so_version)
libgsgraph_@libggraph_api_version@_la_LIBADD = $(glib_LIBS)
libgsgraph_@libggraph_api_version@_la_CFLAGS = $(glib_CFLAGS) -D_LIB_G_GRAPH_COMPILATION_ -I$(top_srcdir) -I$(top_srcdir)/gsgraph $(libggraph_WFLAGS)
