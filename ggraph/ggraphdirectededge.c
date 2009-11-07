#include "ggraphdirectededge.h"
#include "ggraphnode.h"
#include "ggraph.h"
/* internal types. */

enum
{
  PROP_0,
  PROP_TAIL,
  PROP_HEAD
};

/* static function declarations. */

static void
g_graph_directed_edge_dispose(GObject* object);

static void
g_graph_directed_edge_finalize(GObject* object);

static void
g_graph_directed_edge_set_property(GObject* object,
                                   guint prop_id,
                                   const GValue *value,
                                   GParamSpec *pspec);

static void
g_graph_directed_edge_get_property(GObject* object,
                                   guint prop_id,
                                   GValue *value,
                                   GParamSpec *pspec);

G_DEFINE_TYPE(GGraphDirectedEdge, g_graph_directed_edge, G_TYPE_GRAPH_DIRECTED_MULTI_EDGE)

/* function definitions. */

GGraphDirectedEdge*
g_graph_directed_edge_new(GGraphNode* tail,
                          GGraphNode* head,
                          GValue* data,
                          GGraph* graph);
{
  GObject* object;
  
  directed_edge = g_object_new(G_TYPE_GRAPH_DIRECTED_EDGE,
                               "tail", tail,
                               "head", head,
                               "data", data,
                               "graph", graph,
                               NULL);
  
  return G_GRAPH_DIRECTED_EDGE(object);
}

const GGraphNode*
g_graph_directed_edge_get_tail(GGraphDirectedEdge* directed_edge)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  GPtrArray* tails;
  GGraphNode* tail;
  
  g_return_val_if_fail(G_IS_GRAPH_DIRECTED_EDGE(directed_edge), NULL);
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(directed_edge);
  
  tails = g_graph_directed_multi_edge_get_tails(directed_multi_edge);
  
  if (tails && tails->len > 0)
  {
    tail = g_ptr_array_index(tails, 0);
  }
  else
  {
    tail = NULL;
  }
  
  return tail;
}

void
g_graph_directed_edge_set_tail(GGraphDirectedEdge* directed_edge,
                               GGraphNode* node)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  
  g_return_if_fail(G_IS_GRAPH_DIRECTED_EDGE(directed_edge));
  g_return_if_fail((node == NULL) || G_IS_GRAPH_NODE(node));
  
  if (g_graph_directed_edge_get_tail(directed_edge) == node)
  {
    return;
  }
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(directed_edge);
  
  if (node)
  {
    GPtrArray* new_tails = g_ptr_array_sized_new(1);
    g_ptr_array_set_free_func(new_tails, g_object_unref);
    g_ptr_array_add(new_tails, g_object_ref(node));
    g_graph_directed_multi_edge_set_tails(directed_multi_edge, new_tails);
    g_ptr_array_unref(new_tails);
  }
  else
  {
    g_graph_directed_multi_edge_set_tails(directed_multi_edge, NULL);
  }
  
  g_object_notify(directed_edge, "tail");
}

const GGraphNode*
g_graph_directed_edge_get_head(GGraphDirectedEdge* directed_edge)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  GPtrArray* heads;
  GGraphNode* head;
  
  g_return_val_if_fail(G_IS_GRAPH_DIRECTED_EDGE(directed_edge), NULL);
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(directed_edge);
  
  heads = g_graph_directed_multi_edge_get_heads(directed_multi_edge);
  
  if (heads && heads->len > 0)
  {
    head = g_ptr_array_index(tails, 0);
  }
  else
  {
    head = NULL;
  }
  
  return head;
}

void
g_graph_directed_edge_set_head(GGraphDirectedEdge* directed_edge,
                               GGraphNode* node)
{
  GGraphDirectedMultiEdge* directed_multi_edge;
  
  g_return_if_fail(G_IS_GRAPH_DIRECTED_EDGE(directed_edge));
  g_return_if_fail((node == NULL) || G_IS_GRAPH_NODE(node));
  
  if (g_graph_directed_edge_get_head(directed_edge) == node)
  {
    return;
  }
  
  directed_multi_edge = G_GRAPH_DIRECTED_MULTI_EDGE(directed_edge);
  
  if (node)
  {
    GPtrArray* new_heads = g_ptr_array_sized_new(1);
    g_ptr_array_set_free_func(new_heads, g_object_unref);
    g_ptr_array_add(new_heads, g_object_ref(node));
    g_graph_directed_multi_edge_set_heads(directed_multi_edge, new_heads);
    g_ptr_array_unref(new_heads);
  }
  else
  {
    g_graph_directed_multi_edge_set_heads(directed_multi_edge, NULL);
  }
  
  g_object_notify(directed_edge, "head");
}

gboolean
g_graph_directed_edge_is_loop(GGraphDirectedEdge* directed_edge)
{
  g_return_val_if_fail(G_IS_GRAPH_DIRECTED_EDGE(directed_edge), FALSE);
  
  return (g_graph_directed_edge_get_tail(directed_edge) == g_graph_directed_edge_get_head(directed_edge));
}

gboolean
g_graph_directed_edge_is_link(GGraphDirectedEdge* directed_edge)
{
  g_return_val_if_fail(G_IS_GRAPH_DIRECTED_EDGE(directed_edge), FALSE);
  
  return (g_graph_directed_edge_get_tail(directed_edge) != g_graph_directed_edge_get_head(directed_edge));
}

/* static function definitions. */

static void
g_graph_directed_edge_class_init(GGraphDirectedEdgeClass* directed_edge_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(directed_edge_class);

  object_class->finalize = g_graph_directed_edge_finalize;
  object_class->dispose = g_graph_directed_edge_dispose;
  object_class->get_property = g_graph_directed_edge_get_property;
  object_class->set_property = g_graph_directed_edge_set_property;
  /* TODO: add nicks and blurbs. */
  g_object_class_install_property
    (object_class,
     PROP_TAIL,
     g_param_spec_object("tail", NULL, NULL,
                         G_TYPE_GRAPH_NODE,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_HEAD,
     g_param_spec_object("head", NULL, NULL,
                         G_TYPE_GRAPH_NODE,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
g_graph_directed_edge_init(GGraphDirectedEdge* directed_edge)
{}

static void
g_graph_directed_edge_dispose(GObject* object)
{
  G_OBJECT_CLASS(g_graph_directed_edge_parent_class)->dispose(object);
}

static void
g_graph_directed_edge_finalize(GObject* object)
{
  G_OBJECT_CLASS(g_graph_directed_edge_parent_class)->finalize(object);
}

static void
g_graph_directed_edge_set_property(GObject* object,
                                   guint prop_id,
                                   const GValue *value,
                                   GParamSpec *pspec)
{
  GGraphDirectedEdge* directed_edge;
  
  directed_edge = G_GRAPH_DIRECTED_EDGE(object);
  
  switch (prop_id)
  {
    case PROP_TAIL:
    {
      g_graph_directed_edge_set_tail(directed_edge, g_value_get_object(value));
      break;
    }
    case PROP_HEAD:
    {
      g_graph_directed_edge_set_head(directed_edge, g_value_get_object(value));
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
g_graph_directed_edge_get_property(GObject* object,
                                   guint prop_id,
                                   GValue *value,
                                   GParamSpec *pspec)
{
  GGraphDirectedEdge* directed_edge;
  
  directed_edge = G_GRAPH_DIRECTED_EDGE(object);

  switch (prop_id)
  {
    case PROP_TAIL:
    {
      g_value_set_object(value, g_graph_directed_edge_get_tail(directed_edge));
      break;
    }
    case PROP_HEAD:
    {
      g_value_set_object(value, g_graph_directed_edge_get_head(directed_edge));
      break;
    }
    default:
    {
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
  }
}
