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

#if defined (G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsgraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_NODE_H_
#define _G_SGRAPH_NODE_H_

#include <glib.h>

G_BEGIN_DECLS

/**
 * GSGraphNode:
 * @data: #gpointer containing user defined data.
 * @neighbours: #GPtrArray containing pointers to neighbours.
 *
 * Single node of connected graph.
 */
typedef struct _GSGraphNode GSGraphNode;

struct _GSGraphNode
{
  gpointer data;
  GPtrArray* neighbours;
};

GSGraphNode*
g_sgraph_node_new (gpointer data) G_GNUC_WARN_UNUSED_RESULT;

gpointer
g_sgraph_node_free (GSGraphNode* node);

void
g_sgraph_node_connect (GSGraphNode* node,
                       GSGraphNode* other_node);

void
g_sgraph_node_disconnect (GSGraphNode* node,
                          GSGraphNode* other_node);

gboolean
g_sgraph_node_are_separate (GSGraphNode* node,
                            GSGraphNode* other_node);

G_END_DECLS

#endif /* _G_SGRAPH_NODE_H_ */
