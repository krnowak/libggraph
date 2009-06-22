#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_ARRAY_H_
#define _G_SGRAPH_ARRAY_H_

#include <glib.h>
#include <libggraph/gsgraph.h>

/**
 * GSGraphArray:
 *
 * An array holding pointers to #GSGraph instances. It is a #GPtrArray, so its
 * function can be safely used on this type.
 */
typedef GPtrArray GSGraphArray;

/**
 * g_sgraph_array_new:
 *
 * Convenience type-safe macro, creating a new #GSGraphArray.
 *
 * Returns: newly created array.
 */
#define g_sgraph_array_new() \
((GSGraphArray*)(g_ptr_array_new()))

/**
 * g_sgraph_array_sized_new:
 * @num: size of array.
 *
 * Convenience type-safe macro, creating a new #GSGraphArray with given size.
 *
 * Returns: newly created array.
 */
#define g_sgraph_array_sized_new(num) \
((GSGraphArray*)(g_ptr_array_sized_new(num)))

/**
 * g_sgraph_array_index:
 * @array: an array.
 * @index: index of the #GSGraph to return.
 *
 * Convenience type-safe macro, returning a #GSGraph at given index.
 *
 * Returns: #GGraph at given index.
 */
#define g_sgraph_array_index(array, index) \
((GSGraph*)(g_ptr_array_index((array), (index))))

/**
 * g_sgraph_array_free:
 * @array: an array to free.
 * @free_segment: %TRUE if the actual pointer array has to be freed.
 *
 * Convenience type-safe macro, freeing an array. If @free_segment is %TRUE,
 * then underlying pointer array will be freed and %NULL will be returned,
 * otherwise this pointer array will be returned.
 *
 * Returns: %NULL if @free_segment was %TRUE, otherwise pointer to array of
 * #GSGraph is returned - it should be freed with g_free().
 */
#define g_sgraph_array_free(array, free_segment) \
(g_ptr_array_free((array), (free_segment)))

/**
 * g_sgraph_array_add:
 * @array: an array.
 * @pointer: pointer to add to @array.
 *
 * Convenience type-safe macro, adding pointer to array.
 * FIXME: make it a function, it is not type-safe now.
 */
#define g_sgraph_array_add(array, pointer) \
(g_ptr_array_add((array), (pointer)))

/**
 * g_sgraph_array_remove_index:
 * @array: an array.
 * @index: index to #GSGraph to be removed.
 *
 * Convenience type-safe macro, removing pointer from array at given index,
 * preserving order.
 * FIXME: make it a function, it is not type-safe now.
 */
#define g_sgraph_array_remove_index(array, index) \
((GSGraph*)(g_ptr_array_remove_index((array), (index))))

#endif // _G_SGRAPH_ARRAY_H_
