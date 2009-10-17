#include "ggraphedge.h"
#include "libggraphinternal.h"

/**
 * SECTION: ggraphedge
 * @title: Graph edges
 * @short_description: edges connecting graphs and containing user data.
 * @include: libggraph.h
 * @see_also: #GGraph, #GGraphEdgeConnected
 *
 * Edges connect two nodes in graph, describes its relation and holds data. If
 * there is connection between two nodes, there are three possible option, which
 * are described with #GGraphEdgeConnected.
 *
 * To create new edge, use g_graph_edge_new().
 *
 * To free it, use g_graph_edge_free().
 *
 * To get a neighbour of a node in edge, use g_graph_edge_get_graph().
 *
 * To check which member of edge node is, use g_graph_edge_is_first(),
 * g_graph_edge_is_first_extended(), g_graph_edge_is_second() or
 * g_graph_edge_is_second_extended().
 *
 * To check if node is connected to other, use g_graph_edge_is_graph_connected()
 * or g_graph_edge_is_graph_connected_extended().
 */

/* static declarations */

static gboolean
_g_graph_edge_general_member_check(GGraphEdge* edge,
                                   GGraph* node,
                                   gboolean* check_result,
                                   gboolean check_for_first);

static gboolean
_g_graph_edge_disjoin(GGraphEdge* edge);

/* function definitions */

/* TODO: fix docs. */
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
  g_graph_edge_array_append(first->edges, edge);
  if (first != second)
  {
    g_graph_edge_array_append(second->edges, edge);
  }
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
 * g_graph_edge_get_graph:
 * @edge: an edge.
 * @node: a node using this connection.
 *
 * Gets a node to which a @node is connected. If @node is not a part of @edge,
 * this function will return %NULL.
 *
 * Returns: #GGraph associated to @edge being a neighbour of @node.
 */
GGraph*
g_graph_edge_get_graph(GGraphEdge* edge,
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
 * information there is stored, so this function then can actually serve as a
 * checker if @node is a member of @edge.
 *
 * Returns: %TRUE, if @node is member (first or second) of @edge, %FALSE
 * otherwise.
 */
gboolean
g_graph_edge_is_second_extended(GGraphEdge* edge,
                                GGraph* node,
                                gboolean* is_second)
{
  return _g_graph_edge_general_member_check(edge, node, is_second, FALSE);
}

/**
 * g_graph_edge_is_graph_connected:
 * @edge: an edge.
 * @node: a node to check.
 *
 * Checks if @node is connected to other node in @edge. Returned %FALSE can mean
 * that @node is not a part of @edge.
 *
 * Returns: %TRUE, if @node is connected, %FALSE otherwise.
 */
gboolean
g_graph_edge_is_graph_connected(GGraphEdge* edge,
                                GGraph* node)
{
  return ((g_graph_edge_is_first(edge, node) && (edge->connected & G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND)) ||
          (g_graph_edge_is_second(edge, node) && (edge->connected & G_GRAPH_EDGE_CONNECTED_SECOND_TO_FIRST)));
}

/**
 * g_graph_edge_is_graph_connected_extended:
 * @edge: an edge.
 * @node: a node to check.
 * @is_connected: pointer to #gboolean where check is stored.
 *
 * Checks if @node is connected to other node in @edge. Returned %FALSE means
 * that @node is not a member of @edge. Information about being connected is
 * saved in @is_connected. If @is_connected is %NULL, then no information there
 * is stored, so this function then can actually serve as a checker if @node is
 * a member of @edge.
 *
 * Returns: %TRUE, if @node is member (first or second) of @edge, %FALSE
 * otherwise.
 */
gboolean
g_graph_edge_is_graph_connected_extended(GGraphEdge* edge,
                                         GGraph* node,
                                         gboolean* is_connected)
{
  gboolean is_first;
  
  if (g_graph_edge_is_first_extended(edge, node, &is_first))
  {
    if (is_connected)
    {
      *is_connected = ((is_first && (edge->connected & G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND)) ||
                       (!is_first && (edge->connected & G_GRAPH_EDGE_CONNECTED_SECOND_TO_FIRST)));
    }
    return TRUE;
  }
  if (is_connected)
  {
    *is_connected = FALSE;
  }
  return FALSE;
}

/* TODO: document it. */
gboolean
g_graph_edge_remove(GGraphEdge* edge)
{
  gboolean result;
  
  g_return_val_if_fail(edge != NULL, FALSE);
  
  result = _g_graph_edge_disjoin(edge);
  g_graph_edge_free(edge);
  return result;
}

/* TODO: document it. */
gboolean
g_graph_edge_disconnect(GGraphEdge* edge)
{
  g_return_val_if_fail(edge != NULL, FALSE);
  
  return _g_graph_edge_disjoin(edge);
}

/* TODO: document it. */
gboolean
g_graph_edge_connect_graph(GGraphEdge* edge,
                           GGraph* graph)
{
  g_return_if_fail(edge != NULL);
  g_return_if_fail(graph != NULL);
  
  if (g_graph_edge_is_first(edge, graph))
  {
    edge->connected |= G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND;
    return TRUE;
  }
  else if (g_graph_edge_is_second(edge, graph))
  {
    edge->connected |= G_GRAPH_EDGE_CONNECTED_SECOND_TO_FIRST;
    return TRUE;
  }
  return FALSE;
}

/* TODO: document it. rethink it. */
gboolean
g_graph_edge_remove_connection(GGraphEdge* edge,
                               GGraph* graph)
{
  g_return_val_if_fail(edge != NULL, FALSE);
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
static gboolean
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

/* TODO: static - document it. */
static gboolean
_g_graph_edge_disjoin(GGraphEdge* edge)
{
  gboolean are_separate;
  
  g_graph_edge_array_remove(edge->first->edges, edge);
  if (edge->first != edge->second)
  {
    g_graph_edge_array_remove(edge->second->edges, edge);
    are_separate = _libggraph_internal_are_separate(edge->first, edge->second);
  }
  else
  {
    are_separate = FALSE;
  }
  edge->first = NULL;
  edge->second = NULL;
  return are_separate;
}
