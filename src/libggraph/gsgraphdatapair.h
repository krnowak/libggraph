#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_SGRAPH_DATA_PAIR_H_
#define _G_SGRAPH_DATA_PAIR_H_

#include <glib.h>

G_BEGIN_DECLS

/**
 * GSGraphDataPair:
 *
 * Convenience struct holding two #gpointer variables. Used mainly for
 * g_sgraph_construct().
 */
typedef struct _GSGraphDataPair GSGraphDataPair;

struct _GSGraphDataPair
{
  gpointer first;
  gpointer second;
};

GSGraphDataPair*
g_sgraph_data_pair_new(gpointer first,
                       gpointer second) _G_GNUC_WARN_UNUSED_RESULT;

GSGraphDataPair*
g_sgraph_data_pair_copy(GSGraphDataPair* data_pair) _G_GNUC_WARN_UNUSED_RESULT;

void
g_sgraph_data_pair_free(GSGraphDataPair* data_pair);

gpointer
g_sgraph_data_pair_get_first(GSGraphDataPair* data_pair);

gpointer
g_sgraph_data_pair_get_second(GSGraphDataPair* data_pair);

G_END_DECLS

#endif /* _G_SGRAPH_DATA_PAIR_H_ */
