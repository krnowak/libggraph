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

#include "gsgraphnodearray.h"

/**
 * SECTION: gsgraphnodearray
 * @title: Array of simple graphs
 * @short_description: arrays of pointers to #GSGraphNode, which grow automatically
 * as new elements are added
 * @include: gsgraph/gsgraph.h
 * @see_also: #GPtrArray
 *
 * Array of simple graphs is a type-safe wrapper around #GPtrArray - its
 * functions can be safely used here.
 */

/* function definitions */

/**
 * g_sgraph_node_array_new:
 *
 * Convenience type-safe function creating a new #GSGraphNodeArray.
 * 
 * See also: g_ptr_array_new()
 *
 * Returns: newly created array.
 */
GSGraphNodeArray*
g_sgraph_node_array_new(void)
{
  return g_ptr_array_new();
}

/**
 * g_sgraph_node_array_sized_new:
 * @reserved_size: size of array.
 *
 * Convenience type-safe function creating a new #GSGraphNodeArray with given size.
 *
 * See also: g_ptr_array_sized_new()
 *
 * Returns: newly created array.
 */
GSGraphNodeArray*
g_sgraph_node_array_sized_new(guint reserved_size)
{
  return g_ptr_array_sized_new(reserved_size);
}

/**
 * g_sgraph_node_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe function freeing an array.
 *
 * See also: g_ptr_array_free()
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to array of
 * #GSGraphNode is returned - it should be freed with g_free().
 */
GSGraphNode**
g_sgraph_node_array_free(GSGraphNodeArray* array,
                         gboolean free_segment)
{
  return (GSGraphNode**)g_ptr_array_free(array, free_segment);
}

/**
 * g_sgraph_node_array_add:
 * @array: an array.
 * @sgraph: #GSGraphNode to add to array.
 *
 * Convenience type-safe function adding @sgraph to @array.
 *
 * See also: g_ptr_array_add()
 */
void
g_sgraph_node_array_add(GSGraphNodeArray* array,
                        GSGraphNode* sgraph)
{
  g_ptr_array_add(array, sgraph);
}

/**
 * g_sgraph_node_array_remove_index:
 * @array: an array.
 * @index: index of graph to be removed from @array.
 *
 * Convenience type-safe function removing #GSGraphNode under given @index from
 * @array, preserving order.
 *
 * See also: g_ptr_array_remove_index()
 *
 * Returns: Removed #GSGraphNode.
 */
GSGraphNode*
g_sgraph_node_array_remove_index(GSGraphNodeArray* array,
                                 guint index)
{
  return (GSGraphNode*)g_ptr_array_remove_index(array, index);
}

/**
 * g_sgraph_node_array_remove_index_fast:
 * @array: an array.
 * @index: index of graph to be removed from @array.
 *
 * Convenience type-safe function removing #GSGraphNode from #GSGraphNodeArray,
 * not preserving order, but faster than g_sgraph_node_array_remove_index().
 *
 * See also: g_ptr_array_remove_index_fast()
 *
 * Returns: Removed #GSGraphNode.
 */
GSGraphNode*
g_sgraph_node_array_remove_index_fast(GSGraphNodeArray* array,
                                      guint index)
{
  return (GSGraphNode*)g_ptr_array_remove_index_fast(array, index);
}
