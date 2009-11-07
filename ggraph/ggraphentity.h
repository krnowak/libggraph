#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_H_
#define _G_GRAPH_H_

#include <glib.h>
#include <glib-object.h>

#include <libggraph/ggraphtype.h>
#include <libggraph/ggraphoptions.h>
#include <libggraph/ggraphtypes.h>
#include <libggraph/ggraphnode.h>
#include <libggraph/ggraphedge.h>
#include <libggraph/ggraphtriplet.h>

G_BEGIN_DECLS

#define G_TYPE_GRAPH (g_graph_get_type())
#define G_GRAPH(object) (G_TYPE_CHECK_INSTANCE_CAST((object), G_TYPE_GRAPH, GGraph))
#define G_GRAPH_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_GRAPH, GGraphClass))
#define G_IS_GRAPH(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), G_TYPE_GRAPH))
#define G_IS_GRAPH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_GRAPH))
#define G_GRAPH_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_GRAPH, GGraphClass))

/* TODO: fix docs. */
/**
 * GGraph:
 * @data: pointer to user defined data.
 * @edges: an array containing pointers to edges to neighbours.
 *
 * Contains public fields of a node.
 */

typedef struct _GGraphPrivate GGraphPrivate;

struct _GGraph
{
  GObject parent;
  
  GGraphPrivate* priv;
};

struct _GGraphClass
{
  GObjectClass parent_class;
};

typedef enum
{
  G_GRAPH_ERROR_FAILED,
  G_GRAPH_ERROR_NOT_OWNER,
  G_GRAPH_ERROR_POLICY_VIOLATION,
  G_GRAPH_ERROR_INCOHERENCY
} GGraphError;

#define G_GRAPH_ERROR (g_graph_error_quark())

GQuark
g_graph_error_quark(void) G_GNUC_CONST;

GType
g_graph_get_type(void) G_GNUC_CONST;

GGraph*
g_graph_new(GGraphType type,
            GGraphOptions options,
            GType edge_data_type,
            GType node_data_type);

GGraph*
g_graph_copy(GGraph* graph);

GGraphNode*
g_graph_create_node(GGraph* graph);

GGraphEdge*
g_graph_connect_nodes(GGraph* graph,
                      GGraphNode* tail_node,
                      GGraphNode* head_node,
                      GError** error);

GGraphNode*
g_graph_construct(GGraph* graph,
                  GGraphTriplets** triplets,
                  gint n_triplets,
                  GError** error);

typedef void (*GGraphNodeFunc)(GGraphNode* node,
                               gpointer user_data);

void
ggraph_foreach_node(GGraph* graph,
                    GGraphNodeFunc func,
                    gpointer user_data,
                    GError** error);

void
ggraph_foreach_connected_node(GGraph* graph,
                              GGraphNode* node,
                              GGraphNodeFunc func,
                              gpointer user_data,
                              GError** error);

typedef void (*GGraphEdgeFunc)(GGraphEdge* edge,
                               gpointer user_data);

void
ggraph_foreach_edge(GGraph* graph,
                    GGraphEdgeFunc func,
                    gpointer user_data,
                    GError** error);

void
ggraph_foreach_connected_edge(GGraph* graph,
                              GGraphEdge* edge,
                              GGraphEdgeFunc func,
                              gpointer user_data,
                              GError** error);

guint
g_graph_order(GGraph* graph);

guint
g_graph_size(GGraph* graph);

GValue*
ggraph_remove_edge(GGraph* graph,
                   GGraphEdge* edge,
                   GError** error);

void
ggraph_remove_node(GGraph* graph,
                   GGraphNode* node,
                   GError** error);

typedef gboolean (*GGraphNodeFindFunc)(GGraphNode* node,
                                       gpointer user_data);

GPtrArray*
ggraph_find_nodes(GGraph* graph,
                  GGraphNodeFindFunc func,
                  gpointer user_data);

GPtrArray*
ggraph_find_connected_nodes(GGraph* graph,
                            GGraphNode* node,
                            GGraphNodeFindFunc func,
                            gpointer user_data);

typedef gboolean (*GGraphEdgeFindFunc)(GGraphEdge* edge,
                                       gpointer user_data);

GPtrArray*
ggraph_find_edges(GGraph* graph,
                  GGraphNodeFindFunc func,
                  gpointer user_data)

GPtrArray*
ggraph_find_connected_edges(GGraph* graph,
                            GGraphEdge* edge,
                            GGraphEdgeFindFunc func,
                            gpointer user_data);

GGraphNode*
ggraph_get_first_node(GGraph* graph);

GGraphNode*
ggraph_get_last_node(GGraph* graph);

GGraphEdge*
ggraph_get_first_edge(GGraph* graph);

GGraphEdge*
ggraph_get_last_edge(GGraph* graph);

guint
ggraph_multiplicity(GGraph* graph);

guint
ggraph_edge_multiplicity(GGraph* graph,
                         GGraphEdge* edge,
                         GError** error);

G_END_DECLS

#endif /* _G_GRAPH_H_ */
