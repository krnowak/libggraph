#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_EDGE_H_
#define _G_GRAPH_EDGE_H_

#include <libggraph/ggraphtypedefs.h>
#include <libggraph/ggraph.h>
#include <libggraph/ggraphedgeconnected.h>

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

GGraph*
g_graph_edge_free(GGraphEdge* edge);

GGraph*
g_graph_edge_graph(GGraphEdge* edge
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

#endif /* _G_GRAPH_EDGE_H_ */
