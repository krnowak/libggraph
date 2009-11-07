#include "ggraphnode.h"

#include "ggraphedge.h"

/* internal types. */

#define G_GRAPH_NODE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), G_TYPE_GRAPH_NODE, GGraphNodePrivate))

struct _GGraphNodePrivate
{
  GPtrArray* edges;
};

enum
{
  PROP_0,
  PROP_EDGES
};

/* static function declarations. */

static void
g_graph_node_dispose(GObject* object);

static void
g_graph_node_finalize(GObject* object);

static void
g_graph_node_set_property(GObject* object,
                          guint prop_id,
                          const GValue *value,
                          GParamSpec *pspec);

static void
g_graph_node_get_property(GObject* object,
                          guint prop_id,
                          GValue *value,
                          GParamSpec *pspec);

G_DEFINE_TYPE(GGraphNode, g_graph_node, G_TYPE_GRAPH_DATA)

/* function definitions. */

GGraphNode*
g_graph_node_new(GPtrArray* edges,
                 GValue* data,
                 GGraph* graph)
{
  GGraphNode* node;
  
  node = g_object_new(G_TYPE_GRAPH_NODE,
                      "edges", edges,
                      "data", data,
                      "graph", graph,
                      NULL);
  
  return G_GRAPH_NODE(node);
}

const GPtrArray*
g_graph_node_get_edges(GGraphNode* node)
{
  g_return_val_if_fail(G_IS_GRAPH_NODE(node), NULL);
  
  if (!node->priv->edges || !node->priv->edges->len)
  {
    return NULL;
  }
  
  return node->priv->edges;
}

void
g_graph_node_set_edges(GGraphNode* node,
                       GPtrArray* edges)
{
  GPtrArray* old;
  
  g_return_val_if_fail(G_IS_GRAPH_NODE(node));
  
  old = node->priv->edges;
  
  if ((old == edges) || ((!edges || !edges->len) && (!old || !old->len)))
  {
    return;
  }
  
  if (edges)
  {
    node->priv->edges = g_ptr_array_ref(edges);
  }
  else
  {
    node->priv->edges = g_ptr_array_new_with_free_func(g_object_unref);
  }
  
  if (old)
  {
    g_ptr_array_unref(old);
  }
  
  g_object_notify(G_OBJECT(node), "edges");
}

/* static function definitions. */

static void
g_graph_node_class_init(GGraphNodeClass* node_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(node_class);

  object_class->finalize = g_graph_node_finalize;
  object_class->dispose = g_graph_node_dispose;
  object_class->get_property = g_graph_node_get_property;
  object_class->set_property = g_graph_node_set_property;
  /* TODO: add nicks and blurbs. */
  g_object_class_install_property
    (object_class,
     PROP_EDGES,
     g_param_spec_boxed("edges", NULL, NULL,
                         G_TYPE_PTR_ARRAY,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_type_class_add_private(object_class, sizeof(GGraphNodePrivate));
}

static void
g_graph_node_init(GGraphNode* node)
{
  node->priv = G_GRAPH_NODE_GET_PRIVATE(node);
  node->priv->edges = g_ptr_array_new_with_free_func(g_object_unref);
}

static void
g_graph_node_dispose(GObject* object)
{
  GGraphNode* node;
  
  node = G_GRAPH_NODE(object);
  
  if (node->priv->edges->len)
  {
    GPtrArray* old;
    
    old = node->priv->edges;
    node->priv->edges = g_ptr_array_new_with_free_func(g_object_unref);
    g_ptr_array_unref(old);
  }
  
  G_OBJECT_CLASS(g_graph_node_parent_class)->dispose(object);
}

static void
g_graph_node_finalize(GObject* object)
{
  GGraphNode* node;
  
  node = G_GRAPH_NODE(object);
  
  if (node->priv->edges)
  {
    g_ptr_array_unref(node->priv->edges);
    node->priv->edges = NULL;
  }
  
  G_OBJECT_CLASS(g_graph_node_parent_class)->finalize(object);
}

static void
g_graph_edge_set_property(GObject* object,
                          guint prop_id,
                          const GValue *value,
                          GParamSpec *pspec)
{
  GGraphNode* node;
  
  node = G_GRAPH_NODE(object);
  
  switch (prop_id)
  {
  case PROP_EDGES:
  {
    g_graph_node_set_edges(node, g_value_get_boxed(value));
    break;
  }
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    break;
  }
}

static void
g_graph_node_get_property(GObject* object,
                          guint prop_id,
                          GValue *value,
                          GParamSpec *pspec)
{
  GGraphNode* node;
  
  node = G_GRAPH_NODE(object);

  switch (prop_id)
  {
    case PROP_EDGES:
    {
      g_value_set_boxed(value, node->priv->edges);
      break;
    }
    default:
    {
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
  }
}
