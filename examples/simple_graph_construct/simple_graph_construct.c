#include <libggraph.h>

/* See topology.png. */

void free_node_data(gpointer data,
                    gpointer user_data G_GNUC_UNUSED)
{
  g_free(data);
}

GSGraph*
create_graph()
{
  GSGraphArray* graph_array;
  GSGraph* graph;
  gchar* node_A_data = g_strdup("A");
  gchar* node_B_data = g_strdup("B");
  gchar* node_C_data = g_strdup("C");
  gchar* node_D_data = g_strdup("D");
  GSGraphDataPair** data_pairs = g_malloc(5 * sizeof(GSGraphDataPair*));
  
  data_pairs[0] = g_sgraph_data_pair_new(node_A_data, node_B_data);
  data_pairs[1] = g_sgraph_data_pair_new(node_A_data, node_C_data);
  data_pairs[2] = g_sgraph_data_pair_new(node_A_data, node_D_data);
  data_pairs[3] = g_sgraph_data_pair_new(node_B_data, node_C_data);
  data_pairs[4] = g_sgraph_data_pair_new(node_C_data, node_D_data);
  graph_array = g_sgraph_construct(data_pairs, 5);
  g_sgraph_data_pair_free_v(data_pairs, 5);
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
      GSGraph* separate_graph = g_sgraph_array_index(graph_array, iter);
      g_sgraph_foreach(separate_graph, free_node_data, NULL);
      g_sgraph_free(separate_graph);
    }
  }
  graph = g_sgraph_array_index(graph_array, 0);
  g_sgraph_array_free(graph_array, TRUE);
  return graph;
}

/* This function prints neighbours of a node. */
void list_neighbours(GSGraph* node,
                     gpointer user_data G_GNUC_UNUSED)
{
  guint iter;
  
  g_print("neighbours of node %s are:", (gchar*)node->data);
  for (iter = 0; iter < node->neighbours->len; iter++)
  {
    GSGraph* neighbour = g_sgraph_array_index(node->neighbours, iter);
    if (iter)
    {
      g_print(",");
    }
    g_print(" %s", (gchar*)(neighbour->data));
  }
  g_print("\n");
}

int main()
{
  GSGraph* graph = create_graph();
  
  g_return_val_if_fail(graph != NULL, 1);
  
  g_print("There are %u nodes in graph.\n", g_sgraph_count(graph));
  /* This prints all neighbours of each node. */
  g_sgraph_foreach_node(graph, list_neighbours, NULL);
  /* Freeing data in nodes and nodes themselves. */
  g_sgraph_foreach(graph, free_node_data, NULL);
  g_sgraph_free(graph);
  /* We are done. */
  return 0;
}
