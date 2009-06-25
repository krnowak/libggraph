#include "ggraph.h"

// static declarations

static gboolean
_g_graph_no_indirect_connection(GGraph* graph,
                                GGraph* other_graph);

static gboolean
_g_graph_recurrent_connection_check(GGraph* graph,
                                    GGraph* other_graph,
                                    GHashTable* visited_nodes);

static GGraphArray*
_g_graph_array(GGraph* graph,
               gboolean connected_only);

static void
_g_graph_recurrent_array_append(GGraph* graph,
                                GGraphArray* graph_array,
                                GHashTable* visited_nodes);

static void
_g_graph_recurrent_array_append_connected(GGraph* graph,
                                          GGraphArray* graph_array,
                                          GHashTable* visited_nodes);

static GGraphArray*
_g_graph_neighbours_condition(GGraph* graph,
                              gboolean connected);

// function definitions

/** g_graph_new:
 * @data: data.
 *
 * Creates new separate graph consisting of only one node containing @data.
 *
 * Returns: newly created #GGraph.
 */
GGraph*
g_graph_new(gpointer data)
{
  GGraph* graph = g_slice_new(GGraph);
  graph->data = data;
  graph->edges = g_graph_edge_array_new();
  return graph;
}

/** g_graph_connect:
 * @graph: a graph.
 * @other_graph: a soon to be neighbour of @graph.
 * @weight: weight of the connection.
 *
 * Creates a connection, so @graph is connected to @other_graph. Returns %TRUE
 * if making connection succeeded. Returns %FALSE if @graph was already
 * connected to @other_graph.
 *
 * Returns: %TRUE if succeeded, %FALSE otherwise.
 */
gboolean
g_graph_connect(GGraph* graph,
                GGraph* other_graph,
                gdouble weight)
{
  GGraphEdge* edge;
  
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(other_graph != NULL, FALSE);
  
  edge = g_graph_edge(graph, other_graph);
  if (edge)
  {
    if (edge->connected)
    {
      return FALSE;
    }
    edge->connected = TRUE;
    edge->weight = weight;
    return TRUE;
  }
  else
  {
    edge = g_graph_edge_new(other_graph, TRUE, weight);
    g_graph_edge_array_add(graph->edges, edge);
    edge = g_graph_edge_new(graph, FALSE, 0);
    g_graph_edge_array_add(other_graph->edges, edge);
    return TRUE;
  }
}

/** g_graph_remove:
 * @graph: a node.
 *
 * Removes and frees a node. Data of this node should be freed earlier, if
 * applicable. Such removal may cause split of a graph into several smaller
 * ones, so this function returns #GGraphArray holding pointers to all
 * now-separated graphs.
 *
 * Returns: Array of separate graphs, which must be freed using
 * g_graph_array_free().
 */
GGraphArray*
g_graph_remove(GGraph* graph)
{
  GGraphArray* g_n;
  GGraphArray* separate_graphs;
  guint iter;
  
  g_return_val_if_fail(graph != NULL, NULL);
  
  separate_graphs = g_graph_array_new();
  g_n = g_graph_all_neighbours(graph);
  for (iter = 0; iter < g_n->len; iter++)
  {
    GGraph* other_graph = g_graph_array_index(g_n, iter);
    if (g_graph_break_connection(graph, other_graph))
    {
      g_graph_array_add(separate_graphs, other_graph);
    }
  }
  g_graph_array_free(g_n, TRUE);
  g_graph_free(graph);
  return separate_graphs;
}

/** g_graph_free:
 * @graph: a graph.
 *
 * Frees whole graph to which @graph belongs. Each node's data should be freed
 * earlier, if applicable.
 */
void
g_graph_free(GGraph* graph)
{
  GGraphArray* graph_array;
  guint iter;
  
  if (!graph)
  {
    return;
  }
  
  graph_array = _g_graph_array(graph, FALSE);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_array, iter);
    GGraphEdgeArray* n_e = node->edges;
    guint iter2;
    for (iter2 = 0; iter2 < n_e->len; iter2++)
    {
      GGraphEdge* edge = g_graph_edge_array_index(n_e, iter2);
      g_graph_edge_free(edge);
    }
    g_graph_edge_array_free(n_e, TRUE);
    g_slice_free(GGraph, node);
  }
  g_graph_array_free(graph_array, TRUE);
}

/** g_graph_count:
 * @graph: a graph.
 *
 * Counts number of nodes in graph.
 *
 * Returns: number of nodes in graph.
 */
guint
g_graph_count(GGraph* graph)
{
  GGraphArray* graph_array;
  guint count;
  
  g_return_val_if_fail(graph != NULL, 0);
  
  graph_array = _g_graph_array(graph, FALSE);
  count = graph_array->len;
  g_graph_array_free(graph_array, TRUE);
  return count;
}

/** g_graph_count_connected:
 * @graph: a graph.
 *
 * Counts number of nodes @graph is directly or indirectly connected to in
 * graph.
 *
 * Returns: number of such nodes in graph.
 */
guint
g_graph_count_connected(GGraph* graph)
{
  GGraphArray* graph_array;
  guint count;
  
  g_return_val_if_fail(graph != NULL, 0);
  
  graph_array = _g_graph_array(graph, TRUE);
  count = graph_array->len;
  g_graph_array_free(graph_array, TRUE);
  return count;
}

/** g_graph_foreach:
 * @graph: a node.
 * @func: the function to call with each element's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node in graph @graph belongs to.
 */
void
g_graph_foreach(GGraph* graph,
                GFunc func,
                gpointer user_data)
{
  GGraphArray* graph_array;
  guint iter;
  
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  graph_array = _g_graph_array(graph, FALSE);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_array, iter);
    (*func)(node->data, user_data);
  }
  g_graph_array_free(graph_array, TRUE);
}

/** g_graph_foreach_connected:
 * @graph: a node.
 * @func: the function to call with element's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node @graph is directly or indirectly connected to.
 */
void
g_graph_foreach_connected(GGraph* graph,
                          GFunc func,
                          gpointer user_data)
{
  GGraphArray* graph_array;
  guint iter;
  
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  graph_array = _g_graph_array(graph, TRUE);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_array, iter);
    (*func)(node->data, user_data);
  }
  g_graph_array_free(graph_array, TRUE);
}

/** g_graph_all_neighbours:
 * @graph: a node.
 *
 * Gets an array of neighbours, to which @graph is not connected.
 *
 * Returns: #GGraphArray which must be freed with g_graph_array_free() or %NULL
 * if there was no such neighbours.
 */
GGraphArray*
g_graph_all_neighbours(GGraph* graph)
{
  GGraphArray* neighbours_array;
  GGraphEdgeArray* g_e;
  guint iter;
  
  g_return_val_if_fail(graph != NULL, NULL);
  
  g_e = graph->edges;
  if (!g_e->len)
  {
    return NULL;
  }
  neighbours_array = g_graph_array_sized_new(g_e->len);
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(g_e, iter);
    g_graph_array_add(neighbours_array, edge->graph);
  }
  return neighbours_array;
}

/** g_graph_connected_neighbours:
 * @graph: a node.
 *
 * Gets an array of neighbours, to which @graph is connected.
 *
 * Returns: #GGraphArray which must be freed with g_graph_array_free() or %NULL
 * if there was no such neighbours.
 */
GGraphArray*
g_graph_connected_neighbours(GGraph* graph)
{
  g_return_val_if_fail(graph != NULL, NULL);
  
  return _g_graph_neighbours_condition(graph, TRUE);
}

/** g_graph_not_connected_neighbours:
 * @graph: a node.
 *
 * Gets an array of neighbours, to which @graph is not connected.
 *
 * Returns: #GGraphArray which must be freed with g_graph_array_free() or %NULL
 * if there was no such neighbours.
 */
GGraphArray*
g_graph_not_connected_neighbours(GGraph* graph)
{
  g_return_val_if_fail(graph != NULL, NULL);
  
  return _g_graph_neighbours_condition(graph, FALSE);
}

/** g_graph_edge:
 * @graph: a node.
 * @other_graph: other node which is @graph's neighbour.
 *
 * Gets edge from @graph to @other_graph.
 *
 * Returns: #GGraphEdge if there was such edge, %NULL otherwise.
 */
GGraphEdge*
g_graph_edge(GGraph* graph,
             GGraph* other_graph)
{
  guint iter;
  GGraphEdgeArray* g_e;
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(other_graph != NULL, NULL);
  g_e = graph->edges;
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(g_e, iter);
    if (edge->graph == other_graph)
    {
      return edge;
    }
  }
  return NULL;
}

/** g_graph_remove_connection:
 * @graph: a node.
 * @other_graph: other node which is @graph's neighbour.
 *
 * Removes connection from @graph to @other_graph, so in effect they might stop
 * being neighbours if @other_graph wasn't connected to @graph. If such breakage
 * causes to create two separate graphs, notify about it by returning %TRUE.
 * This function return %FALSE also when @graph and @other_graph aren't
 * neighbours, so there was no connection to break.
 *
 * Returns: %TRUE if @graph and @other_graph are now in separate graphs,
 * otherwise %FALSE.
 */
gboolean
g_graph_remove_connection(GGraph* graph,
                          GGraph* other_graph)
{
  GGraphEdge* edge;
  GGraphEdge* other_edge;
  
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(other_graph != NULL, FALSE);
  
  edge = g_graph_edge(graph, other_graph);
  if (!edge)
  {
    return FALSE;
  }
  other_edge = g_graph_edge(other_graph, graph);
  g_return_val_if_fail(other_edge != NULL, FALSE);
  
  if (edge->connected && !other_edge->connected)
  {
    g_graph_edge_array_remove(graph->edges, edge);
    g_graph_edge_array_remove(other_graph->edges, other_edge);
    g_graph_edge_free(edge);
    g_graph_edge_free(other_edge);
    return _g_graph_no_indirect_connection(graph, other_graph);
  }
  else if (edge->connected)
  {
    edge->connected = FALSE;
    edge->weight = 0;
  }
  return FALSE;
}

/** g_graph_break_connection:
 * @graph: a node.
 * @other_graph: other node which is @graph's neighbour.
 *
 * Breaks both connections from one node to another, so in effect they stop
 * being neighbours. If such breakage causes to create two separate graphs,
 * notify about it by returning %TRUE. This function return %FALSE also when
 * @graph and @other_graph aren't neighbours, so there was no connection to
 * break.
 *
 * Returns: %TRUE if @graph and @other_graph are now in separate graphs,
 * otherwise %FALSE.
 */
gboolean
g_graph_break_connection(GGraph* graph,
                         GGraph* other_graph)
{
  GGraphEdge* edge;
  GGraphEdge* other_edge;
  
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(other_graph != NULL, FALSE);
  
  edge = g_graph_edge(graph, other_graph);
  if (!edge)
  {
    return FALSE;
  }
  other_edge = g_graph_edge(other_graph, graph);
  g_return_val_if_fail(other_edge != NULL, FALSE);
  g_graph_edge_array_remove(graph->edges, edge);
  g_graph_edge_array_remove(other_graph->edges, other_edge);
  g_graph_edge_free(edge);
  g_graph_edge_free(other_edge);
  return _g_graph_no_indirect_connection(graph, other_graph);
}

/** g_graph_find:
 * @graph: a node.
 * @data: the element data to find.
 *
 * It calls g_graph_find_custom() with g_direct_equal() as a third parameter.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find(GGraph* graph,
             gconstpointer data)
{
  return g_graph_find_custom(graph, data, g_direct_equal);
}

/** g_graph_find_connected:
 * @graph: a node.
 * @data: the element data to find.
 *
 * It calls g_graph_find_custom_conneted() with g_direct_equal() as a third
 * parameter.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_connected(GGraph* graph,
                       gconstpointer data)
{
  return g_graph_find_custom_connected(graph, data, g_direct_equal);
}

/** g_graph_find_custom:
 * @graph: a node.
 * @data: data passed to @func.
 * @func: the function to call for each element.
 *
 * Finds elements in a #GGraph, using a supplied function to find the desired
 * elements. It iterates over the whole graph, calling the given function which
 * should return %TRUE when the desired element is found. The function takes two
 * gconstpointer arguments, the #GGraph element's data as the first argument and
 * the given user data as second. If an element is found it is added to a
 * #GGraphArray.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_custom(GGraph* graph,
                    gconstpointer data,
                    GEqualFunc func)
{
  GGraphArray* matching_graphs;
  GGraphArray* graph_array;
  guint iter;
  
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  matching_graphs = g_graph_array_new();
  graph_array = _g_graph_array(graph, FALSE);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_array, iter);
    if ((*func)(node->data, data))
    {
      g_graph_array_add(matching_graphs, node);
    }
  }
  g_graph_array_free(graph_array, TRUE);
  if (!matching_graphs->len)
  {
    g_graph_array_free(matching_graphs, TRUE);
    matching_graphs = NULL;
  }
  return matching_graphs;
}

/** g_graph_find_custom_connected:
 * @graph: a node.
 * @data: data passed to @func.
 * @func: the function to call for each element.
 *
 * Finds elements in a #GGraph, using a supplied function to find the desired
 * elements. It iterates over this nodes in graph, to which @graph is directly
 * or indirectly connected, calling the given function which should return %TRUE
 * when the desired element is found. The function takes two gconstpointer
 * arguments, the #GGraph element's data as the first argument and the given
 * user data as second. If an element is found it is added to a #GGraphArray.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_custom_connected(GGraph* graph,
                              gconstpointer data,
                              GEqualFunc func)
{
  GGraphArray* matching_graphs;
  GGraphArray* graph_array;
  guint iter;
  
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  matching_graphs = g_graph_array_new();
  graph_array = _g_graph_array(graph, TRUE);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_array, iter);
    if ((*func)(node->data, data))
    {
      g_graph_array_add(matching_graphs, node);
    }
  }
  g_graph_array_free(graph_array, TRUE);
  if (!matching_graphs->len)
  {
    g_graph_array_free(matching_graphs, TRUE);
    matching_graphs = NULL;
  }
  return matching_graphs;
}

/** g_graph_data:
 * @graph: a node.
 *
 * It's an accessor function for language bindings.
 *
 * Returns: data associated to @graph.
 */
gpointer
g_graph_data(GGraph* graph)
{
  return graph->data;
}

/** g_graph_edges:
 * @graph: a node.
 *
 * It's an accessor function for language bindings.
 *
 * Returns: #GGraphEdgeArray associated to @graph, which shouldn't be modified.
 */
const GGraphEdgeArray*
g_graph_edges(GGraph* graph)
{
  return graph->edges;
}

// static function definitions

/* _g_graph_no_indirect_connection:
 * @graph: starting node.
 * @other_graph: node for which we want to check if it has series of edges to
 * @graph.
 *
 * Mind it that name of this function is confusing - it checks if there is no
 * series of edges between @graph and @other_graph, not if @graph is indirectly
 * connected to @other_graph.
 *
 * Returns: %TRUE if @graph and @other_graph are in separate graphs.
 */
static gboolean
_g_graph_no_indirect_connection(GGraph* graph,
                                GGraph* other_graph)
{
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  gboolean not_connected = !_g_graph_recurrent_connection_check(graph,
                                                                other_graph,
                                                                visited_nodes);
  g_hash_table_unref(visited_nodes);
  return not_connected;
}

/* _g_graph_recurrent_connection_check:
 * @graph: starting node.
 * @other_graph: other node.
 * @visited_nodes: #GHashTable holding information which nodes were already
 * visited.
 *
 * Checks if @graph == @other_graph. If not it checks @graph's neighbours.
 *
 * Returns: %TRUE if @graph == @other_graph, otherwise %FALSE.
 */
static gboolean
_g_graph_recurrent_connection_check(GGraph* graph,
                                    GGraph* other_graph,
                                    GHashTable* visited_nodes)
{
  guint iter;
  GGraphEdgeArray* g_e;
  if (graph == other_graph)
  {
    return TRUE;
  }
  if (g_hash_table_lookup_extended(visited_nodes, graph, NULL, NULL))
  {
    return FALSE;
  }
  g_hash_table_insert(visited_nodes, graph, NULL);
  g_e = graph->edges;
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(g_e, iter);
    if (_g_graph_recurrent_connection_check(edge->graph, other_graph,
                                            visited_nodes))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/* _g_graph_array:
 * @graph: graph, which will be put into an array.
 * @connected_only: %TRUE if only connected nodes should be put.
 *
 * Puts graph into an array, so every node can be accessed without hassle. If
 * @connected_only is %FALSE, then whole graph is put into array.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
static GGraphArray*
_g_graph_array(GGraph* graph,
               gboolean connected_only)
{
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  GGraphArray* graph_array = g_graph_array_new();
  if (connected_only)
  {
    _g_graph_recurrent_array_append_connected(graph, graph_array,
                                              visited_nodes);
  }
  else
  {
    _g_graph_recurrent_array_append(graph, graph_array, visited_nodes);
  }
  g_hash_table_unref(visited_nodes);
  return graph_array;
}

/* _g_graph_recurrent_array_append:
 * @graph: node which will be (probably) put into @graph_array.
 * @graph_array: array of nodes of a whole graph.
 * @visited_nodes: #GHashTable holding information which nodes were already
 * visited.
 *
 * If @graph isn't already in @graph_array, it will be put into it.
 */
static void
_g_graph_recurrent_array_append(GGraph* graph,
                                GGraphArray* graph_array,
                                GHashTable* visited_nodes)
{
  guint iter;
  GGraphEdgeArray* g_e;
  if (g_hash_table_lookup_extended(visited_nodes, graph, NULL, NULL))
  {
    return;
  }
  g_hash_table_insert(visited_nodes, graph, NULL);
  g_graph_array_add(graph_array, graph);
  g_e = graph->edges;
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(g_e, iter);
    _g_graph_recurrent_array_append(edge->graph, graph_array, visited_nodes);
  }
}

/* _g_graph_recurrent_array_append_connected:
 * @graph: node which will be (probably) put into @graph_array.
 * @graph_array: array of nodes of a graph.
 * @visited_nodes: #GHashTable holding information which nodes were already
 * visited.
 *
 * If @graph isn't already in @graph_array, it will be put into it.
 */
static void
_g_graph_recurrent_array_append_connected(GGraph* graph,
                                          GGraphArray* graph_array,
                                          GHashTable* visited_nodes)
{
  guint iter;
  GGraphEdgeArray* g_e;
  if (g_hash_table_lookup_extended(visited_nodes, graph, NULL, NULL))
  {
    return;
  }
  g_hash_table_insert(visited_nodes, graph, NULL);
  g_graph_array_add(graph_array, graph);
  g_e = graph->edges;
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(g_e, iter);
    if (edge->connected)
    {
      _g_graph_recurrent_array_append_connected(edge->graph, graph_array,
                                                visited_nodes);
    }
  }
}

/* _g_graph_neighbours_condition:
 * @graph: node which neighbours are to be returned.
 * @connected: set it to %TRUE, if function have to return nodes @graph is
 * connected to.
 *
 * Gets @graph's neighbours. @connected decides, which ones.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
static GGraphArray*
_g_graph_neighbours_condition(GGraph* graph,
                              gboolean connected)
{
  GGraphArray* neighbours_array;
  GGraphEdgeArray* g_e = graph->edges;
  guint iter;
  if (!g_e->len)
  {
    return NULL;
  }
  neighbours_array = g_graph_array_new();
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(g_e, iter);
    if (edge->connected == connected)
    {
      g_graph_array_add(neighbours_array, edge->graph);
    }
  }
  if (!neighbours_array->len)
  {
    g_graph_array_free(neighbours_array, TRUE);
    neighbours_array = NULL;
  }
  return neighbours_array;
}
