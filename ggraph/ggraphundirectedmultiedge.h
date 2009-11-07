#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_UNDIRECTED_MULTI_EDGE_H_
#define _G_GRAPH_UNDIRECTED_MULTI_EDGE_H_

#include <glib.h>
#include <glib-object.h>

#include <libggraph/ggraphtypes.h>

G_BEGIN_DECLS

#define G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE (g_graph_undirected_multi_edge_get_type())
#define G_GRAPH_UNDIRECTED_MULTI_EDGE(object) (G_TYPE_CHECK_INSTANCE_CAST((object), G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE, GGraphUndirectedMultiEdge))
#define G_GRAPH_UNDIRECTED_MULTI_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE, GGraphUndirectedMultiEdgeClass))
#define G_IS_GRAPH_UNDIRECTED_MULTI_EDGE(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE))
#define G_IS_GRAPH_UNDIRECTED_MULTI_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE))
#define G_GRAPH_UNDIRECTED_MULTI_EDGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE, GGraphUndirectedMultiEdgeClass))

/**
 * GGraphUndirectedMultiEdge:
 * @first: first end of this undirected_multi_edge.
 * @second: second end of this undirected_multi_edge.
 * @connected: flag describing interconnection between @first and @second.
 * @data: data contained by this undirected_multi_edge.
 *
 * An undirected_multi_edge from the node containing it to the @graph.
 */

typedef struct _GGraphUndirectedMultiEdgePrivate GGraphUndirectedMultiEdgePrivate;

struct _GGraphUndirectedMultiEdge
{
  GGraphEdge parent;
  
  GGraphUndirectedMultiEdgePrivate* priv;
};

struct _GGraphUndirectedMultiEdgeClass
{
  GGraphEdgeClass parent_class;
};

GType
g_graph_undirected_multi_edge_get_type(void) G_GNUC_CONST;

GGraphUndirectedMultiEdge*
g_graph_undirected_multi_edge_new(GPtrArray* nodes,
                                  GValue* data,
                                  GGraph* graph);

const GPtrArray*
g_graph_undirected_multi_edge_get_nodes(GGraphUndirectedMultiEdge* undirected_multi_edge);

void
g_graph_undirected_multi_edge_set_nodes(GGraphUndirectedMultiEdge* undirected_multi_edge,
                                        GPtrArray* nodes);

G_END_DECLS

#endif /* _G_GRAPH_UNDIRECTED_MULTI_EDGE_H_ */
