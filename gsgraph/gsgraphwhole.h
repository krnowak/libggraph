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

#ifndef _G_SGRAPH_WHOLE_H_
#define _G_SGRAPH_WHOLE_H_

#include <glib.h>

#include <gsgraph/gsgraphenums.h>
#include <gsgraph/gsgraphdatapair.h>

G_BEGIN_DECLS

/**
 * GSGraphWhole:
 * @node_array: array of all nodes in graph.
 *
 * Convenient structure holding whole graph in one array.
 */
typedef struct _GSGraphWhole GSGraphWhole;

struct _GSGraphWhole
{
  GPtrArray* node_array;
};

GPtrArray*
g_sgraph_whole_new(GSGraphDataPair** data_pair,
                   guint count);

GSGraphWhole*
g_sgraph_whole_new_from_node(GSGraphNode* node,
                             GSGraphTraverseType traverse_type);

GSGraphWhole*
g_sgraph_whole_copy(GSGraphWhole* graph);

GSGraphWhole*
g_sgraph_whole_copy_deep(GSGraphWhole* graph,
                         GCopyFunc node_data_copy_func,
                         gpointer node_user_data);

void
g_sgraph_whole_free(GSGraphWhole* graph,
                    gboolean deep_free);

guint
g_sgraph_whole_get_order(GSGraphWhole* graph);

guint
g_sgraph_whole_get_size(GSGraphWhole* graph);

void
g_sgraph_whole_foreach_node(GSGraphWhole* graph,
                            GFunc func,
                            gpointer user_data);

GSGraphNode*
g_sgraph_whole_find_node_custom(GSGraphWhole* graph,
                                gpointer user_data,
                                GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS

#endif /* _G_SGRAPH_WHOLE_H_ */
