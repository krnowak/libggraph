#include "ggraphedgearray.h"

/**
 * SECTION: ggraphedgearray
 * @title: Array of edges to graphs
 * @short_description: arrays of pointers to #GGraphEdge, which grow
 * automatically as new elements are added
 * @include: libggraph.h
 * @see_also: #GPtrArray
 *
 * Array of edges is a type-safe wrapper around #GPtrArray.
 */

/* functions definitions */

/**
 * g_graph_edge_array_new:
 *
 * Convenience type-safe function, creating a new #GGraphEdgeArray.
 *
 * See also: g_ptr_array_new()
 *
 * Returns: newly created array.
 */
GGraphEdgeArray*
g_graph_edge_array_new(void)
{
  return g_ptr_array_new();
}

/**
 * g_graph_edge_array_sized_new:
 * @reserved_size: size of array.
 *
 * Convenience type-safe function, creating a new #GGraphEdgeArray with given
 * size.
 *
 * See also: g_ptr_array_sized_new()
 *
 * Returns: newly created array.
 */
GGraphEdgeArray*
g_graph_edge_array_sized_new(guint reserved_size)
{
  return g_ptr_array_sized_new(reserved_size);
}

/**
 * g_graph_edge_array_add:
 * @array: an array.
 * @edge: #GGraphEdge to add to array.
 *
 * Convenience type-safe function adding @edge to @array.
 *
 * See also: g_ptr_array_add()
 */
void
g_graph_edge_array_add(GGraphEdgeArray* array,
                       GGraphEdge* edge)
{
  g_ptr_array_add(array, edge);
}

/**
 * g_graph_edge_array_remove:
 * @array: an array.
 * @edge: edge to be removed from @array.
 *
 * Convenience type-safe function removing #GSGraph from #GSGraphArray,
 * preserving order.
 *
 * See also: g_ptr_array_remove()
 *
 * Returns: %TRUE if @edge was removed.
 */
gboolean
g_graph_edge_array_remove(GGraphEdgeArray* array,
                          GGraphEdge* edge)
{
  return g_ptr_array_remove(array, edge);
}

/**
 * g_graph_edge_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe function, freeing an array.
 *
 * See also: g_ptr_array_free()
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to array of
 * #GGraphEdge is returned - it should be freed with g_free().
 */
GGraphEdge**
g_graph_edge_array_free(GGraphEdgeArray* array,
                        gboolean free_segment)
{
  return (GGraphEdge**)g_ptr_array_free(array, free_segment);
}
