#include "ggrapharray.h"

/**
 * g_graph_array_add:
 * @array: an array.
 * @graph: graph to add to @array.
 *
 * Convenience type-safe function, adding #GGraph to #GGraphArray.
 */
void
g_graph_array_add(GGraphArray* array,
                  GGraph* graph)
{
  g_ptr_array_add(array, graph);
}
