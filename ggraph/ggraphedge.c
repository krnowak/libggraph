#include "ggraphedge.h"
#include "ggraphnode.h"

/* static function declarations. */

static void
g_graph_edge_dispose(GObject* object);

static void
g_graph_edge_finalize(GObject* object);

G_DEFINE_TYPE(GGraphEdge, g_graph_edge, G_TYPE_GRAPH_DATA)

/* function definitions */

GQuark
g_graph_error_quark(void)
{
  static GQuark quark = 0;
  if (G_UNLIKELY(!quark))
  {
    quark = g_quark_from_static_string("g-graph-edge-error-quark");
  }
  return quark;
}

gboolean
g_graph_edge_is_connected_to(GGraphEdge* edge,
                             GGraphNode* node1,
                             GGraphNode* node2,
                             GError** error)
{
  g_return_val_if_fail(G_IS_GRAPH_EDGE(edge), FALSE);
  g_return_val_if_fail(G_IS_GRAPH_NODE(node1), FALSE);
  g_return_val_if_fail(G_IS_GRAPH_NODE(node2), FALSE);
  g_return_val_if_fail((error == NULL) || (*error == NULL), FALSE); 

  return G_GRAPH_EDGE_GET_CLASS(edge)->is_connected_to(edge, node1, node2,
                                                       error);
}

/* static function definitions. */

static void
g_graph_edge_class_init(GGraphEdgeClass* edge_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(edge_class);

  object_class->finalize = g_graph_edge_finalize;
  object_class->dispose = g_graph_edge_dispose;
  object_class->is_connected_to = NULL;
}

static void
g_graph_edge_init(GGraphEdge* edge)
{}

static void
g_graph_edge_dispose(GObject* object)
{
  G_OBJECT_CLASS(g_graph_edge_parent_class)->dispose(object);
}

static void
g_graph_edge_finalize(GObject* object)
{
  G_OBJECT_CLASS(g_graph_edge_parent_class)->finalize(object);
}
