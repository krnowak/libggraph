/*
 * Copyright (C) 2009 Krzesimir Nowak <qdlacz@gmail.com>
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

#include "gsegraphwhole.h"

#include "gsegraphedge.h"
#include "gsegraphnode.h"

/**
 * SECTION: gsegraphwhole
 * @title: Whole graph
 * @short_description: convenient structure holding nodes and edges in arrays.
 * @include: gsgraph/gsegraph.h
 * @see_also: #GSEGraphNode, #GSEGraphEdge, #GSEGraphDataTriplet,
 * #GSEGraphTraverseType
 *
 * Convenient structure holding all nodes and all edges in arrays for easy
 * processing them with simply iterating the arrays. This wrapper is sort of
 * semi-snapshot of a graph - that is, if after creation of #GSEGraphWhole new
 * node or edge is added to graph, this snapshot is out of date, but changes
 * inside nodes already inside the wrapper are reflected in wrapper too.
 *
 * Wrapper can be created using two traversing algorithms to add nodes and edges
 * to arrays inside the wrapper or by specifing data triplets.
 *
 * To create a structure, use g_segraph_whole_new(),
 * g_segraph_whole_new_from_node(), g_segraph_whole_new_from_edge(),
 * g_segraph_whole_new_only_nodes() or g_segraph_whole_new_only_edges().
 *
 * To make a copy of graph, use g_segraph_whole_copy() or
 * g_segraph_whole_copy_deep().
 *
 * To free a wrapper or whole graph too, use g_segraph_whole_free().
 *
 * To find a node or edge, use g_segraph_whole_find_node_custom() or
 * g_segraph_whole_find_edge_custom().
 *
 * To process nodes or edges, use g_segraph_whole_foreach_node() or
 * g_segraph_whole_foreach_edge().
 *
 * <note>
 *   <para>
 *     Remember to always take care about data graph holds. When freeing whole
 *     graph, not only wrapper, it is good to free data in nodes and edges to
 *     avoid memory leaks.
 *   </para>
 * </note>
 */

/* internal types. */

/**
 * GSEGraphConstructFlags:
 * @G_SEGRAPH_NONE: no nodes were created.
 * @G_SEGRAPH_FIRST: node containing first data was created.
 * @G_SEGRAPH_SECOND: node containing second data was created.
 * @G_SEGRAPH_BOTH: both nodes were created.
 *
 * Internal enum for g_segraph_whole_new() needs. Describes which nodes were
 * created.
 */
typedef enum
{
  G_SEGRAPH_NONE = 0,
  G_SEGRAPH_FIRST = 1 << 0,
  G_SEGRAPH_SECOND = 1 << 1,
  G_SEGRAPH_BOTH = G_SEGRAPH_FIRST | G_SEGRAPH_SECOND
} GSEGraphConstructFlags;

/* static function declarations. */

static void
_g_segraph_whole_append_DFS(GSEGraphNode* node,
                            GPtrArray* node_array,
                            GHashTable* visited_nodes,
                            GPtrArray* edge_array,
                            GHashTable* visited_edges);

static void
_g_segraph_whole_append_BFS(GSEGraphNode* node,
                            GPtrArray* node_array,
                            GHashTable* visited_nodes,
                            GPtrArray* edge_array,
                            GHashTable* visited_edges);

static GSEGraphWhole*
_g_segraph_whole_new_from_node_general(GSEGraphNode* node,
                                       gboolean put_nodes,
                                       gboolean put_edges,
                                       GSEGraphTraverseType traverse_type);

static GSEGraphWhole*
_g_segraph_whole_new_from_edge_general(GSEGraphEdge* edge,
                                       gboolean put_nodes,
                                       gboolean put_edges,
                                       GSEGraphTraverseType traverse_type);

static GSEGraphWhole*
_g_segraph_whole_copy_general(GSEGraphWhole* graph,
                              GCopyFunc node_data_copy_func,
                              gpointer node_user_data,
                              GCopyFunc edge_data_copy_func,
                              gpointer edge_user_data);

static GSEGraphWhole*
_g_segraph_whole_new_blank(guint node_array_size,
                           guint edge_array_size,
                           gboolean create_node_array,
                           gboolean create_edge_array);

/* public function definitions. */

/**
 * g_segraph_whole_new:
 * @data_triplets: array of data triplets.
 * @count: length of @data_pairs.
 *
 * Creates a graph from passed data triplets. Resulting construction can be
 * several separate graphs, so an array of #GSEGraphWhole is returned. Also, if
 * both first and second #GSEGraphDataTriplet member are %NULL, then this
 * triplet is omitted in creation. If @count is 0, it is assumed that
 * @data_triplets is %NULL terminated array. If one of first and second members
 * of #GSEGraphDataTriplet is %NULL, then only one node with not %NULL data and
 * a half-edge holding whatever data was passed as a third member of triplet
 * will be created. For performance reasons it would be good if in second and
 * later pairs one of node have data describing already created node.
 * Lets assume we want to create this graph:
 * <informalexample>
 *   <programlisting>
 *     cA--B-
 *      |
 *     -C--D
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
 *       A, A
 *     </para>
 *   </listitem>
 *   <listitem>
 *     <para>
 *       B, %NULL
 *     </para>
 *   </listitem>
 *   <listitem>
 *     <para>
 *       A, C
 *     </para>
 *   </listitem>
 *   <listitem>
 *     <para>
 *       C, %NULL
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
 * Swaping fourth and sixth pair will cause creation of a separate graph while
 * processing fourth pair and checking if two graphs are one when processing
 * sixth pair.
 *
 * Returns: array of newly created separate graphs or %NULL if no nodes were
 * created.
 */
GPtrArray*
g_segraph_whole_new(GSEGraphDataTriplet** data_triplets,
                    guint count)
{
  GPtrArray* separate_graphs;
  GHashTable* data_to_nodes;
  GHashTable* nodes_to_wholes;
  guint iter;

  g_return_val_if_fail(data_triplets != NULL, NULL);

  if (!count)
  {
    while (data_triplets[count])
    {
      count++;
    }
  }

  if (!count)
  {
    return NULL;
  }

  separate_graphs = g_ptr_array_new();
  data_to_nodes = g_hash_table_new(NULL, NULL);
  nodes_to_wholes = g_hash_table_new(NULL, NULL);
  for (iter = 0; iter < count; iter++)
  {
    GSEGraphDataTriplet* data_triplet;

    data_triplet = data_triplets[iter];
    if (g_segraph_data_triplet_is_valid(data_triplet))
    {
      GSEGraphNode* first_node;
      GSEGraphNode* second_node;
      GSEGraphEdge* edge;
      GSEGraphConstructFlags created;

      created = G_SEGRAPH_NONE;
      if (data_triplet->first)
      {
        if (!g_hash_table_lookup_extended(data_to_nodes, data_triplet->first, NULL,
                                          (gpointer*)&first_node))
        {
          first_node = g_segraph_node_new(data_triplet->first);
          g_hash_table_insert(data_to_nodes, data_triplet->first, first_node);
          created |= G_SEGRAPH_FIRST;
        }
      }
      else
      {
        first_node = NULL;
      }

      if (data_triplet->second)
      {
        if (!g_hash_table_lookup_extended(data_to_nodes, data_triplet->second, NULL,
                                          (gpointer*)&second_node))
        {
          second_node = g_segraph_node_new(data_triplet->second);
          g_hash_table_insert(data_to_nodes, data_triplet->second, second_node);
          created |= G_SEGRAPH_SECOND;
        }
      }
      else
      {
        second_node = NULL;
      }

      edge = g_segraph_edge_new(data_triplet->edge);
      g_segraph_edge_connect_nodes(edge, first_node, second_node);
      switch (created)
      {
        case G_SEGRAPH_NONE:
        {
          /* no nodes were created, so they can join two separate graphs. */
          #define JOIN_COUNT 2
          guint which;
          GSEGraphWhole* joined_graphs[JOIN_COUNT];
          GSEGraphNode* check_nodes[JOIN_COUNT];

          /* if loop was created, then no graph join occurs. */
          /* if half-edge was created, then no graph join occurs. */
          if ((first_node == second_node) || !first_node || !second_node)
          {
            break;
          }

          check_nodes[0] = edge->first;
          check_nodes[1] = edge->second;
          for (which = 0; which < JOIN_COUNT; which++)
          {
            guint iter2;

            for (iter2 = 0; iter2 < separate_graphs->len; iter2++)
            {
              GSEGraphWhole* graph;
              guint iter3;
              gboolean hit;

              graph = g_ptr_array_index(separate_graphs, iter2);
              hit = FALSE;
              for (iter3 = 0; iter3 < graph->node_array->len; iter3++)
              {
                GSEGraphNode* temp_node;

                temp_node = g_ptr_array_index(graph->node_array, iter3);
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

            for (iter2 = 0; iter2 < joined_graphs[1]->node_array->len; iter2++)
            {
              GSEGraphNode* temp_node;

              temp_node = g_ptr_array_index(joined_graphs[1]->node_array,
                                                     iter2);
              if (temp_node != check_nodes[1])
              {
                g_ptr_array_add(joined_graphs[0]->node_array,
                                         temp_node);
                g_hash_table_insert(nodes_to_wholes, temp_node,
                                    joined_graphs[0]);
              }
            }

            for (iter2 = 0; iter2 < joined_graphs[1]->edge_array->len; iter2++)
            {
              GSEGraphEdge* temp_edge;

              temp_edge = g_ptr_array_index(joined_graphs[1]->edge_array,
                                                     iter2);
              g_ptr_array_add(joined_graphs[0]->edge_array, temp_edge);
            }
            g_ptr_array_remove_fast(separate_graphs,
                                              joined_graphs[1]);
          }
          g_ptr_array_add(joined_graphs[0]->edge_array, edge);
          #undef JOIN_COUNT
          break;
        }
        case G_SEGRAPH_FIRST:
        {
          /* first node was created, so maybe it will belong to existing graph. */
          GSEGraphWhole* temp_graph;

          /* creating self-connected node means it creates separate graph. */
          /* creating half-edged node means it creates separate graph. */
          if ((first_node == second_node) || !second_node)
          {
            temp_graph = _g_segraph_whole_new_blank(1, 1, TRUE, TRUE);
            g_ptr_array_add(separate_graphs, temp_graph);
          }
          else
          {
            temp_graph = g_hash_table_lookup(nodes_to_wholes, second_node);
          }
          g_ptr_array_add(temp_graph->node_array, first_node);
          g_ptr_array_add(temp_graph->edge_array, edge);
          g_hash_table_insert(nodes_to_wholes, first_node, temp_graph);
          break;
        }
        case G_SEGRAPH_SECOND:
        {
          /* second node was created, so maybe it will belong to existing graph. */
          GSEGraphWhole* temp_graph;

          /* creating self-connected node means it creates separate graph. */
          /* creating half-edged node means it creates separate graph. */
          if ((first_node == second_node) || !first_node)
          {
            temp_graph = _g_segraph_whole_new_blank(1, 1, TRUE, TRUE);
            g_ptr_array_add(separate_graphs, temp_graph);
          }
          else
          {
            temp_graph = g_hash_table_lookup(nodes_to_wholes, first_node);
          }
          g_ptr_array_add(temp_graph->node_array, second_node);
          g_ptr_array_add(temp_graph->edge_array, edge);
          g_hash_table_insert(nodes_to_wholes, second_node, temp_graph);
          break;
        }
        case G_SEGRAPH_BOTH:
        {
          /* if both nodes were created then they create separate graph. */
          GSEGraphWhole* temp_graph;

          temp_graph = _g_segraph_whole_new_blank(2, 1, TRUE, TRUE);
          g_ptr_array_add(temp_graph->node_array, first_node);
          g_ptr_array_add(temp_graph->node_array, second_node);
          g_ptr_array_add(temp_graph->edge_array, edge);
          g_ptr_array_add(separate_graphs, temp_graph);
          g_hash_table_insert(nodes_to_wholes, first_node, temp_graph);
          g_hash_table_insert(nodes_to_wholes, second_node, temp_graph);
          break;
        }
      }
    }
  }
  g_hash_table_unref(data_to_nodes);
  g_hash_table_unref(nodes_to_wholes);
  if (!separate_graphs->len)
  {
    g_ptr_array_free(separate_graphs, TRUE);
    separate_graphs = NULL;
  }
  return separate_graphs;
}

/**
 * g_segraph_whole_new_from_node:
 * @node: a node in graph.
 * @traverse_type: which traversing algorithm to use.
 *
 * Creates new #GSEGraphWhole with arrays holding nodes and edges of a graph
 * containing @node in order specified by @traverse_type.
 *
 * Returns: new #GSEGraphWhole.
 */
GSEGraphWhole*
g_segraph_whole_new_from_node(GSEGraphNode* node,
                              GSEGraphTraverseType traverse_type)
{
  g_return_val_if_fail(node != NULL, NULL);

  return _g_segraph_whole_new_from_node_general(node, TRUE, TRUE,
                                                traverse_type);
}

/**
 * g_segraph_whole_new_from_edge:
 * @edge: an edge in graph.
 * @traverse_type: which traversing algorithm to use.
 *
 * Creates new #GSEGraphWhole with arrays holding nodes and edges of a graph
 * containing @edge in order specified by @traverse_type.
 *
 * Returns: new #GSEGraphWhole.
 */
GSEGraphWhole*
g_segraph_whole_new_from_edge(GSEGraphEdge* edge,
                              GSEGraphTraverseType traverse_type)
{
  g_return_val_if_fail(edge != NULL, NULL);

  return _g_segraph_whole_new_from_edge_general(edge, TRUE, TRUE,
                                                traverse_type);
}

/**
 * g_segraph_whole_new_only_nodes:
 * @node: a node in graph.
 * @traverse_type: which traversing algorithm to use.
 *
 * Creates new #GSEGraphWhole with array holding nodes of a graph containing
 * @node in order specified by @traverse_type. Array holding edges is %NULL.
 *
 * Returns: new #GSEGraphWhole.
 */
GSEGraphWhole*
g_segraph_whole_new_only_nodes(GSEGraphNode* node,
                               GSEGraphTraverseType traverse_type)
{
  g_return_val_if_fail(node != NULL, NULL);

  return _g_segraph_whole_new_from_node_general(node, TRUE, FALSE,
                                                traverse_type);
}

/**
 * g_segraph_whole_new_only_edges:
 * @edge: an edge in graph.
 * @traverse_type: which traversing algorithm to use.
 *
 * Creates new #GSEGraphWhole with array holding edges of a graph containing
 * @edge in order specified by @traverse_type. Array holding nodes is %NULL.
 *
 * Returns: new #GSEGraphWhole.
 */
GSEGraphWhole*
g_segraph_whole_new_only_edges(GSEGraphEdge* edge,
                               GSEGraphTraverseType traverse_type)
{
  g_return_val_if_fail(edge != NULL, NULL);

  return _g_segraph_whole_new_from_edge_general(edge, FALSE, TRUE,
                                                traverse_type);
}

/**
 * g_segraph_whole_copy:
 * @graph: a graph to be copied.
 *
 * Does a shallow copy of @graph. This means that original graph and its copy
 * shares the data, so when freeing both graphs you should be careful not to
 * free same data twice. @graph's members must not be %NULL.
 *
 * Returns: A copy of @graph.
 */
GSEGraphWhole*
g_segraph_whole_copy(GSEGraphWhole* graph)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail((graph->node_array != NULL) && (graph->edge_array != NULL), NULL);

  return _g_segraph_whole_copy_general(graph, NULL, NULL, NULL, NULL);
}

/**
 * g_segraph_whole_copy_deep:
 * @graph: a graph to be copied.
 * @node_data_copy_func: function copying data in nodes.
 * @node_user_data: data passed to node data copying function.
 * @edge_data_copy_func: function copying data in edges.
 * @edge_user_data: data passed to edge data copying function.
 *
 * Does a deep copy of @graph. Each node data in @graph copy and each edge data
 * are duplicated using passed functions. @graph's members,
 * @node_data_copy_func and @edge_data_copy_func must not be %NULL.
 *
 * Returns: A copy of @graph.
 */
GSEGraphWhole*
g_segraph_whole_copy_deep(GSEGraphWhole* graph,
                          GCopyFunc node_data_copy_func,
                          gpointer node_user_data,
                          GCopyFunc edge_data_copy_func,
                          gpointer edge_user_data)
{
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail((graph->node_array != NULL) && (graph->edge_array != NULL), NULL);
  g_return_val_if_fail((node_data_copy_func != NULL) || (edge_data_copy_func != NULL), NULL);

  return _g_segraph_whole_copy_general(graph, node_data_copy_func,
                                       node_user_data, edge_data_copy_func,
                                       edge_user_data);
}

/**
 * g_segraph_whole_free:
 * @graph: #GSEGraphWhole to free.
 * @deep_free: whether to do a deep free.
 *
 * Frees memory allocated to @graph. If @deep_free is %TRUE then all nodes and
 * edges are also freed. If @deep_free is %TRUE then node and edge arrays in
 * @graph cannot be %NULL - this means that %TRUE can be passed to this function
 * only when @graph was created with g_segraph_whole_new_from_node(),
 * g_segraph_whole_new_from_edge() or g_segraph_whole_new().
 */
void
g_segraph_whole_free(GSEGraphWhole* graph,
                     gboolean deep_free)
{
  g_return_if_fail(graph != NULL);
  g_return_if_fail(deep_free == FALSE || (graph->node_array != NULL && graph->edge_array != NULL));

  if (deep_free)
  {
    guint iter;

    for (iter = 0; iter < graph->node_array->len; iter++)
    {
      GSEGraphNode* node;

      node = g_ptr_array_index(graph->node_array, iter);
      g_segraph_node_free(node);
    }
    g_ptr_array_free(graph->node_array, TRUE);

    for (iter = 0; iter < graph->edge_array->len; iter++)
    {
      GSEGraphEdge* edge;

      edge = g_ptr_array_index(graph->edge_array, iter);
      g_segraph_edge_free(edge);
    }
    g_ptr_array_free(graph->edge_array, TRUE);
  }
  else
  {
    if (graph->node_array)
    {
      g_ptr_array_free(graph->node_array, TRUE);
    }

    if (graph->edge_array)
    {
      g_ptr_array_free(graph->edge_array, TRUE);
    }
  }

  g_slice_free(GSEGraphWhole, graph);
}

/**
 * g_segraph_whole_get_order:
 * @graph: a graph.
 *
 * Gets @graph's order, that is - number of nodes in graph.
 *
 * Returns: number of nodes in graph.
 */
guint
g_segraph_whole_get_order(GSEGraphWhole* graph)
{
  g_return_val_if_fail(graph != NULL, 0);
  g_return_val_if_fail(graph->node_array != NULL, 0);

  return graph->node_array->len;
}

/**
 * g_segraph_whole_get_size:
 * @graph: a graph.
 *
 * Gets @graph's size, that is - number of edges in graph.
 *
 * Returns: number of edges in graph.
 */
guint
g_segraph_whole_get_size(GSEGraphWhole* graph)
{
  g_return_val_if_fail(graph != NULL, 0);
  g_return_val_if_fail(graph->edge_array != NULL, 0);

  return graph->edge_array->len;
}

/**
 * g_segraph_whole_foreach_node:
 * @graph: a graph.
 * @func: the function to call with each element's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each node's data in @graph.
 */
void
g_segraph_whole_foreach_node(GSEGraphWhole* graph,
                             GFunc func,
                             gpointer user_data)
{
  guint iter;

  g_return_if_fail(graph != NULL);
  g_return_if_fail(graph->node_array != NULL);
  g_return_if_fail(func != NULL);

  for (iter = 0; iter < graph->node_array->len; iter++)
  {
    GSEGraphNode* node = g_ptr_array_index(graph->node_array, iter);
    (*func)(node, user_data);
  }
}

/**
 * g_segraph_whole_find_node_custom:
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
 * Returns: found #GSEGraphNode or %NULL if there was no such node.
 */
GSEGraphNode*
g_segraph_whole_find_node_custom(GSEGraphWhole* graph,
                                 gpointer user_data,
                                 GEqualFunc func)
{
  guint iter;

  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(graph->node_array != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);

  for (iter = 0; iter < graph->node_array->len; iter++)
  {
    GSEGraphNode* node = g_ptr_array_index(graph->node_array, iter);
    if ((*func)(node, user_data))
    {
      return node;
    }
  }
  return NULL;
}

/**
 * g_segraph_whole_foreach_edge:
 * @graph: a graph.
 * @func: the function to call with each element's data.
 * @user_data: data passed to @func.
 *
 * Calls @func for each edge's data in @graph.
 */
void
g_segraph_whole_foreach_edge(GSEGraphWhole* graph,
                             GFunc func,
                             gpointer user_data)
{
  guint iter;

  g_return_if_fail(graph != NULL);
  g_return_if_fail(graph->edge_array != NULL);
  g_return_if_fail(func != NULL);

  for (iter = 0; iter < graph->edge_array->len; iter++)
  {
    GSEGraphEdge* edge = g_ptr_array_index(graph->edge_array, iter);
    (*func)(edge, user_data);
  }
}

/**
 * g_segraph_whole_find_edge_custom:
 * @graph: a graph.
 * @user_data: user data passed to @func.
 * @func: the function to call for each node.
 *
 * Finds first element in a @graph, using a supplied function to find the
 * desired element. It iterates over the edge array, calling the given function
 * which should return %TRUE when the desired element is found. The function
 * takes two arguments, the edge from the iterated array as the first argument
 * and the given @user_data as second. If an element is found it is returned.
 *
 * Returns: found #GSEGraphEdge or %NULL if there was no such edge.
 */
GSEGraphEdge*
g_segraph_whole_find_edge_custom(GSEGraphWhole* graph,
                                 gpointer user_data,
                                 GEqualFunc func)
{
  guint iter;

  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(graph->edge_array != NULL, NULL);
  g_return_val_if_fail(func != NULL, NULL);

  for (iter = 0; iter < graph->edge_array->len; iter++)
  {
    GSEGraphEdge* edge = g_ptr_array_index(graph->edge_array, iter);
    if ((*func)(edge, user_data))
    {
      return edge;
    }
  }
  return NULL;
}

/* static function definitions. */

/**
 * _g_segraph_whole_append_DFS:
 * @node: a node, which will be put into arrays with its edges.
 * @node_array: array of all nodes in graph.
 * @visited_nodes: a map of already visited nodes.
 * @edge_array: array of all edges in graph.
 * @visited_edges: a map of already visited edges.
 *
 * Checks all nodes and edges and put them into arrays using depth first search
 * algorithm.
 */
static void
_g_segraph_whole_append_DFS(GSEGraphNode* node,
                            GPtrArray* node_array,
                            GHashTable* visited_nodes,
                            GPtrArray* edge_array,
                            GHashTable* visited_edges)
{
  guint iter;
  GPtrArray* n_e;

  if (g_hash_table_lookup_extended(visited_nodes, node, NULL, NULL))
  {
    return;
  }

  g_hash_table_insert(visited_nodes, node, NULL);
  if (node_array)
  {
    g_ptr_array_add(node_array, node);
  }
  n_e = node->edges;
  for (iter = 0; iter < n_e->len; iter++)
  {
    GSEGraphNode* other_node;
    GSEGraphEdge* edge;

    edge = g_ptr_array_index(n_e, iter);
    if (g_hash_table_lookup_extended(visited_edges, edge, NULL, NULL))
    {
      continue;
    }
    g_hash_table_insert(visited_edges, edge, NULL);
    if (edge_array)
    {
      g_ptr_array_add(edge_array, edge);
    }
    other_node = g_segraph_edge_get_node(edge, node);
    if (!other_node)
    {
      continue;
    }
    _g_segraph_whole_append_DFS(other_node, node_array, visited_nodes,
                                edge_array, visited_edges);
  }
}

/**
 * _g_segraph_whole_append_DFS:
 * @node: a node, which will be put into arrays with its edges.
 * @node_array: array of all nodes in graph.
 * @visited_nodes: a map of already visited nodes.
 * @edge_array: array of all edges in graph.
 * @visited_edges: a map of already visited edges.
 *
 * Checks all nodes and edges and put them into arrays using breadth first
 * search algorithm.
 */
static void
_g_segraph_whole_append_BFS(GSEGraphNode* node,
                            GPtrArray* node_array,
                            GHashTable* visited_nodes,
                            GPtrArray* edge_array,
                            GHashTable* visited_edges)
{
  GQueue* queue;

  queue = g_queue_new();

  g_hash_table_insert(visited_nodes, node, NULL);
  g_queue_push_tail(queue, node);
  if (node_array)
  {
    g_ptr_array_add(node_array, node);
  }

  while (!g_queue_is_empty(queue))
  {
    GSEGraphNode* temp_node;
    guint iter;

    temp_node = g_queue_pop_head(queue);

    for (iter = 0; iter < temp_node->edges->len; iter++)
    {
      GSEGraphEdge* edge;
      GSEGraphNode* other_node;

      edge = g_ptr_array_index(temp_node->edges, iter);

      if (g_hash_table_lookup_extended(visited_edges, edge, NULL, NULL))
      {
        continue;
      }
      g_hash_table_insert(visited_edges, edge, NULL);
      if (edge_array)
      {
        g_ptr_array_add(edge_array, edge);
      }

      other_node = g_segraph_edge_get_node(edge, temp_node);
      if (!other_node)
      {
        continue;
      }
      if (g_hash_table_lookup_extended(visited_nodes, other_node, NULL, NULL))
      {
        continue;
      }

      g_hash_table_insert(visited_nodes, other_node, NULL);
      g_queue_push_tail(queue, other_node);
      if (node_array)
      {
        g_ptr_array_add(node_array, other_node);
      }
    }
  }

  g_queue_free(queue);
}

/**
 * _g_segraph_whole_new_from_node_general:
 * @node: a node being a part of a graph.
 * @put_nodes: whether to put nodes in array.
 * @put_edges: whether to put edges in array.
 * @traverse_type: which traversing algorithm to use.
 *
 * General function creating a #GSEGraphWhole from given @node using traversing
 * algorithm denoted by @traverse_type.
 *
 * Returns: new #GSEGraphWhole.
 */
static GSEGraphWhole*
_g_segraph_whole_new_from_node_general(GSEGraphNode* node,
                                       gboolean put_nodes,
                                       gboolean put_edges,
                                       GSEGraphTraverseType traverse_type)
{
  typedef void (*GraphSearchFunc)(GSEGraphNode* node,
                                  GPtrArray* node_array,
                                  GHashTable* visited_nodes,
                                  GPtrArray* edge_array,
                                  GHashTable* visited_edges);

  GSEGraphWhole* graph;
  GHashTable* visited_nodes;
  GHashTable* visited_edges;
  GraphSearchFunc gsfunc;

  switch (traverse_type)
  {
    case G_SEGRAPH_TRAVERSE_BFS:
    {
      gsfunc = _g_segraph_whole_append_BFS;
      break;
    }
    case G_SEGRAPH_TRAVERSE_DFS:
    {
      gsfunc = _g_segraph_whole_append_DFS;
      break;
    }
    default:
    {
      g_return_val_if_reached(NULL);
    }
  }

  graph = _g_segraph_whole_new_blank(0, 0, put_nodes, put_edges);

  visited_nodes = g_hash_table_new(NULL, NULL);
  visited_edges = g_hash_table_new(NULL, NULL);

  (*gsfunc)(node, graph->node_array, visited_nodes, graph->edge_array,
            visited_edges);

  g_hash_table_unref(visited_nodes);
  g_hash_table_unref(visited_edges);

  return graph;
}

/**
 * _g_segraph_whole_new_from_edge_general:
 * @edge: a edge being a part of a graph.
 * @put_nodes: whether to put nodes in array.
 * @put_edges: whether to put edges in array.
 * @traverse_type: which traversing algorithm to use.
 *
 * General function creating a #GSEGraphWhole from given @edge using traversing
 * algorithm denoted by @traverse_type.
 *
 * Returns: new #GSEGraphWhole.
 */
static GSEGraphWhole*
_g_segraph_whole_new_from_edge_general(GSEGraphEdge* edge,
                                       gboolean put_nodes,
                                       gboolean put_edges,
                                       GSEGraphTraverseType traverse_type)
{
  if (edge->first)
  {
    return _g_segraph_whole_new_from_node_general(edge->first, put_nodes,
                                                  put_edges, traverse_type);
  }
  else if (edge->second)
  {
    return _g_segraph_whole_new_from_node_general(edge->second, put_nodes,
                                                  put_edges, traverse_type);
  }
  else
  {
    GSEGraphWhole* graph;

    graph = _g_segraph_whole_new_blank(0, 1, put_nodes, put_edges);

    if (put_edges)
    {
      g_ptr_array_add(graph->edge_array, edge);
    }

    return graph;
  }
}

/**
 * _g_segraph_whole_copy_general:
 * @graph: a graph to be copied.
 * @node_data_copy_func: function copying data in nodes.
 * @node_user_data: data passed to node data copying function.
 * @edge_data_copy_func: function copying data in edges.
 * @edge_user_data: data passed to edge data copying function.
 *
 * Does a copy of @graph. Each node data in @graph copy and each edge data
 * are duplicated using passed functions. If @node_data_copy_func or
 * @edge_data_copy_func are NULL, then shallow copy of data is done.
 *
 * Returns: A copy of @graph.
 */
static GSEGraphWhole*
_g_segraph_whole_copy_general(GSEGraphWhole* graph,
                              GCopyFunc node_data_copy_func,
                              gpointer node_user_data,
                              GCopyFunc edge_data_copy_func,
                              gpointer edge_user_data)
{
  GHashTable* nodes_to_dups;
  GHashTable* edges_to_dups;
  GSEGraphWhole* dup_graph;
  guint iter;

  nodes_to_dups = g_hash_table_new(NULL, NULL);
  edges_to_dups = g_hash_table_new(NULL, NULL);
  dup_graph = _g_segraph_whole_new_blank(graph->node_array->len,
                                         graph->edge_array->len, TRUE, TRUE);

  for (iter = 0; iter < graph->node_array->len; iter++)
  {
    GSEGraphNode* node;
    GSEGraphNode* dup_node;

    node = g_ptr_array_index(graph->node_array, iter);
    dup_node = g_slice_new(GSEGraphNode);

    if (node_data_copy_func)
    {
      dup_node->data = (*node_data_copy_func)(node->data, node_user_data);
    }
    else
    {
      dup_node->data = node->data;
    }

    g_hash_table_insert(nodes_to_dups, node, dup_node);
    g_ptr_array_add(dup_graph->node_array, dup_node);
  }

  for (iter = 0; iter < graph->edge_array->len; iter++)
  {
    GSEGraphEdge* edge;
    GSEGraphEdge* dup_edge;

    edge = g_ptr_array_index(graph->edge_array, iter);
    dup_edge = g_slice_new(GSEGraphEdge);
    dup_edge->first = g_hash_table_lookup(nodes_to_dups, edge->first);
    dup_edge->second = g_hash_table_lookup(nodes_to_dups, edge->second);

    if (edge_data_copy_func)
    {
      dup_edge->data = (*edge_data_copy_func)(edge->data, edge_user_data);
    }
    else
    {
      dup_edge->data = edge->data;
    }

    g_hash_table_insert(edges_to_dups, edge, dup_edge);
    g_ptr_array_add(dup_graph->edge_array, dup_edge);
  }

  for (iter = 0; iter < graph->node_array->len; iter++)
  {
    GSEGraphNode* node;
    GSEGraphNode* dup_node;
    GPtrArray* dup_edges;
    guint iter2;

    node = g_ptr_array_index(graph->node_array, iter);
    dup_node = g_ptr_array_index(dup_graph->node_array, iter);
    dup_edges = g_ptr_array_sized_new(node->edges->len);

    for (iter2 = 0; iter2 < node->edges->len; iter2++)
    {
      GSEGraphEdge* edge = g_ptr_array_index(node->edges, iter2);
      GSEGraphEdge* dup_edge = g_hash_table_lookup(edges_to_dups, edge);
      g_ptr_array_add(dup_edges, dup_edge);
    }
    dup_node->edges = dup_edges;
  }

  g_hash_table_unref(nodes_to_dups);
  g_hash_table_unref(edges_to_dups);
  return dup_graph;
}

/**
 * _g_segraph_whole_new_blank:
 * @node_array_size: size of node array.
 * @edge_array_size: size of edge array.
 * @create_node_array: whether to create node array or make it %NULL.
 * @create_edge_array: whether to create edge array or make it %NULL.
 *
 * Constructs #GSEGraphWhole with two empty or %NULL arrays with given sizes.
 *
 * Returns: new #GSEGraphWhole.
 */
static GSEGraphWhole*
_g_segraph_whole_new_blank(guint node_array_size,
                           guint edge_array_size,
                           gboolean create_node_array,
                           gboolean create_edge_array)
{
  GSEGraphWhole* graph;

  graph = g_slice_new(GSEGraphWhole);

  if (create_node_array)
  {
    graph->node_array = g_ptr_array_sized_new(node_array_size);
  }
  else
  {
    graph->node_array = NULL;
  }

  if (create_edge_array)
  {
    graph->edge_array = g_ptr_array_sized_new(edge_array_size);
  }
  else
  {
    graph->edge_array = NULL;
  }

  return graph;
}
