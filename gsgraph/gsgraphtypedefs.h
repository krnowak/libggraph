#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_G_SGRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <gsgraph/gsgraph.h> can be included directly."
#endif

/*
 * This header exist because there is circular dependency:
 * .-> GSGraphNode -> GSGraphNodeArray -.
 * `------------------------------------'
 * So here exists typedefs to these structs. It should be enough, because
 * none of them needs information about sizes of other structs.
 */

#ifndef _G_SGRAPH_TYPEDEFS_H_
#define _G_SGRAPH_TYPEDEFS_H_

#include <glib.h>

G_BEGIN_DECLS

typedef GPtrArray GSGraphNodeArray;

typedef struct _GSGraphNode GSGraphNode;

G_END_DECLS

#endif /* _G_SGRAPH_TYPEDEFS_H_ */
