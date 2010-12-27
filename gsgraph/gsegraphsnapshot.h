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

#ifndef _G_SEGRAPH_SNAPSHOT_H_
#define _G_SEGRAPH_SNAPSHOT_H_

#include <glib.h>

#include <gsgraph/gsegraphtypedefs.h>
#include <gsgraph/gsegraphdatatriplet.h>
#include <gsgraph/gsegraphenums.h>

G_BEGIN_DECLS

/**
 * GSEGraphSnapshot:
 * @node_array: array of all nodes in graph.
 * @edge_array: array of all edges in graph.
 *
 * Convenient structure holding whole graph in two arrays.
 */
typedef struct _GSEGraphSnapshot GSEGraphSnapshot;

struct _GSEGraphSnapshot
{
  GPtrArray* node_array;
  GPtrArray* edge_array;
};

GPtrArray*
g_segraph_snapshot_new (GSEGraphDataTriplet** data_triplets,
                        guint count);

GSEGraphSnapshot*
g_segraph_snapshot_new_from_node (GSEGraphNode* node,
                                  GSEGraphTraverseType traverse_type);

GSEGraphSnapshot*
g_segraph_snapshot_new_from_edge (GSEGraphEdge* edge,
                                  GSEGraphTraverseType traverse_type);

GSEGraphSnapshot*
g_segraph_snapshot_new_only_nodes (GSEGraphNode* node,
                                   GSEGraphTraverseType traverse_type);

GSEGraphSnapshot*
g_segraph_snapshot_new_only_edges (GSEGraphEdge* edge,
                                   GSEGraphTraverseType traverse_type);

GSEGraphSnapshot*
g_segraph_snapshot_copy (GSEGraphSnapshot* graph);

GSEGraphSnapshot*
g_segraph_snapshot_copy_deep (GSEGraphSnapshot* graph,
                              GCopyFunc node_data_copy_func,
                              gpointer node_user_data,
                              GCopyFunc edge_data_copy_func,
                              gpointer edge_user_data);

void
g_segraph_snapshot_free (GSEGraphSnapshot* graph,
                         gboolean deep_free);

guint
g_segraph_snapshot_get_order (GSEGraphSnapshot* graph);

guint
g_segraph_snapshot_get_size (GSEGraphSnapshot* graph);

void
g_segraph_snapshot_foreach_node (GSEGraphSnapshot* graph,
                                 GFunc func,
                                 gpointer user_data);

GSEGraphNode*
g_segraph_snapshot_find_node_custom (GSEGraphSnapshot* graph,
                                     gpointer user_data,
                                     GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

void
g_segraph_snapshot_foreach_edge (GSEGraphSnapshot* graph,
                                 GFunc func,
                                 gpointer user_data);

GSEGraphEdge*
g_segraph_snapshot_find_edge_custom (GSEGraphSnapshot* graph,
                                     gpointer user_data,
                                     GEqualFunc func) G_GNUC_WARN_UNUSED_RESULT;

G_END_DECLS

#endif /* _G_SEGRAPH_SNAPSHOT_H_ */
