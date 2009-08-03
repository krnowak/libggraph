#include <libggraph.h>

/* See topology.png. */

GSGraph*
create_graph()
{
  GSGraph* node_A = g_sgraph_new(g_strdup("A"));
  GSGraph* node_B = g_sgraph_new(g_strdup("B"));
  g_sgraph_connect(node_A, node_B);
  GSGraph* node_C = g_sgraph_new(g_strdup("C"));
  g_sgraph_connect(node_A, node_C);
  g_sgraph_connect(node_B, node_C);
  GSGraph* node_D = g_sgraph_new(g_strdup("D"));
  g_sgraph_connect(node_A, node_D);
  g_sgraph_connect(node_C, node_D);
  GSGraph* node_E = g_sgraph_new(g_strdup("E"));
  g_sgraph_connect(node_A, node_E);
  GSGraph* node_F = g_sgraph_new(g_strdup("F"));
  g_sgraph_connect(node_A, node_F);
  GSGraph* node_G = g_sgraph_new(g_strdup("G"));
  g_sgraph_connect(node_A, node_G);
  GSGraph* node_H = g_sgraph_new(g_strdup("H"));
  g_sgraph_connect(node_A, node_H);
  GSGraph* node_I = g_sgraph_new(g_strdup("I"));
  g_sgraph_connect(node_H, node_I);
  GSGraph* node_J = g_sgraph_new(g_strdup("J"));
  g_sgraph_connect(node_F, node_J);
  GSGraph* node_K = g_sgraph_new(g_strdup("K"));
  g_sgraph_connect(node_F, node_K);
  g_sgraph_connect(node_K, node_J);
  GSGraph* node_L = g_sgraph_new(g_strdup("L"));
  g_sgraph_connect(node_E, node_L);
  g_sgraph_connect(node_G, node_L);
  GSGraph* node_M = g_sgraph_new(g_strdup("M"));
  g_sgraph_connect(node_A, node_M);
  return node_A;
}

void print_node_data(gpointer data,
                     gpointer user_data G_GNUC_UNUSED)
{
  g_print(" `%s'", (gchar*)data);
}

void free_node_data(gpointer data,
                    gpointer user_data G_GNUC_UNUSED)
{
  g_free(data);
}

int main()
{
  GSGraph* graph = create_graph();
  g_print("There are %u nodes in graph.\n", g_sgraph_count(graph));
  /* Here we search for "I" nodes and break connections to all its neighbours.
   * In this topology there is only one such node and has only one neighbour.
   */
  GSGraphArray* array_I = g_sgraph_find_custom(graph, "I", g_str_equal);
  g_print("\"I\" nodes found: %u\n", array_I->len);
  guint i_nodes_iter;
  /* For every node that has "I"... */
  for (i_nodes_iter = 0; i_nodes_iter < array_I->len; i_nodes_iter++)
  {
    GSGraph* node_I = g_sgraph_array_index(array_I, i_nodes_iter);
    /* ...get its neighbours,... */
    GSGraphArray* node_I_neighbours = node_I->neighbours;
    g_print("%u \"I\" node's neighbours count: %u\n", i_nodes_iter,
            node_I_neighbours->len);
    guint neighbours_iter;
    for (neighbours_iter = 0; neighbours_iter < node_I_neighbours->len;
         neighbours_iter++)
    {
      GSGraph* node_I_neighbour = g_sgraph_array_index(node_I_neighbours,
                                                       neighbours_iter);
      /* ...if after breaking connection both nodes are in separate graphs,
       * print info about it...
       */
      if (g_sgraph_break_connection(node_I, node_I_neighbour))
      {
        g_print("%u \"I\" node and its neighbour nr %u (\"%s\" node) are now in "
                "separate graphs.\n", i_nodes_iter, neighbours_iter,
                (gchar*)(node_I_neighbour->data));
      }
    }
    /* ...and remove the "I" node. */
    g_free(node_I->data);
    g_sgraph_free(node_I);
    g_print("\"I\" node nr %u deleted.\n", i_nodes_iter);
  }
  g_sgraph_array_free(array_I, TRUE);
  GSGraphArray* node_A_neighbours = graph->neighbours;
  guint A_iter;
  g_print("\"A\" node neighbours:");
  for (A_iter = 0; A_iter < node_A_neighbours->len; A_iter++)
  {
    GSGraph* node = g_sgraph_array_index(node_A_neighbours, A_iter);
    g_print(" `%s'", (gchar*)(node->data));
  }
  g_print("\n");
  /* Now lets remove the "A" node. */
  g_free(graph->data);
  GSGraphArray* separate_graphs = g_sgraph_remove(graph);
  /* Print names of all nodes in all separate graphs and delete the subgraph. */
  g_print("There are now %u separate graphs.\n", separate_graphs->len);
  guint s_g_iter;
  for (s_g_iter = 0; s_g_iter < separate_graphs->len; s_g_iter++)
  {
    g_print("Subgraph %u:", s_g_iter);
    GSGraph* node = g_sgraph_array_index(separate_graphs, s_g_iter);
    g_sgraph_foreach(node, print_node_data, NULL);
    g_print("\n");
    g_sgraph_foreach(node, free_node_data, NULL);
    g_sgraph_free(node);
  }
  g_sgraph_array_free(separate_graphs, TRUE);
  return 0;
}
