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
g_sgraph_construct(GSGraphDataPair** data_pairs,
                   gint count) G_GNUC_WARN_UNUSED_RESULT;

void
g_sgraph_connect(GSGraph* sgraph,
                 GSGraph* other_sgraph);

GSGraphArray*
g_sgraph_remove(GSGraph* sgraph) G_GNUC_WARN_UNUSED_RESULT;

GSGraph*
g_sgraph_copy(GSGraph* sgraph);

GSGraph*
g_sgraph_copy_deep(GSGraph* sgraph,
                   GCopyFunc copy_func,
                   gpointer user_data);

void
g_sgraph_free(GSGraph* sgraph);

guint
g_sgraph_count(GSGraph* sgraph);

void
g_sgraph_foreach(GSGraph* sgraph,
                 GFunc func,
                 gpointer user_data);

/**
 * GSGraphFunc:
 * @graph: a node.
 * @user_data: user data passed to g_sgraph_foreach_node().
 *
 * Specifies the type of functions passed to g_sgraph_foreach_node().
 */
typedef void (*GSGraphFunc)(GSGraph* graph,
                            gpointer user_data);

void
g_sgraph_foreach_node(GSGraph* sgraph,
                      GSGraphFunc func,
                      gpointer user_data);

gboolean
g_sgraph_break_connection(GSGraph* sgraph,
                          GSGraph* other_sgraph) G_GNUC_WARN_UNUSED_RESULT;

GSGraphArray*
g_sgraph_find(GSGraph* sgraph,
              gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;

GSGraphArray*
g_sgraph_find_custom(GSGraph* sgraph,
                     gconstpointer data,
                     GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

/**
 * GSGraphEqualFunc:
 * @sgraph: a node.
 * @other_sgraph: other node.
 *
 * Specifies the type of functions passed to g_sgraph_find_custom_node().
 *
 * Returns: %TRUE if @sgraph and @other_sgraph are equal, otherwise %FALSE.
 */
typedef gboolean (*GSGraphEqualFunc)(GSGraph* sgraph,
                                     GSGraph* other_sgraph);

GSGraphArray*
g_sgraph_find_custom_node(GSGraph* sgraph,
                          GSGraph* node,
                          GSGraphEqualFunc func);

G_END_DECLS

#endif /* _G_SGRAPH_H_ */
