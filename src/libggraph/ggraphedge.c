#include "ggraphedge.h"

/**
 * g_graph_edge_new:
 * @graph: node it connects to.
 * @connection: if node containing this edge is connected to @graph.
 * @weight: weight of the connection.
 *
 * Creates new edge with given @graph. If @connection is false, then @weight is
 * ignored and set to 0.
 *
 * Returns: newly created #GGraphEdge.
 */
GGraphEdge*
g_graph_edge_new(GGraph* graph,
                 gboolean connected,
                 gdouble weight)
{
  GGraphEdge* edge = g_slice_new(GGraphEdge);
  edge->graph = graph;
  edge->connected = connected;
  if (connected)
  {
    edge->weight = weight;
  }
  else
  {
    edge->weight = 0;
  }
  return edge;
}

/**
 * g_graph_edge_free:
 * @edge: an edge to free.
 *
 * Frees an edge. A #GGraph pointed by @edge is not freed, but is returned.
 *
 * Returns: #GGraph pointed by @edge or %NULL if @edge was %NULL also.
 */
GGraph*
g_graph_edge_free(GGraphEdge* edge)
{
  if (!edge)
  {
    return NULL;
  }
  GGraph* graph = edge->graph;
  g_slice_free(GGraphEdge, edge);
  return graph;
}

/**
 * g_graph_edge_graph:
 * @edge: an edge.
 *
 * It's an accessor function for language bindings.
 *
 * Returns: #GGraph associated to @edge.
 */
GGraph*
g_graph_edge_graph(GGraphEdge* edge)
{
  return edge->graph;
}

/**
 * g_graph_edge_connected:
 * @edge: an edge.
 *
 * It's an accessor function for language bindings.
 *
 * Returns: if @edge is connected to graph.
 */
gboolean
g_graph_edge_connected(GGraphEdge* edge)
{
  return edge->connected;
}

/**
 * g_graph_edge_weight:
 * @edge: an edge.
 *
 * It's an accessor function for language bindings.
 *
 * Returns: @edge's weight or 0 if @edge is not connected to graph.
 */
gdouble
g_graph_edge_weight(GGraphEdge* edge)
{
  if (edge->connected)
  {
    return edge->weight;
  }
  return 0;
}
