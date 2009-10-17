#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_ARRAY_H_
#define _G_SGRAPH_ARRAY_H_

#include <glib.h>
#include <libggraph/gsgraphtypedefs.h>
#include <libggraph/gsgraph.h>

G_BEGIN_DECLS

/**
 * GSGraphArray:
 *
 * An array holding pointers to #GSGraph instances. It is a #GPtrArray, so its
 * function can be safely used on this type.
 */

GSGraphArray*
g_sgraph_array_new(void);
/* 
#define g_sgraph_array_new() \
((GSGraphArray*)(g_ptr_array_new()))
*/

GSGraphArray*
g_sgraph_array_sized_new(guint reserved_size);
/*
#define g_sgraph_array_sized_new(num) \
((GSGraphArray*)(g_ptr_array_sized_new(num)))
*/

/**
 * g_sgraph_array_index:
 * @array: an array.
 * @index: index of the #GSGraph to return.
 *
 * Convenience type-safe macro returning a #GSGraph at given index.
 *
 * See also: g_ptr_array_index()
 *
 * Returns: #GGraph at given index.
 */
#define g_sgraph_array_index(array, index) \
((GSGraph*)(g_ptr_array_index((array), (index))))

GSGraph**
g_sgraph_array_free(GSGraphArray* array,
                    gboolean free_segment);
/*
#define g_sgraph_array_free(array, free_segment) \
(g_ptr_array_free((array), (free_segment)))
*/

void
g_sgraph_array_add(GSGraphArray* array,
                   GSGraph* sgraph);

GSGraph*
g_sgraph_array_remove_index(GSGraphArray* array,
                            guint index);

GSGraph*
g_sgraph_array_remove_index_fast(GSGraphArray* array,
                                 guint index);
/*
#define g_sgraph_array_remove_index_fast(array, index) \
((GSGraph*)(g_ptr_array_remove_index_fast((array), (index))))
*/

G_END_DECLS

#endif /* _G_SGRAPH_ARRAY_H_ */
