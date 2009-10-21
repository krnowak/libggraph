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

#ifndef _G_SEGRAPH_EDGE_ARRAY_H_
#define _G_SEGRAPH_EDGE_ARRAY_H_

#include <glib.h>
#include <gsgraph/gsegraphtypedefs.h>
#include <gsgraph/gsegraphedge.h>

G_BEGIN_DECLS

GSEGraphEdgeArray*
g_segraph_edge_array_new(void);

GSEGraphEdgeArray*
g_segraph_edge_array_sized_new(guint reserved_size);

/**
 * g_segraph_edge_array_index:
 * @array: an array.
 * @index: index of the #GSEGraphEdge to return.
 *
 * Convenience type-safe macro, returning a #GSEGraphEdge at given index.
 *
 * See also: g_ptr_array_index()
 *
 * Returns: #GSEGraphEdge at given index.
 */
#define g_segraph_edge_array_index(array, index) \
((GSEGraphEdge*)(g_ptr_array_index((array), (index))))

void
g_segraph_edge_array_add(GSEGraphEdgeArray* array,
                         GSEGraphEdge* edge);

gboolean
g_segraph_edge_array_remove(GSEGraphEdgeArray* array,
                            GSEGraphEdge* edge);

GSEGraphEdge**
g_segraph_edge_array_free(GSEGraphEdgeArray* array,
                          gboolean free_segment);

G_END_DECLS

#endif /* _G_SEGRAPH_EDGE_ARRAY_H_ */
