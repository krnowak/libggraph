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

#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsgraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_NODE_H_
#define _G_SGRAPH_NODE_H_

#include <glib.h>

#include <gsgraph/gsgraphdatapair.h>

G_BEGIN_DECLS

/**
 * GSGraphNode:
 * @data: #gpointer containing user defined data.
 * @neighbours: #GPtrArray containing pointers to neighbours.
 *
 * Single node of simple graph. It contains data and an array of pointers to all
 * it's neighbours.
 * If you want to have weights, one way edges, multiple edges between two nodes,
 * data on edges - use #GGraph.
 */
typedef struct _GSGraphNode GSGraphNode;

struct _GSGraphNode
{
  gpointer data;
  GPtrArray* neighbours;
};

GSGraphNode*
g_sgraph_node_new(gpointer data) G_GNUC_WARN_UNUSED_RESULT;

GPtrArray*
g_sgraph_node_construct(GSGraphDataPair** data_pairs,
                        gint count) G_GNUC_WARN_UNUSED_RESULT;

void
g_sgraph_node_connect(GSGraphNode* sgraph_node,
                      GSGraphNode* other_sgraph_node);

GPtrArray*
g_sgraph_node_remove(GSGraphNode* sgraph_node) G_GNUC_WARN_UNUSED_RESULT;

GSGraphNode*
g_sgraph_node_copy(GSGraphNode* sgraph_node);

GSGraphNode*
g_sgraph_node_copy_deep(GSGraphNode* sgraph_node,
                        GCopyFunc copy_func,
                        gpointer user_data);

void
g_sgraph_node_free(GSGraphNode* sgraph_node);

guint
g_sgraph_node_count(GSGraphNode* sgraph_node);

/* TODO: add traverse type option. */
void
g_sgraph_node_foreach(GSGraphNode* sgraph_node,
                      GFunc func,
                      gpointer user_data);

/**
 * GSGraphNodeFunc:
 * @sgraph_node: a node.
 * @user_data: user data passed to g_sgraph_node_foreach_node().
 *
 * Specifies the type of functions passed to g_sgraph_node_foreach_node().
 */
typedef void (*GSGraphNodeFunc)(GSGraphNode* sgraph_node,
                                gpointer user_data);

/* TODO: add traverse type option. */
void
g_sgraph_node_foreach_node(GSGraphNode* sgraph_node,
                           GSGraphNodeFunc func,
                           gpointer user_data);

gboolean
g_sgraph_node_break_connection(GSGraphNode* sgraph_node,
                               GSGraphNode* other_sgraph_node) G_GNUC_WARN_UNUSED_RESULT;

/* TODO: add traverse type option. */
GPtrArray*
g_sgraph_node_find(GSGraphNode* sgraph_node,
                   gconstpointer data) G_GNUC_WARN_UNUSED_RESULT;

/* TODO: add traverse type option. */
GPtrArray*
g_sgraph_node_find_custom(GSGraphNode* sgraph_node,
                          gconstpointer data,
                          GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

/**
 * GSGraphNodeEqualFunc:
 * @sgraph_node: a node.
 * @other_sgraph_node: other node.
 *
 * Specifies the type of functions passed to g_sgraph_node_find_custom_node().
 *
 * Returns: %TRUE if @sgraph_node and @other_sgraph_node are equal, otherwise %FALSE.
 */
typedef gboolean (*GSGraphNodeEqualFunc)(GSGraphNode* sgraph_node,
                                         GSGraphNode* other_sgraph_node);

/* TODO: add traverse type option. */
GPtrArray*
g_sgraph_node_find_custom_node(GSGraphNode* sgraph_node,
                               GSGraphNode* node,
                               GSGraphNodeEqualFunc func);

G_END_DECLS

#endif /* _G_SGRAPH_NODE_H_ */
