#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

/*
 * This header exist because there is circular dependency:
 * .-> GSGraph -> GSGraphArray -.
 * `----------------------------'
 * So here exists typedefs to these structs. It should be enough, because
 * none of them needs information about sizes of other structs.
 */

#include <glib.h>

#ifndef _G_SGRAPH_TYPEDEFS_H_
#define _G_SGRAPH_TYPEDEFS_H_

G_BEGIN_DECLS

/**
 * GSGraphArray:
 *
 * An array holding pointers to #GSGraph instances. It is a #GPtrArray, so its
 * function can be safely used on this type.
 */
typedef GPtrArray GSGraphArray;

typedef struct _GSGraph GSGraph;

G_END_DECLS

#endif /* _G_SGRAPH_TYPEDEFS_H_ */
