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

#define g_sgraph_array_new() \
((GSGraphArray*)(g_ptr_array_new()))

#define g_sgraph_array_sized_new(num) \
((GSGraphArray*)(g_ptr_array_sized_new(num)))

#define g_sgraph_array_index(array, index) \
((GSGraph*)(g_ptr_array_index((array), (index))))

#define g_sgraph_array_free(array, free_segment) \
(g_ptr_array_free((array), (free_segment)))

#define g_sgraph_array_add(array, pointer) \
(g_ptr_array_add((array), (pointer)))

#define g_sgraph_array_remove_index(array, index) \
((GSGraph*)(g_ptr_array_remove_index((array), (index))))

#endif // _G_SGRAPH_ARRAY_H_
