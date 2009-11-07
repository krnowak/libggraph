#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_NODE_H_
#define _G_GRAPH_NODE_H_

#include <glib.h>
#include <glib-object.h>

#include <libggraph/ggraphtypes.h>

G_BEGIN_DECLS

#define G_TYPE_GRAPH_NODE (g_graph_node_get_type())
#define G_GRAPH_NODE(object) (G_TYPE_CHECK_INSTANCE_CAST((object), G_TYPE_GRAPH_NODE, GGraphNode))
#define G_GRAPH_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_GRAPH_NODE, GGraphNodeClass))
#define G_IS_GRAPH_NODE(object) (G_TYPE_CHECK_INSTANCE_TYPE((object), G_TYPE_GRAPH_NODE))
#define G_IS_GRAPH_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_GRAPH_NODE))
#define G_GRAPH_NODE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_GRAPH_NODE, GGraphNodeClass))

/* TODO: Fix docs. */
/**
 * GGraph:
 * @data: pointer to user defined data.
 * @edges: an array containing pointers to edges to neighbours.
 *
 * Contains public fields of a node.
 */
typedef struct _GGraphNodePrivate GGraphNodePrivate;

struct _GGraphNode
{
  GGraphData parent;
  
  GGraphNodePrivate* priv;
};

struct _GGraphNodeClass
{
  GGraphDataClass parent_class;
}

GType
g_graph_node_get_type(void) G_GNUC_CONST;

GGraphNode*
g_graph_node_new(GPtrArray* edges,
                 GValue* data,
                 Graph* graph);

const GPtrArray*
g_graph_node_get_edges(GGraphNode* node);

void
g_graph_node_set_edges(GGraphNode* node,
                       GPtrArray* edges);

G_END_DECLS

#endif /* _G_GRAPH_NODE_H_ */
