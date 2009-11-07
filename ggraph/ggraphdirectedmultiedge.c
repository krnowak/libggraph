#include "ggraphdirectedmultiedge.h"
#include "ggraphnode.h"
/* internal types. */

#define G_GRAPH_DIRECTED_MULTI_EDGE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), G_TYPE_GRAPH_DIRECTED_MULTI_EDGE, GGraphDirectedMultiEdgePrivate))

struct _GGraphDirectedMultiEdgePrivate
{
  GPtrArray* tails;
  GPtrArray* heads;
};

enum
{
  PROP_0,
  PROP_TAILS,
  PROP_HEADS
};

/* static function declarations. */

static void
g_graph_directed_multi_edge_dispose(GObject* object);

static void
g_graph_directed_multi_edge_finalize(GObject* object);

static void
g_graph_directed_multi_edge_set_property(GObject* object,
                                         guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec);

static void
g_graph_directed_multi_edge_get_property(GObject* object,
                                         guint prop_id,
                                         GValue *value,
                                         GParamSpec *pspec);

static gboolean
g_graph_directed_multi_edge_is_connected_to(GGraphEdge* edge,
                                            GGraphNode* node1,
                                            GGraphNode* node2,
                                            GError** error);

static gboolean
g_graph_directed_multi_edge_check_node(GGraphNode* node,
                                       GPtrArray* nodes);

G_DEFINE_TYPE(GGraphDirectedMultiEdge, g_graph_directed_multi_edge, G_TYPE_GRAPH_EDGE)

/* function definitions. */

GGraphDirectedMultiEdge*
g_graph_directed_multi_edge_new(GPtrArray* tails,
                                GPtrArray* heads,
                                GValue* data,
                                GGraph* graph)
{
  GObject* object;
  
  object = g_object_new(G_TYPE_GRAPH_DIRECTED_MULTI_EDGE,
                        "tails", tails,
                        "heads", heads,
                        "data", data,
                        "graph", graph,
                        NULL);
  
  return G_GRAPH_DIRECTED_MULTI_EDGE(object);
}

const GPtrArray*
g_graph_directed_multi_edge_get_tails(GGraphDirectedMultiEdge* directed_multi_edge)
{
  g_return_val_if_fail(G_IS_GRAPH_DIRECTED_MULTI_EDGE(directed_multi_edge), NULL);
  
  if (!directed_multi_edge->priv->tails || !directed_multi_edge->priv->tails->len)
  {
    return NULL;
  }
  
  return directed_multi_edge->priv->tails;
}

void
g_graph_directed_multi_edge_set_tails(GGraphDirectedMultiEdge* directed_multi_edge,
                                      GPtrArray* nodes)
{
  GPtrArray* old;
  
  g_return_if_fail(G_IS_GRAPH_DIRECTED_MULTI_EDGE(directed_multi_edge));
  
  old = directed_multi_edge->priv->tails;
  
  if (old == nodes || ((!nodes || !nodes->len) && (!old || !old->len)))
  {
    return;
  }
  
  if (nodes)
  {
    directed_multi_edge->priv->tails = g_ptr_array_ref(nodes);
  }
  else
  {
    directed_multi_edge->priv->tails = g_ptr_array_new_with_free_func(g_object_unref);
  }
  
  if (old)
  {
    g_ptr_array_unref(old);
  }
  
  g_object_notify(directed_multi_edge, "tails");
}

const GPtrArray*
g_graph_directed_multi_edge_get_heads(GGraphDirectedMultiEdge* directed_multi_edge)
{
  g_return_val_if_fail(G_IS_GRAPH_DIRECTED_MULTI_EDGE(directed_multi_edge), NULL);
  
  if (!directed_multi_edge->priv->heads || !directed_multi_edge->priv->heads->len)
  {
    return NULL;
  }
  
  return directed_multi_edge->priv->heads;
}

void
g_graph_directed_multi_edge_set_heads(GGraphDirectedMultiEdge* directed_multi_edge,
                                      GPtrArray* nodes)
{
  GPtrArray* old;
  
  g_return_if_fail(G_IS_GRAPH_DIRECTED_MULTI_EDGE(directed_multi_edge));
  
  old = directed_multi_edge->priv->heads;
  
  if ((old == nodes) || ((!nodes || !nodes->len) && (!old || !old->len)))
  {
    return;
  }
  
  if (nodes)
  {
    directed_multi_edge->priv->heads = g_ptr_array_ref(nodes);
  }
  else
  {
    directed_multi_edge->priv->heads = g_ptr_array_new_with_free_func(g_object_unref);
  }
  
  if (old)
  {
    g_ptr_array_unref(old);
  }
  
  g_object_notify(directed_multi_edge, "heads");
}

/* static function definitions. */

static void
g_graph_directed_multi_edge_class_init(GGraphDirectedMultiEdgeClass* directed_multi_edge_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(directed_multi_edge_class);
  GGraphEdgeClass* edge_class = G_GRAPH_EDGE_CLASS(directed_multi_edge_class);

  object_class->finalize = g_graph_directed_multi_edge_finalize;
  object_class->dispose = g_graph_directed_multi_edge_dispose;
  object_class->get_property = g_graph_directed_multi_edge_get_property;
  object_class->set_property = g_graph_directed_multi_edge_set_property;
  edge_class->is_connected_to = g_graph_directed_multi_edge_is_connected_to;
  /* TODO: add nicks and blurbs. */
  g_object_class_install_property
    (object_class,
     PROP_TAILS,
     g_param_spec_boxed("tails", NULL, NULL,
                         G_TYPE_PTR_ARRAY,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_HEADS,
     g_param_spec_boxed("heads", NULL, NULL,
                        G_TYPE_PTR_ARRAY,
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_type_class_add_private(object_class, sizeof(GGraphDirectedMultiEdgePrivate));
}

static void
g_graph_directed_multi_edge_init(GGraphDirectedMultiEdge* directed_multi_edge)
{
  directed_multi_edge->priv = G_GRAPH_DIRECTED_MULTI_EDGE_GET_PRIVATE(directed_multi_edge);
  directed_multi_edge->priv->tails = g_ptr_array_new_with_free_func(g_object_unref);
  directed_multi_edge->priv->heads = g_ptr_array_new_with_free_func(g_object_unref);
}

static void
g_graph_directed_multi_edge_dispose(GObject* object)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(object);
  
  if (directed_multi_edge->priv->tails)
  {
    GPtrArray* old;
    
    old = directed_multi_edge->priv->tails;
    directed_multi_edge->priv->tails = NULL;
    g_ptr_array_unref(old);
  }
  
  if (directed_multi_edge->priv->heads)
  {
    GPtrArray* old;
    
    old = directed_multi_edge->priv->heads;
    directed_multi_edge->priv->heads = NULL;
    g_ptr_array_unref(old);
  }
  
  G_OBJECT_CLASS(g_graph_directed_multi_edge_parent_class)->dispose(object);
}

static void
g_graph_directed_multi_edge_finalize(GObject* object)
{
  G_OBJECT_CLASS(g_graph_directed_multi_edge_parent_class)->finalize(object);
}

static void
g_graph_directed_multi_edge_set_property(GObject* object,
                                         guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(object);
  
  switch (prop_id)
  {
    case PROP_TAILS:
    {
      g_graph_directed_multi_edge_set_tails(directed_multi_edge,
                                            g_value_get_object(value));
      break;
    }
    case PROP_HEADS:
    {
      g_graph_directed_multi_edge_set_heads(directed_multi_edge,
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
g_graph_directed_multi_edge_get_property(GObject* object,
                                         guint prop_id,
                                         GValue *value,
                                         GParamSpec *pspec)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(object);

  switch (prop_id)
  {
    case PROP_TAILS:
    {
      g_value_set_object(value, directed_multi_edge->priv->tails);
      break;
    }
    case PROP_HEADS:
    {
      g_value_set_object(value, directed_multi_edge->priv->heads);
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
g_graph_directed_multi_edge_is_connected_to(GGraphEdge* edge,
                                            GGraphNode* node1,
                                            GGraphNode* node2,
                                            GError** error)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(edge);
  
  if (g_graph_directed_multi_edge_check_node(node1, directed_multi_edge->priv->tails))
  {
    if (g_graph_directed_multi_edge_check_node(node2, directed_multi_edge->priv->heads))
    {
      return TRUE;
    }
    else if (!g_graph_directed_multi_edge_check_node(node2, directed_multi_edge->priv->tails))
    {
      g_set_error_literal(error, G_GRAPH_EDGE_ERROR,
                          G_GRAPH_EDGE_ERROR_NO_SUCH_NODE,
                          "There is no node in edge like second one.");
    }
  }
  else if (!g_graph_directed_multi_edge_check_node(node1, directed_multi_edge->priv->heads))
  {
    g_set_error_literal(error, G_GRAPH_EDGE_ERROR,
                G_GRAPH_EDGE_ERROR_NO_SUCH_NODE,
                "There is no node in edge like first one.");
  }
  return FALSE;
}

static gboolean
g_graph_directed_multi_edge_check_node(GGraphNode* node,
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
