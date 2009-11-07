#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_UNDIRECTED_EDGE_H_
#define _G_GRAPH_UNDIRECTED_EDGE_H_

#include <glib.h>
#include <glib-object.h>

#include <libggraph/ggraphtypes.h>

G_BEGIN_DECLS

#define G_TYPE_GRAPH_UNDIRECTED_EDGE (g_graph_undirected_edge_get_type())
#define G_GRAPH_UNDIRECTED_EDGE(object) (G_TYPE_CHECK_INSTANCE_CAST((object), G_TYPE_GRAPH_UNDIRECTED_EDGE, GGraphUndirectedEdge))
#define G_GRAPH_UNDIRECTED_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_GRAPH_UNDIRECTED_EDGE, GGraphUndirectedEdgeClass))
#define G_IS_GRAPH_UNDIRECTED_EDGE(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), G_TYPE_GRAPH_UNDIRECTED_EDGE))
#define G_IS_GRAPH_UNDIRECTED_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_GRAPH_UNDIRECTED_EDGE))
#define G_GRAPH_UNDIRECTED_EDGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_GRAPH_UNDIRECTED_EDGE, GGraphUndirectedEdgeClass))

/**
 * GGraphUndirectedEdge:
 * @first: first end of this undirected_edge.
 * @second: second end of this undirected_edge.
 * @connected: flag describing interconnection between @first and @second.
 * @data: data contained by this undirected_edge.
 *
 * An directed edge from the node containing it to the @graph.
 */

struct _GGraphUndirectedEdge
{
  GGraphUndirectedMultiEdge parent;
};

struct _GGraphUndirectedEdgeClass
{
  GGraphUndirectedMultiEdgeClass parent_class;
}

GType
g_graph_undirected_edge_get_type(void) G_GNUC_CONST;

GGraphUndirectedEdge*
g_graph_undirected_edge_new(GGraphNode* first,
                          GGraphNode* second,
                          GValue* data
                          GGraph* graph);

const GGraphNode*
g_graph_undirected_edge_get_first_node(GGraphUndirectedEdge* undirected_edge);

void
g_graph_undirected_edge_set_first_node(GGraphUndirectedEdge* undirected_edge,
                                       GGraphNode* node);

const GGraphNode*
g_graph_undirected_edge_get_second_node(GGraphUndirectedEdge* undirected_edge);

void
g_graph_undirected_edge_set_second_node(GGraphUndirectedEdge* undirected_edge,
                                        GGraphNode* node);

gboolean
g_graph_undirected_edge_is_loop(GGraphUndirectedEdge* undirected_edge);

gboolean
g_graph_undirected_edge_is_link(GGraphUndirectedEdge* undirected_edge);

G_END_DECLS

#endif /* _G_GRAPH_UNDIRECTED_EDGE_H_ */
