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

#include "gsegraphedgearray.h"

/**
 * SECTION: gsegraphedgearray
 * @title: Array of edges to simple edged graphs
 * @short_description: arrays of pointers to #GSEGraphEdge, which grow
 * automatically as new elements are added
 * @include: gsgraph/gsegraph.h
 * @see_also: #GPtrArray
 *
 * Array of edges is a type-safe wrapper around #GPtrArray.
 */

/* functions definitions */

/**
 * g_segraph_edge_array_new:
 *
 * Convenience type-safe function, creating a new #GSEGraphEdgeArray.
 *
 * See also: g_ptr_array_new()
 *
 * Returns: newly created array.
 */
GSEGraphEdgeArray*
g_segraph_edge_array_new(void)
{
  return g_ptr_array_new();
}

/**
 * g_segraph_edge_array_sized_new:
 * @reserved_size: size of array.
 *
 * Convenience type-safe function, creating a new #GSEGraphEdgeArray with given
 * size.
 *
 * See also: g_ptr_array_sized_new()
 *
 * Returns: newly created array.
 */
GSEGraphEdgeArray*
g_segraph_edge_array_sized_new(guint reserved_size)
{
  return g_ptr_array_sized_new(reserved_size);
}

/**
 * g_segraph_edge_array_add:
 * @array: an array.
 * @edge: #GSEGraphEdge to add to array.
 *
 * Convenience type-safe function adding @edge to @array.
 *
 * See also: g_ptr_array_add()
 */
void
g_segraph_edge_array_add(GSEGraphEdgeArray* array,
                         GSEGraphEdge* edge)
{
  g_ptr_array_add(array, edge);
}

/**
 * g_segraph_edge_array_remove:
 * @array: an array.
 * @edge: edge to be removed from @array.
 *
 * Convenience type-safe function removing @edge from @array, preserving order.
 *
 * See also: g_ptr_array_remove()
 *
 * Returns: %TRUE if @edge was removed.
 */
gboolean
g_segraph_edge_array_remove(GSEGraphEdgeArray* array,
                            GSEGraphEdge* edge)
{
  return g_ptr_array_remove(array, edge);
}

/**
 * g_segraph_edge_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe function, freeing an array.
 *
 * See also: g_ptr_array_free()
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to data of
 * @array is returned - it should be freed with g_free().
 */
GSEGraphEdge**
g_segraph_edge_array_free(GSEGraphEdgeArray* array,
                          gboolean free_segment)
{
  return (GSEGraphEdge**)g_ptr_array_free(array, free_segment);
}
