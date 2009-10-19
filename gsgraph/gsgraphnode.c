#include "gsgraphnode.h"

/**
 * SECTION: gsgraphnode
 * @title: Simple graphs
 * @short_description: simple graphs for simple purposes.
 * @include: gsgraph/gsgraph.h
 *
 * Single node of simple graph. It contains data and an array of pointers to all
 * it's neighbours. If you want one way edges, data on edges - use #GGraph.
 *
 * To create a single node, use g_sgraph_node_new(). If you have a set of data you
 * can use g_sgraph_node_construct() to build graphs.
 *
 * To connect two nodes use g_sgraph_node_connect().
 *
 * To remove a node from graph and delete it, use g_sgraph_node_remove(). If you
 * don't want to delete the node itself, use g_sgraph_node_break_connection() on all
 * its neighbours.
 *
 * To get neighbours, use its public field.
 *
 * To get count of a graph, use g_sgraph_node_count().
 *
 * To free whole graph, use g_sgraph_node_free().
 *
 * <note>
 *   <para>
 *     Remember to always take care about data graph holds. Especially when
 *     freeing nodes. So, for example, when freeing whole graph, it is
 *     good to use g_sgraph_node_foreach() to free data before.
 *   </para>
 * </note>
 */

/* internal types */

typedef enum
{
  G_SGRAPH_NODE_NONE = 0,
  G_SGRAPH_NODE_FIRST = 1 << 0,
  G_SGRAPH_NODE_SECOND = 1 << 1,
  G_SGRAPH_NODE_BOTH = G_SGRAPH_NODE_FIRST | G_SGRAPH_NODE_SECOND
} GSGraphConstructFlags;

/* static declarations */

static gboolean
_g_sgraph_node_are_separate(GSGraphNode* sgraph_node,
                            GSGraphNode* other_sgraph_node);

static gboolean
_g_sgraph_node_recurrent_connection_check(GSGraphNode* sgraph_node,
                                          GSGraphNode* other_sgraph_node,
                                          GHashTable* visited_nodes);

static GSGraphNodeArray*
_g_sgraph_node_array(GSGraphNode* sgraph_node);

static void
_g_sgraph_node_recurrent_array_append(GSGraphNode* sgraph_node,
                                      GSGraphNodeArray* sgraph_node_array,
                                      GHashTable* visited_nodes);

static gboolean
_data_pair_is_valid(GSGraphDataPair* data_pair);

/* function definitions */

/**
 * g_sgraph_node_new:
 * @data: data.
 *
 * Creates new separate graph consisting of only one node containing @data.
 *
 * Returns: newly created #GSGraphNode.
 */
GSGraphNode*
g_sgraph_node_new(gpointer data)
{
  GSGraphNode* sgraph_node = g_slice_new(GSGraphNode);
  sgraph_node->data = data;
  sgraph_node->neighbours = g_sgraph_node_array_new();
  return sgraph_node;
}

/**
 * g_sgraph_node_construct:
 * @data_pairs: array of data pairs.
 * @count: length of @data_pairs.
 *
 * Creates a graph from passed data pairs. Resulting construction can be several
 * separate graphs, so an array of them is returned. Also, if any of
 * #GSGraphDataPair members are %NULL, then this pair is omitted in creation. If
 * @count is -1, it is assumed that @data_pairs is %NULL terminated.
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
GSGraphNodeArray*
g_sgraph_node_construct(GSGraphDataPair** data_pairs,
                        gint count)
{
  GSGraphNodeArray* separate_graphs;
  GHashTable* all_nodes;
  gint iter;
  
  g_return_val_if_fail(data_pairs != NULL, NULL);
  
  if (count < 0)
  {
    count = 0;
    while (data_pairs[count])
    {
      count++;
    }
  }
  
  if (!count)
  {
    return NULL;
  }
  
  separate_graphs = g_sgraph_node_array_new();
  all_nodes = g_hash_table_new(NULL, NULL);
  for (iter = 0; iter < count; iter++)
  {
    GSGraphDataPair* data_pair = data_pairs[iter];
    
    if (_data_pair_is_valid(data_pair))
    {
      GSGraphNode* first_node;
      GSGraphNode* second_node;
      GSGraphConstructFlags created = G_SGRAPH_NODE_NONE;
      
      if (!g_hash_table_lookup_extended(all_nodes, data_pair->first, NULL,
                                        (gpointer*)&first_node))
      {
        first_node = g_sgraph_node_new(data_pair->first);
        g_hash_table_insert(all_nodes, data_pair->first, first_node);
        created |= G_SGRAPH_NODE_FIRST;
      }
      
      if (!g_hash_table_lookup_extended(all_nodes, data_pair->second, NULL,
                                        (gpointer*)&second_node))
      {
        second_node = g_sgraph_node_new(data_pair->second);
        g_hash_table_insert(all_nodes, data_pair->second, second_node);
        created |= G_SGRAPH_NODE_SECOND;
      }
      
      g_sgraph_node_connect(first_node, second_node);
      switch (created)
      {
        case G_SGRAPH_NODE_NONE:
        {
          /* no nodes were created, so they can join two separate graphs. */
          guint iter2;
          guint iter3;
          gboolean hit = FALSE;
          
          for (iter2 = 0; iter2 < (separate_graphs->len - 1); iter2++)
          {
            for (iter3 = iter2 + 1; iter3 < separate_graphs->len; iter3++)
            {
              /* lets reuse the variables, they won't be needed. */
              first_node = g_sgraph_node_array_index(separate_graphs, iter2);
              second_node = g_sgraph_node_array_index(separate_graphs, iter3);
              if (!_g_sgraph_node_are_separate(first_node, second_node))
              {
                g_sgraph_node_array_remove_index_fast(separate_graphs, iter3);
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
        case G_SGRAPH_NODE_FIRST:
        case G_SGRAPH_NODE_SECOND:
        { 
          /* newly created node belongs to already created graph. */
          break;
        }
        case G_SGRAPH_NODE_BOTH:
        {
          /* if both nodes were created then they create separate graph. */
          g_sgraph_node_array_add(separate_graphs, first_node);
          break;
        }
      }
    }
  }
  g_hash_table_unref(all_nodes);
  if (!separate_graphs->len)
  {
    g_sgraph_node_array_free(separate_graphs, TRUE);
    separate_graphs = NULL;
  }
  return separate_graphs;
}

/**
 * g_sgraph_node_connect:
 * @sgraph_node: a graph.
 * @other_sgraph_node: a soon to be neighbour of @sgraph_node.
 *
 * Creates a connection between @sgraph_node and @other_sgraph_node. If connection already
 * existed, it does nothing.
 */
void
g_sgraph_node_connect(GSGraphNode* sgraph_node,
                      GSGraphNode* other_sgraph_node)
{
  guint iter;
  GSGraphNodeArray* s_n;
  
  g_return_if_fail(sgraph_node != NULL);
  g_return_if_fail(other_sgraph_node != NULL);
  
  s_n = sgraph_node->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(s_n, iter);
    if (node == other_sgraph_node)
    {
      return;
    }
  }
  g_sgraph_node_array_add(s_n, other_sgraph_node);
  g_sgraph_node_array_add(other_sgraph_node->neighbours, sgraph_node);
}

/**
 * g_sgraph_node_remove:
 * @sgraph_node: a node.
 *
 * Removes and frees a node. Data of this node should be freed earlier, if
 * applicable. Such removal may cause split of a graph into several smaller
 * ones, so this function returns #GSGraphNodeArray holding pointers to all
 * now-separated graphs.
 *
 * Returns: Array of separate graphs, which must be freed using
 * g_sgraph_node_array_free().
 */
GSGraphNodeArray*
g_sgraph_node_remove(GSGraphNode* sgraph_node)
{
  GSGraphNodeArray* s_n;
  GSGraphNodeArray* c_n;
  GSGraphNodeArray* separate_sgraphs;
  guint iter;
  
  g_return_val_if_fail(sgraph_node != NULL, NULL);
  
  separate_sgraphs = g_sgraph_node_array_new();
  s_n = sgraph_node->neighbours;
  c_n = g_sgraph_node_array_sized_new(s_n->len);
  for (iter = 0; iter < s_n->len; iter++)
  {
    g_sgraph_node_array_add(c_n, g_sgraph_node_array_index(s_n, iter));
  }
  for (iter = 0; iter < c_n->len; iter++)
  {
    GSGraphNode* other_sgraph_node = g_sgraph_node_array_index(c_n, iter);
    if (g_sgraph_node_break_connection(sgraph_node, other_sgraph_node))
    {
      g_sgraph_node_array_add(separate_sgraphs, other_sgraph_node);
    }
  }
  g_sgraph_node_free(sgraph_node);
  g_sgraph_node_array_free(c_n, TRUE);
  return separate_sgraphs;
}

/**
 * g_sgraph_node_copy:
 * @sgraph_node: a node of graph to be copied.
 *
 * Does a shallow copy of @sgraph_node. This means that original graph and its copy
 * shares the data, so when freeing both graphs you should be careful not to
 * free same data twice.
 *
 * Returns: A copy of @sgraph_node.
 */
GSGraphNode*
g_sgraph_node_copy(GSGraphNode* sgraph_node)
{
  return g_sgraph_node_copy_deep(sgraph_node, NULL, NULL);
}

/**
 * g_sgraph_node_copy_deep:
 * @sgraph_node: a node of graph to be copied.
 * @copy_func: function copying data.
 * @user_data: data passed to copying function.
 *
 * Does a deep copy of @sgraph_node. Each node in @sgraph_node copy is duplicated using
 * passed function. If @copy_func is NULL, then shallow copy is done.
 *
 * Returns: A copy of @sgraph_node.
 */
GSGraphNode*
g_sgraph_node_copy_deep(GSGraphNode* sgraph_node,
                        GCopyFunc copy_func,
                        gpointer user_data)
{
  GSGraphNodeArray* sgraph_node_array = _g_sgraph_node_array(sgraph_node);
  GHashTable* nodes_to_dups = g_hash_table_new(NULL, NULL);
  GSGraphNode* dup_graph;
  guint iter;
  
  if (!sgraph_node)
  {
    return NULL;
  }
  
  for (iter = 0; iter < sgraph_node_array->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(sgraph_node_array, iter);
    GSGraphNode* dup_node = g_slice_new(GSGraphNode);
    if (copy_func)
    {
      dup_node->data = (*copy_func)(node->data, user_data);
    }
    else
    {
      dup_node->data = node->data;
    }
    g_hash_table_insert(nodes_to_dups, node, dup_node);
  }
  for (iter = 0; iter < sgraph_node_array->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(sgraph_node_array, iter);
    GSGraphNode* dup_node = g_hash_table_lookup(nodes_to_dups, node);
    GSGraphNodeArray* dup_neighbours = g_sgraph_node_array_sized_new(node->neighbours->len);
    guint iter2;
    for (iter2 = 0; iter2 < node->neighbours->len; iter++)
    {
      GSGraphNode* neighbour = g_sgraph_node_array_index(node->neighbours, iter2);
      GSGraphNode* dup_neighbour = g_hash_table_lookup(nodes_to_dups, neighbour);
      g_sgraph_node_array_add(dup_neighbours, dup_neighbour);
    }
    dup_node->neighbours = dup_neighbours;
  }
  dup_graph = g_hash_table_lookup(nodes_to_dups, sgraph_node);
  g_hash_table_unref(nodes_to_dups);
  g_sgraph_node_array_free(sgraph_node_array, TRUE);
  return dup_graph;
}

/**
 * g_sgraph_node_free:
 * @sgraph_node: a graph.
 *
 * Frees whole graph to which @sgraph_node belongs. Each node's data should be freed
 * earlier, if applicable.
 */
void
g_sgraph_node_free(GSGraphNode* sgraph_node)
{
  GSGraphNodeArray* sgraph_node_array;
  guint iter;
  
  if (!sgraph_node)
  {
    return;
  }
  
  sgraph_node_array = _g_sgraph_node_array(sgraph_node);
  for (iter = 0; iter < sgraph_node_array->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(sgraph_node_array, iter);
    g_sgraph_node_array_free(node->neighbours, TRUE);
    g_slice_free(GSGraphNode, node);
  }
  g_sgraph_node_array_free(sgraph_node_array, TRUE);
}

/**
 * g_sgraph_node_count:
 * @sgraph_node: a graph.
 *
 * Counts number of nodes in graph.
 *
 * Returns: number of nodes in graph.
 */
guint
g_sgraph_node_count(GSGraphNode* sgraph_node)
{
  GSGraphNodeArray* sgraph_node_array;
  guint count;
  
  g_return_val_if_fail(sgraph_node != NULL, 0);
  
  sgraph_node_array = _g_sgraph_node_array(sgraph_node);
  count = sgraph_node_array->len;
  g_sgraph_node_array_free(sgraph_node_array, TRUE);
  return count;
}

/**
 * g_sgraph_node_foreach:
 * @sgraph_node: a node.
 * @func: the function to call with each element's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node in graph @sgraph_node belongs to.
 */
void
g_sgraph_node_foreach(GSGraphNode* sgraph_node,
                      GFunc func,
                      gpointer user_data)
{
  GSGraphNodeArray* sgraph_node_array;
  guint iter;
  
  g_return_if_fail(sgraph_node != NULL);
  g_return_if_fail(func != NULL);
  
  sgraph_node_array = _g_sgraph_node_array(sgraph_node);
  for (iter = 0; iter < sgraph_node_array->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(sgraph_node_array, iter);
    (*func)(node->data, user_data);
  }
  g_sgraph_node_array_free(sgraph_node_array, TRUE);
}

/**
 * g_sgraph_node_foreach_node:
 * @sgraph_node: a node.
 * @func: the function to call with each node.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node in graph @sgraph_node belongs to. The difference between
 * this function and g_sgraph_node_foreach is that this function calls @func on
 * nodes, not only on their data.
 */
void
g_sgraph_node_foreach_node(GSGraphNode* sgraph_node,
                           GSGraphNodeFunc func,
                           gpointer user_data)
{
  GSGraphNodeArray* sgraph_node_array;
  guint iter;
  
  g_return_if_fail(sgraph_node != NULL);
  g_return_if_fail(func != NULL);
  
  sgraph_node_array = _g_sgraph_node_array(sgraph_node);
  for (iter = 0; iter < sgraph_node_array->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(sgraph_node_array, iter);
    (*func)(node, user_data);
  }
  g_sgraph_node_array_free(sgraph_node_array, TRUE);
}

/**
 * g_sgraph_node_break_connection:
 * @sgraph_node: a node.
 * @other_sgraph_node: other node which is @sgraph_node's neighbour.
 *
 * Breaks both connections from one node to another, so in effect they stop
 * being neighbours. If such breakage causes to create two separate graphs,
 * notify about it by returning %TRUE. This function return %FALSE also when
 * @sgraph_node and @other_sgraph_node aren't neighbours, so there was no connection to
 * break.
 *
 * Returns: %TRUE if @sgraph_node and @other_sgraph_node are now in separate graphs,
 * otherwise %FALSE.
 */
gboolean
g_sgraph_node_break_connection(GSGraphNode* sgraph_node,
                               GSGraphNode* other_sgraph_node)
{
  GSGraphNodeArray* s_n;
  GSGraphNodeArray* o_n;
  guint iter;
  gboolean found = FALSE;
  
  g_return_val_if_fail(sgraph_node != NULL, FALSE);
  g_return_val_if_fail(other_sgraph_node != NULL, FALSE);
  
  s_n = sgraph_node->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(s_n, iter);
    if (node == other_sgraph_node)
    {
      g_sgraph_node_array_remove_index(s_n, iter);
      found = TRUE;
      break;
    }
  }
  if (!found)
  {
    return FALSE;
  }
  o_n = other_sgraph_node->neighbours;
  for (iter = 0; iter < o_n->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(o_n, iter);
    if (node == sgraph_node)
    {
      g_sgraph_node_array_remove_index(o_n, iter);
      break;
    }
  }
  
  return _g_sgraph_node_are_separate(sgraph_node, other_sgraph_node);
}

/**
 * g_sgraph_node_find:
 * @sgraph_node: a node.
 * @data: the element data to find.
 *
 * It calls g_sgraph_node_find_custom() with g_direct_equal() as a third parameter.
 *
 * Returns: a #GSGraphNodeArray* that must be freed with g_sgraph_node_array_free().
 */
GSGraphNodeArray*
g_sgraph_node_find(GSGraphNode* sgraph_node,
                   gconstpointer data)
{
  return g_sgraph_node_find_custom(sgraph_node, data, g_direct_equal);
}

/**
 * g_sgraph_node_find_custom:
 * @sgraph_node: a node.
 * @data: data passed to @func.
 * @func: the function to call for each element.
 *
 * Finds an element in a #GSGraphNode, using a supplied function to find the desired
 * elements. It iterates over the whole graph, calling the given function which
 * should return %TRUE when the desired element is found. The function takes two
 * gconstpointer arguments, the #GSGraphNode element's data as the first argument
 * and the given user data as second. If an element is found it is added to a
 * #GSGraphNodeArray.
 *
 * Returns: a #GSGraphNodeArray* that must be freed with g_sgraph_node_array_free().
 */
GSGraphNodeArray*
g_sgraph_node_find_custom(GSGraphNode* sgraph_node,
                          gconstpointer data,
                          GEqualFunc func)
{
  GSGraphNodeArray* matching_sgraphs;
  GSGraphNodeArray* sgraph_node_array;
  guint iter;
  
  g_return_val_if_fail(sgraph_node != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  matching_sgraphs = g_sgraph_node_array_new();
  sgraph_node_array = _g_sgraph_node_array(sgraph_node);
  for (iter = 0; iter < sgraph_node_array->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(sgraph_node_array, iter);
    if ((*func)(node->data, data))
    {
      g_sgraph_node_array_add(matching_sgraphs, node);
    }
  }
  g_sgraph_node_array_free(sgraph_node_array, TRUE);
  if (!matching_sgraphs->len)
  {
    g_sgraph_node_array_free(matching_sgraphs, TRUE);
    matching_sgraphs = NULL;
  }
  return matching_sgraphs;
}

/**
 * g_sgraph_node_find_custom_node:
 * @sgraph_node: a node.
 * @node: node passed to @func.
 * @func: the function to call for each element.
 *
 * Finds an element in a #GSGraphNode, using a supplied function to find the desired
 * elements. It iterates over the whole graph, calling the given function which
 * should return %TRUE when the desired element is found. The function takes two
 * #GSGraphNode arguments, the #GSGraphNode from graph as the first argument and @node
 * as second. If an element is found it is added to a #GSGraphNodeArray.
 *
 * Returns: a #GSGraphNodeArray* that must be freed with g_sgraph_node_array_free().
 */
GSGraphNodeArray*
g_sgraph_node_find_custom_node(GSGraphNode* sgraph_node,
                               GSGraphNode* node,
                               GSGraphNodeEqualFunc func)
{
  GSGraphNodeArray* matching_sgraphs;
  GSGraphNodeArray* sgraph_node_array;
  guint iter;
  
  g_return_val_if_fail(sgraph_node != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);
  
  matching_sgraphs = g_sgraph_node_array_new();
  sgraph_node_array = _g_sgraph_node_array(sgraph_node);
  for (iter = 0; iter < sgraph_node_array->len; iter++)
  {
    GSGraphNode* other_node = g_sgraph_node_array_index(sgraph_node_array, iter);
    if ((*func)(other_node, node))
    {
      g_sgraph_node_array_add(matching_sgraphs, other_node);
    }
  }
  g_sgraph_node_array_free(sgraph_node_array, TRUE);
  if (!matching_sgraphs->len)
  {
    g_sgraph_node_array_free(matching_sgraphs, TRUE);
    matching_sgraphs = NULL;
  }
  return matching_sgraphs;
}

/* static function definitions */

/*
 * _g_sgraph_node_are_separate:
 * @sgraph_node: starting node.
 * @other_sgraph_node: node for which we want to check if it has series of
 * connections to @sgraph_node.
 *
 * This function checks if there is no series of connections between @sgraph_node and
 * @other_sgraph_node.
 *
 * Returns: %TRUE if @sgraph_node and @other_sgraph_node are in separate graphs.
 */
static gboolean
_g_sgraph_node_are_separate(GSGraphNode* sgraph_node,
                            GSGraphNode* other_sgraph_node)
{
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  gboolean not_connected = !_g_sgraph_node_recurrent_connection_check(sgraph_node,
                                                                      other_sgraph_node,
                                                                      visited_nodes);
  g_hash_table_unref(visited_nodes);
  return not_connected;
}

/*
 * _g_sgraph_node_recurrent_connection_check:
 * @sgraph_node: starting node.
 * @other_sgraph_node: other node.
 * @visited_nodes: #GHashTable holding information which nodes were already
 * visited.
 *
 * Checks if @sgraph_node == @other_sgraph_node. If not it checks @sgraph_node's neighbours.
 *
 * Returns: %TRUE if @sgraph_node == @other_sgraph_node, otherwise %FALSE.
 */
static gboolean
_g_sgraph_node_recurrent_connection_check(GSGraphNode* sgraph_node,
                                          GSGraphNode* other_sgraph_node,
                                          GHashTable* visited_nodes)
{
  guint iter;
  GSGraphNodeArray* s_n;
  if (sgraph_node == other_sgraph_node)
  {
    return TRUE;
  }
  if (g_hash_table_lookup_extended(visited_nodes, sgraph_node, NULL, NULL))
  {
    return FALSE;
  }
  g_hash_table_insert(visited_nodes, sgraph_node, NULL);
  s_n = sgraph_node->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(s_n, iter);
    if (_g_sgraph_node_recurrent_connection_check(node, other_sgraph_node, visited_nodes))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * _g_sgraph_node_array:
 * @sgraph_node: graph, which will be put into an array.
 *
 * Puts whole graph into an array, so every node can be accessed without hassle.
 *
 * Returns: a #GSGraphNodeArray* that must be freed with g_sgraph_node_array_free().
 */
static GSGraphNodeArray*
_g_sgraph_node_array(GSGraphNode* sgraph_node)
{
  GHashTable* visited_nodes = g_hash_table_new(NULL, NULL);
  GSGraphNodeArray* sgraph_node_array = g_sgraph_node_array_new();
  _g_sgraph_node_recurrent_array_append(sgraph_node, sgraph_node_array, visited_nodes);
  g_hash_table_unref(visited_nodes);
  return sgraph_node_array;
}

/*
 * _g_sgraph_node_recurrent_array_append:
 * @sgraph_node: node which will be (probably) put into @sgraph_node_array.
 * @sgraph_node_array: array of nodes of a whole graph.
 * @visited_nodes: #GHashTable holding information which nodes were already
 * visited.
 *
 * If @sgraph_node isn't already in @sgraph_node_array, it will be put into it.
 */
static void
_g_sgraph_node_recurrent_array_append(GSGraphNode* sgraph_node,
                                      GSGraphNodeArray* sgraph_node_array,
                                      GHashTable* visited_nodes)
{
  guint iter;
  GSGraphNodeArray* s_n;
  if (g_hash_table_lookup_extended(visited_nodes, sgraph_node, NULL, NULL))
  {
    return;
  }
  g_hash_table_insert(visited_nodes, sgraph_node, NULL);
  g_sgraph_node_array_add(sgraph_node_array, sgraph_node);
  s_n = sgraph_node->neighbours;
  for (iter = 0; iter < s_n->len; iter++)
  {
    GSGraphNode* node = g_sgraph_node_array_index(s_n, iter);
    _g_sgraph_node_recurrent_array_append(node, sgraph_node_array, visited_nodes);
  }
}

/*
 * _data_pair_is_valid:
 * @data_pair: a #GSGraphDataPair to check
 *
 * Checks if both members of @data_pair are not %NULL.
 *
 * Returns: %TRUE if both members are not %NULL, otherwise %FALSE.
 */
static gboolean
_data_pair_is_valid(GSGraphDataPair* data_pair)
{
  return ((data_pair->first != NULL) && (data_pair->second != NULL));
}
