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

#include "gsegraphedge.h"

#include "gsegraphnode.h"

/**
 * SECTION: gsegraphedge
 * @title: Simple edged graph edges
 * @short_description: edges connecting two nodes and holding user data.
 * @include: gsgraph/gsegraph.h
 * @see_also: #GSEGraphNode
 *
 * Edges connect two nodes in graph and holds data. As it is simple so
 * edges are always undirected. Loops and multiple edges are allowed.
 * Edge is clean if its both pointers to nodes are %NULL.
 *
 * To create new edge, use g_segraph_edge_new().
 *
 * To free it, use g_segraph_edge_free().
 *
 * To get a neighbour of a node in edge, use g_segraph_edge_get_node() or
 * g_segraph_edge_get_node_extended().
 *
 * To check which member of edge node is, use g_segraph_edge_is_first(),
 * g_segraph_edge_is_first_extended(), g_segraph_edge_is_second() or
 * g_segraph_edge_is_second_extended().
 *
 * To connect two nodes, use g_segraph_edge_connect_nodes().
 *
 * To clean an edge, use g_segraph_edge_clean(), g_segraph_edge_clean_first(),
 * g_segraph_edge_clean_second() or g_segraph_edge_clean_node().
 *
 * To disconnect two nodes connected by edge, use g_segraph_edge_disconnect(),
 * g_segraph_edge_clean_disconnect(), g_segraph_edge_disconnect_first(),
 * g_segraph_edge_clean_disconnect_first(), g_segraph_edge_disconnect_second(),
 * g_segraph_edge_clean_disconnect_second(), g_segraph_edge_disconnect_node() or
 * g_segraph_edge_clean_disconnect_node().
 *
 * To check some edge's properties, use g_segraph_edge_is_link(),
 * g_segraph_edge_is_loop(), g_segraph_edge_is_half_edge(),
 * g_segraph_edge_is_bridge() g_segraph_edge_get_multiplicity().
 *
 * <note>
 *   <para>
 *     Remember to always take care about data edge holds. g_segraph_edge_free()
 *     returns a pointer to freed edge's data. Useful to free it, if applicable.
 *   </para>
 * </note>
 */

/* static function declarations */

static gboolean
_g_segraph_edge_general_member_check (GSEGraphEdge* edge,
                                      GSEGraphNode* node,
                                      gboolean* check_result,
                                      gboolean check_for_first);

static gboolean
_g_segraph_edge_bridge_check (GSEGraphNode* first,
                              GSEGraphNode* second,
                              GHashTable* visited_nodes,
                              GHashTable* visited_edges);

static void
_g_segraph_edge_disjoin (GSEGraphEdge* edge);

/* public function definitions */

/**
 * g_segraph_edge_new:
 * @data: data of the connection.
 *
 * Creates new clean edge containing @data. @data can be %NULL.
 *
 * Returns: newly created #GSEGraphEdge.
 */
GSEGraphEdge*
g_segraph_edge_new (gpointer data)
{
  GSEGraphEdge* edge;

  edge = g_slice_new (GSEGraphEdge);
  edge->first = NULL;
  edge->second = NULL;
  edge->data = data;

  return edge;
}

/**
 * g_segraph_edge_connect_nodes:
 * @edge: an edge.
 * @first: first node to connect.
 * @second: second node to connect.
 *
 * Creates new edge containing @data and connecting @first and @second. Only one
 * node (@first or @second) can be %NULL. @data can also be %NULL. To connect
 * nodes, @edge have to be clean.
 *
 * Returns: newly created #GSEGraphEdge.
 */
void
g_segraph_edge_connect_nodes (GSEGraphEdge* edge,
                              GSEGraphNode* first,
                              GSEGraphNode* second)
{
  g_return_if_fail (edge != NULL);
  g_return_if_fail ((edge->first == NULL) && (edge->second == NULL));
  g_return_if_fail ((first != NULL) || (second != NULL));

  edge->first = first;
  edge->second = second;

  if (first)
  {
    g_ptr_array_add (first->edges, edge);
  }

  if (second)
  {
    g_ptr_array_add (second->edges, edge);
  }
}

/**
 * g_segraph_edge_free:
 * @edge: an edge to free.
 *
 * Frees an edge. A #GSEGraphNode members in @edge are not freed, so it is up to
 * programmist to get pointers to them if needed. Data also is not freed, but
 * a pointer is returned. It is recommended to call g_segraph_edge_disconnect()
 * before calling this function, to avoid borking up the graph or manual setting
 * nodes having pointer to @edge.
 *
 * Returns: Data in freed @edge or %NULL if its data was %NULL.
 */
gpointer
g_segraph_edge_free (GSEGraphEdge* edge)
{
  gpointer data;

  g_return_val_if_fail (edge != NULL, NULL);

  data = edge->data;
  g_slice_free (GSEGraphEdge, edge);
  return data;
}

/**
 * g_segraph_edge_get_node:
 * @edge: an edge.
 * @node: a node using this connection.
 *
 * Gets a node to which a @node is connected. It doesn't recognize if @node is
 * not a part of @edge from @edge being half-edge - in both cases it returns
 * %NULL.
 *
 * Returns: #GSEGraphNode associated to @edge being a neighbour of @node.
 */
GSEGraphNode*
g_segraph_edge_get_node (GSEGraphEdge* edge,
                         GSEGraphNode* node)
{
  g_return_val_if_fail (edge != NULL, NULL);

  if (edge->first == node)
  {
    return edge->second;
  }
  else if (edge->second == node)
  {
    return edge->first;
  }
  return NULL;
}

/**
 * g_segraph_edge_get_node_extended:
 * @edge: an edge.
 * @node: a node using this connection.
 * @neighbour: place for neighbour to be stored.
 *
 * Gets a node to which a @node is connected by @edge and saves it in
 * @neighbour. If @node is not a part of @edge, this function will save %NULL to
 * @neighbour and return %FALSE. %NULL can be passed as a @neighbour, then this
 * function just checks if @node is a part of @edge.
 *
 * Returns: %TRUE, if @node is a part of @edge, %FALSE otherwise.
 */
gboolean
g_segraph_edge_get_node_extended (GSEGraphEdge* edge,
                                  GSEGraphNode* node,
                                  GSEGraphNode** neighbour)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  if (edge->first == node)
  {
    if (neighbour)
    {
      *neighbour = edge->second;
    }
    return TRUE;
  }
  else if (edge->second == node)
  {
    if (neighbour)
    {
      *neighbour = edge->first;
    }
    return TRUE;
  }

  if (neighbour)
  {
    *neighbour = NULL;
  }

  return FALSE;
}

/**
 * g_segraph_edge_is_first:
 * @edge: an edge.
 * @node: a node to check.
 *
 * Checks if node is first member of connection. It doesn't recognize if @node
 * is not a part of @edge from @node not being first in @edge.
 *
 * Returns: %TRUE, if @node is first member of @edge, %FALSE otherwise.
 */
gboolean
g_segraph_edge_is_first (GSEGraphEdge* edge,
                         GSEGraphNode* node)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  return (edge->first == node);
}

/**
 * g_segraph_edge_is_second:
 * @edge: an edge.
 * @node: a node to check.
 *
 * Checks if node is second member of connection. It doesn't recognize if @node
 * is not a part of @edge from @node not being second in @edge.
 *
 * Returns: %TRUE, if @node is second member of @edge, %FALSE otherwise.
 */
gboolean
g_segraph_edge_is_second (GSEGraphEdge* edge,
                          GSEGraphNode* node)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  return (edge->second == node);
}

/**
 * g_segraph_edge_is_first_extended:
 * @edge: an edge.
 * @node: a node to check.
 * @is_first: pointer to #gboolean where check is stored.
 *
 * Checks if node is first member of connection. Returned %FALSE means that
 * @node is not first nor second member of @edge. Information about being
 * a first member is saved in @is_first. If @is_first is %NULL, then no
 * information there is stored, so this function then can actually serve as a
 * checker if @node is a member of @edge.
 *
 * Returns: %TRUE, if @node is member (first or second) of @edge, %FALSE
 * otherwise.
 */
gboolean
g_segraph_edge_is_first_extended (GSEGraphEdge* edge,
                                  GSEGraphNode* node,
                                  gboolean* is_first)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  return _g_segraph_edge_general_member_check (edge, node, is_first, TRUE);
}

/**
 * g_segraph_edge_is_second_extended:
 * @edge: an edge.
 * @node: a node to check.
 * @is_second: pointer to #gboolean where check is stored.
 *
 * Checks if node is second member of connection. Returned %FALSE means that
 * @node is not first nor second member of @edge. Information about being
 * a second member is saved in @is_second. If @is_second is %NULL, then no
 * information there is stored, so this function then can actually serve as a
 * checker if @node is a member of @edge.
 *
 * Returns: %TRUE, if @node is member (first or second) of @edge, %FALSE
 * otherwise.
 */
gboolean
g_segraph_edge_is_second_extended (GSEGraphEdge* edge,
                                   GSEGraphNode* node,
                                   gboolean* is_second)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  return _g_segraph_edge_general_member_check (edge, node, is_second, FALSE);
}

/**
 * g_segraph_edge_clean:
 * @edge: an edge.
 *
 * Cleans @edge's pointers to nodes leaving its data intact. Such @edge can be
 * used for connecting nodes with g_segraph_edge_connect_nodes(). It is up to
 * programmist to make sure if cleaning @edge won't bork up a graph. Calling
 * g_segraph_edge_disconnect() before and checking if nodes are separate before
 * calling this function is a safe bet.
 */
void
g_segraph_edge_clean (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  edge->first = NULL;
  edge->second = NULL;
}

/**
 * g_segraph_edge_disconnect:
 * @edge: an edge.
 *
 * Removes connection between nodes in @edge, that is - in both nodes @edge is
 * removed, but in edge both pointer to nodes are left intact. Checking if nodes
 * in edges are separate is up to programmist.
 */
void
g_segraph_edge_disconnect (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  _g_segraph_edge_disjoin (edge);
}

/**
 * g_segraph_edge_clean_disconnect:
 * @edge: an edge.
 *
 * Removes connection between nodes in @edge, that is - in both nodes @edge is
 * removed and in @edge both pointer to nodes are set to %NULL, so @edge can be
 * used for connecting another nodes with g_segraph_edge_connect_nodes().
 * Checking if nodes in edges are separate is up to programmist, provided that
 * pointers to nodes were saved before calling this function.
 */
void
g_segraph_edge_clean_disconnect (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  _g_segraph_edge_disjoin (edge);
  edge->first = NULL;
  edge->second = NULL;
}

/**
 * g_segraph_edge_clean_first:
 * @edge: an edge.
 *
 * Cleans edge in its first member. If second member is not %NULL, @edge becomes
 * half-edge.
 */
void
g_segraph_edge_clean_first (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  edge->first = NULL;
}

/**
 * g_segraph_edge_disconnect_first:
 * @edge: an edge.
 *
 * Disconnects node in first member of @edge from @edge, but leaves a pointer to
 * node in @edge intact.
 */
void
g_segraph_edge_disconnect_first (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  if (edge->first)
  {
    g_ptr_array_remove (edge->first->edges, edge);
  }
}

/**
 * g_segraph_edge_clean_disconnect_first:
 * @edge: an edge.
 *
 * Disconnects node in first member of @edge from @edge and cleans first member.
 */
void
g_segraph_edge_clean_disconnect_first (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  if (edge->first)
  {
    g_ptr_array_remove (edge->first->edges, edge);
    edge->first = NULL;
  }
}

/**
 * g_segraph_edge_clean_second:
 * @edge: an edge.
 *
 * Cleans edge in its second member. If first member is not %NULL, @edge becomes
 * half-edge.
 */
void
g_segraph_edge_clean_second (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  edge->second = NULL;
}

/**
 * g_segraph_edge_disconnect_second:
 * @edge: an edge.
 *
 * Disconnects node in second member of @edge from @edge, but leaves a pointer
 * to node in @edge intact.
 */
void
g_segraph_edge_disconnect_second (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  if (edge->second)
  {
    g_ptr_array_remove (edge->second->edges, edge);
  }
}

/**
 * g_segraph_edge_clean_disconnect_second:
 * @edge: an edge.
 *
 * Disconnects node in second member of @edge from @edge and cleans second
 * member.
 */
void
g_segraph_edge_clean_disconnect_second (GSEGraphEdge* edge)
{
  g_return_if_fail (edge != NULL);

  if (edge->second)
  {
    g_ptr_array_remove (edge->second->edges, edge);
    edge->second = NULL;
  }
}

/**
 * g_segraph_edge_clean_node:
 * @edge: an edge.
 * @node: a node.
 *
 * Cleans edge in member which holds @node. If other member is not %NULL, @edge
 * becomes half-edge.
 *
 * Returns: %TRUE if pointer to @node was cleaned, otherwise %FALSE (there was
 * no such node in @edge).
 */
gboolean
g_segraph_edge_clean_node (GSEGraphEdge* edge,
                           GSEGraphNode* node)
{
  g_return_val_if_fail (edge != NULL, FALSE);
  g_return_val_if_fail (node != NULL, FALSE);

  if (edge->first == node)
  {
    edge->first = NULL;
    return TRUE;
  }
  if (edge->second == node)
  {
    edge->second = NULL;
    return TRUE;
  }
  return FALSE;
}

/**
 * g_segraph_edge_disconnect_node:
 * @edge: an edge.
 * @node: a node.
 *
 * Disconnects node in member of @edge which holds @node from @edge, but leaves
 * a pointer to node in @edge intact.
 *
 * Returns: %TRUE if @node was disconnected, otherwise %FALSE (there was
 * no such node in @edge).
 */
gboolean
g_segraph_edge_disconnect_node (GSEGraphEdge* edge,
                                GSEGraphNode* node)
{
  g_return_val_if_fail (edge != NULL, FALSE);
  g_return_val_if_fail (node != NULL, FALSE);

  if ((edge->first == node) || (edge->second == node))
  {
    g_ptr_array_remove (node->edges, edge);
    return TRUE;
  }
  return FALSE;
}

/**
 * g_segraph_edge_clean_disconnect_node:
 * @edge: an edge.
 * @node: a node.
 *
 * Disconnects node in member of @edge holding @node from @edge and cleans this
 * member.
 *
 * Returns: %TRUE if @node was disconnected and pointer to @node was cleaned,
 * otherwise %FALSE (there was no such node in @edge).
 */
gboolean
g_segraph_edge_clean_disconnect_node (GSEGraphEdge* edge,
                                      GSEGraphNode* node)
{
  g_return_val_if_fail (edge != NULL, FALSE);
  g_return_val_if_fail (node != NULL, FALSE);

  if (edge->first == node)
  {
    g_ptr_array_remove (node->edges, edge);
    edge->first = NULL;
    return TRUE;
  }

  if (edge->second == node)
  {
    g_ptr_array_remove (node->edges, edge);
    edge->second = NULL;
    return TRUE;
  }
  return FALSE;
}

/**
 * g_segraph_edge_is_link:
 * @edge: an edge.
 *
 * Checks if @edge is a link, that is - connects two different nodes. Half-edges
 * are not links.
 *
 * Returns: %TRUE if @edge is a link, otherwise %FALSE.
 */
gboolean
g_segraph_edge_is_link (GSEGraphEdge* edge)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  return (edge->first && edge->second && (edge->first != edge->second));
}

/**
 * g_segraph_edge_is_loop:
 * @edge: an edge.
 *
 * Checks if @edge is a loop, that is - its both endpoints are same node.
 * Half-edges are not loops.
 *
 * Returns: %TRUE if @edge is a loop, otherwise %FALSE.
 */
gboolean
g_segraph_edge_is_loop (GSEGraphEdge* edge)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  return (edge->first && edge->second && (edge->first == edge->second));
}

/**
 * g_segraph_edge_get_multiplicity:
 * @edge: an edge.
 *
 * Checks @edge's multiplicity, that is - how many edges are in graph which
 * connects the same nodes as @edge.
 *
 * Returns: @edge's multiplicity.
 */
guint
g_segraph_edge_get_multiplicity (GSEGraphEdge* edge)
{
  guint iter;
  guint multiplicity;
  GSEGraphNode* checking_node;
  GSEGraphNode* checked_node;

  g_return_val_if_fail (edge != NULL, 0);
  g_return_val_if_fail ((edge->first != NULL) || (edge->second != NULL), 0);

  if (edge->first)
  {
    checking_node = edge->first;
    checked_node = edge->second;
  }
  else
  {
    checking_node = edge->second;
    checked_node = NULL;
  }

  multiplicity = 1;
  for (iter = 0; iter < checking_node->edges->len; ++iter)
  {
    GSEGraphEdge* temp_edge;
    GSEGraphNode* temp_node;

    temp_edge = g_ptr_array_index (checking_node->edges, iter);
    if (temp_edge == edge)
    {
      continue;
    }
    temp_node = g_segraph_edge_get_node (temp_edge, checking_node);
    if (temp_node == checked_node)
    {
      ++multiplicity;
    }
  }
  return multiplicity;
}

/**
 * g_segraph_edge_is_bridge:
 * @edge: an edge.
 *
 * Checks if @edge is a bridge, that is - an edge which is the only one keeping
 * its nodes in one graph. After deletion of a bridge, two nodes connected by
 * this edge are in separate graphs.
 *
 * Returns: %TRUE if @edge is a bridge, otherwise %FALSE.
 */
gboolean
g_segraph_edge_is_bridge (GSEGraphEdge* edge)
{
  GHashTable* visited_nodes;
  GHashTable* visited_edges;
  gboolean result;

  g_return_val_if_fail (edge != NULL, FALSE);

  if (!edge->first || !edge->second)
  {
    return FALSE;
  }

  visited_nodes = g_hash_table_new (NULL, NULL);
  visited_edges = g_hash_table_new (NULL, NULL);

  g_hash_table_insert (visited_edges, edge, NULL);

  result = _g_segraph_edge_bridge_check (edge->first,
                                         edge->second,
                                         visited_nodes,
                                         visited_edges);

  g_hash_table_unref (visited_nodes);
  g_hash_table_unref (visited_edges);

  return result;
}

/**
 * g_segraph_edge_is_half_edge:
 * @edge: an edge.
 *
 * Checks if @edge is half-edge, that is - one of its endpoints points nowhere.
 * In this case - points to %NULL.
 *
 * Returns: %TRUE if @edge is half-edge, otherwise %FALSE.
 */
gboolean
g_segraph_edge_is_half_edge (GSEGraphEdge* edge)
{
  g_return_val_if_fail (edge != NULL, FALSE);

  return (!edge->first || !edge->second);
}

/* static function definitions */

/**
 * _g_segraph_edge_general_member_check:
 * @edge: an edge.
 * @node: a node to check.
 * @check_result: pointer to #gboolean where check is stored.
 * @check_for_first: #gboolean indicating against which member check has to be
 * made.
 *
 * Checks if @node is a member of @edge. Which to check is indicated by
 * @check_for_first - if %TRUE than function will check if @node is first member
 * of @edge; if %FALSE - second member.
 *
 * Returns: %TRUE if @node is a member (first or second) of @edge, otherwise
 * %FALSE.
 */
static gboolean
_g_segraph_edge_general_member_check (GSEGraphEdge* edge,
                                      GSEGraphNode* node,
                                      gboolean* check_result,
                                      gboolean check_for_first)
{
  GSEGraphNode* check_node;
  GSEGraphNode* other_node;
  gboolean result;

  if (check_for_first)
  {
    check_node = edge->first;
    other_node = edge->second;
  }
  else
  {
    check_node = edge->second;
    other_node = edge->first;
  }

  result = (node == check_node);

  if (check_result)
  {
    *check_result = result;
  }

  if (result || (node == other_node))
  {
    return TRUE;
  }

  return FALSE;
}

/**
 * _g_segraph_edge_bridge_check:
 * @first: first node.
 * @second: second node.
 * @visited_nodes: hash table holding already visited nodes.
 * @visited_edges: hash table holding already visited edges.
 *
 * Recursively checks if @first is connected to @second. This function is used
 * for checking if an edge is a bridge.
 *
 * Returns: %TRUE if edge is a bridge, otherwise %FALSE.
 */
static gboolean
_g_segraph_edge_bridge_check (GSEGraphNode* first,
                              GSEGraphNode* second,
                              GHashTable* visited_nodes,
                              GHashTable* visited_edges)
{
  guint iter;

  if (first == second)
  {
    return FALSE;
  }

  g_hash_table_insert (visited_nodes, second, NULL);

  for (iter = 0; iter < second->edges->len; ++iter)
  {
    GSEGraphEdge* edge;
    GSEGraphNode* node;

    edge = g_ptr_array_index (second->edges, iter);
    if (g_hash_table_lookup_extended (visited_edges, edge, NULL, NULL))
    {
      continue;
    }
    else
    {
      g_hash_table_insert (visited_edges, edge, NULL);
    }
    node = g_segraph_edge_get_node (edge, second);
    if (g_hash_table_lookup_extended (visited_nodes, node, NULL, NULL))
    {
      continue;
    }

    if (!_g_segraph_edge_bridge_check (first,
                                       node,
                                       visited_nodes,
                                       visited_edges))
    {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * _g_segraph_edge_disjoin:
 * @edge: an edge.
 *
 * Removes pointers to @edge in both nodes it holds.
 */
static void
_g_segraph_edge_disjoin (GSEGraphEdge* edge)
{
  if (edge->first)
  {
    g_ptr_array_remove (edge->first->edges, edge);
  }
  if (edge->second)
  {
    g_ptr_array_remove (edge->second->edges, edge);
  }
}
