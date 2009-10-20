#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_EDGE_H_
#define _G_GRAPH_EDGE_H_

#include <libggraph/ggraphtypedefs.h>
#include <libggraph/ggraph.h>
#include <libggraph/ggraphedgeconnected.h>

G_BEGIN_DECLS

/**
 * GGraphEdge:
 * @first: first end of this edge.
 * @second: second end of this edge.
 * @connected: flag describing interconnection between @first and @second.
 * @data: data contained by this edge.
 *
 * An edge from the node containing it to the @graph.
 */
struct _GGraphEdge
{
  GGraph* first;
  GGraph* second;
  GGraphEdgeConnected connected;
  gpointer data;
};

GGraphEdge*
g_graph_edge_new(GGraph* first,
                 GGraph* second,
                 GGraphEdgeConnected connected,
                 gpointer data);

gpointer
g_graph_edge_free(GGraphEdge* edge);

GGraph*
g_graph_edge_get_graph(GGraphEdge* edge,
                       GGraph* node);

gboolean
g_graph_edge_is_first(GGraphEdge* edge,
                      GGraph* node);

gboolean
g_graph_edge_is_second(GGraphEdge* edge,
                       GGraph* node);

gboolean
g_graph_edge_is_first_extended(GGraphEdge* edge,
                               GGraph* node,
                               gboolean* is_first);

gboolean
g_graph_edge_is_second_extended(GGraphEdge* edge,
                                GGraph* node,
                                gboolean* is_second);

gboolean
g_graph_edge_is_graph_connected(GGraphEdge* edge,
                                GGraph* node);

gboolean
g_graph_edge_is_graph_connected_extended(GGraphEdge* edge,
                                         GGraph* node,
                                         gboolean* is_connected);

gboolean
g_graph_edge_remove(GGraphEdge* edge);

gboolean
g_graph_edge_disconnect(GGraphEdge* edge);

gboolean
g_graph_edge_connect_graph(GGraphEdge* edge,
                           GGraph* graph);

gboolean
g_graph_edge_remove_connection(GGraphEdge* edge,
                               GGraph* graph);

/* TODO: implement it. */
gboolean
g_graph_edge_is_link(GGraphEdge* edge);

/* TODO: implement it. */
gboolean
g_graph_edge_is_loop(GGraphEdge* edge);

/* TODO: implement it. */
guint
g_graph_edge_get_multiplicity(GGraphEdge* edge);

/* TODO: implement it. */
g_graph_edge_is_bridge(GGraphEdge* edge);

G_END_DECLS

#endif /* _G_GRAPH_EDGE_H_ */
