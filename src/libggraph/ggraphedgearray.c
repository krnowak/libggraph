#include "ggraphedgearray.h"

/**
 * g_graph_edge_array_add:
 * @array: an array.
 * @edge: edge to add to @array.
 *
 * Convenience type-safe macro, adding #GGraphEdge to #GGraphEdgeArray.
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
 * @edge: edge to remove from @array.
 *
 * Convenience type-safe function, removing #GGraphEdge from #GGraphEdgeArray,
 * preserving order.
 *
 * Returns: %TRUE if the pointer is removed. %FALSE if the pointer is not found
 * in the array.
 */
gboolean
g_graph_edge_array_remove(GGraphEdgeArray* array,
                          GGraphEdge* edge)
{
  return g_ptr_array_remove(array, edge);
}
