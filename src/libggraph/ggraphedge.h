#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_EDGE_H_
#define _G_GRAPH_EDGE_H_

#include <libggraph/ggraphtypedefs.h>
#include <libggraph/ggraph.h>

/**
 * GGraphEdge:
 * @graph: pointer to an end of this edge.
 * @weight: weight of this edge.
 * @connected: %TRUE if a node containing this edge is connected to @graph. If
 * it is %FALSE, then it means that @graph is connected to node containing this
 * edge.
 *
 * An edge from the node containing it to the @graph.
 */
struct _GGraphEdge
{
  GGraph* graph;
  gboolean connected;
  gdouble weight;
};

GGraphEdge*
g_graph_edge_new(GGraph* graph,
                 gboolean connected,
                 gdouble weight);

GGraph*
g_graph_edge_free(GGraphEdge* edge);

GGraph*
g_graph_edge_graph(GGraphEdge* edge);

gboolean
g_graph_edge_connected(GGraphEdge* edge);

gdouble
g_graph_edge_weight(GGraphEdge* edge);

#endif /* _G_GRAPH_EDGE_H_ */
