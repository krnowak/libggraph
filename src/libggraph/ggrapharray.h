#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_ARRAY_H_
#define _G_GRAPH_ARRAY_H_

#include <glib.h>
#include <libggraph/ggraphtypedefs.h>
#include <libggraph/ggraph.h>

/**
 * g_graph_array_new:
 *
 * Convenience type-safe macro, creating a new #GGraphArray.
 *
 * Returns: newly created array.
 */
#define g_graph_array_new() \
((GGraphArray*)(g_ptr_array_new()))

/**
 * g_graph_array_sized_new:
 * @num: size of array.
 *
 * Convenience type-safe macro, creating a new #GGraphArray with given size.
 *
 * Returns: newly created array.
 */
#define g_graph_array_sized_new(num) \
((GGraphArray*)(g_ptr_array_sized_new(num)))

/**
 * g_graph_array_index:
 * @array: an array.
 * @index: index of the #GGraph to return.
 *
 * Convenience type-safe macro, returning a #GGraph at given index.
 *
 * Returns: #GGraph at given index.
 */
#define g_graph_array_index(array, index) \
((GGraph*)(g_ptr_array_index((array), (index))))

/**
 * g_graph_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe macro, freeing an array. If @free_segment is %TRUE,
 * then underlying pointer array will be freed and %NULL will be returned,
 * otherwise this pointer array will be returned.
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to array of
 * #GGraph is returned - it should be freed with g_free().
 */
#define g_graph_array_free(array, free_segment) \
((GGraph**)(g_ptr_array_free((array), (free_segment))))

void
g_graph_array_add(GGraphArray* array,
                  GGraph* graph);

#endif /* _G_GRAPH_ARRAY_H_ */
