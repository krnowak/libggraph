#include "gsgrapharray.h"

/**
 * SECTION: gsgrapharray
 * @title: Array of simple graphs
 * @short_description: arrays of pointers to #GSGraph, which grow automatically
 * as new elements are added
 * @include: libggraph.h
 * @see_also: #GPtrArray
 *
 * Array of simple graphs is a type-safe wrapper around #GPtrArray.
 */

/* function definitions */

/**
 * g_sgraph_array_new:
 *
 * Convenience type-safe function creating a new #GSGraphArray.
 * 
 * See also: g_ptr_array_new()
 *
 * Returns: newly created array.
 */
GSGraphArray*
g_sgraph_array_new(void)
{
  return g_ptr_array_new();
}

/**
 * g_sgraph_array_sized_new:
 * @reserved_size: size of array.
 *
 * Convenience type-safe function creating a new #GSGraphArray with given size.
 *
 * See also: g_ptr_array_sized_new()
 *
 * Returns: newly created array.
 */
GSGraphArray*
g_sgraph_array_sized_new(guint reserved_size)
{
  return g_ptr_array_sized_new(reserved_size);
}

/**
 * g_sgraph_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe function freeing an array.
 *
 * See also: g_ptr_array_free()
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to array of
 * #GSGraph is returned - it should be freed with g_free().
 */
GSGraph**
g_sgraph_array_free(GSGraphArray* array,
                    gboolean free_segment)
{
  return (GSGraph**)g_ptr_array_free(array, free_segment);
}

/**
 * g_sgraph_array_add:
 * @array: an array.
 * @sgraph: #GSGraph to add to array.
 *
 * Convenience type-safe function adding @sgraph to @array.
 *
 * See also: g_ptr_array_add()
 */
void
g_sgraph_array_add(GSGraphArray* array,
                   GSGraph* sgraph)
{
  g_ptr_array_add(array, sgraph);
}

/**
 * g_sgraph_array_remove_index:
 * @array: an array.
 * @index: index of graph to be removed from @array.
 *
 * Convenience type-safe function removing #GSGraph under given @index from
 * @array, preserving order.
 *
 * See also: g_ptr_array_remove_index()
 *
 * Returns: Removed #GSGraph.
 */
GSGraph*
g_sgraph_array_remove_index(GSGraphArray* array,
                            guint index)
{
  return (GSGraph*)g_ptr_array_remove_index(array, index);
}

/**
 * g_sgraph_array_remove_index_fast:
 * @array: an array.
 * @index: index of graph to be removed from @array.
 *
 * Convenience type-safe function removing #GSGraph from #GSGraphArray,
 * not preserving order, but faster than g_sgraph_array_remove_index().
 *
 * See also: g_ptr_array_remove_index_fast()
 *
 * Returns: Removed #GSGraph.
 */
GSGraph*
g_sgraph_array_remove_index_fast(GSGraphArray* array,
                                 guint index)
{
  return (GSGraph*)g_ptr_array_remove_index_fast(array, index);
}
