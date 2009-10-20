#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_ARRAY_H_
#define _G_GRAPH_ARRAY_H_

#include <glib.h>
#include <libggraph/ggraphtypedefs.h>
#include <libggraph/ggraph.h>

G_BEGIN_DECLS

/**
 * GGraphArray:
 *
 * Contains the public fields of an array.
 */

GGraphArray*
g_graph_array_new(void);
/*
#define g_graph_array_new() \
((GGraphArray*)(g_ptr_array_new()))
*/

GGraphArray*
g_graph_array_sized_new(guint reserved_size);
/*
#define g_graph_array_sized_new(num) \
((GGraphArray*)(g_ptr_array_sized_new(num)))
*/

/**
 * g_graph_array_index:
 * @array: an array.
 * @index: index of the #GGraph to return.
 *
 * Convenience type-safe macro, returning a #GGraph at given index.
 *
 * See also: g_ptr_array_index()
 *
 * Returns: #GGraph at given index.
 */
#define g_graph_array_index(array, index) \
((GGraph*)(g_ptr_array_index((array), (index))))

GGraph**
g_graph_array_free(GGraphArray* array,
                   gboolean free_segment);
/*
#define g_graph_array_free(array, free_segment) \
((GGraph**)(g_ptr_array_free((array), (free_segment))))
*/

void
g_graph_array_add(GGraphArray* array,
                  GGraph* graph);

GGraph*
g_graph_array_remove_index_fast(GGraphArray* array,
                                guint index);
/*
#define g_graph_array_remove_index_fast(array, index) \
((GGraph*)(g_ptr_array_remove_index_fast((array), (index))))
*/

G_END_DECLS

#endif /* _G_GRAPH_ARRAY_H_ */
