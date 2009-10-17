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
#include <libggraph/ggraphdataquadruplet.h>

G_BEGIN_DECLS

/**
 * GGraph:
 * @data: pointer to user defined data.
 * @edges: an array containing pointers to edges to neighbours.
 *
 * Contains public fields of a node.
 */
struct _GGraph
{
  gpointer data;
  GGraphEdgeArray* edges;
};

GGraph*
g_graph_new(gpointer data) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_construct(GGraphDataQuadruplet** data_quadruplets,
                  gint count);

/*
GGraphEdge*
g_graph_connection_new(GGraph* graph,
                       GGraph* other_graph);

GGraphEdge*
g_graph_connection_new_with_data(GGraph* graph,
                                 GGraph* other_graph,
                                 gpointer data);
*/

GGraphArray*
g_graph_remove(GGraph* graph,
               GPtrArray* edge_data_array) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_disconnect(GGraph* graph,
                   GPtrArray* edge_data_array) G_GNUC_WARN_UNUSED_RESULT;

GGraph*
g_graph_copy(GGraph* graph);

GGraph*
g_graph_copy_deep(GGraph* graph,
                  GCopyFunc graph_data_copy_func,
                  gpointer graph_user_data,
                  GCopyFunc edge_data_copy_func,
                  gpointer edge_user_data);

void
g_graph_free(GGraph* graph);

guint
/* g_graph_count_nodes(GGraph* graph); */
g_graph_get_order(GGraph* graph);

guint
/* g_graph_count_edges(GGraph* graph); */
g_graph_get_size(GGraph* graph);

guint
/* g_graph_count_connected_nodes(GGraph* graph); */
g_graph_get_connected_order(GGraph* graph);

guint
/* g_graph_count_connected_edges(GGraph* graph); */
g_graph_get_connected_size(GGraph* graph);

void
g_graph_foreach(GGraph* graph,
                GFunc func,
                gpointer user_data);

void
g_graph_foreach_connected(GGraph* graph,
                          GFunc func,
                          gpointer user_data);

/**
 * GGraphFunc:
 * @graph: a node.
 * @user_data: user data.
 *
 * Specifies the type of functions passed to g_graph_foreach_node()
 * or g_graph_foreach_connected_node().
 */
typedef void (*GGraphFunc)(GGraph* graph,
                           gpointer user_data);

void
g_graph_foreach_node(GGraph* graph,
                     GGraphFunc func,
                     gpointer user_data);

void
g_graph_foreach_connected_node(GGraph* graph,
                               GGraphFunc func,
                               gpointer user_data);

void
g_graph_edge_foreach(GGraph* graph,
                     GFunc func,
                     gpointer user_data);

void
g_graph_edge_foreach_connected(GGraph* graph,
                               GFunc func,
                               gpointer user_data);

/**
 * GGraphEdgeFunc:
 * @edge: an edge.
 * @user_data: user data.
 *
 * Specifies the type of functions passed to g_graph_foreach_edge()
 * or g_graph_foreach_connected_edge().
 */
typedef void (*GGraphEdgeFunc)(GGraphEdge* edge,
                               gpointer user_data);

void
g_graph_edge_foreach_edge(GGraph* graph,
                          GGraphEdgeFunc func,
                          gpointer user_data);

void
g_graph_edge_foreach_connected_edge(GGraph* graph,
                                    GGraphEdgeFunc func,
                                    gpointer user_data);

GGraphArray*
g_graph_get_all_neighbours(GGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_get_connected_neighbours(GGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_get_not_connected_neighbours(GGraph* graph) G_GNUC_WARN_UNUSED_RESULT;

GGraphEdgeArray*
g_graph_get_edges(GGraph* graph,
                  GGraph* other_graph) G_GNUC_WARN_UNUSED_RESULT;
 
/* TODO: move it to ggraphedge. */
gboolean
g_graph_remove_connection(GGraph* graph,
                          GGraph* other_graph,
                          gpointer* edge_data) G_GNUC_WARN_UNUSED_RESULT;

gboolean
g_graph_remove_all_connections(GGraph* graph,
                               GGraph* other_graph,
                               GPtrArray* edge_data_array);

gboolean
g_graph_break_all_connections(GGraph* graph,
                              GGraph* other_graph,
                              GPtrArray* edge_data_array);

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

/**
 * GGraphEqualFunc:
 * @graph: a node.
 * @other_graph: other node.
 *
 * Specifies the type of functions passed to g_graph_find_custom_node()
 * or g_graph_find_custom_connected_node().
 *
 * Returns: %TRUE if @graph and @other_graph are equal, otherwise %FALSE.
 */
typedef gboolean (*GGraphEqualFunc)(GGraph* graph,
                                    GGraph* other_graph);

GGraphArray*
g_graph_find_custom_node(GGraph* graph,
                         GGraph* node,
                         GGraphEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

GGraphArray*
g_graph_find_custom_connected_node(GGraph* graph,
                                   GGraph* node,
                                   GGraphEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

GGraphEdgeArray*
g_graph_edge_find(GGraph* graph,
                  gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;

GGraphEdgeArray*
g_graph_edge_find_connected(GGraph* graph,
                            gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;

GGraphEdgeArray*
g_graph_edge_find_custom(GGraph* graph,
                         gconstpointer data,
                         GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

GGraphEdgeArray*
g_graph_edge_find_custom_connected(GGraph* graph,
                                   gconstpointer data,
                                   GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

/**
 * GGraphEdgeEqualFunc:
 * @edge: an edge.
 * @other_edge: other edge.
 *
 * Specifies the type of functions passed to g_graph_edge_find_custom_edge()
 * or g_graph_edge_find_custom_connected_edge().
 *
 * Returns: %TRUE if @edge and @other_edge are equal, otherwise %FALSE.
 */
typedef gboolean (*GGraphEdgeEqualFunc)(GGraphEdge* edge,
                                        GGraphEdge* other_edge);

GGraphEdgeArray*
g_graph_edge_find_custom_edge(GGraph* graph,
                              GGraphEdge* edge,
                              GGraphEdgeEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

GGraphEdgeArray*
g_graph_edge_find_custom_connected_edge(GGraph* graph,
                                        GGraphEdge* edge,
                                        GGraphEdgeEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS

#endif /* _G_GRAPH_H_ */
