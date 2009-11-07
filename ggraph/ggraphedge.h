#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_EDGE_H_
#define _G_GRAPH_EDGE_H_

#include <glib.h>
#include <glib-object.h>

#include <libggraph/ggraphtypes.h>

G_BEGIN_DECLS

#define G_TYPE_GRAPH_EDGE (g_graph_edge_get_type())
#define G_GRAPH_EDGE(object) (G_TYPE_CHECK_INSTANCE_CAST((object), G_TYPE_GRAPH_EDGE, GGraphEdge))
#define G_GRAPH_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_GRAPH_EDGE, GGraphEdgeClass))
#define G_IS_GRAPH_EDGE(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), G_TYPE_GRAPH_EDGE))
#define G_IS_GRAPH_EDGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_GRAPH_EDGE))
#define G_GRAPH_EDGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_GRAPH_EDGE, GGraphEdgeClass))

/**
 * GGraphEdge:
 * @first: first end of this edge.
 * @second: second end of this edge.
 * @connected: flag describing interconnection between @first and @second.
 * @data: data contained by this edge.
 *
 * An edge from the node containing it to the @graph.
 */

struct _GGraphEdge
{
  GGraphData parent;
};

struct _GGraphEdgeClass
{
  GGraphDataClass parent_class;
  
  /* pure virtual. */
  gboolean (*is_connected_to)(GGraphEdge* edge,
                              GGraphNode* node1,
                              GGraphNode* node2,
                              GError** error);
}

typedef enum
{
  G_GRAPH_EDGE_ERROR_NO_SUCH_NODE
} GGraphEdgeError;

#define G_GRAPH_EDGE_ERROR (g_graph_edge_error_quark())

GQuark
g_graph_edge_error_quark(void) G_GNUC_CONST;

GType
g_graph_edge_get_type(void) G_GNUC_CONST;

gboolean
g_graph_edge_is_connected_to(GGraphEdge* edge,
                             GGraphNode* node1,
                             GGraphNode* node2,
                             GError** error);

G_END_DECLS

#endif /* _G_GRAPH_EDGE_H_ */
