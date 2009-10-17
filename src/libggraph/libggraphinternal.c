#include "libggraphinternal.h"

/* static declarations */

static gboolean
_libggraph_internal_recurrent_connection_check(GGraph* graph,
                                               GGraph* other_graph,
                                               GHashTable* visited_nodes)

/* function definitions */

/*
 * _libggraph_internal_are_separate:
 * @graph: starting node.
 * @other_graph: node for which we want to check if it has series of edges to
 * @graph.
 *
 * Checks if there is no series of edges between @graph and @other_graph.
 *
 * Returns: %TRUE if @graph and @other_graph are in separate graphs.
 */
gboolean
_libggraph_internal_are_separate(GGraph* graph,
                                 GGraph* other_graph)
{
  gboolean separate;
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  separate = !_libggraph_internal_recurrent_connection_check(graph,
                                                             other_graph,
                                                             visited_nodes);
  g_hash_table_unref(visited_nodes);
  return separate;
}

/* static definitions */

/*
 * _libggraph_internal_recurrent_connection_check
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
_libggraph_internal_recurrent_connection_check(GGraph* graph,
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
    GGraph* node = g_graph_edge_get_graph(edge, graph);
    if (_g_graph_recurrent_connection_check(node, other_graph, visited_nodes))
    {
      return TRUE;
    }
  }
  return FALSE;
}
