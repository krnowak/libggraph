#include "ggrapharray.h"

/**
 * SECTION: ggrapharray
 * @title: Array of Graphs
 * @short_description: arrays of pointers to #GGraph, which grow automatically
 * as new elements are added
 * @include: libggraph.h
 * @see_also: #GPtrArray
 *
 * Array of graphs is a type-safe wrapper around #GPtrArray.
 */

/* function definitions */

/**
 * g_graph_array_new:
 *
 * Convenience type-safe function, creating a new #GGraphArray.
 *
 * See also: g_ptr_array_new()
 *
 * Returns: newly created array.
 */
GGraphArray*
g_graph_array_new(void)
{
  return g_ptr_array_new();
}

/**
 * g_graph_array_sized_new:
 * @reserved_size: size of array.
 *
 * Convenience type-safe function, creating a new #GGraphArray with given size.
 *
 * See also: g_ptr_array_sized_new()
 *
 * Returns: newly created array.
 */
GGraphArray*
g_graph_array_sized_new(guint reserved_size)
{
  return g_ptr_array_sized_new(reserved_size);
}

/**
 * g_graph_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe function, freeing an array.
 *
 * See also: g_ptr_array_free()
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to array of
 * #GGraph is returned - it should be freed with g_free().
 */
GGraph**
g_graph_array_free(GGraphArray* array,
                   gboolean free_segment)
{
  return (GGraph**)g_ptr_array_free(array, free_segment);
}

/**
 * g_graph_array_add:
 * @array: an array.
 * @graph: #GGraph to add to array.
 *
 * Convenience type-safe function adding @graph to @array.
 *
 * See also: g_ptr_array_add()
 */
void
g_graph_array_add(GGraphArray* array,
                  GGraph* graph)
{
  g_ptr_array_add(array, graph);
}

/**
 * g_graph_array_remove_index_fast:
 * @array: an array.
 * @index: index of graph to be removed from @array.
 *
 * Convenience type-safe function removing #GGraph under given @index from
 * #GGraphArray, not preserving order, but faster than
 * g_graph_array_remove_index().
 *
 * See also: g_ptr_array_remove_index_fast()
 *
 * Returns: Removed #GGraph.
 */
GGraph*
g_graph_array_remove_index_fast(GGraphArray* array,
                                guint index)
{
  return (GGraph*)g_ptr_array_remove_index_fast(array, index);
}
