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
