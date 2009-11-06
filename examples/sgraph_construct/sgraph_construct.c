#include <gsgraph/gsgraph.h>

/* See topology.png. */

void free_node_data(GSGraphNode* node,
                    gpointer user_data G_GNUC_UNUSED)
{
  g_free(node->data);
}

/* This function should rather read something from file, manual creation of
 * every pair is here just to show, how it works.
 */
GSGraphSnapshot*
create_graph(void)
{
  GSGraphSnapshot* graph;
  GPtrArray* graph_array;
  gchar* node_A_data;
  gchar* node_B_data;
  gchar* node_C_data;
  gchar* node_D_data;
  GSGraphDataPair** data_pairs;

  node_A_data = g_strdup("A");
  node_B_data = g_strdup("B");
  node_C_data = g_strdup("C");
  node_D_data = g_strdup("D");
  data_pairs = g_malloc(5 * sizeof(GSGraphDataPair*));

  data_pairs[0] = g_sgraph_data_pair_new(node_A_data, node_B_data);
  data_pairs[1] = g_sgraph_data_pair_new(node_A_data, node_C_data);
  data_pairs[2] = g_sgraph_data_pair_new(node_A_data, node_D_data);
  data_pairs[3] = g_sgraph_data_pair_new(node_B_data, node_C_data);
  data_pairs[4] = g_sgraph_data_pair_new(node_C_data, node_D_data);
  graph_array = g_sgraph_snapshot_new(data_pairs, 5);
  g_sgraph_data_pair_free_v(data_pairs, 5, TRUE);
  /* Check if only one graph was created. If no - free all but first. And warn
   * about it - this should not happen in this example. */
  if (!graph_array)
  {
    g_warning("No graphs were created.\n");
    return NULL;
  }
  if (graph_array->len > 1)
  {
    guint iter;

    g_warning("More than one graph was created: %d graphs."
              " Returning first one only - rest is freed.\n", graph_array->len);
    for (iter = 1; iter < graph_array->len; iter++)
    {
      GSGraphSnapshot* separate_graph;

      separate_graph = g_ptr_array_index(graph_array, iter);
      g_sgraph_snapshot_foreach_node(separate_graph, (GFunc)free_node_data,
                                     NULL);
      g_sgraph_snapshot_free(separate_graph, TRUE);
    }
  }
  graph = g_ptr_array_index(graph_array, 0);
  g_ptr_array_free(graph_array, TRUE);
  return graph;
}

/* This function prints neighbours of a node. */
void list_neighbours(GSGraphNode* node,
                     gpointer user_data G_GNUC_UNUSED)
{
  guint iter;

  g_print("neighbours of node %s are:", (gchar*)node->data);
  for (iter = 0; iter < node->neighbours->len; iter++)
  {
    GSGraphNode* neighbour;

    neighbour = g_ptr_array_index(node->neighbours, iter);
    if (iter)
    {
      g_print(",");
    }
    g_print(" %s", (gchar*)(neighbour->data));
  }
  g_print("\n");
}

int main(void)
{
  GSGraphSnapshot* graph;

  graph = create_graph();

  g_return_val_if_fail(graph != NULL, 1);

  g_print("There are %u nodes and %u edges in graph.\n",
          g_sgraph_snapshot_get_order(graph),
          g_sgraph_snapshot_get_size(graph));
  /* This prints all neighbours of each node. */
  g_sgraph_snapshot_foreach_node(graph, (GFunc)list_neighbours, NULL);
  /* Freeing data in nodes and nodes themselves. */
  g_sgraph_snapshot_foreach_node(graph, (GFunc)free_node_data, NULL);
  g_sgraph_snapshot_free(graph, TRUE);
  /* We are done. */
  return 0;
}
