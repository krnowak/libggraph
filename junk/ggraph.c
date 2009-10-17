#include "ggraph.h"
#include "libggraphinternal.h"

/* TODO: when getting neighbours, do not return a node connected to self? */
/* TODO: handle connections to self better - set of static functions for s-c? */

/**
 * SECTION: ggraph
 * @title: Graphs
 * @short_description: directed graphs with edges containing user data
 * @include: libggraph.h
 * @see_also: #GGraphEdge
 *
 * Single node of directed graph. It contains data and an array of pointers to
 * edges to all its neighbours - that is, all nodes that are connected to this
 * node or all nodes this node is connected to. By term "node A being connected
 * to node B" it is meant that there is directed edge from A to B.
 *
 * To create a single node, use g_graph_new(). If you have a set of data you can
 * use g_graph_construct() to build graphs.
 *
 * To connect two nodes use g_graph_connect() or g_graph_connect_with_data().
 *
 * To remove a node from graph and delete it, use g_graph_remove(). If you don't
 * want to delete the node itself, use g_graph_break_connection() on all its
 * neighbours.
 *
 * To get neighbours, use g_graph_get_all_neighbours(),
 * g_graph_get_connected_neighbours() or g_graph_get_not_connected_neighbours().
 *
 * To get count of a graph, use g_graph_count_nodes(), g_graph_count_edges(),
 * g_graph_count_connected_nodes() or g_graph_count_connected_edges().
 *
 * To free whole graph, use g_graph_free().
 *
 * <note>
 *   <para>
 *     Remember to always take care about data graph holds. Especially when
 *     freeing nodes and edges. Some functions will return a data of freed
 *     entities, but not all. So, for example, when freeing whole graph, it is
 *     good to use g_graph_foreach() and g_graph_edge_foreach() to free data
 *     before.
 *   </para>
 * </note>
 */

/* internal types. */

/*
 * GGraphWhole:
 * @graph_array: array of all nodes in graph.
 * @edge_array: array of all edges in graph.
 *
 * Convenient structure holding whole graph in two arrays.
 */
typedef struct _GGraphWhole GGraphWhole;

struct _GGraphWhole
{
  GGraphArray* graph_array;
  GGraphEdgeArray* edge_array;
};

/*
 * GGraphConstructFlags:
 * @G_GRAPH_NONE: no nodes were created.
 * @G_GRAPH_FIRST: node containing first data was created.
 * @G_GRAPH_SECOND: node containing second data was created.
 * @G_GRAPH_BOTH: both nodes were created.
 *
 * Internal enum for g_graph_construct() needs. Describes which nodes were
 * created.
 */
typedef enum
{
  G_GRAPH_NONE = 0,
  G_GRAPH_FIRST = 1 << 0,
  G_GRAPH_SECOND = 1 << 1,
  G_GRAPH_BOTH = G_GRAPH_FIRST | G_GRAPH_SECOND
} GGraphConstructFlags;

/* static declarations */

static GGraphArray*
_g_graph_array(GGraph* graph,
               gboolean connected_only);

static GGraphArray*
_g_graph_get_neighbours_general(GGraph* graph,
                              gboolean connected);

static GGraphEdge*
_g_graph_connection_new(GGraph* graph,
                        GGraph* other_graph,
                        gpointer data,
                        gboolean one_way);

static GGraphWhole*
_g_graph_whole_new(GGraphArray* graph_array,
                   GGraphEdgeArray* edge_array);

static void
_g_graph_whole_free(GGraphWhole* graph_whole,
                    gboolean free_arrays);

static GGraphWhole*
_g_graph_whole(GGraph* graph);

static void
_g_graph_recurrent_whole_append(GGraph* graph,
                                GGraphArray* graph_array,
                                GHashTable* visited_nodes,
                                GGraphEdgeArray* edge_array,
                                GHashTable* visited_edges,
                                gboolean connected_only);

static gboolean
_data_quadruplet_is_valid(GGraphDataQuadruplet* data_quadruplet);

static GGraphEdgeArray*
_g_graph_edge_array(GGraph* graph,
                    gboolean connected_only);

static void
_g_graph_foreach_general(GGraph* graph,
                         GFunc func,
                         gpointer user_data,
                         gboolean connected_only);

static void
_g_graph_foreach_node_general(GGraph* graph,
                              GGraphFunc func,
                              gpointer user_data,
                              gboolean connected_only);

static void
_g_graph_edge_foreach_general(GGraph* graph,
                              GFunc func,
                              gpointer user_data,
                              gboolean connected_only);

static void
_g_graph_edge_foreach_edge_general(GGraph* graph,
                                   GGraphEdgeFunc func,
                                   gpointer user_data,
                                   gboolean connected_only);

static GGraphArray*
_g_graph_find_custom_general(GGraph* graph,
                             gconstpointer data,
                             GEqualFunc func,
                             gboolean connected_only);

static GGraphArray*
_g_graph_find_custom_node_general(GGraph* graph,
                                  GGraph* node,
                                  GGraphEqualFunc func,
                                  gboolean connected_only);

static GGraphEdgeArray*
_g_graph_edge_find_custom_general(GGraph* graph,
                                  gconstpointer data,
                                  GEqualFunc func,
                                  gboolean connected_only);

static GGraphEdgeArray*
_g_graph_edge_find_custom_edge_general(GGraph* graph,
                                       GGraphEdge* edge,
                                       GGraphEdgeEqualFunc func,
                                       gboolean connected_only);

static GGraphArray*
_g_graph_disjoin(GGraph* graph,
                 GPtrArray* edge_data_array);

/* function definitions */

/**
 * g_graph_new:
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

/**
 * g_graph_construct:
 * @data_quadruplets: array of data quadruplets.
 * @count: length of @data_pairs.
 *
 * Creates a graph from passed data quadruplets. Resulting construction can be
 * several separate graphs, so an array of them is returned. Also, if first or
 * second #GGraphDataQuadruplet member is %NULL or an option is invalid, then
 * this quadruplet is omitted in creation. If @count is -1, it is assumed that
 * @data_quadruplets is %NULL terminated.
 * For performance reasons it would be good if in second and later pairs one
 * of node have data describing already created node.
 * Lets assume we want to create this graph:
 * <informalexample>
 *   <programlisting>
 *     A--B
 *     |
 *     C--D
 *   </programlisting>
 * </informalexample>
 * So best order of pairs is:
 * <itemizedlist>
 *   <listitem>
 *     <para>
 *       A, B
 *     </para>
 *   </listitem>
 *   <listitem>
 *     <para>
 *       A, C
 *     </para>
 *   </listitem>
 *   <listitem>
 *     <para>
 *       C, D
 *     </para>
 *   </listitem>
 * </itemizedlist>
 * This way we never create a separate graph, because in given pair one of the
 * members is already created, so newly will be its neighbour and a part of
 * larger graph.
 * Swaping second and third pair will cause creation of a separate graph while
 * processing second pair and checking if two graphs are one when processing
 * third pair.
 *
 * Returns: array of newly created separate graphs or %NULL if no nodes were
 * created.
 */
GGraphArray*
g_graph_construct(GGraphDataQuadruplet** data_quadruplets,
                  gint count)
{
  GGraphArray* separate_graphs;
  GHashTable* all_nodes;
  gint iter;
  
  g_return_val_if_fail(data_quadruplets != NULL, NULL);
  
  if (count < 0)
  {
    count = 0;
    while (data_quadruplets[count])
    {
      count++;
    }
  }
  
  if (!count)
  {
    return NULL;
  }
  
  separate_graphs = g_graph_array_new();
  all_nodes = g_hash_table_new(NULL, NULL);
  for (iter = 0; iter < count; iter++)
  {
    GGraphDataQuadruplet* data_quadruplet = data_quadruplets[iter];
    
    if (_data_quadruplet_is_valid(data_quadruplet))
    {
      GGraph* first_node;
      GGraph* second_node;
      GGraphConstructFlags created = G_GRAPH_NONE;
      gboolean one_way = FALSE;
      
      if (!g_hash_table_lookup_extended(all_nodes, data_quadruplet->first, NULL,
                                        (gpointer*)&first_node))
      {
        first_node = g_graph_new(data_quadruplet->first);
        g_hash_table_insert(all_nodes, data_quadruplet->first, first_node);
        created |= G_GRAPH_FIRST;
      }
      
      if (!g_hash_table_lookup_extended(all_nodes, data_quadruplet->second, NULL,
                                        (gpointer*)&second_node))
      {
        second_node = g_graph_new(data_quadruplet->second);
        g_hash_table_insert(all_nodes, data_quadruplet->second, second_node);
        created |= G_GRAPH_SECOND;
      }
      
      if (first_node != second_node)
      {
        switch (data_quadruplet->connected)
        {
          case G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND:
          {
            one_way = TRUE;
            break;
          }
          case G_GRAPH_EDGE_CONNECTED_SECOND_TO_FIRST:
          {
            GGraph* temp = first_node;
            first_node = second_node;
            second_node = temp;
            one_way = TRUE;
            break;
          }
          case G_GRAPH_EDGE_CONNECTED_BOTH:
          {
            break;
          }
          /* default won't happen - _data_quadruplet_is_valid makes it sure. */
        }
      }
      _g_graph_connection_new(first_node, second_node, data_quadruplet->data,
                              one_way);
      switch (created)
      {
        case G_GRAPH_NONE:
        {
          /* no nodes were created, so they can join two separate graphs. */
          guint iter2;
          guint iter3;
          gboolean hit = FALSE;
          
          /* if only self-connection was created, then no graph joins occur. */
          if (first_node == second_node)
          {
            break;
          }
          for (iter2 = 0; iter2 < (separate_graphs->len - 1); iter2++)
          {
            for (iter3 = iter2 + 1; iter3 < separate_graphs->len; iter3++)
            {
              /* lets reuse the variables, they won't be needed. */
              first_node = g_graph_array_index(separate_graphs, iter2);
              second_node = g_graph_array_index(separate_graphs, iter3);
              if (!_libggraph_internal_are_separate(first_node, second_node))
              {
                g_graph_array_remove_index_fast(separate_graphs, iter3);
                hit = TRUE;
                break;
              }
            }
            if (hit)
            {
              break;
            }
          }
          break;
        }
        case G_GRAPH_FIRST:
        case G_GRAPH_SECOND:
        { 
          /* creating self-connected node means it creates separate graph. */
          if (first_node == second_node)
          {
            g_graph_array_add(separate_graphs, first_node);
          }
          break;
        }
        case G_GRAPH_BOTH:
        {
          /* if both nodes were created then they create separate graph. */
          g_graph_array_add(separate_graphs, first_node);
          break;
        }
      }
    }
  }
  g_hash_table_unref(all_nodes);
  if (!separate_graphs->len)
  {
    g_graph_array_free(separate_graphs, TRUE);
    separate_graphs = NULL;
  }
  return separate_graphs;
}

/* TODO: document it. */
GGraphEdge*
g_graph_connection_new(GGraph* graph,
                       GGraph* other_graph)
{
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(other_graph != NULL, FALSE);
  
  return _g_graph_connection_new(graph, other_graph, NULL, TRUE);
}

/* TODO: document it. */
GGraphEdge*
g_graph_connection_new_with_data(GGraph* graph,
                                 GGraph* other_graph,
                                 gpointer data)
{
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(other_graph != NULL, FALSE);
  
  return _g_graph_connection_new(graph, other_graph, data, TRUE);
}

/**
 * g_graph_remove:
 * @graph: a node.
 * @edge_data_array: an array storing edges data.
 *
 * Removes and frees a node. Data of this node should be freed earlier, if
 * applicable. Such removal may cause split of a graph into several smaller
 * ones, so this function returns #GGraphArray holding pointers to all
 * now-separated graphs. Data of all deleted edges are appended to
 * @edge_data_array. @edge_data_array must be created before calling this
 * function.
 *
 * Returns: Array of separate graphs, which must be freed using
 * g_graph_array_free().
 */
GGraphArray*
g_graph_remove(GGraph* graph,
               GPtrArray* edge_data_array)
{
  GGraphArray* separate_graphs;
  
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(edge_data_array != NULL, NULL);
  
  separate_graphs = _g_graph_disjoin(graph, edge_data_array);
  g_graph_free(graph);
  return separate_graphs;
}

/**
 * g_graph_disconnect:
 * @graph: a node.
 * @edge_data_array: an array storing edges data.
 *
 * Removes a node. Such removal may cause split of a graph into several smaller
 * ones, so this function returns #GGraphArray holding pointers to all
 * now-separated graphs. Data of all deleted edges are appended to
 * @edge_data_array. @edge_data_array must be created before calling this
 * function.
 *
 * Returns: Array of separate graphs, which must be freed using
 * g_graph_array_free().
 */
GGraphArray*
g_graph_disconnect(GGraph* graph,
                   GPtrArray* edge_data_array)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(edge_data_array != NULL, NULL);
  
  return _g_graph_disjoin(graph, edge_data_array);
}

/**
 * g_graph_copy:
 * @graph: a node of graph to be copied.
 *
 * Does a shallow copy of @graph. This means that original graph and its copy
 * shares the data, so when freeing both graphs you should be careful not to
 * free same data twice.
 *
 * Returns: A copy of @graph.
 */
GGraph*
g_graph_copy(GGraph* graph)
{
  return g_graph_copy_deep(graph, NULL, NULL, NULL, NULL);
}

/**
 * g_graph_copy_deep:
 * @graph: a node of graph to be copied.
 * @graph_data_copy_func: function copying data in nodes.
 * @graph_user_data: data passed to node data copying function.
 * @edge_data_copy_func: function copying data in edges.
 * @edge_user_data: data passed to edge data copying function.
 *
 * Does a deep copy of @graph. Each node data in @graph copy and each edge data
 * are duplicated using passed functions. If @graph_data_copy_func or
 * @edge_data_copy_func are NULL, then shallow copy of data is done.
 *
 * Returns: A copy of @graph.
 */
GGraph*
g_graph_copy_deep(GGraph* graph,
                  GCopyFunc graph_data_copy_func,
                  gpointer graph_user_data,
                  GCopyFunc edge_data_copy_func,
                  gpointer edge_user_data)
{
  GGraphWhole* graph_whole = _g_graph_whole(graph);
  GHashTable* nodes_to_dups = g_hash_table_new(NULL, NULL);
  GHashTable* edges_to_dups = g_hash_table_new(NULL, NULL);
  GGraph* dup_graph;
  guint iter;
  
  if (!graph)
  {
    return NULL;
  }
  
  for (iter = 0; iter < graph_whole->graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_whole->graph_array, iter);
    GGraph* dup_node = g_slice_new(GGraph);
    if (graph_data_copy_func)
    {
      dup_node->data = (*graph_data_copy_func)(node->data, graph_user_data);
    }
    else
    {
      dup_node->data = node->data;
    }
    g_hash_table_insert(nodes_to_dups, node, dup_node);
  }
  for (iter = 0; iter < graph_whole->edge_array->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(graph_whole->edge_array, iter);
    GGraphEdge* dup_edge = g_slice_new(GGraphEdge);
    dup_edge->first = g_hash_table_lookup(nodes_to_dups, edge->first);
    dup_edge->second = g_hash_table_lookup(nodes_to_dups, edge->second);
    dup_edge->connected = edge->connected;
    if (edge_data_copy_func)
    {
      dup_edge->data = (*edge_data_copy_func)(edge->data, edge_user_data);
    }
    else
    {
      dup_edge->data = edge->data;
    }
    g_hash_table_insert(edges_to_dups, edge, dup_edge);
  }
  for (iter = 0; iter < graph_whole->graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_whole->graph_array, iter);
    GGraph* dup_node = g_hash_table_lookup(nodes_to_dups, node);
    GGraphEdgeArray* dup_edges = g_graph_edge_array_sized_new(node->edges->len);
    guint iter2;
    for (iter2 = 0; iter2 < node->edges->len; iter++)
    {
      GGraphEdge* edge = g_graph_edge_array_index(node->edges, iter2);
      GGraphEdge* dup_edge = g_hash_table_lookup(edges_to_dups, edge);
      g_graph_edge_array_add(dup_edges, dup_edge);
    }
    dup_node->edges = dup_edges;
  }
  dup_graph = g_hash_table_lookup(nodes_to_dups, graph);
  g_hash_table_unref(nodes_to_dups);
  g_hash_table_unref(edges_to_dups);
  _g_graph_whole_free(graph_whole, TRUE);
  return dup_graph;
}

/**
 * g_graph_free:
 * @graph: a graph.
 *
 * Frees whole graph to which @graph belongs. Each node's data should be freed
 * earlier, if applicable.
 */
void
g_graph_free(GGraph* graph)
{
  GGraphWhole* whole_graph;
  guint iter;
  
  if (!graph)
  {
    return;
  }
  
  whole_graph = _g_graph_whole(graph);
  for (iter = 0; iter < whole_graph->graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(whole_graph->graph_array, iter);
    g_graph_edge_array_free(node->edges, TRUE);
    g_slice_free(GGraph, node);
  }
  for (iter = 0; iter < whole_graph->edge_array->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(whole_graph->edge_array, iter);
    g_graph_edge_free(edge);
  }
  _g_graph_whole_free(whole_graph, TRUE);
}

/**
 * g_graph_count_nodes:
 * @graph: a graph.
 *
 * Counts number of nodes in graph.
 *
 * Returns: number of nodes in graph.
 */
guint
g_graph_count_nodes(GGraph* graph)
{
  GGraphArray* graph_array;
  guint count;
  
  g_return_val_if_fail(graph != NULL, 0);
  
  graph_array = _g_graph_array(graph, FALSE);
  count = graph_array->len;
  g_graph_array_free(graph_array, TRUE);
  return count;
}

/**
 * g_graph_count_edges:
 * @graph: a graph.
 *
 * Counts number of edges in graph.
 *
 * Returns: number of edges in graph.
 */
guint
g_graph_count_edges(GGraph* graph)
{
  GGraphEdgeArray* edge_array;
  guint count;
  
  g_return_val_if_fail(graph != NULL, 0);
  
  edge_array = _g_graph_edge_array(graph, FALSE);
  count = edge_array->len;
  g_graph_edge_array_free(edge_array, TRUE);
  return count;
}

/**
 * g_graph_count_connected_nodes:
 * @graph: a graph.
 *
 * Counts number of nodes to which @graph is directly or indirectly connected
 * to in graph.
 *
 * Returns: number of such nodes in graph.
 */
guint
g_graph_count_connected_nodes(GGraph* graph)
{
  GGraphArray* graph_array;
  guint count;
  
  g_return_val_if_fail(graph != NULL, 0);
  
  graph_array = _g_graph_array(graph, TRUE);
  count = graph_array->len;
  g_graph_array_free(graph_array, TRUE);
  return count;
}

/**
 * g_graph_count_connected_edges:
 * @graph: a graph.
 *
 * Counts number of edges to which @graph is directly or indirectly connected
 * to in graph.
 *
 * Returns: number of such edges in graph.
 */
guint
g_graph_count_connected_edges(GGraph* graph)
{
  GGraphEdgeArray* edge_array;
  guint count;
  
  g_return_val_if_fail(graph != NULL, 0);
  
  edge_array = _g_graph_edge_array(graph, TRUE);
  count = edge_array->len;
  g_graph_edge_array_free(edge_array, TRUE);
  return count;
}

/**
 * g_graph_foreach:
 * @graph: a node.
 * @func: the function to call with each element's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node's data in graph @graph belongs to.
 */
void
g_graph_foreach(GGraph* graph,
                GFunc func,
                gpointer user_data)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_foreach_general(graph, func, user_data, FALSE);
}

/**
 * g_graph_foreach_connected:
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
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_foreach_general(graph, func, user_data, TRUE);
}

/**
 * g_graph_foreach_node:
 * @graph: a node.
 * @func: the function to call with each node.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node in graph @graph belongs to.
 */
void
g_graph_foreach_node(GGraph* graph,
                     GGraphFunc func,
                     gpointer user_data)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_foreach_node_general(graph, func, user_data, FALSE);
}

/**
 * g_graph_foreach_connected_node:
 * @graph: a node.
 * @func: the function to call with node.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node @graph is directly or indirectly connected to.
 */
void
g_graph_foreach_connected_node(GGraph* graph,
                               GGraphFunc func,
                               gpointer user_data)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_foreach_node_general(graph, func, user_data, TRUE);
}

/**
 * g_graph_edge_foreach:
 * @graph: a node.
 * @func: the function to call with each edge's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each edge's data in graph @graph belongs to.
 */
void
g_graph_edge_foreach(GGraph* graph,
                     GFunc func,
                     gpointer user_data)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_edge_foreach_general(graph, func, user_data, FALSE);
}

/**
 * g_graph_edge_foreach_connected:
 * @graph: a node.
 * @func: the function to call with edge's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each edge @graph is directly or indirectly connected to.
 */
void
g_graph_edge_foreach_connected(GGraph* graph,
                               GFunc func,
                               gpointer user_data)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_edge_foreach_general(graph, func, user_data, TRUE);
}

/**
 * g_graph_edge_foreach_edge:
 * @graph: a node.
 * @func: the function to call with each edge.
 * @user_data: data passed to @func.
 *
 * Calls @func for each edge in graph @graph belongs to.
 */
void
g_graph_edge_foreach_edge(GGraph* graph,
                          GGraphEdgeFunc func,
                          gpointer user_data)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_edge_foreach_edge_general(graph, func, user_data, FALSE);
}

/**
 * g_graph_edge_foreach_connected_edge:
 * @graph: a node.
 * @func: the function to call with edge.
 * @user_data: data passed to @func.
 *
 * Calls @func for each edge @graph is directly or indirectly connected to.
 */
void
g_graph_edge_foreach_connected_edge(GGraph* graph,
                                    GGraphEdgeFunc func,
                                    gpointer user_data)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(func != NULL);
  
  _g_graph_edge_foreach_edge_general(graph, func, user_data, TRUE);
}

/**
 * g_graph_get_all_neighbours:
 * @graph: a node.
 *
 * Gets an array of neighbours, to which @graph is not connected.
 *
 * Returns: #GGraphArray which must be freed with g_graph_array_free() or %NULL
 * if there was no such neighbours.
 */
GGraphArray*
g_graph_get_all_neighbours(GGraph* graph)
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
    g_graph_array_add(neighbours_array, g_graph_edge_get_graph(edge, graph));
  }
  return neighbours_array;
}

/**
 * g_graph_get_connected_neighbours:
 * @graph: a node.
 *
 * Gets an array of neighbours, to which @graph is connected.
 *
 * Returns: #GGraphArray which must be freed with g_graph_array_free() or %NULL
 * if there was no such neighbours.
 */
GGraphArray*
g_graph_get_connected_neighbours(GGraph* graph)
{
  g_return_val_if_fail(graph != NULL, NULL);
  
  return _g_graph_get_neighbours_general(graph, TRUE);
}

/**
 * g_graph_get_not_connected_neighbours:
 * @graph: a node.
 *
 * Gets an array of neighbours, to which @graph is not connected.
 *
 * Returns: #GGraphArray which must be freed with g_graph_array_free() or %NULL
 * if there was no such neighbours.
 */
GGraphArray*
g_graph_get_not_connected_neighbours(GGraph* graph)
{
  g_return_val_if_fail(graph != NULL, NULL);
  
  return _g_graph_get_neighbours_general(graph, FALSE);
}

/**
 * g_graph_get_edges:
 * @graph: a node.
 * @other_graph: other node which is @graph's neighbour.
 *
 * Gets edges from @graph to @other_graph.
 *
 * Returns: #GGraphEdgeArray if there was at least one such edge, %NULL
 * otherwise.
 */
GGraphEdgeArray*
g_graph_get_edges(GGraph* graph,
                  GGraph* other_graph)
{
  guint iter;
  GGraphEdgeArray* g_e;
  GGraphEdgeArray* edges;
  
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(other_graph != NULL, NULL);
  
  g_e = graph->edges;
  edges = g_graph_edge_array_new();
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(g_e, iter);
    if (g_graph_edge_get_graph(edge, graph) == other_graph)
    {
      g_graph_edge_array_add(edges, edge);
    }
  }
  if (!edges->len)
  {
    g_graph_edge_array_free(edges, TRUE);
    edges = NULL;
  }
  return edges;
}

/* TODO: fix docs. */
/**
 * g_graph_remove_connection:
 * @graph: a node.
 * @other_graph: other node which is @graph's neighbour.
 * @edge_data: a place to store connection's data.
 *
 * Removes connection from @graph to @other_graph, so in effect they might stop
 * being neighbours if @other_graph wasn't connected to @graph. If such
 * situation happens, connection is deleted and its data is stored in
 * @edge_data. If such breakage causes to create two separate graphs, notify
 * about it by returning %TRUE. If @graph and @other_graph aren't neighbours
 * or @graph is not connected to @other_graph then this function just checks if
 * @graph and @other_graph are in separate graphs.
 *
 * Returns: %TRUE if @graph and @other_graph are now in separate graphs,
 * otherwise %FALSE.
 */
gboolean
g_graph_remove_connection(GGraph* graph,
                          GGraphEdge* edge,
                          gpointer* edge_data)
{
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(edge != NULL, FALSE);
  g_return_val_if_fail(edge_data != NULL, FALSE);
  
  if (g_graph_edge_is_graph_connected(edge, graph))
  {
    GGraph* other_graph = g_graph_edge_get_graph(edge, graph);
    
    if (g_graph_edge_is_graph_connected(edge, other_graph))
    {
      if (g_graph_edge_is_first(edge, graph))
      {
        edge->connected &= ~G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND;
      }
      else
      {
        edge->connected &= ~G_GRAPH_EDGE_CONNECTED_SECOND_TO_FIRST;
      }
      return FALSE;
    }
    else
    {
      g_graph_edge_array_remove(graph->edges, edge);
      g_graph_edge_array_remove(other_graph->edges, edge);
      *edge_data = edge->data;
      g_graph_edge_free(edge);
    }
  }
  return _libggraph_internal_are_separate(graph, other_graph);
}

/* TODO: document it. */
gboolean
g_graph_remove_all_connections(GGraph* graph,
                               GGraph* other_graph,
                               GPtrArray* edge_data_array)
{
  GGraphEdgeArray* edges;
  
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(other_graph != NULL, FALSE);
  g_return_val_if_fail(edge_data_array != NULL, FALSE);
  
  if (graph == other_graph)
  {
    return g_graph_break_all_connections(graph, other_graph, edge_data_array);
  }
  edges = g_graph_get_edges(graph, other_graph);
  if (edges)
  {
    guint iter;
    
    if (graph == other_graph)
    {
      for (iter = 0; iter < edges->len; iter++)
      {
        GGraphEdge* edge = g_graph_edge_array_index(edges, iter);
        g_graph_edge_array_remove(graph->edges, edge);
        g_ptr_array_index(edge_data_array, iter) = edge->data;
        g_graph_edge_free(edge);
      }
    }
    
    for (iter = 0; iter < edges->len; iter++)
    {
      GGraphEdge* edge = g_graph_edge_array_index(edges, iter);
      
      if (graph == other_graph)
      if (g_graph_edge_is_connected(edge, other_graph))
      {
        if (g_graph_edge_is_first(edge, graph))
        {
          edge->connected &= ~G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND;
        }
        else
        {
          edge->connected &= ~G_GRAPH_EDGE_CONNECTED_SECOND_TO_FIRST;
        }
      }
      g_graph_edge_array_remove(graph->edges, edge);
      g_graph_edge_array_remove(other_graph->edges, edge);
      g_ptr_array_add(edge_data_array, edge->data);
      g_graph_edge_free(edge);
    }
  }
  return _libggraph_internal_are_separate(graph, other_graph);
}

/* TODO: document it. */
gboolean
g_graph_break_all_connections(GGraph* graph,
                              GGraph* other_graph,
                              GPtrArray* edge_data_array)
{
  GGraphEdgeArray* edges;
  
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(other_graph != NULL, FALSE);
  g_return_val_if_fail(edge_data_array != NULL, FALSE);
  
  edges = g_graph_get_edges(graph, other_graph);
  if (edges)
  {
    guint iter;
    
    g_ptr_array_set_size(edge_data_array, edge_data_array->len + edges->len);
    for (iter = 0; iter < edges->len; iter++)
    {
      GGraphEdge* edge = g_graph_edge_array_index(edges, iter);
      g_graph_edge_array_remove(graph->edges, edge);
      if (graph != other_graph)
      {
        g_graph_edge_array_remove(other_graph->edges, edge);
      }
      g_ptr_array_index(edge_data_array, iter) = edge->data;
      g_graph_edge_free(edge);
    }
  }
  if (graph == other_graph)
  {
    return FALSE;
  }
  return _libggraph_internal_are_separate(graph, other_graph);
}

/* TODO: change it to g_graph_edge_remove() and g_graph_edge_disconnect(). */
/**
 * g_graph_break_connection:
 * @graph: a node.
 * @other_graph: other node which is @graph's neighbour.
 * @edge_data: a place to store connection's data.
 *
 * Breaks both connections from one node to another, so in effect they stop
 * being neighbours. Removed connection's data is stored in @edge_data.
 * If such breakage causes to create two separate graphs, notify about it by
 * returning %TRUE. If @graph and @other_graph aren't neighbours then this
 * function just checks if @graph and @other_graph are in separate graphs.
 *
 * Returns: %TRUE if @graph and @other_graph are now in separate graphs,
 * otherwise %FALSE.
 */
/*
gboolean
g_graph_break_connection(GGraph* graph,
                         GGraphEdge* edge,
                         gpointer* edge_data)
{
  g_return_val_if_fail(graph != NULL, FALSE);
  g_return_val_if_fail(edge != NULL, FALSE);
  g_return_val_if_fail(edge_data != NULL, FALSE);
  
  if (g_graph_edge_is_first(edge, graph) || g_graph_edge_is_second(edge, graph))
  {
    g_graph_edge_array_remove(graph->edges, edge);
    g_graph_edge_array_remove(other_graph->edges, edge);
    *edge_data = edge->data;
    g_graph_edge_free(edge);
  }
  return _libggraph_internal_are_separate(graph, other_graph);
}
*/

/* TODO: fix docs. */
/**
 * g_graph_find:
 * @graph: a node.
 * @data: the element data to find.
 *
 * It calls g_graph_find_custom() with g_direct_equal() as a third parameter.
 *
 * Returns: a #GGraphArray that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find(GGraph* graph,
             gconstpointer data)
{
  g_return_val_if_fail(graph != NULL, NULL);
  
  return _g_graph_find_custom_general(graph, data, g_direct_equal, FALSE);
}

/* TODO: fix docs. */
/**
 * g_graph_find_connected:
 * @graph: a node.
 * @data: the element data to find.
 *
 * It calls g_graph_find_custom_connected() with g_direct_equal() as a third
 * parameter.
 *
 * Returns: a #GGraphArray that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_connected(GGraph* graph,
                       gconstpointer data)
{
  g_return_val_if_fail(graph != NULL, NULL);
  
  return _g_graph_find_custom_general(graph, data, g_direct_equal, TRUE);
}

/**
 * g_graph_find_custom:
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
 * Returns: a #GGraphArray that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_custom(GGraph* graph,
                    gconstpointer data,
                    GEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_find_custom_general(graph, data, func, FALSE);
}

/**
 * g_graph_find_custom_connected:
 * @graph: a node.
 * @data: data passed to @func.
 * @func: the function to call for each element.
 *
 * Finds elements in a #GGraph, using a supplied function to find the desired
 * elements. It iterates over these nodes in graph, to which @graph is directly
 * or indirectly connected, calling the given function which should return %TRUE
 * when the desired element is found. The function takes two gconstpointer
 * arguments, the #GGraph element's data as the first argument and the given
 * user data as second. If an element is found it is added to a #GGraphArray.
 *
 * Returns: a #GGraphArray that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_custom_connected(GGraph* graph,
                              gconstpointer data,
                              GEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_find_custom_general(graph, data, func, TRUE);
}

/**
 * g_graph_find_custom_node:
 * @graph: a node.
 * @node: node passed to @func.
 * @func: the function to call for each node.
 *
 * Finds elements in a @graph, using a supplied function to find the desired
 * elements. It iterates over the whole graph, calling the given function which
 * should return %TRUE when the desired element is found. The function takes two
 * #GGraph arguments, the node from the iterated graph as the first argument and
 * the given @node as second. If an element is found it is added to a
 * #GGraphArray.
 *
 * Returns: a #GGraphArray that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_custom_node(GGraph* graph,
                         GGraph* node,
                         GGraphEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_find_custom_node_general(graph, node, func, FALSE);
}

/**
 * g_graph_find_custom_connected_node:
 * @graph: a node.
 * @node: node passed to @func.
 * @func: the function to call for each node.
 *
 * Finds elements in a @graph, using a supplied function to find the desired
 * elements. It iterates over these nodes in graph, to which @graph is directly
 * or indirectly connected, calling the given function which should return %TRUE
 * when the desired element is found. The function takes two #GGraph arguments,
 * the node from the iterated graph as the first argument and the given
 * @node as second. If an element is found it is added to a #GGraphArray.
 *
 * Returns: a #GGraphArray that must be freed with g_graph_array_free().
 */
GGraphArray*
g_graph_find_custom_connected_node(GGraph* graph,
                                   GGraph* node,
                                   GGraphEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_find_custom_node_general(graph, node, func, TRUE);
}

/**
 * g_graph_edge_find:
 * @graph: a node.
 * @data: the element data to find.
 *
 * It calls g_graph_edge_find_custom() with g_direct_equal() as a third
 * parameter.
 *
 * Returns: a #GGraphEdgeArray that must be freed with
 * g_graph_edge_array_free().
 */
GGraphEdgeArray*
g_graph_edge_find(GGraph* graph,
                  gconstpointer data)
{
  return g_graph_edge_find_custom(graph, data, g_direct_equal);
}

/**
 * g_graph_edge_find_connected:
 * @graph: a node.
 * @data: the element data to find.
 *
 * It calls g_graph_edge_find_custom_connected() with g_direct_equal() as a
 * third parameter.
 *
 * Returns: a #GGraphEdgeArray that must be freed with
 * g_graph_edge_array_free().
 */
GGraphEdgeArray*
g_graph_edge_find_connected(GGraph* graph,
                            gconstpointer data)
{
  return g_graph_edge_find_custom_connected(graph, data, g_direct_equal);
}

/**
 * g_graph_edge_find_custom:
 * @graph: a node.
 * @data: data passed to @func.
 * @func: the function to call for each element.
 *
 * Finds elements in a #GGraph, using a supplied function to find the desired
 * elements. It iterates over the whole graph, calling the given function which
 * should return %TRUE when the desired element is found. The function takes two
 * gconstpointer arguments, the #GGraphEdge element's data as the first argument
 * and the given user data as second. If an element is found it is added to a
 * #GGraphEdgeArray.
 *
 * Returns: a #GGraphEdgeArray that must be freed with
 * g_graph_edge_array_free().
 */
GGraphEdgeArray*
g_graph_edge_find_custom(GGraph* graph,
                         gconstpointer data,
                         GEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_edge_find_custom_general(graph, data, func, FALSE);
}

/**
 * g_graph_edge_find_custom_connected:
 * @graph: a node.
 * @data: data passed to @func.
 * @func: the function to call for each element.
 *
 * Finds elements in a #GGraph, using a supplied function to find the desired
 * elements. It iterates over these edges in graph, to which @graph is directly
 * or indirectly connected, calling the given function which should return %TRUE
 * when the desired element is found. The function takes two gconstpointer
 * arguments, the #GGraphEdge element's data as the first argument and the given
 * user data as second. If an element is found it is added to a
 * #GGraphEdgeArray.
 *
 * Returns: a #GGraphEdgeArray that must be freed with
 * g_graph_edge_array_free().
 */
GGraphEdgeArray*
g_graph_edge_find_custom_connected(GGraph* graph,
                                   gconstpointer data,
                                   GEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_edge_find_custom_general(graph, data, func, TRUE);
}

/**
 * g_graph_edge_find_custom_edge:
 * @graph: a node.
 * @edge: edge passed to @func.
 * @func: the function to call for each edge.
 *
 * Finds edges in a @graph, using a supplied function to find the desired
 * elements. It iterates over the whole graph, calling the given function which
 * should return %TRUE when the desired element is found. The function takes two
 * #GGraphEdge arguments, the edge from the iterated graph as the first argument
 * and the given @edge as second. If an element is found it is added to a
 * #GGraphEdgeArray.
 *
 * Returns: a #GGraphArrayEdge that must be freed with
 * g_graph_edge_array_free().
 */
GGraphEdgeArray*
g_graph_edge_find_custom_edge(GGraph* graph,
                              GGraphEdge* edge,
                              GGraphEdgeEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_edge_find_custom_edge_general(graph, edge, func, FALSE);
}

/**
 * g_graph_edge_find_custom_connected_edge:
 * @graph: a node.
 * @edge: edge passed to @func.
 * @func: the function to call for each node.
 *
 * Finds edge in a @graph, using a supplied function to find the desired
 * elements. It iterates over these edges in graph, to which @graph is directly
 * or indirectly connected, calling the given function which should return %TRUE
 * when the desired element is found. The function takes two #GGraphEdge
 * arguments, the node from the iterated graph as the first argument and the
 * given @edge as second. If an element is found it is added to a
 * #GGraphEdgeArray.
 *
 * Returns: a #GGraphEdgeArray that must be freed with
 * g_graph_edge_array_free().
 */
GGraphEdgeArray*
g_graph_edge_find_custom_connected_edge(GGraph* graph,
                                        GGraphEdge* edge,
                                        GGraphEdgeEqualFunc func)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  return _g_graph_edge_find_custom_edge_general(graph, edge, func, TRUE);
}

/* static function definitions */

/*
 * _g_graph_array:
 * @graph: graph, which will be put into an array.
 * @connected_only: %TRUE if only connected nodes should be put.
 *
 * Puts graph into an array, so every node can be accessed without hassle. If
 * @connected_only is %FALSE, then all nodes in graph are put into array.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
static GGraphArray*
_g_graph_array(GGraph* graph,
               gboolean connected_only)
{
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  GHashTable* visited_edges = g_hash_table_new(NULL, NULL);
  GGraphArray* graph_array = g_graph_array_new();
  _g_graph_recurrent_whole_append(graph, graph_array, visited_nodes, NULL,
                                  visited_edges, connected_only);
  g_hash_table_unref(visited_nodes);
  g_hash_table_unref(visited_edges);
  return graph_array;
}

/*
 * _g_graph_get_neighbours_general:
 * @graph: node which neighbours are to be returned.
 * @connected: set it to %TRUE, if function have to return nodes @graph is
 * connected to.
 *
 * Gets @graph's neighbours. @connected decides, which ones.
 *
 * Returns: a #GGraphArray* that must be freed with g_graph_array_free().
 */
static GGraphArray*
_g_graph_get_neighbours_general(GGraph* graph,
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
    if (g_graph_edge_is_graph_connected(edge, graph) == connected)
    {
      g_graph_array_add(neighbours_array, g_graph_edge_get_graph(edge, graph));
    }
  }
  if (!neighbours_array->len)
  {
    g_graph_array_free(neighbours_array, TRUE);
    neighbours_array = NULL;
  }
  return neighbours_array;
}

/* TODO: fix docs. */
/*
 * _g_graph_connection_new:
 * @node: first node.
 * @other_node: second node.
 * @data: data of new connection.
 * @force_if_null: whether to force @data, even if it is %NULL.
 * @one_way: whether connection has to be one way.
 *
 * Creates new or modifies already existing connection between @node and
 * @other_node. If it creates new connection then it will contain whatever @data
 * is passed and if @one_way is %TRUE, then it will be marked as connecting
 * @node to @other_node, but not other way. Two way connections are made in two
 * cases: when @one_way is %FALSE or when @node == @other_node. After creation
 * this connection will be added to @node and @other_node. If connection between
 * @node and @other_node already existed then it will be marked as connecting
 * @node to @other_node also (so if @other_node was connected to @node then this
 * connection become two way) or two way, depends on @one_way value. Data of
 * this connection will be overwritten with new when @data is not %NULL or if
 * @force_if_null is %TRUE.
 *
 * Returns: an edge between @node and @other_node.
 */
static GGraphEdge*
_g_graph_connection_new(GGraph* node,
                        GGraph* other_node,
                        gpointer data,
                        gboolean one_way)
{
  GGraphEdge* edge;
  GGraphEdgeConnected setting;
  
  /* if creating connection to self, make it two way. */
  if (one_way && (node != other_node))
  {
    setting = G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND;
  }
  else
  {
    setting = G_GRAPH_EDGE_CONNECTED_BOTH;
  }
  edge = g_graph_edge_new(node, other_node, setting, data);
  g_graph_edge_array_add(node->edges, edge);
  if (node != other_node)
  {
    g_graph_edge_array_add(other_node->edges, edge);
  }
  return edge;
}

/*
 * _g_graph_whole_new:
 * @graph_array: array of nodes.
 * @edge_array: array of edges.
 *
 * Creates new #GGraphWhole with given arrays as its members.
 *
 * Returns: new #GGraphWhole.
 */
static GGraphWhole*
_g_graph_whole_new(GGraphArray* graph_array,
                   GGraphEdgeArray* edge_array)
{
  GGraphWhole* graph_whole = g_slice_new(GGraphWhole);
  graph_whole->graph_array = graph_array;
  graph_whole->edge_array = edge_array;
  return graph_whole;
}

/*
 * _g_graph_whole_free:
 * @graph_whole: #GGraphWhole to free.
 * @free_arrays: %TRUE if @graph_whole's members are to be freed.
 *
 * Frees memory allocated to @graph_whole. If @free_arrays is %TRUE, then also
 * both arrays are freed with their members.
 */
static void
_g_graph_whole_free(GGraphWhole* graph_whole,
                    gboolean free_arrays)
{
  if (!graph_whole)
  {
    return;
  }
  
  if (free_arrays)
  {
    g_graph_array_free(graph_whole->graph_array, TRUE);
    g_graph_edge_array_free(graph_whole->edge_array, TRUE);
  }
  g_slice_free(GGraphWhole, graph_whole);
}

/*
 * _g_graph_whole:
 * @graph: a graph.
 *
 * Puts all @graph's nodes and edges into arrays, packs them to #GGraphWhole and
 * return it.
 *
 * Returns: a #GGraphWhole, which should be freed with _g_graph_whole_free().
 */
static GGraphWhole*
_g_graph_whole(GGraph* graph)
{
  GGraphWhole* graph_whole;
  GGraphArray* graph_array;
  GHashTable* visited_nodes;
  GGraphEdgeArray* edge_array;
  GHashTable* visited_edges;
  
  graph_array = g_graph_array_new();
  visited_nodes = g_hash_table_new(NULL, NULL);
  edge_array = g_graph_edge_array_new();
  visited_edges = g_hash_table_new(NULL, NULL);
  _g_graph_recurrent_whole_append(graph, graph_array, visited_nodes, edge_array, visited_edges, FALSE);
  g_hash_table_unref(visited_nodes);
  g_hash_table_unref(visited_edges);
  graph_whole = _g_graph_whole_new(graph_array, edge_array);
  return graph_whole;
}

/*
 * _g_graph_recurrent_whole_append:
 * @graph: a node, which will be put into array with his edges.
 * @graph_array: array of all nodes in graph.
 * @visited_nodes: a map of already visited nodes.
 * @edge_array: array of all edges in graph.
 * @visited_edges: a map of already visited edges.
 * @connected_only: %TRUE if only connected nodes/edges should be put into
 * arrays.
 *
 * Recursivly checks all nodes and edges and put them into arrays.
 */
static void
_g_graph_recurrent_whole_append(GGraph* graph,
                                GGraphArray* graph_array,
                                GHashTable* visited_nodes,
                                GGraphEdgeArray* edge_array,
                                GHashTable* visited_edges,
                                gboolean connected_only)
{
  guint iter;
  GGraphEdgeArray* g_e;
  
  if (g_hash_table_lookup_extended(visited_nodes, graph, NULL, NULL))
  {
    return;
  }
  
  g_hash_table_insert(visited_nodes, graph, NULL);
  if (graph_array)
  {
    g_graph_array_add(graph_array, graph);
  }
  g_e = graph->edges;
  for (iter = 0; iter < g_e->len; iter++)
  {
    GGraph* other_graph;
    GGraphEdge* edge;
    
    edge = g_graph_edge_array_index(g_e, iter);
    if (g_hash_table_lookup_extended(visited_edges, edge, NULL, NULL))
    {
      continue;
    }
    if (connected_only && !g_graph_edge_is_graph_connected(edge, graph))
    {
      continue;
    }
    g_hash_table_insert(visited_edges, edge, NULL);
    if (edge_array)
    {
      g_graph_edge_array_add(edge_array, edge);
    }
    other_graph = g_graph_edge_get_graph(edge, graph);
    _g_graph_recurrent_whole_append(graph, graph_array, visited_nodes, edge_array, visited_edges, connected_only);
  }
}

/*
 * _data_quadruplet_is_valid:
 * @data_quadruplet: a #GGraphDataQuadruplet to check
 *
 * Checks if first and second gpointer members of @data_quadruplet are not %NULL
 * and if connection option has a proper value. Data gpointer member is allowed
 * to be %NULL, so it isn't checked.
 *
 * Returns: %TRUE if data quadruplet is valid, otherwise %FALSE.
 */
static gboolean
_data_quadruplet_is_valid(GGraphDataQuadruplet* data_quadruplet)
{
  return ((data_quadruplet->first != NULL) && (data_quadruplet->second != NULL) && ((data_quadruplet->connected & ~G_GRAPH_EDGE_CONNECTED_BOTH) == 0) && data_quadruplet->connected != 0);
}

/*
 * _g_graph_edge_array:
 * @graph: graph, which edges will be put into an array.
 * @connected_only: %TRUE if only connected edges should be put.
 *
 * Puts graph's edges into an array, so every edge can be accessed without
 * hassle. If @connected_only is %FALSE, then all edges in graph are put into
 * array.
 *
 * Returns: a #GGraphEdgeArray* that must be freed with
 * g_graph_edge_array_free().
 */
static GGraphEdgeArray*
_g_graph_edge_array(GGraph* graph,
                    gboolean connected_only)
{
  GHashTable* visited_nodes;
  GGraphEdgeArray* edge_array;
  GHashTable* visited_edges;
  
  visited_nodes = g_hash_table_new(NULL, NULL);
  edge_array = g_graph_edge_array_new();
  visited_edges = g_hash_table_new(NULL, NULL);
  _g_graph_recurrent_whole_append(graph, NULL, visited_nodes, edge_array, visited_edges, connected_only);
  g_hash_table_unref(visited_nodes);
  g_hash_table_unref(visited_edges);
  return edge_array;
}

/* TODO: static - document it. */
void
_g_graph_foreach_general(GGraph* graph,
                         GFunc func,
                         gpointer user_data,
                         gboolean connected_only)
{
  GGraphArray* graph_array;
  guint iter;
  
  graph_array = _g_graph_array(graph, connected_only);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_array, iter);
    (*func)(node->data, user_data);
  }
  g_graph_array_free(graph_array, TRUE);
}

/* TODO: static - document it. */
void
_g_graph_foreach_node_general(GGraph* graph,
                              GGraphFunc func,
                              gpointer user_data,
                              gboolean connected_only)
{
  GGraphArray* graph_array;
  guint iter;
  
  graph_array = _g_graph_array(graph, connected_only);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* node = g_graph_array_index(graph_array, iter);
    (*func)(node, user_data);
  }
  g_graph_array_free(graph_array, TRUE);
}

/* TODO: static - document it. */
void
_g_graph_edge_foreach_general(GGraph* graph,
                              GFunc func,
                              gpointer user_data,
                              gboolean connected_only)
{
  GGraphEdgeArray* edge_array;
  guint iter;
  
  edge_array = _g_graph_edge_array(graph, connected_only);
  for (iter = 0; iter < edge_array->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(edge_array, iter);
    (*func)(edge->data, user_data);
  }
  g_graph_edge_array_free(edge_array, TRUE);
}

/* TODO: static - document it. */
void
_g_graph_edge_foreach_edge_general(GGraph* graph,
                                   GGraphEdgeFunc func,
                                   gpointer user_data,
                                   gboolean connected_only)
{
  GGraphEdgeArray* edge_array;
  guint iter;
  
  edge_array = _g_graph_edge_array(graph, connected_only);
  for (iter = 0; iter < edge_array->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(edge_array, iter);
    (*func)(edge, user_data);
  }
  g_graph_edge_array_free(edge_array, TRUE);
}

/* TODO: static - document it. */
GGraphArray*
_g_graph_find_custom_general(GGraph* graph,
                             gconstpointer data,
                             GEqualFunc func,
                             gboolean connected_only)
{
  GGraphArray* matching_graphs;
  GGraphArray* graph_array;
  guint iter;
  
  matching_graphs = g_graph_array_new();
  graph_array = _g_graph_array(graph, connected_only);
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

/* TODO: static - document it. */
GGraphArray*
_g_graph_find_custom_node_general(GGraph* graph,
                                  GGraph* node,
                                  GGraphEqualFunc func,
                                  gboolean connected_only)
{
  GGraphArray* matching_graphs;
  GGraphArray* graph_array;
  guint iter;
  
  matching_graphs = g_graph_array_new();
  graph_array = _g_graph_array(graph, connected_only);
  for (iter = 0; iter < graph_array->len; iter++)
  {
    GGraph* other_node = g_graph_array_index(graph_array, iter);
    if ((*func)(other_node, node))
    {
      g_graph_array_add(matching_graphs, other_node);
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

/* TODO: static - document it. */
GGraphEdgeArray*
_g_graph_edge_find_custom_general(GGraph* graph,
                                  gconstpointer data,
                                  GEqualFunc func,
                                  gboolean connected_only)
{
  GGraphEdgeArray* matching_edges;
  GGraphEdgeArray* edge_array;
  guint iter;
  
  matching_edges = g_graph_edge_array_new();
  edge_array = _g_graph_edge_array(graph, connected_only);
  for (iter = 0; iter < edge_array->len; iter++)
  {
    GGraphEdge* edge = g_graph_edge_array_index(edge_array, iter);
    if ((*func)(edge->data, data))
    {
      g_graph_edge_array_add(matching_edges, edge);
    }
  }
  g_graph_edge_array_free(edge_array, TRUE);
  if (!matching_edges->len)
  {
    g_graph_edge_array_free(matching_edges, TRUE);
    matching_edges = NULL;
  }
  return matching_edges;
}

/* TODO: static - document it. */
GGraphEdgeArray*
_g_graph_edge_find_custom_edge_general(GGraph* graph,
                                       GGraphEdge* edge,
                                       GGraphEdgeEqualFunc func,
                                       gboolean connected_only)
{
  GGraphEdgeArray* matching_edges;
  GGraphEdgeArray* edge_array;
  guint iter;
  
  matching_edges = g_graph_edge_array_new();
  edge_array = _g_graph_edge_array(graph, connected_only);
  for (iter = 0; iter < edge_array->len; iter++)
  {
    GGraphEdge* other_edge = g_graph_edge_array_index(edge_array, iter);
    if ((*func)(other_edge, edge))
    {
      g_graph_edge_array_add(matching_edges, other_edge);
    }
  }
  g_graph_edge_array_free(edge_array, TRUE);
  if (!matching_edges->len)
  {
    g_graph_edge_array_free(matching_edges, TRUE);
    matching_edges = NULL;
  }
  return matching_edges;
}

/* TODO: static - document it. */
static GGraphArray*
_g_graph_disjoin(GGraph* graph,
                 GPtrArray* edge_data_array)
{
  GGraphArray* g_n;
  GGraphArray* separate_graphs;
  guint iter;
  
  separate_graphs = g_graph_array_new();
  /* TODO: do not use own public functions in own static functions. */
  g_n = g_graph_get_all_neighbours(graph);
  for (iter = 0; iter < g_n->len; iter++)
  {
    guint iter2;
    guint iter3 = edge_data_array->len;
    GGraph* other_graph = g_graph_array_index(g_n, iter);
    GGraphEdgeArray* edges = g_graph_get_edges(graph, other_graph);
    GPtrArray* temp_edge_data_array = g_ptr_array_new();
    
    /* TODO: do not use own public functions in own static functions. */
    if (g_graph_break_all_connections(graph, other_graph, temp_edge_data_array))
    {
      g_graph_array_add(separate_graphs, other_graph);
    }
    g_ptr_array_set_size(edge_data_array, edge_data_array->len + temp_edge_data_array->len);
    for (iter2 = 0; iter2 < temp_edge_data_array->len; iter2++, iter3++)
    {
      g_ptr_array_index(edge_data_array, iter3) = g_ptr_array_index(temp_edge_data_array, iter2);
    }
  }
  g_graph_array_free(g_n, TRUE);
  return separate_graphs;
}
