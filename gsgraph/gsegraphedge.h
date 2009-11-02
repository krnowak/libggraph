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

#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SEGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsegraph.h> can be included directly."
#endif

#ifndef _G_SEGRAPH_EDGE_H_
#define _G_SEGRAPH_EDGE_H_

#include <glib.h>

#include <gsgraph/gsegraphtypedefs.h>

G_BEGIN_DECLS

/**
 * GSEGraphEdge:
 * @first: first end of this edge.
 * @second: second end of this edge.
 * @connected: flag describing interconnection between @first and @second.
 * @data: data contained by this edge.
 *
 * An edge from the node containing it to the @graph.
 */
struct _GSEGraphEdge
{
  GSEGraphNode* first;
  GSEGraphNode* second;
  gpointer data;
};

GSEGraphEdge*
g_segraph_edge_new(gpointer data);

void
g_segraph_edge_connect_nodes(GSEGraphEdge* edge,
                             GSEGraphNode* first,
                             GSEGraphNode* second);

gpointer
g_segraph_edge_free(GSEGraphEdge* edge);

GSEGraphNode*
g_segraph_edge_get_node(GSEGraphEdge* edge,
                        GSEGraphNode* node);

gboolean
g_segraph_edge_get_node_extended(GSEGraphEdge* edge,
                                 GSEGraphNode* node,
                                 GSEGraphNode** neighbour);

gboolean
g_segraph_edge_is_first(GSEGraphEdge* edge,
                        GSEGraphNode* node);

gboolean
g_segraph_edge_is_second(GSEGraphEdge* edge,
                         GSEGraphNode* node);

gboolean
g_segraph_edge_is_first_extended(GSEGraphEdge* edge,
                                 GSEGraphNode* node,
                                 gboolean* is_first);

gboolean
g_segraph_edge_is_second_extended(GSEGraphEdge* edge,
                                  GSEGraphNode* node,
                                  gboolean* is_second);

void
g_segraph_edge_clean(GSEGraphEdge* edge);

void
g_segraph_edge_disconnect(GSEGraphEdge* edge);

void
g_segraph_edge_clean_disconnect(GSEGraphEdge* edge);

void
g_segraph_edge_clean_first(GSEGraphEdge* edge);

void
g_segraph_edge_disconnect_first(GSEGraphEdge* edge);

void
g_segraph_edge_clean_disconnect_first(GSEGraphEdge* edge);

void
g_segraph_edge_clean_second(GSEGraphEdge* edge);

void
g_segraph_edge_disconnect_second(GSEGraphEdge* edge);

void
g_segraph_edge_clean_disconnect_second(GSEGraphEdge* edge);

gboolean
g_segraph_edge_clean_node(GSEGraphEdge* edge,
                          GSEGraphNode* node);

gboolean
g_segraph_edge_disconnect_node(GSEGraphEdge* edge,
                               GSEGraphNode* node);

gboolean
g_segraph_edge_clean_disconnect_node(GSEGraphEdge* edge,
                                     GSEGraphNode* node);

gboolean
g_segraph_edge_is_link(GSEGraphEdge* edge);

gboolean
g_segraph_edge_is_loop(GSEGraphEdge* edge);

guint
g_segraph_edge_get_multiplicity(GSEGraphEdge* edge);

gboolean
g_segraph_edge_is_bridge(GSEGraphEdge* edge);

gboolean
g_segraph_edge_is_half_edge(GSEGraphEdge* edge);

G_END_DECLS

#endif /* _G_SEGRAPH_EDGE_H_ */
