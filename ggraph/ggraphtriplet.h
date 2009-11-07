#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_TRIPLET_H_
#define _G_GRAPH_TRIPLET_H_

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _GGraphTriplet GGraphTriplet;

struct _GGraphTriplet
{
  GValue* tail_data;
  GValue* head_data;
  GValue* edge_data;
};

#define G_TYPE_GRAPH_TRIPLET (g_graph_triplet_type())

GType
g_graph_triplet_get_type(void) G_GNUC_CONST;

GGraphTriplet*
g_graph_triplet_new(GValue* tail_data
                    GValue* head_data,
                    GValue* edge_data);

GGraphTriplet*
g_graph_triplet_copy(const GGraphTriplet* triplet);

void
g_graph_triplet_free(GGraphTriplet* triplet);

void
g_graph_triplet_freev(GGraphTriplet** triplets,
                      gint n_triplets);

G_END_DECLS

#endif /* _G_GRAPH_TRIPLET_H_ */
