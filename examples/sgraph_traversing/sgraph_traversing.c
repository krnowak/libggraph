#include <gsgraph/gsgraph.h>

/* See topology.png. */

/* This function is ugly. */
GSGraphNode*
create_graph(void)
{
#define NODE(desc) node_##desc

#define NODE_DECL(desc) \
GSGraphNode* NODE(desc);
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
NODE(desc) = g_sgraph_node_new(g_strdup(#desc));
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
g_sgraph_node_connect(NODE(desc1), NODE(desc2));
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

gboolean
find_node(GSGraphNode* node,
          const gchar* desc)
{
  if (g_strcmp0(node->data, desc))
  {
    return FALSE;
  }
  return TRUE;
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
  GSGraphWhole* graph;
  GSGraphNode* node;

  node = create_graph();
  graph = g_sgraph_whole_new_from_node(node, G_SGRAPH_TRAVERSE_BFS);
  g_print("BFS from `A':");
  g_sgraph_whole_foreach_node(graph, (GFunc)print_node_desc, NULL);
  g_sgraph_whole_free(graph, FALSE);
  graph = g_sgraph_whole_new_from_node(node, G_SGRAPH_TRAVERSE_DFS);
  g_print("\nDFS from `A':");
  g_sgraph_whole_foreach_node(graph, (GFunc)print_node_desc, NULL);
  node = g_sgraph_whole_find_node_custom(graph, "I", (GEqualFunc)find_node);
  g_sgraph_whole_free(graph, FALSE);
  graph = g_sgraph_whole_new_from_node(node, G_SGRAPH_TRAVERSE_BFS);
  g_print("\nBFS from `I':");
  g_sgraph_whole_foreach_node(graph, (GFunc)print_node_desc, NULL);
  g_sgraph_whole_free(graph, FALSE);
  graph = g_sgraph_whole_new_from_node(node, G_SGRAPH_TRAVERSE_DFS);
  g_print("\nDFS from `I':");
  g_sgraph_whole_foreach_node(graph, (GFunc)print_node_desc, NULL);
  g_print("\n");
  g_sgraph_whole_foreach_node(graph, (GFunc)free_node_desc, NULL);
  g_sgraph_whole_free(graph, TRUE);
  return 0;
}
