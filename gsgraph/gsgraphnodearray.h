#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsgraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_NODE_ARRAY_H_
#define _G_SGRAPH_NODE_ARRAY_H_

#include <glib.h>

#include <gsgraph/gsgraphtypedefs.h>
#include <gsgraph/gsgraphnode.h>

G_BEGIN_DECLS

/**
 * GSGraphNodeArray:
 *
 * An array holding pointers to #GSGraphNode instances. It is a #GPtrArray, so
 * its functions can be safely used on this type.
 */

GSGraphNodeArray*
g_sgraph_node_array_new(void);

GSGraphNodeArray*
g_sgraph_node_array_sized_new(guint reserved_size);

/**
 * g_sgraph_node_array_index:
 * @array: an array.
 * @index: index of the #GSGraph to return.
 *
 * Convenience type-safe macro returning a #GSGraph at given index.
 *
 * See also: g_ptr_array_index()
 *
 * Returns: #GGraph at given index.
 */
#define g_sgraph_node_array_index(array, index) \
((GSGraphNode*)(g_ptr_array_index((array), (index))))

GSGraphNode**
g_sgraph_node_array_free(GSGraphNodeArray* array,
                         gboolean free_segment);

void
g_sgraph_node_array_add(GSGraphNodeArray* array,
                        GSGraphNode* sgraph);

GSGraphNode*
g_sgraph_node_array_remove_index(GSGraphNodeArray* array,
                                 guint index);

GSGraphNode*
g_sgraph_node_array_remove_index_fast(GSGraphNodeArray* array,
                                      guint index);

G_END_DECLS

#endif /* _G_SGRAPH_NODE_ARRAY_H_ */
