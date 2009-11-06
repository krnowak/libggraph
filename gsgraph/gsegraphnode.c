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

#include "gsegraphnode.h"

#include "gsegraphedge.h"

/**
 * SECTION: gsegraphnode
 * @title: Simple edged graph nodes
 * @short_description: nodes holding user data.
 * @include: gsgraph/gsegraph.h
 * @see_also: #GSEGraphEdge
 *
 * Single node of connected graph. It contains data and an array of pointers to
 * edges to all its neighbours.
 *
 * To create a single node, use g_segraph_node_new().
 *
 * To check if two nodes are in the same graph, use
 * g_segraph_node_are_separate().
 *
 * To free a node, use g_segraph_node_free().
 *
 * <note>
 *   <para>
 *     Remember to always take care about data node holds. g_segraph_node_free()
 *     returns a pointer to freed node's data. Useful to free it, if applicable.
 *   </para>
 * </note>
 */

/* static function declarations. */

static gboolean
_g_segraph_node_separation_recurrent_check(GSEGraphNode* node,
                                           GSEGraphNode* other_node,
                                           GHashTable* visited_nodes,
                                           GHashTable* visited_edges);

/* public function definitions */

/**
 * g_segraph_node_new:
 * @data: data.
 *
 * Creates new separate graph node holding @data.
 *
 * Returns: newly created #GSEGraphNode.
 */
GSEGraphNode*
g_segraph_node_new(gpointer data)
{
  GSEGraphNode* graph;

  graph = g_slice_new(GSEGraphNode);
  graph->data = data;
  graph->edges = g_ptr_array_new();
  return graph;
}

/**
 * g_segraph_node_free:
 * @node: a node.
 *
 * Frees memory allocated to @node and returns a pointer to data of freed node.
 * It is up to programmist to handle bad edges. Running
 * g_segraph_edge_disconnect_node() or g_segraph_edge_disconnect() on all edges
 * connecting @node is a safe bet.
 *
 * Returns: pointer to @node's data.
 */
gpointer
g_segraph_node_free(GSEGraphNode* node)
{
  gpointer data;

  g_return_val_if_fail(node != NULL, NULL);

  data = node->data;
  g_ptr_array_free(node->edges, TRUE);
  g_slice_free(GSEGraphNode, node);
  return data;
}

/**
 * g_segraph_node_are_separate:
 * @node: first node.
 * @other_node: second node.
 *
 * Checks if @node and @other_node are separate.
 *
 * <note>
 *   <para>
 *     Performance hint: if you suspect two nodes being in two separate graphs
 *     and you are able to say which of graphs will have lower order, call this
 *     function with @node being in larger graph and @other_node being in
 *     smaller one.
 *   </para>
 * </note>
 *
 * Returns: %TRUE if there is no path between @node and @other_node.
 */
gboolean
g_segraph_node_are_separate(GSEGraphNode* node,
                            GSEGraphNode* other_node)
{
  GHashTable* visited_nodes;
  GHashTable* visited_edges;
  gboolean are_separate;

  g_return_val_if_fail(node != NULL, FALSE);
  g_return_val_if_fail(other_node != NULL, FALSE);

  visited_nodes = g_hash_table_new(NULL, NULL);
  visited_edges = g_hash_table_new(NULL, NULL);

  are_separate = _g_segraph_node_separation_recurrent_check(node, other_node,
                                                            visited_nodes,
                                                            visited_edges);

  g_hash_table_unref(visited_nodes);
  g_hash_table_unref(visited_edges);

  return are_separate;
}

/* static function definitions. */

/**
 * _g_segraph_node_separation_recurrent_check:
 * @node: a node.
 * @other_node: other node.
 * @visited_nodes: map of already visited nodes.
 * @visited_edges: map of already visited edges.
 *
 * Checks if @node and @other_node are same. If so - returns %FALSE, otherwise
 * it runs same check against @node and @other_node's neighbours, until it
 * happens or whole graph is traversed.
 *
 * Returns: %TRUE if @node and other_node are not the same, otherwise @FALSE.
 */
static gboolean
_g_segraph_node_separation_recurrent_check(GSEGraphNode* node,
                                           GSEGraphNode* other_node,
                                           GHashTable* visited_nodes,
                                           GHashTable* visited_edges)
{
  guint iter;

  if (node == other_node)
  {
    return FALSE;
  }

  g_hash_table_insert(visited_nodes, other_node, NULL);

  for (iter = 0; iter < other_node->edges->len; iter++)
  {
    GSEGraphEdge* edge;
    GSEGraphNode* temp_node;

    edge = g_ptr_array_index(other_node->edges, iter);
    if (g_hash_table_lookup_extended(visited_edges, edge, NULL, NULL))
    {
      continue;
    }
    g_hash_table_insert(visited_edges, edge, NULL);

    temp_node = g_segraph_edge_get_node(edge, other_node);
    if (!temp_node)
    {
      continue;
    }

    if (g_hash_table_lookup_extended(visited_nodes, temp_node, NULL, NULL))
    {
      continue;
    }

    if (!_g_segraph_node_separation_recurrent_check(node, temp_node,
                                                    visited_nodes,
                                                    visited_edges))
    {
      return FALSE;
    }
  }
  return TRUE;
}
