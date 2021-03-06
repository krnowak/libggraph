/*
 * Copyright (C) 2009, 2010 Krzesimir Nowak
 *
 * This file is part of libggraph.
 *
 * libggraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libggraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libggraph.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gsgraphsnapshot.h"

/**
 * SECTION: gsgraphsnapshot
 * @title: Simple graph snapshot
 * @short_description: convenient structure holding nodes in array.
 * @include: gsgraph/gsgraph.h
 * @see_also: #GSGraphNode, #GSGraphDataPair, #GSGraphTraverseType
 *
 * Convenient structure holding all nodes in array for easy processing them with
 * simply iterating the array. This wrapper is sort of semi-snapshot of a graph
 * - that is, if after creation of #GSGraphSnapshot new node is added to graph,
 * this snapshot is out of date, but changes inside nodes already inside the
 * wrapper are reflected in wrapper too.
 *
 * Wrapper can be created using two traversing algorithms to add nodes to arrays
 * inside the wrapper or by specifing data triplets.
 *
 * To create a structure, use g_sgraph_snapshot_new() or
 * g_sgraph_snapshot_new_from_node().
 *
 * To make a copy of graph, use g_sgraph_snapshot_copy() or
 * g_sgraph_snapshot_copy_deep().
 *
 * To free a wrapper or whole graph too, use g_sgraph_snapshot_free().
 *
 * To find a node, use g_sgraph_snapshot_find_node_custom().
 *
 * To process nodes, use g_sgraph_snapshot_foreach_node().
 *
 * <note>
 *   <para>
 *     Remember to always take care about data graph holds. When freeing whole
 *     graph, not only wrapper, it is good to free data in nodes to avoid memory
 *     leaks.
 *   </para>
 * </note>
 */

/* internal types. */

/**
 * GSGraphConstructFlags:
 * @G_SGRAPH_NONE: no nodes were created.
 * @G_SGRAPH_FIRST: node containing first data was created.
 * @G_SGRAPH_SECOND: node containing second data was created.
 * @G_SGRAPH_BOTH: both nodes were created.
 *
 * Internal enum for g_sgraph_snapshot_new() needs. Describes which nodes were
 * created.
 */
typedef enum
{
  G_SGRAPH_NONE = 0,
  G_SGRAPH_FIRST = 1 << 0,
  G_SGRAPH_SECOND = 1 << 1,
  G_SGRAPH_BOTH = G_SGRAPH_FIRST | G_SGRAPH_SECOND
} GSGraphConstructFlags;

/* static function declarations. */

static void
_g_sgraph_snapshot_append_DFS (GSGraphNode* node,
                               GPtrArray* node_array,
                               GHashTable* visited_nodes);

static void
_g_sgraph_snapshot_append_BFS (GSGraphNode* node,
                               GPtrArray* node_array,
                               GHashTable* visited_nodes);

static GSGraphSnapshot*
_g_sgraph_snapshot_new_from_node_general (GSGraphNode* node,
                                          GSGraphTraverseType traverse_type);

static GSGraphSnapshot*
_g_sgraph_snapshot_copy_general (GSGraphSnapshot* graph,
                                 GCopyFunc node_data_copy_func,
                                 gpointer node_user_data);

static GSGraphSnapshot*
_g_sgraph_snapshot_new_blank (guint node_array_size);

/* public function definitions. */

/**
 * g_sgraph_snapshot_new:
 * @data_pairs: array of data pairs.
 * @count: length of @data_pairs.
 *
 * Creates a graph from passed data pairs. Resulting construction can be several
 * separate graphs, so an array of #GSGraphSnapshot<!-- -->s is returned. Also,
 * if either first or second #GSGraphDataPair member is %NULL, then this pair is
 * omitted in creation. If @count is 0, it  is assumed that @data_pairs is %NULL
 * terminated array.
 *
 * For performance reasons it would be good if in second and
 * later pairs one of node have data describing already created node.
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
 * members is already created, so newly created one will be its neighbour and a
 * part of larger graph.
 * Swaping third and fourth pair will cause creation of a separate graph while
 * processing third pair and checking if two graphs are one when processing
 * fourth pair.
 *
 * Returns: array of newly created separate graphs or %NULL if no nodes were
 * created.
 */
GPtrArray*
g_sgraph_snapshot_new (GSGraphDataPair** data_pairs,
                       guint count)
{
  GPtrArray* separate_graphs;
  GHashTable* data_to_nodes;
  GHashTable* nodes_to_wholes;
  guint iter;

  g_return_val_if_fail (data_pairs != NULL, NULL);

  if (!count)
  {
    while (data_pairs[count])
    {
      ++count;
    }
  }

  if (!count)
  {
    return NULL;
  }

  separate_graphs = g_ptr_array_new ();
  data_to_nodes = g_hash_table_new (NULL, NULL);
  nodes_to_wholes = g_hash_table_new (NULL, NULL);
  for (iter = 0; iter < count; ++iter)
  {
    GSGraphDataPair* data_pair;

    data_pair = data_pairs[iter];
    if (g_sgraph_data_pair_is_valid (data_pair))
    {
      GSGraphNode* first_node;
      GSGraphNode* second_node;
      GSGraphConstructFlags created;

      created = G_SGRAPH_NONE;
      if (!g_hash_table_lookup_extended (data_to_nodes,
                                         data_pair->first,
                                         NULL,
                                         (gpointer*)&first_node))
      {
        first_node = g_sgraph_node_new (data_pair->first);
        g_hash_table_insert (data_to_nodes, data_pair->first, first_node);
        created |= G_SGRAPH_FIRST;
      }

      if (!g_hash_table_lookup_extended (data_to_nodes,
                                         data_pair->second,
                                         NULL,
                                         (gpointer*)&second_node))
      {
        second_node = g_sgraph_node_new (data_pair->second);
        g_hash_table_insert (data_to_nodes, data_pair->second, second_node);
        created |= G_SGRAPH_SECOND;
      }

      g_sgraph_node_connect (first_node, second_node);
      switch (created)
      {
        case G_SGRAPH_NONE:
        {
          /* no nodes were created, so they can join two separate graphs. */
          #define JOIN_COUNT 2
          guint which;
          GSGraphSnapshot* joined_graphs[JOIN_COUNT];
          GSGraphNode* check_nodes[JOIN_COUNT];

          check_nodes[0] = first_node;
          check_nodes[1] = second_node;
          for (which = 0; which < JOIN_COUNT; ++which)
          {
            guint iter2;

            for (iter2 = 0; iter2 < separate_graphs->len; ++iter2)
            {
              GSGraphSnapshot* graph;
              guint iter3;
              gboolean hit;

              graph = g_ptr_array_index (separate_graphs, iter2);
              hit = FALSE;
              for (iter3 = 0; iter3 < graph->node_array->len; ++iter3)
              {
                GSGraphNode* temp_node;

                temp_node = g_ptr_array_index (graph->node_array, iter3);
                if (temp_node == check_nodes[which])
                {
                  joined_graphs[which] = graph;
                  hit = TRUE;
                  break;
                }
              }
              if (hit)
              {
                break;
              }
            }
          }

          if (joined_graphs[0] != joined_graphs[1])
          {
            guint iter2;

            for (iter2 = 0; iter2 < joined_graphs[1]->node_array->len; ++iter2)
            {
              GSGraphNode* temp_node;

              temp_node = g_ptr_array_index (joined_graphs[1]->node_array,
                                             iter2);
              if (temp_node != check_nodes[1])
              {
                g_ptr_array_add (joined_graphs[0]->node_array, temp_node);
                g_hash_table_insert (nodes_to_wholes,
                                     temp_node,
                                     joined_graphs[0]);
              }
            }
            g_ptr_array_remove_fast (separate_graphs, joined_graphs[1]);
            g_sgraph_snapshot_free (joined_graphs[1], FALSE);
          }
          #undef JOIN_COUNT
          break;
        }
        case G_SGRAPH_FIRST:
        {
          /* first node was created, so it belongs to existing graph. */
          GSGraphSnapshot* temp_graph;

          temp_graph = g_hash_table_lookup (nodes_to_wholes, second_node);
          g_ptr_array_add (temp_graph->node_array, first_node);
          g_hash_table_insert (nodes_to_wholes, first_node, temp_graph);
          break;
        }
        case G_SGRAPH_SECOND:
        {
          /* second node was created, so it belongs to existing graph. */
          GSGraphSnapshot* temp_graph;

          temp_graph = g_hash_table_lookup (nodes_to_wholes, first_node);
          g_ptr_array_add (temp_graph->node_array, second_node);
          g_hash_table_insert (nodes_to_wholes, second_node, temp_graph);
          break;
        }
        case G_SGRAPH_BOTH:
        {
          /* if both nodes were created then they create separate graph. */
          GSGraphSnapshot* temp_graph;

          temp_graph = _g_sgraph_snapshot_new_blank (2);
          g_ptr_array_add (temp_graph->node_array, first_node);
          g_ptr_array_add (temp_graph->node_array, second_node);
          g_ptr_array_add (separate_graphs, temp_graph);
          g_hash_table_insert (nodes_to_wholes, first_node, temp_graph);
          g_hash_table_insert (nodes_to_wholes, second_node, temp_graph);
          break;
        }
      }
    }
  }
  g_hash_table_unref (data_to_nodes);
  g_hash_table_unref (nodes_to_wholes);
  if (!separate_graphs->len)
  {
    g_ptr_array_free (separate_graphs, TRUE);
    separate_graphs = NULL;
  }
  return separate_graphs;
}

/**
 * g_sgraph_snapshot_new_from_node:
 * @node: a node in graph.
 * @traverse_type: which traversing algorithm to use.
 *
 * Creates new #GSGraphSnapshot with array holding nodes of a graph containing
 * @node in order specified by @traverse_type.
 *
 * Returns: new #GSGraphSnapshot.
 */
GSGraphSnapshot*
g_sgraph_snapshot_new_from_node (GSGraphNode* node,
                                 GSGraphTraverseType traverse_type)
{
  g_return_val_if_fail (node != NULL, NULL);

  return _g_sgraph_snapshot_new_from_node_general (node, traverse_type);
}

/**
 * g_sgraph_snapshot_copy:
 * @graph: a graph to be copied.
 *
 * Does a shallow copy of @graph. This means that original graph and its copy
 * shares the data, so when freeing both graphs you should be careful not to
 * free same data twice. @graph's members must not be %NULL.
 *
 * Returns: A copy of @graph.
 */
GSGraphSnapshot*
g_sgraph_snapshot_copy (GSGraphSnapshot* graph)
{
  g_return_val_if_fail (graph != NULL, NULL);

  return _g_sgraph_snapshot_copy_general (graph, NULL, NULL);
}

/**
 * g_sgraph_snapshot_copy_deep:
 * @graph: a graph to be copied.
 * @node_data_copy_func: function copying data in nodes.
 * @node_user_data: data passed to node data copying function.
 *
 * Does a deep copy of @graph. Each node data in @graph copy is duplicated using
 * passed function. @node_data_copy_func must not be %NULL.
 *
 * Returns: A copy of @graph.
 */
GSGraphSnapshot*
g_sgraph_snapshot_copy_deep (GSGraphSnapshot* graph,
                             GCopyFunc node_data_copy_func,
                             gpointer node_user_data)
{
  g_return_val_if_fail (graph != NULL, NULL);
  g_return_val_if_fail (node_data_copy_func != NULL, NULL);

  return _g_sgraph_snapshot_copy_general (graph,
                                          node_data_copy_func,
                                          node_user_data);
}

/**
 * g_sgraph_snapshot_free:
 * @graph: #GSGraphSnapshot to free.
 * @deep_free: whether to do a deep free.
 *
 * Frees memory allocated to @graph. If @deep_free is %TRUE then all nodes are
 * also freed.
 */
void
g_sgraph_snapshot_free (GSGraphSnapshot* graph,
                        gboolean deep_free)
{
  g_return_if_fail (graph != NULL);

  if (deep_free)
  {
    guint iter;

    for (iter = 0; iter < graph->node_array->len; ++iter)
    {
      GSGraphNode* node;

      node = g_ptr_array_index (graph->node_array, iter);
      g_sgraph_node_free (node);
    }
  }

  g_ptr_array_free (graph->node_array, TRUE);
  g_slice_free (GSGraphSnapshot, graph);
}

/**
 * g_sgraph_snapshot_get_order:
 * @graph: a graph.
 *
 * Gets @graph's order, that is - number of nodes in graph.
 *
 * Returns: number of nodes in graph.
 */
guint
g_sgraph_snapshot_get_order (GSGraphSnapshot* graph)
{
  g_return_val_if_fail (graph != NULL, 0);
  g_return_val_if_fail (graph->node_array != NULL, 0);

  return graph->node_array->len;
}

/**
 * g_sgraph_snapshot_get_size:
 * @graph: a graph.
 *
 * Gets @graph's size, that is - number of edges in graph. Since there are no
 * edges then number of all connections is returned.
 *
 * Returns: number of edges in graph.
 */
guint
g_sgraph_snapshot_get_size (GSGraphSnapshot* graph)
{
  guint iter;
  guint size;

  g_return_val_if_fail (graph != NULL, 0);
  g_return_val_if_fail (graph->node_array != NULL, 0);

  size = 0;
  for (iter = 0; iter < graph->node_array->len; ++iter)
  {
    GSGraphNode* node;

    node = g_ptr_array_index (graph->node_array, iter);
    size += node->neighbours->len;
  }

  return (size / 2);
}

/**
 * g_sgraph_snapshot_foreach_node:
 * @graph: a graph.
 * @func: the function to call with each @graph's node.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node in @graph.
 */
void
g_sgraph_snapshot_foreach_node (GSGraphSnapshot* graph,
                                GFunc func,
                                gpointer user_data)
{
  guint iter;

  g_return_if_fail (graph != NULL);
  g_return_if_fail (graph->node_array != NULL);
  g_return_if_fail (func != NULL);

  for (iter = 0; iter < graph->node_array->len; ++iter)
  {
    GSGraphNode* node = g_ptr_array_index (graph->node_array, iter);
    (*func) (node, user_data);
  }
}

/**
 * g_sgraph_snapshot_find_node_custom:
 * @graph: a graph.
 * @user_data: user data passed to @func.
 * @func: the function to call for each node.
 *
 * Finds first element in a @graph, using a supplied function to find the
 * desired element. It iterates over the node array, calling the given function
 * which should return %TRUE when the desired element is found. The function
 * takes two arguments, the node from the iterated array as the first argument
 * and the given @user_data as second. If an element is found it is returned.
 *
 * Returns: found #GSGraphNode or %NULL if there was no such node.
 */
GSGraphNode*
g_sgraph_snapshot_find_node_custom (GSGraphSnapshot* graph,
                                    gpointer user_data,
                                    GEqualFunc func)
{
  guint iter;

  g_return_val_if_fail (graph != NULL, NULL);
  g_return_val_if_fail (graph->node_array != NULL, NULL);
  g_return_val_if_fail (func != NULL, NULL);

  for (iter = 0; iter < graph->node_array->len; ++iter)
  {
    GSGraphNode* node = g_ptr_array_index (graph->node_array, iter);
    if ((*func) (node, user_data))
    {
      return node;
    }
  }
  return NULL;
}

/* static function definitions. */

/**
 * _g_sgraph_snapshot_append_DFS:
 * @node: a node, which will be put into arrays with its edges.
 * @node_array: array of all nodes in graph.
 * @visited_nodes: a map of already visited nodes.
 *
 * Checks all nodes and put them into array using depth first search algorithm.
 */
static void
_g_sgraph_snapshot_append_DFS (GSGraphNode* node,
                               GPtrArray* node_array,
                               GHashTable* visited_nodes)
{
  guint iter;

  if (g_hash_table_lookup_extended (visited_nodes, node, NULL, NULL))
  {
    return;
  }

  g_hash_table_insert (visited_nodes, node, NULL);
  g_ptr_array_add (node_array, node);
  for (iter = 0; iter < node->neighbours->len; ++iter)
  {
    GSGraphNode* other_node;

    other_node = g_ptr_array_index (node->neighbours, iter);
    _g_sgraph_snapshot_append_DFS (other_node, node_array, visited_nodes);
  }
}

/**
 * _g_sgraph_snapshot_append_BFS:
 * @node: a node, which will be put into arrays with its edges.
 * @node_array: array of all nodes in graph.
 * @visited_nodes: a map of already visited nodes.
 *
 * Checks all nodes and put them into array using breadth first search
 * algorithm.
 */
static void
_g_sgraph_snapshot_append_BFS (GSGraphNode* node,
                               GPtrArray* node_array,
                               GHashTable* visited_nodes)
{
  GQueue* queue;

  queue = g_queue_new ();

  g_hash_table_insert (visited_nodes, node, NULL);
  g_queue_push_tail (queue, node);
  g_ptr_array_add (node_array, node);

  while (!g_queue_is_empty (queue))
  {
    GSGraphNode* temp_node;
    guint iter;

    temp_node = g_queue_pop_head (queue);

    for (iter = 0; iter < temp_node->neighbours->len; ++iter)
    {
      GSGraphNode* other_node;

      other_node = g_ptr_array_index (temp_node->neighbours, iter);

      if (g_hash_table_lookup_extended (visited_nodes, other_node, NULL, NULL))
      {
        continue;
      }

      g_hash_table_insert (visited_nodes, other_node, NULL);
      g_queue_push_tail (queue, other_node);
      g_ptr_array_add (node_array, other_node);
    }
  }

  g_queue_free (queue);
}

/**
 * _g_sgraph_snapshot_new_from_node_general:
 * @node: a node being a part of a graph.
 * @traverse_type: which traversing algorithm to use.
 *
 * General function creating a #GSGraphSnapshot from given @node using
 * traversing algorithm denoted by @traverse_type.
 *
 * Returns: new #GSGraphSnapshot.
 */
static GSGraphSnapshot*
_g_sgraph_snapshot_new_from_node_general (GSGraphNode* node,
                                          GSGraphTraverseType traverse_type)
{
  typedef void (*GraphSearchFunc) (GSGraphNode* node,
                                   GPtrArray* node_array,
                                   GHashTable* visited_nodes);

  GSGraphSnapshot* graph;
  GHashTable* visited_nodes;
  GraphSearchFunc gsfunc;

  switch (traverse_type)
  {
    case G_SGRAPH_TRAVERSE_BFS:
    {
      gsfunc = _g_sgraph_snapshot_append_BFS;
      break;
    }
    case G_SGRAPH_TRAVERSE_DFS:
    {
      gsfunc = _g_sgraph_snapshot_append_DFS;
      break;
    }
    default:
    {
      g_return_val_if_reached (NULL);
    }
  }

  graph = _g_sgraph_snapshot_new_blank (0);

  visited_nodes = g_hash_table_new (NULL, NULL);

  (*gsfunc) (node, graph->node_array, visited_nodes);

  g_hash_table_unref (visited_nodes);

  return graph;
}

/**
 * _g_sgraph_snapshot_copy_general:
 * @graph: a graph to be copied.
 * @node_data_copy_func: function copying data in nodes.
 * @node_user_data: data passed to node data copying function.
 *
 * Does a copy of @graph. Each node data in @graph copy is duplicated using
 * passed function. If @node_data_copy_func is %NULL, then shallow copy of data
 * is done.
 *
 * Returns: A copy of @graph.
 */
static GSGraphSnapshot*
_g_sgraph_snapshot_copy_general (GSGraphSnapshot* graph,
                                 GCopyFunc node_data_copy_func,
                                 gpointer node_user_data)
{
  GHashTable* nodes_to_dups;
  GSGraphSnapshot* dup_graph;
  guint iter;

  nodes_to_dups = g_hash_table_new (NULL, NULL);
  dup_graph = _g_sgraph_snapshot_new_blank (graph->node_array->len);

  for (iter = 0; iter < graph->node_array->len; ++iter)
  {
    GSGraphNode* node;
    GSGraphNode* dup_node;

    node = g_ptr_array_index (graph->node_array, iter);
    dup_node = g_slice_new (GSGraphNode);

    if (node_data_copy_func)
    {
      dup_node->data = (*node_data_copy_func) (node->data, node_user_data);
    }
    else
    {
      dup_node->data = node->data;
    }

    g_hash_table_insert (nodes_to_dups, node, dup_node);
    g_ptr_array_add (dup_graph->node_array, dup_node);
  }

  for (iter = 0; iter < graph->node_array->len; ++iter)
  {
    GSGraphNode* node;
    GSGraphNode* dup_node;
    GPtrArray* dup_edges;
    guint iter2;

    node = g_ptr_array_index (graph->node_array, iter);
    dup_node = g_ptr_array_index (dup_graph->node_array, iter);
    dup_edges = g_ptr_array_sized_new (node->neighbours->len);

    for (iter2 = 0; iter2 < node->neighbours->len; ++iter2)
    {
      GSGraphNode* neighbour;
      GSGraphNode* dup_neighbour;

      neighbour = g_ptr_array_index (node->neighbours, iter2);
      dup_neighbour = g_hash_table_lookup (nodes_to_dups, neighbour);
      g_ptr_array_add (dup_edges, dup_neighbour);
    }
    dup_node->neighbours = dup_edges;
  }

  g_hash_table_unref (nodes_to_dups);
  return dup_graph;
}

/**
 * _g_sgraph_snapshot_new_blank:
 * @node_array_size: size of node array.
 *
 * Constructs #GSGraphSnapshot with an array with given size.
 *
 * Returns: new #GSGraphSnapshot.
 */
static GSGraphSnapshot*
_g_sgraph_snapshot_new_blank (guint node_array_size)
{
  GSGraphSnapshot* graph;

  graph = g_slice_new (GSGraphSnapshot);

  graph->node_array = g_ptr_array_sized_new (node_array_size);

  return graph;
}
