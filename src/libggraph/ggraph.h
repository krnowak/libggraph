#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_H_
#define _G_GRAPH_H_

#include <glib.h>
#include <libggraph/ggraphtypedefs.h>
#include <libggraph/ggraphedge.h>
#include <libggraph/ggrapharray.h>
#include <libggraph/ggraphedgearray.h>

G_BEGIN_DECLS

/**
 * GGraph:
 * @data: #gpointer containing user defined data.
 * @edges: #GGraphEdgeArray containing pointers to edges to neighbours.
 *
 * Single node of directed, weighted graph. It contains data and an array of
 * pointers to edges to all it's neighbours - that is, all nodes that are connected to this
 * node or all nodes this node is connected to.
 * By term "node A being connected to node B" we mean that there is directed
 * edge from A to B.
 */
struct _GGraph
{
  gpointer data;
  GGraphEdgeArray* edges;
};

GGraph*
g_graph_new(gpointer data) G_GNUC_WARN_UNUSED_RESULT;

gboolean
g_graph_connect(GGraph* graph,
                GGraph* other_graph,
                gdouble weight);

GGraphArray*
g_graph_remove(GGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

void
g_graph_free(GGraph* graph);

guint
g_graph_count(GGraph* graph);

guint
g_graph_count_connected(GGraph* graph);

void
g_graph_foreach(GGraph* graph,
                GFunc func,
                gpointer user_data);

void
g_graph_foreach_connected(GGraph* graph,
                          GFunc func,
                          gpointer user_data);

GGraphArray*
g_graph_all_neighbours(GGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_connected_neighbours(GGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_not_connected_neighbours(GGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

GGraphEdge*
g_graph_edge(GGraph* graph,
             GGraph* other_graph) G_GNUC_WARN_UNUSED_RESULT;

gboolean
g_graph_remove_connection(GGraph* graph,
                          GGraph* other_graph) G_GNUC_WARN_UNUSED_RESULT;

gboolean
g_graph_break_connection(GGraph* graph,
                         GGraph* other_graph) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_find(GGraph* graph,
             gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_find_connected(GGraph* graph,
                       gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_find_custom(GGraph* graph,
                    gconstpointer data,
                    GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_find_custom_connected(GGraph* graph,
                              gconstpointer data,
                              GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

gpointer
g_graph_data(GGraph* graph);

const GGraphEdgeArray*
g_graph_edges(GGraph* graph);

G_END_DECLS

#endif // _G_GRAPH_H_
