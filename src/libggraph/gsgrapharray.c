#include "gsgrapharray.h"

/* function definitions */

/**
 * g_sgraph_array_add:
 * @array: an array.
 * @sgraph: graph to add to @array.
 *
 * Convenience type-safe function, adding #GSGraph to #GSGraphArray.
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
 * Convenience type-safe function, removing #GSGraph from #GSGraphArray,
 * preserving order.
 *
 * Returns: Removed #GSGraph.
 */
GSGraph*
g_sgraph_array_remove_index(GSGraphArray* array,
                            guint index)
{
  return g_ptr_array_remove_index(array, index);
}
