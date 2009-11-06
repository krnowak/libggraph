#include <gsgraph/gsgraph.h>

/* See topology.png. */

/* This function is ugly. */
GSGraphNode*
create_graph(void)
{
#define NODE(desc) node_##desc

#define NODE_DECL(desc) \
GSGraphNode* NODE(desc)
  NODE_DECL(A);
  NODE_DECL(B);
  NODE_DECL(C);
  NODE_DECL(D);
  NODE_DECL(E);
  NODE_DECL(F);
  NODE_DECL(G);
  NODE_DECL(H);
  NODE_DECL(I);
  NODE_DECL(J);
  NODE_DECL(K);
  NODE_DECL(L);
  NODE_DECL(M);
#undef NODE_DECL

#define NODE_DEF(desc) \
NODE(desc) = g_sgraph_node_new(g_strdup(#desc))
  NODE_DEF(A);
  NODE_DEF(B);
  NODE_DEF(C);
  NODE_DEF(D);
  NODE_DEF(E);
  NODE_DEF(F);
  NODE_DEF(G);
  NODE_DEF(H);
  NODE_DEF(I);
  NODE_DEF(J);
  NODE_DEF(K);
  NODE_DEF(L);
  NODE_DEF(M);
#undef NODE_DEF

#define NODES_CON(desc1, desc2) \
g_sgraph_node_connect(NODE(desc1), NODE(desc2))
  NODES_CON(A, B);
  NODES_CON(A, C);
  NODES_CON(B, C);
  NODES_CON(A, D);
  NODES_CON(C, D);
  NODES_CON(A, E);
  NODES_CON(A, F);
  NODES_CON(A, G);
  NODES_CON(A, H);
  NODES_CON(H, I);
  NODES_CON(F, J);
  NODES_CON(F, K);
  NODES_CON(K, J);
  NODES_CON(E, L);
  NODES_CON(G, L);
  NODES_CON(A, M);
#undef NODES_CON
  return NODE(A);
#undef NODE
}

void
print_node_desc(GSGraphNode* node,
                gpointer user_data G_GNUC_UNUSED)
{
  g_print(" `%s'", (gchar*)(node->data));
}

void
free_node_desc(GSGraphNode* node,
               gpointer user_data G_GNUC_UNUSED)
{
  g_free(node->data);
}

int
main(void)
{
  GSGraphNode* node;
  GPtrArray* separate_nodes;
  guint iter;

  node = create_graph();
  separate_nodes = g_ptr_array_new();
  while (node->neighbours->len)
  {
    GSGraphNode* neighbour;

    neighbour = g_ptr_array_index(node->neighbours, 0);
    g_sgraph_node_disconnect(node, neighbour);
    if (g_sgraph_node_are_separate(node, neighbour))
    {
      g_ptr_array_add(separate_nodes, neighbour);
    }
  }
  g_free(g_sgraph_node_free(node));

  for (iter = 0; iter < separate_nodes->len; iter++)
  {
    GSGraphWhole* subgraph;
    GSGraphNode* subnode;

    subnode = g_ptr_array_index(separate_nodes, iter);
    subgraph = g_sgraph_whole_new_from_node(subnode, G_SGRAPH_TRAVERSE_DFS);
    g_sgraph_whole_foreach_node(subgraph, (GFunc)print_node_desc, NULL);
    g_print("\n");
    g_sgraph_whole_foreach_node(subgraph, (GFunc)free_node_desc, NULL);
    g_sgraph_whole_free(subgraph, TRUE);
  }
  g_ptr_array_free(separate_nodes, TRUE);
  return 0;
}
