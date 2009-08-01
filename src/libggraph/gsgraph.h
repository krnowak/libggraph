#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_H_
#define _G_SGRAPH_H_

#include <glib.h>
#include <libggraph/gsgraphtypedefs.h>
#include <libggraph/gsgrapharray.h>
#include <libggraph/gsgraphdatapair.h>

G_BEGIN_DECLS

/**
 * GSGraph:
 * @data: #gpointer containing user defined data.
 * @neighbours: #GSGraphArray containing pointers to neighbours.
 *
 * Single node of simple graph. It contains data and an array of pointers to all
 * it's neighbours.
 * If you want to have weights, one way edges, multiple edges between two nodes,
 * data on edges - use #GGraph.
 */
struct _GSGraph
{
  gpointer data;
  GSGraphArray* neighbours;
};

GSGraph*
g_sgraph_new(gpointer data) G_GNUC_WARN_UNUSED_RESULT;

GSGraphArray*
g_sgraph_construct(GGraphDataPair** data_pairs,
                   gint count) G_GNUC_WARN_UNUSED_RESULT;

gboolean
g_sgraph_connect(GSGraph* graph,
                 GSGraph* other_graph);

GSGraphArray*
g_sgraph_remove(GSGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

GSGraph*
g_sgraph_copy(GSGraph* graph);

GSGraph*
g_sgraph_copy_deep(GSGraph* graph,
                   GCopyFunc copy_func,
                   gpointer user_data);

void
g_sgraph_free(GSGraph* graph);

guint
g_sgraph_count(GSGraph* graph);

void
g_sgraph_foreach(GSGraph* graph,
                 GFunc func,
                 gpointer user_data);

gboolean
g_sgraph_break_connection(GSGraph* graph,
                          GSGraph* other_graph) G_GNUC_WARN_UNUSED_RESULT;

GSGraphArray*
g_sgraph_find(GSGraph* graph,
              gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;

GSGraphArray*
g_sgraph_find_custom(GSGraph* graph,
                     gconstpointer data,
                     GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

gpointer
g_sgraph_data(GSGraph* sgraph);

G_CONST_RETURN GSGraphArray*
g_sgraph_neighbours(GSGraph* sgraph);

G_END_DECLS

#endif /* _G_SGRAPH_H_ */
