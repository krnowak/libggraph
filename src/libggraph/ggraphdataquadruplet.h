#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_DATA_QUADRUPLET_H_
#define _G_GRAPH_DATA_QUADRUPLET_H_

#include <glib.h>

#include <libggraph/ggraphedgeconnected.h>

G_BEGIN_DECLS

/**
 * GGraphDataQuadruplet:
 *
 * Convenience struct holding three #gpointer and one #GGraphEdgeConnected
 * variables. Used mainly for g_graph_construct().
 */
typedef struct _GGraphDataQuadruplet GGraphDataQuadruplet;

struct _GGraphDataQuadruplet
{
  gpointer first;
  gpointer second;
  gpointer data;
  GGraphEdgeConnected connected;
};

GGraphDataQuadruplet*
g_graph_data_quadruplet_new(gpointer first,
                            gpointer second,
                            gpointer data,
                            GGraphEdgeConnected connected) G_GNUC_WARN_UNUSED_RESULT;

GGraphDataQuadruplet*
g_graph_data_quadruplet_copy(GGraphDataQuadruplet* data_quadruplet) G_GNUC_WARN_UNUSED_RESULT;

void
g_graph_data_quadruplet_free(GGraphDataQuadruplet* data_quadruplet);

void
g_graph_data_quadruplet_free_v(GGraphDataQuadruplet** data_quadruplets,
                               gint count);

G_END_DECLS

#endif /* _G_GRAPH_DATA_QUADRUPLET_H_ */
