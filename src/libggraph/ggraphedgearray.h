#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_EDGE_ARRAY_H_
#define _G_GRAPH_EDGE_ARRAY_H_

#include <glib.h>
#include <libggraph/ggraphedge.h>

/**
 * GGraphEdgeArray:
 *
 * An array holding pointers to #GGraphEdge instances. It is a #GPtrArray, so
 * its functions can be used on this type.
 */
typedef GPtrArray GGraphEdgeArray;

/**
 * g_graph_edge_array_new:
 *
 * Convenience type-safe macro, creating a new #GGraphEdgeArray.
 *
 * Returns: newly created array.
 */
#define g_graph_edge_array_new() \
((GGraphEdgeArray*)(g_ptr_array_new()))

/**
 * g_graph_edge_array_index:
 * @array: an array.
 * @index: index of the #GGraphEdge to return.
 *
 * Convenience type-safe macro, returning a #GGraphEdge at given index.
 *
 * Returns: #GGraphEdge at given index.
 */
#define g_graph_edge_array_index(array, index) \
((GGraphEdge*)(g_ptr_array_index((array), (index))))

/**
 * g_graph_edge_array_add:
 * @array: an array.
 * @pointer: pointer to add to @array.
 *
 * Convenience type-safe macro, adding pointer to array.
 * FIXME: make it a function, it is not type-safe now.
 */
#define g_graph_edge_array_add(array, pointer) \
(g_ptr_array_add((array), (pointer)))

/**
 * g_graph_edge_array_remove:
 * @array: an array.
 * @pointer: pointer to be removed from @array.
 *
 * Convenience type-safe macro, removing pointer from array, preserving order.
 * FIXME: make it a function, it is not type-safe now.
 */
#define g_graph_edge_array_remove(array, pointer) \
(g_ptr_array_remove((array), (pointer)))

/**
 * g_graph_edge_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe macro, freeing an array. If @free_segment is %TRUE,
 * then underlying pointer array will be freed and %NULL will be returned,
 * otherwise this pointer array will be returned.
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to array of
 * #GGraphEdge is returned - it should be freed with g_free().
 */
#define g_graph_edge_array_free(array, free_segment) \
((GGraphEdge**)(g_ptr_array_free((array), (free_segment))))

#endif // _G_GRAPH_EDGE_ARRAY_H_
