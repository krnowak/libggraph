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

#include "gsgraphnode.h"

/**
 * SECTION: gsgraphnode
 * @title: Simple graph nodes
 * @short_description: simple graph nodes.
 * @include: gsgraph/gsgraph.h
 *
 * Single node of simple graph. It contains data and an array of pointers to all
 * it's neighbours. If you want to have multiple edges, loops, data on edges,
 * half-edges - use #GSEGraphNode.
 *
 * To create a single node, use g_sgraph_node_new().
 *
 * To connect two nodes, use g_sgraph_node_connect().
 *
 * To disconnect two nodes, use g_sgraph_node_disconnect().
 *
 * To free a node, use g_sgraph_node_free().
 *
 * To check if two nodes are in separate graphs, use g_sgraph_are_separate().
 *
 * <note>
 *   <para>
 *     Remember to always take care about data node holds.
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
_g_sgraph_node_connection_check(GSGraphNode* sgraph_node,
                                GSGraphNode* other_sgraph_node,
                                GHashTable* visited_nodes);

/* function definitions */

/**
 * g_sgraph_node_new:
 * @data: data.
 *
 * Creates new node holding @data.
 *
 * Returns: newly created node.
 */
GSGraphNode*
g_sgraph_node_new(gpointer data)
{
  GSGraphNode* node;

  node = g_slice_new(GSGraphNode);
  node->data = data;
  node->neighbours = g_ptr_array_new();
  return node;
}

/**
 * g_sgraph_node_free:
 * @node: a node.
 *
 * Frees memory allocated to @node. It is recommended to call
 * g_sgraph_node_disconnect() and g_sgraph_node_are_separate() for all @node's
 * neighbours.
 *
 * Returns: data of freed node.
 */
gpointer
g_sgraph_node_free(GSGraphNode* node)
{
  gpointer data;

  g_return_val_if_fail(node != NULL, NULL);

  data = node->data;

  g_ptr_array_free(node->neighbours, TRUE);
  g_slice_free(GSGraphNode, node);

  return data;
}

/**
 * g_sgraph_node_connect:
 * @node: a node.
 * @other_node: other @node.
 *
 * Creates a connection between @node and @other_node. If connection already
 * existed, it does nothing.
 */
void
g_sgraph_node_connect(GSGraphNode* node,
                      GSGraphNode* other_node)
{
  guint iter;

  g_return_if_fail(node != NULL);
  g_return_if_fail(other_node != NULL);
  g_return_if_fail(node != other_node);

  for (iter = 0; iter < node->neighbours->len; iter++)
  {
    GSGraphNode* temp_node;

    temp_node = g_ptr_array_index(node->neighbours, iter);
    if (temp_node == other_node)
    {
      return;
    }
  }
  g_ptr_array_add(node->neighbours, other_node);
  g_ptr_array_add(other_node->neighbours, node);
}

/**
 * g_sgraph_node_disconnect:
 * @node: a node.
 * @other_node: other node which is @node's neighbour.
 *
 * Removes connection from one node to another, so in effect they stop
 * being neighbours. If @node and @other_node are not neighbours then nothing
 * happens.
 */
void
g_sgraph_node_disconnect(GSGraphNode* node,
                         GSGraphNode* other_node)
{
  g_return_if_fail(node != NULL);
  g_return_if_fail(other_node != NULL);

  if (!g_ptr_array_remove(node->neighbours, other_node))
  {
    return;
  }
  g_ptr_array_remove(other_node->neighbours, node);
}

/**
 * g_sgraph_node_are_separate:
 * @node: a node.
 * @other_node: other node.
 *
 * This function checks if @node lies in another graph than @other_node.
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
 * Returns: %TRUE if @node and @other_node are in separate graphs.
 */
gboolean
g_sgraph_node_are_separate(GSGraphNode* node,
                           GSGraphNode* other_node)
{
  GHashTable* visited_nodes;
  gboolean not_connected;

  visited_nodes = g_hash_table_new(NULL, NULL);
  not_connected = _g_sgraph_node_connection_check(node, other_node,
                                                  visited_nodes);
  g_hash_table_unref(visited_nodes);
  return not_connected;
}

/* static function definitions */

/**
 * _g_sgraph_node_connection_check:
 * @node: starting node.
 * @other_node: other node.
 * @visited_nodes: #GHashTable holding information which nodes were already
 * visited.
 *
 * Checks if @node == @other_node. If not it checks @node's neighbours.
 *
 * Returns: %TRUE if @node != @other_node, otherwise %FALSE.
 */
static gboolean
_g_sgraph_node_connection_check(GSGraphNode* node,
                                GSGraphNode* other_node,
                                GHashTable* visited_nodes)
{
  guint iter;

  if (node == other_node)
  {
    return FALSE;
  }
  if (g_hash_table_lookup_extended(visited_nodes, other_node, NULL, NULL))
  {
    return TRUE;
  }
  g_hash_table_insert(visited_nodes, other_node, NULL);

  for (iter = 0; iter < other_node->neighbours->len; iter++)
  {
    GSGraphNode* temp_node = g_ptr_array_index(other_node->neighbours, iter);
    if (!_g_sgraph_node_connection_check(node, temp_node, visited_nodes))
    {
      return FALSE;
    }
  }
  return TRUE;
}
