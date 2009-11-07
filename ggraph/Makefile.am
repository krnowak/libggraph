ggraph_c = 				\
	ggraphdata.c			\
	ggraphedge.c			\
	ggraphdirectedmultiedge.c	\
	ggraphdirectededge.c		\
	ggraphundirectedmultiedge.c	\
	ggraphundirectededge.c		\
	ggraphnode.c			\
	ggraphentity.c			\
	ggraphtriplet.c

ggraph_h = $(ggraph_c:.c=.h)

ggraph_c_more = 		\
	$(ggraph_c)		\
	ggraphenumtypes.c

ggraph_h_more =			\
	$(ggraph_c_more:.c=.h)	\
	ggraphtypedefs.h	\
	ggraph.h		\
	ggraphoptions.h		\
	ggraphtype.h

BUILT_SOURCES = ggraphenumtypes.h ggraphenumtypes.c

ggraphenumtypes.h: $(ggraph_h) ggraphenumtypes.h.template
       	( cd $(srcdir) && $(GLIB_MKENUMS) --template ggraphenumtypes.h.template \
                $(ggraph_h) ) >> xgen-geth \
        && mv xgen-geth gtktypebuiltins.h
ggraphenumtypes.c: $(ggraph_h) ggraphenumtypes.c.template
        ( cd $(srcdir) && $(GLIB_MKENUMS) --template ggraphenumtypes.c.template \
               	$(gtk_public_h_sources) ) > xgen-getc \
        && mv xgen-getc ggraphenumtypes.c

ggraph_includedir = $(includedir)/@libggraph_libname@/ggraph/
ggraph_include_HEADERS = $(ggraph_h_more)

lib_LTLIBRARIES = libggraph-@libggraph_api_version@.la
libggraph_@libggraph_api_version@_la_SOURCES = $(ggraph_c_more)
libggraph_@libggraph_api_version@_la_LDFLAGS = -version-info $(libggraph_so_version)
libggraph_@libggraph_api_version@_la_LIBADD = $(gobject_LIBS)
libggraph_@libggraph_api_version@_la_CFLAGS = $(gobject_CFLAGS) -D_LIB_G_GRAPH_COMPILATION_ -I$(top_srcdir) -I$(top_srcdir)/ggraph $(libggraph_WFLAGS)
