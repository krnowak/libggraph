#include "ggraphedge.h"

/* static declarations */

gboolean
_g_graph_edge_general_member_check(GGraphEdge* edge,
                                   GGraph* node,
                                   gboolean* check_result,
                                   gboolean check_for_first);

/* function definitions */

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
g_graph_edge_new(GGraph* first,
                 GGraph* second,
                 GGraphEdgeConnected connected,
                 gpointer data)
{
  GGraphEdge* edge;
  
  edge = g_slice_new(GGraphEdge);
  edge->first = first;
  edge->second = second;
  edge->connected = connected;
  edge->data = data;
  return edge;
}

/**
 * g_graph_edge_free:
 * @edge: an edge to free.
 *
 * Frees an edge. A #GGraph members in @edge are not freed, so it is up to
 * programmist to get pointers to them if needed. Data also is not freed, but
 * a pointer is returned.
 *
 * Returns: Data in freed @edge or %NULL if @edge was %NULL also.
 */
gpointer
g_graph_edge_free(GGraphEdge* edge)
{
  gpointer data;
  
  if (!edge)
  {
    return NULL;
  }
  data = edge->data;
  g_slice_free(GGraphEdge, edge);
  return data;
}

/**
 * g_graph_edge_graph:
 * @edge: an edge.
 * @node: a node using this connection.
 *
 * Gets a node to which a @node is connected. If @node is not a part of @edge,
 * this function will return %NULL.
 *
 * Returns: #GGraph associated to @edge being a neighbour of @node.
 */
GGraph*
g_graph_edge_graph(GGraphEdge* edge
                   GGraph* node)
{
  if (edge->first == node)
  {
    return edge->second;
  }
  else if (edge->second == node)
  {
    return edge->first;
  }
  return NULL;
}

/**
 * g_graph_edge_is_first:
 * @edge: an edge.
 * @node: a node to check.
 *
 * Checks if node is first member of connection. Returned %FALSE does not mean
 * that @node is second member of @edge.
 *
 * Returns: %TRUE, if @node is first member of @edge, %FALSE otherwise.
 */
gboolean
g_graph_edge_is_first(GGraphEdge* edge,
                      GGraph* node)
{
  return (edge->first == node);
}

/**
 * g_graph_edge_is_second:
 * @edge: an edge.
 * @node: a node to check.
 *
 * Checks if node is second member of connection. Returned %FALSE does not mean
 * that @node is first member of @edge.
 *
 * Returns: %TRUE, if @node is second member of @edge, %FALSE otherwise.
 */
gboolean
g_graph_edge_is_second(GGraphEdge* edge,
                       GGraph* node)
{
  return (edge->second == node);
}

/**
 * g_graph_edge_is_first_extended:
 * @edge: an edge.
 * @node: a node to check.
 * @is_first: pointer to #gboolean where check is stored.
 *
 * Checks if node is first member of connection. Returned %FALSE means that 
 * @node is not first nor second member of @edge. Information about being
 * a first member is saved in @is_first. If @is_first is %NULL, then no
 * no information there is stored, so this function then can actually serve as
 * a checker if @node is a member of @edge.
 *
 * Returns: %TRUE, if @node is member (first or second) of @edge, %FALSE
 * otherwise.
 */
gboolean
g_graph_edge_is_first_extended(GGraphEdge* edge,
                               GGraph* node,
                               gboolean* is_first)
{
  return _g_graph_edge_general_member_check(edge, node, is_first, TRUE);
}

/**
 * g_graph_edge_is_second_extended:
 * @edge: an edge.
 * @node: a node to check.
 * @is_second: pointer to #gboolean where check is stored.
 *
 * Checks if node is second member of connection. Returned %FALSE means that 
 * @node is not first nor second member of @edge. Information about being
 * a second member is saved in @is_second. If @is_second is %NULL, then no
 * no information there is stored, so this function then can actually serve as
 * a checker if @node is a member of @edge.
 *
 * Returns: %TRUE, if @node is member (first or second) of @edge, %FALSE
 * otherwise.
 */
gboolean
g_graph_edge_is_second_extended(GGraphEdge* edge,
                                GGraph* node,
                                gboolean* is_second)
{
  return _g_graph_edge_general_member_check(edge, node, is_first, FALSE);
}

/* static definitions */

/*
 * _g_graph_edge_general_member_check:
 * @edge: an edge.
 * @node: a node to check.
 * @check_result: pointer to #gboolean where check is stored.
 * @check_for_first: #gboolean indicating against which member check has to be
 * made.
 *
 * Checks if @node is a member of @edge. Which to check is indicated by
 * @check_for_first - if %TRUE than function will check if @node is first member
 * of @edge; if %FALSE - second member.
 *
 * Returns: %TRUE if @node is a member (first or second) of @edge, otherwise
 * %FALSE. 
 */
gboolean
_g_graph_edge_general_member_check(GGraphEdge* edge,
                                   GGraph* node,
                                   gboolean* check_result,
                                   gboolean check_for_first)
{
  GGraph* check_node;
  GGraph* other_node;
  
  if (check_for_first)
  {
    check_node = edge->first;
    other_node = edge->second;
  }
  else
  {
    check_node = edge->second;
    other_node = edge->first;
  }
  
  if (node == check_node)
  {
    if (check_result)
    {
      *check_result = TRUE;
    }
    return TRUE;
  }
  if (check_result)
  {
    *check_result = FALSE;
  }
  else if (node == other_node)
  {
    return TRUE;
  }
  return FALSE;
}
