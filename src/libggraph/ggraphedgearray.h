#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_EDGE_ARRAY_H_
#define _G_GRAPH_EDGE_ARRAY_H_

#include <glib.h>
#include <libggraph/ggraphtypedefs.h>
#include <libggraph/ggraphedge.h>

G_BEGIN_DECLS

/**
 * GGraphEdgeArray:
 *
 * Contains the public fields of an array.
 */

GGraphEdgeArray*
g_graph_edge_array_new(void);
/*
#define g_graph_edge_array_new() \
((GGraphEdgeArray*)(g_ptr_array_new()))
*/

GGraphEdgeArray*
g_graph_edge_array_sized_new(guint reserved_size);

/**
 * g_graph_edge_array_index:
 * @array: an array.
 * @index: index of the #GGraphEdge to return.
 *
 * Convenience type-safe macro, returning a #GGraphEdge at given index.
 *
 * See also: g_ptr_array_index()
 *
 * Returns: #GGraphEdge at given index.
 */
#define g_graph_edge_array_index(array, index) \
((GGraphEdge*)(g_ptr_array_index((array), (index))))

void
g_graph_edge_array_add(GGraphEdgeArray* array,
                       GGraphEdge* edge);

gboolean
g_graph_edge_array_remove(GGraphEdgeArray* array,
                          GGraphEdge* edge);

GGraphEdge**
g_graph_edge_array_free(GGraphEdgeArray* array,
                        gboolean free_segment);
/*
#define g_graph_edge_array_free(array, free_segment) \
((GGraphEdge**)(g_ptr_array_free((array), (free_segment))))
*/

G_END_DECLS

#endif /* _G_GRAPH_EDGE_ARRAY_H_ */
