#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_DIRECTED_EDGE_H_
#define _G_GRAPH_DIRECTED_EDGE_H_

#include <glib.h>
#include <glib-object.h>

#include <libggraph/ggraphtypes.h>

G_BEGIN_DECLS

#define G_TYPE_GRAPH_DIRECTED_EDGE (g_graph_directed_edge_get_type())
#define G_GRAPH_DIRECTED_EDGE(object) (G_TYPE_CHECK_INSTANCE_CAST((object), G_TYPE_GRAPH_DIRECTED_EDGE, GGraphDirectedEdge))
#define G_GRAPH_DIRECTED_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_GRAPH_DIRECTED_EDGE, GGraphDirectedEdgeClass))
#define G_IS_GRAPH_DIRECTED_EDGE(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), G_TYPE_GRAPH_DIRECTED_EDGE))
#define G_IS_GRAPH_DIRECTED_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_GRAPH_DIRECTED_EDGE))
#define G_GRAPH_DIRECTED_EDGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_GRAPH_DIRECTED_EDGE, GGraphDirectedEdgeClass))

/**
 * GGraphDirectedEdge:
 * @first: first end of this directed_edge.
 * @second: second end of this directed_edge.
 * @connected: flag describing interconnection between @first and @second.
 * @data: data contained by this directed_edge.
 *
 * An directed edge from the node containing it to the @graph.
 */

struct _GGraphDirectedEdge
{
  GGraphDirectedMultiEdge parent;
};

struct _GGraphDirectedEdgeClass
{
  GGraphDirectedMultiEdgeClass parent_class;
}

GType
g_graph_directed_edge_get_type(void) G_GNUC_CONST;

GGraphDirectedEdge*
g_graph_directed_edge_new(GGraphNode* tail,
                          GGraphNode* head,
                          GValue* data
                          GGraph* graph);

const GGraphNode*
g_graph_directed_edge_get_tail(GGraphDirectedEdge* directed_edge);

void
g_graph_directed_edge_set_tail(GGraphDirectedEdge* directed_edge,
                               GGraphNode* node);

const GGraphNode*
g_graph_directed_edge_get_head(GGraphDirectedEdge* directed_edge);

void
g_graph_directed_edge_set_head(GGraphDirectedEdge* directed_edge,
                               GGraphNode* node);

gboolean
g_graph_directed_edge_is_loop(GGraphDirectedEdge* directed_edge);

gboolean
g_graph_directed_edge_is_link(GGraphDirectedEdge* directed_edge);

G_END_DECLS

#endif /* _G_GRAPH_DIRECTED_EDGE_H_ */
