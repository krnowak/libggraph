#include "ggraphundirectedmultiedge.h"
#include "ggraphnode.h"
/* internal types. */

#define G_GRAPH_UNDIRECTED_MULTI_EDGE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE, GGraphUndirectedMultiEdgePrivate))

struct _GGraphUndirectedMultiEdgePrivate
{
  GPtrArray* nodes;
};

enum
{
  PROP_0,
  PROP_NODES
};

/* static function declarations. */

static void
g_graph_undirected_multi_edge_dispose(GObject* object);

static void
g_graph_undirected_multi_edge_finalize(GObject* object);

static void
g_graph_undirected_multi_edge_set_property(GObject* object,
                                         guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec);

static void
g_graph_undirected_multi_edge_get_property(GObject* object,
                                         guint prop_id,
                                         GValue *value,
                                         GParamSpec *pspec);

static gboolean
g_graph_undirected_multi_edge_is_connected_to(GGraphEdge* edge,
                                            GGraphNode* node1,
                                            GGraphNode* node2,
                                            GError** error);

static gboolean
g_graph_undirected_multi_edge_check_node(GGraphNode* node,
                                       GPtrArray* nodes);

G_DEFINE_TYPE(GGraphUndirectedMultiEdge, g_graph_undirected_multi_edge, G_TYPE_GRAPH_EDGE)

/* function definitions. */

GGraphUndirectedMultiEdge*
g_graph_undirected_multi_edge_new(GPtrArray* nodes,
                                  GValue* data,
                                  GGraph* graph)
{
  GObject* object;
  
  object = g_object_new(G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE,
                        "nodes", nodes,
                        "data", data,
                        "graph", graph,
                        NULL);
  
  return G_GRAPH_UNDIRECTED_MULTI_EDGE(object);
}

const GPtrArray*
g_graph_undirected_multi_edge_get_nodes(GGraphUndirectedMultiEdge* undirected_multi_edge)
{
  g_return_val_if_fail(G_IS_GRAPH_UNDIRECTED_MULTI_EDGE(undirected_multi_edge), NULL);
  
  if (!undirected_multi_edge->priv->nodes || !undirected_multi_edge->priv->nodes->len)
  {
    return NULL;
  }
  
  return undirected_multi_edge->priv->nodes;
}

void
g_graph_undirected_multi_edge_set_nodes(GGraphUndirectedMultiEdge* undirected_multi_edge,
                                        GPtrArray* nodes)
{
  GPtrArray* old;
  
  g_return_if_fail(G_IS_GRAPH_UNDIRECTED_MULTI_EDGE(undirected_multi_edge));
  
  old = undirected_multi_edge->priv->nodes;
  
  if (old == nodes || ((!nodes || !nodes->len) && (!old || !old->len)))
  {
    return;
  }
  
  if (nodes)
  {
    undirected_multi_edge->priv->nodes = g_ptr_array_ref(nodes);
  }
  else
  {
    undirected_multi_edge->priv->nodes = g_ptr_array_new_with_free_func(g_object_unref);
  }
  
  if (old)
  {
    g_ptr_array_unref(old);
  }
  
  g_object_notify(undirected_multi_edge, "nodes");
}

/* static function definitions. */

static void
g_graph_undirected_multi_edge_class_init(GGraphUndirectedMultiEdgeClass* undirected_multi_edge_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(undirected_multi_edge_class);
  GGraphEdgeClass* edge_class = G_GRAPH_EDGE_CLASS(undirected_multi_edge_class);

  object_class->finalize = g_graph_undirected_multi_edge_finalize;
  object_class->dispose = g_graph_undirected_multi_edge_dispose;
  object_class->get_property = g_graph_undirected_multi_edge_get_property;
  object_class->set_property = g_graph_undirected_multi_edge_set_property;
  edge_class->is_connected_to = g_graph_undirected_multi_edge_is_connected_to;
  /* TODO: add nicks and blurbs. */
  g_object_class_install_property
    (object_class,
     PROP_TAILS,
     g_param_spec_boxed("nodes", NULL, NULL,
                        G_TYPE_PTR_ARRAY,
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_type_class_add_private(object_class, sizeof(GGraphUndirectedMultiEdgePrivate));
}

static void
g_graph_undirected_multi_edge_init(GGraphUndirectedMultiEdge* undirected_multi_edge)
{
  undirected_multi_edge->priv = G_GRAPH_UNDIRECTED_MULTI_EDGE_GET_PRIVATE(undirected_multi_edge);
  undirected_multi_edge->priv->nodes = g_ptr_array_new_with_free_func(g_object_unref);
}

static void
g_graph_undirected_multi_edge_dispose(GObject* object)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(object);
  
  if (undirected_multi_edge->priv->nodes)
  {
    GPtrArray* old;
    
    old = undirected_multi_edge->priv->nodes;
    undirected_multi_edge->priv->nodes = NULL;
    g_ptr_array_unref(old);
  }
  
  G_OBJECT_CLASS(g_graph_undirected_multi_edge_parent_class)->dispose(object);
}

static void
g_graph_undirected_multi_edge_finalize(GObject* object)
{
  G_OBJECT_CLASS(g_graph_undirected_multi_edge_parent_class)->finalize(object);
}

static void
g_graph_undirected_multi_edge_set_property(GObject* object,
                                           guint prop_id,
                                           const GValue *value,
                                           GParamSpec *pspec)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(object);
  
  switch (prop_id)
  {
    case PROP_NODES:
    {
      g_graph_undirected_multi_edge_set_nodes(undirected_multi_edge,
                                              g_value_get_object(value));
      break;
    }
    default:
    {
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
  }
}

static void
g_graph_undirected_multi_edge_get_property(GObject* object,
                                           guint prop_id,
                                           GValue *value,
                                           GParamSpec *pspec)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(object);

  switch (prop_id)
  {
    case PROP_NODES:
    {
      g_value_set_object(value, undirected_multi_edge->priv->nodes);
      break;
    }
    default:
    {
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
  }
}

static gboolean
g_graph_undirected_multi_edge_is_connected_to(GGraphEdge* edge,
                                              GGraphNode* node1,
                                              GGraphNode* node2,
                                              GError** error)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(edge);
  
  if (g_graph_undirected_multi_edge_check_node(node1, undirected_multi_edge->priv->nodes))
  {
    if (g_graph_undirected_multi_edge_check_node(node2, undirected_multi_edge->priv->nodes))
    {
      return TRUE;
    }
    else
    {
      g_set_error_literal(error, G_GRAPH_EDGE_ERROR,
                          G_GRAPH_EDGE_ERROR_NO_SUCH_NODE,
                          "There is no node in edge like second one.");
    }
  }
  else
  {
    g_set_error_literal(error, G_GRAPH_EDGE_ERROR,
                G_GRAPH_EDGE_ERROR_NO_SUCH_NODE,
                "There is no node in edge like first one.");
  }
  return FALSE;
}

static gboolean
g_graph_undirected_multi_edge_check_node(GGraphNode* node,
                                         GPtrArray* nodes)
{
  guint iter;
  
  for (iter = 0; iter < nodes->len; iter++)
  {
    GGraphNode* test_node;
    
    test_node = g_ptr_array_index(nodes, iter);
    
    if (test_node == node)
    {
      return TRUE;
    }
  }
  return FALSE;
}
