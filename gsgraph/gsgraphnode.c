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
_g_sgraph_node_recurrent_connection_check(GSGraphNode* sgraph_node,
                                          GSGraphNode* other_sgraph_node,
                                          GHashTable* visited_nodes);

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
  sgraph_node->neighbours = g_ptr_array_new();
  return sgraph_node;
}

/**
 * g_sgraph_node_free:
 * @sgraph_node: a graph.
 *
 * Frees memory allocated to @sgraph_node. It is recommended to call
 * g_sgraph_node_disconnect() and g_sgraph_node_are_separate() for all
 * @sgraph_node's neighbours.
 *
 * Returns: data of freed node.
 */
gpointer
g_sgraph_node_free(GSGraphNode* sgraph_node)
{
  gpointer data;

  g_return_val_if_fail(sgraph_node != NULL, NULL);

  data = sgraph_node->data;

  g_ptr_array_free(sgraph_node->neighbours, TRUE);
  g_slice_free(GSGraphNode, sgraph_node);

  return data;
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

  g_return_if_fail(sgraph_node != NULL);
  g_return_if_fail(other_sgraph_node != NULL);
  g_return_if_fail(sgraph_node != other_sgraph_node);

  for (iter = 0; iter < sgraph_node->neighbours->len; iter++)
  {
    GSGraphNode* node = g_ptr_array_index(sgraph_node->neighbours, iter);
    if (node == other_sgraph_node)
    {
      return;
    }
  }
  g_ptr_array_add(sgraph_node->neighbours, other_sgraph_node);
  g_ptr_array_add(other_sgraph_node->neighbours, sgraph_node);
}

/**
 * g_sgraph_node_disconnect:
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
void
g_sgraph_node_disconnect(GSGraphNode* sgraph_node,
                         GSGraphNode* other_sgraph_node)
{
  g_return_val_if_fail(sgraph_node != NULL, FALSE);
  g_return_val_if_fail(other_sgraph_node != NULL, FALSE);

  if (!g_ptr_array_remove(sgraph_node->neighbours, other_sgraph_node))
  {
    return;
  }
  g_ptr_array_remove(other_sgraph_node->neighbours, sgraph_node);
}

/**
 * g_sgraph_node_are_separate:
 * @sgraph_node: starting node.
 * @other_sgraph_node: node for which we want to check if it has series of
 * connections to @sgraph_node.
 *
 * This function checks if there is no series of connections between
 * @sgraph_node and @other_sgraph_node.
 *
 * Returns: %TRUE if @sgraph_node and @other_sgraph_node are in separate graphs.
 */
gboolean
g_sgraph_node_are_separate(GSGraphNode* sgraph_node,
                           GSGraphNode* other_sgraph_node)
{
  GHashTable* visited_nodes;
  gboolean not_connected;

  visited_nodes = g_hash_table_new(NULL, NULL);
  not_connected = _g_sgraph_node_recurrent_connection_check(sgraph_node,
                                                            other_sgraph_node,
                                                            visited_nodes);
  g_hash_table_unref(visited_nodes);
  return not_connected;
}

/* static function definitions */

/**
 * _g_sgraph_node_recurrent_connection_check:
 * @sgraph_node: starting node.
 * @other_sgraph_node: other node.
 * @visited_nodes: #GHashTable holding information which nodes were already
 * visited.
 *
 * Checks if @sgraph_node == @other_sgraph_node. If not it checks @sgraph_node's neighbours.
 *
 * Returns: %TRUE if @sgraph_node != @other_sgraph_node, otherwise %FALSE.
 */
static gboolean
_g_sgraph_node_recurrent_connection_check(GSGraphNode* sgraph_node,
                                          GSGraphNode* other_sgraph_node,
                                          GHashTable* visited_nodes)
{
  guint iter;

  if (sgraph_node == other_sgraph_node)
  {
    return FALSE;
  }
  if (g_hash_table_lookup_extended(visited_nodes, sgraph_node, NULL, NULL))
  {
    return TRUE;
  }
  g_hash_table_insert(visited_nodes, sgraph_node, NULL);

  for (iter = 0; iter < sgraph_node->neighbours->len; iter++)
  {
    GSGraphNode* node = g_ptr_array_index(sgraph_node->neighbours, iter);
    if (!_g_sgraph_node_recurrent_connection_check(node, other_sgraph_node,
                                                   visited_nodes))
    {
      return FALSE;
    }
  }
  return TRUE;
}
