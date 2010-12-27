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

#if defined (G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SEGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsegraph.h> can be included directly."
#endif

#ifndef _G_SEGRAPH_NODE_H_
#define _G_SEGRAPH_NODE_H_

#include <glib.h>

#include <gsgraph/gsegraphtypedefs.h>

G_BEGIN_DECLS

/**
 * GSEGraphNode:
 * @data: pointer to user defined data.
 * @edges: an array containing pointers to edges to neighbours.
 *
 * Contains public fields of a node.
 */
struct _GSEGraphNode
{
  gpointer data;
  GPtrArray* edges;
};

GSEGraphNode*
g_segraph_node_new (gpointer data) G_GNUC_WARN_UNUSED_RESULT;

gpointer
g_segraph_node_free (GSEGraphNode* node);

gboolean
g_segraph_node_are_separate (GSEGraphNode* node,
                             GSEGraphNode* other_node);

G_END_DECLS

#endif /* _G_SEGRAPH_NODE_H_ */
