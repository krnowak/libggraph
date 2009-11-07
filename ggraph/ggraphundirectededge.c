#include "ggraphundirectededge.h"
#include "ggraphnode.h"
#include "ggraph.h"
/* internal types. */

enum
{
  PROP_0,
  PROP_FIRST,
  PROP_SECOND
};

/* static function declarations. */

static void
g_graph_undirected_edge_dispose(GObject* object);

static void
g_graph_undirected_edge_finalize(GObject* object);

static void
g_graph_undirected_edge_set_property(GObject* object,
                                   guint prop_id,
                                   const GValue *value,
                                   GParamSpec *pspec);

static void
g_graph_undirected_edge_get_property(GObject* object,
                                   guint prop_id,
                                   GValue *value,
                                   GParamSpec *pspec);

G_DEFINE_TYPE(GGraphUndirectedEdge, g_graph_undirected_edge, G_TYPE_GRAPH_UNDIRECTED_MULTI_EDGE)

/* function definitions. */

GGraphUndirectedEdge*
g_graph_undirected_edge_new(GGraphNode* first,
                            GGraphNode* second,
                            GValue* data,
                            GGraph* graph);
{
  GObject* object;
  
  undirected_edge = g_object_new(G_TYPE_GRAPH_UNDIRECTED_EDGE,
                                 "first", first,
                                 "second", second,
                                 "data", data,
                                 "graph", graph,
                                 NULL);
  
  return G_GRAPH_UNDIRECTED_EDGE(object);
}

const GGraphNode*
g_graph_undirected_edge_get_first_node(GGraphUndirectedEdge* undirected_edge)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  GPtrArray* nodes;
  GGraphNode* first;
  
  g_return_val_if_fail(G_IS_GRAPH_UNDIRECTED_EDGE(undirected_edge), NULL);
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(undirected_edge);
  
  nodes = g_graph_undirected_multi_edge_get_nodes(undirected_multi_edge);
  
  if (nodes && nodes->len > 0)
  {
    first = g_ptr_array_index(nodes, 0);
  }
  else
  {
    first = NULL;
  }
  
  return tail;
}

void
g_graph_undirected_edge_set_first_node(GGraphUndirectedEdge* undirected_edge,
                                       GGraphNode* node)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  GPtrArray* new_nodes;
  
  g_return_if_fail(G_IS_GRAPH_UNDIRECTED_EDGE(undirected_edge));
  g_return_if_fail((node == NULL) || G_IS_GRAPH_NODE(node));
  
  if (g_graph_undirected_edge_get_first_node(undirected_edge) == node)
  {
    return;
  }
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(undirected_edge);
  
  new_nodes = g_ptr_array_sized_new(2);
  g_ptr_array_set_free_func(new_nodes, g_object_unref);
  
  if (node)
  {
    g_ptr_array_add(new_nodes, g_object_ref(node));
  }
  else
  {
    g_ptr_array_add(new_nodes, NULL);
  }
  
  g_ptr_array_add(new_nodes, g_object_ref(g_graph_undirected_edge_get_second_node(undirected_edge)));
  g_graph_undirected_multi_edge_set_nodes(undirected_multi_edge, new_nodes);
  g_ptr_array_unref(new_nodes);
  
  g_object_notify(undirected_edge, "first");
}

const GGraphNode*
g_graph_undirected_edge_get_second_node(GGraphUndirectedEdge* undirected_edge)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  GPtrArray* nodes;
  GGraphNode* second;
  
  g_return_val_if_fail(G_IS_GRAPH_UNDIRECTED_EDGE(undirected_edge), NULL);
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(undirected_edge);
  
  nodes = g_graph_undirected_multi_edge_get_nodes(undirected_multi_edge);
  
  if (nodes && nodes->len > 0)
  {
    second = g_ptr_array_index(nodes, 1);
  }
  else
  {
    second = NULL;
  }
  
  return second;
}

void
g_graph_undirected_edge_set_second_node(GGraphUndirectedEdge* undirected_edge,
                                        GGraphNode* node)
{
  GGraphUndirectedMultiEdge* undirected_multi_edge;
  GPtrArray* new_nodes;
  
  g_return_if_fail(G_IS_GRAPH_UNDIRECTED_EDGE(undirected_edge));
  g_return_if_fail((node == NULL) || G_IS_GRAPH_NODE(node));
  
  if (g_graph_undirected_edge_get_second_node(undirected_edge) == node)
  {
    return;
  }
  
  undirected_multi_edge = G_GRAPH_UNDIRECTED_MULTI_EDGE(undirected_edge);
  
  new_nodes = g_ptr_array_sized_new(2);
  g_ptr_array_set_free_func(new_nodes, g_object_unref);
  g_ptr_array_add(new_nodes, g_object_ref(g_graph_undirected_edge_get_first_node(undirected_edge)));
  
  if (node)
  {
    g_ptr_array_add(new_nodes, g_object_ref(node));
  }
  else
  {
    g_ptr_array_add(new_nodes, NULL);
  }
  
  g_graph_undirected_multi_edge_set_nodes(undirected_multi_edge, new_nodes);
  g_ptr_array_unref(new_nodes);
  
  g_object_notify(undirected_edge, "second");
}

gboolean
g_graph_undirected_edge_is_loop(GGraphUndirectedEdge* undirected_edge)
{
  g_return_val_if_fail(G_IS_GRAPH_UNDIRECTED_EDGE(undirected_edge), FALSE);
  
  return (g_graph_undirected_edge_get_first_node(undirected_edge) == g_graph_undirected_edge_get_second_node(undirected_edge));
}

gboolean
g_graph_undirected_edge_is_link(GGraphUndirectedEdge* undirected_edge)
{
  g_return_val_if_fail(G_IS_GRAPH_UNDIRECTED_EDGE(undirected_edge), FALSE);
  
  return (g_graph_undirected_edge_get_first_node(undirected_edge) != g_graph_undirected_edge_get_second_node(undirected_edge));
}

/* static function definitions. */

static void
g_graph_undirected_edge_class_init(GGraphUndirectedEdgeClass* undirected_edge_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(undirected_edge_class);

  object_class->finalize = g_graph_undirected_edge_finalize;
  object_class->dispose = g_graph_undirected_edge_dispose;
  object_class->get_property = g_graph_undirected_edge_get_property;
  object_class->set_property = g_graph_undirected_edge_set_property;
  /* TODO: add nicks and blurbs. */
  g_object_class_install_property
    (object_class,
     PROP_TAIL,
     g_param_spec_object("first", NULL, NULL,
                         G_TYPE_GRAPH_NODE,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_HEAD,
     g_param_spec_object("second", NULL, NULL,
                         G_TYPE_GRAPH_NODE,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
g_graph_undirected_edge_init(GGraphUndirectedEdge* undirected_edge)
{}

static void
g_graph_undirected_edge_dispose(GObject* object)
{
  G_OBJECT_CLASS(g_graph_undirected_edge_parent_class)->dispose(object);
}

static void
g_graph_undirected_edge_finalize(GObject* object)
{
  G_OBJECT_CLASS(g_graph_undirected_edge_parent_class)->finalize(object);
}

static void
g_graph_undirected_edge_set_property(GObject* object,
                                     guint prop_id,
                                     const GValue *value,
                                     GParamSpec *pspec)
{
  GGraphUndirectedEdge* undirected_edge;
  
  undirected_edge = G_GRAPH_UNDIRECTED_EDGE(object);
  
  switch (prop_id)
  {
    case PROP_FIRST:
    {
      g_graph_undirected_edge_set_first_node(undirected_edge, g_value_get_object(value));
      break;
    }
    case PROP_SECOND:
    {
      g_graph_undirected_edge_set_second_node(undirected_edge, g_value_get_object(value));
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
g_graph_undirected_edge_get_property(GObject* object,
                                     guint prop_id,
                                     GValue *value,
                                     GParamSpec *pspec)
{
  GGraphUndirectedEdge* undirected_edge;
  
  undirected_edge = G_GRAPH_UNDIRECTED_EDGE(object);

  switch (prop_id)
  {
    case PROP_FIRST:
    {
      g_value_set_object(value, g_graph_undirected_edge_get_first_node(undirected_edge));
      break;
    }
    case PROP_HEAD:
    {
      g_value_set_object(value, g_graph_undirected_edge_get_second_node(undirected_edge));
      break;
    }
    default:
    {
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
  }
}
