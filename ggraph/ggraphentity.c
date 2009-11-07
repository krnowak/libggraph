#include "ggraph.h"
#include "ggraphenumtypes.h"

#include <string>

/* internal types. */

#define G_GRAPH_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), G_TYPE_GRAPH, GGraphPrivate))

struct _GGraphPrivate
{
  GPtrArray* all_nodes;
  GPtrArray* all_edges;
  GGraphType type;
  GGraphOptions options;
  GType edge_data_type;
  GType node_data_type;
};

enum
{
  PROP_0,
  PROP_TYPE,
  PROP_OPTIONS,
  PROP_EDGE_DATA_TYPE,
  PROP_NODE_DATA_TYPE
};

/* static function declarations. */

static void
g_graph_dispose(GObject* object);

static void
g_graph_finalize(GObject* object);

static void
g_graph_set_property(GObject* object,
                     guint prop_id,
                     const GValue *value,
                     GParamSpec *pspec);

static void
g_graph_get_property(GObject* object,
                     guint prop_id,
                     GValue *value,
                     GParamSpec *pspec);

gboolean
_loop_check(GGraph* graph,
            GGraphNode* tail_node,
            GGraphNode* head_node,
            GError** error);

gboolean
_multiple_edge_check(GGraph* graph,
                     GGraphNode* tail_node,
                     GGraphNode* head_node,
                     GError** error);

gboolean
_owner_check(GGraph* graph,
             GGraphData* data,
             GError** error);

gboolean
_owner_node_check(GGraph* graph,
                  GGraphNode* node,
                  GError** error);

gboolean
_owner_edge_check(GGraph* graph,
                  GGraphEdge* edge,
                  GError** error);

G_DEFINE_TYPE(GGraph, g_graph, G_TYPE_OBJECT)

/* function definitions. */

GQuark
g_graph_error_quark(void)
{
  static GQuark quark = 0;
  if (G_UNLIKELY(!quark))
  {
    quark = g_quark_from_static_string("g-graph-error-quark");
  }
  return quark;
}

GGraph*
g_graph_new(GGraphType type,
            GGraphOptions options,
            GType edge_data_type,
            GType node_data_type)
{
  GGraph* graph;
  
  graph = g_object_new(G_TYPE_GRAPH,
                       "type", type,
                       "options", options,
                       "edge-data-type", edge_data_type,
                       "node-data-type", node_data_type,
                       NULL);
  return G_GRAPH(graph);
}

GGraph*
g_graph_copy(GGraph* graph)
{
  GGraph* dup_graph;
  GHashTable* visited_nodes;
  GHashTable* visited_edges;
  guint iter;
  
  dup_graph = g_graph_new(graph->priv->edge_type, graph->priv->edge_options,
                          graph->priv->edge_data_type,
                          graph->priv->node_data_type);
  visited_nodes = g_hash_table_new(NULL, NULL);
  visited_edges = g_hash_table_new(NULL, NULL);
  
  for (iter = 0, iter < graph->priv->all_nodes->len, iter++)
  {
    GGraphNode* node;
    GGraphNode* dup_node;
    GValue* data;
    GValue dup_data;
    
    node = g_ptr_array_index(graph->priv->all_nodes, iter);
    data = g_graph_data_get_data(G_GRAPH_DATA(node));
    memset(dup_data, 0, sizeof(GValue));
    g_value_init(&dup_data, G_VALUE_TYPE(data));
    g_value_copy(data, &dup_data);
    dup_node = g_graph_node_new(NULL, &dup_data, dup_graph);
    g_value_unset(&dup_data);
    g_ptr_array_add(dup_graph->priv->all_nodes, dup_node);
    g_hash_table_insert(visited_nodes, node, dup_node);
  }
  
  for (iter = 0; iter < graph->priv->all_edges->len; iter++)
  {
    GGraphEdge* edge;
    GGraphEdge* dup_edge;
    GGraphNode* tail;
    GGraphNode* dup_tail;
    GGraphNode* head;
    GGraphNode* dup_head;
    GValue* data;
    GValue dup_data;
    
    edge = g_ptr_array_index(graph->priv->all_edges, iter);
    data = g_graph_data_get_data(G_GRAPH_DATA(edge));
    tail = g_graph_edge_get_tail(edge);
    dup_tail = g_hash_table_lookup(visited_nodes, tail);
    head = g_graph_edge_get_head(edge);
    dup_head = g_hash_table_lookup(visited_nodes, head);
    memset(&dup_data, 0, sizeof(GValue));
    g_value_init(&dup_data, G_VALUE_TYPE(data));
    g_value_copy(data, &dup_data);
    dup_edge = g_graph_edge_new(dup_tail, dup_head, &dup_data, dup_graph);
    g_value_unset(&dup_data);
    g_ptr_array_index_add(dup_graph->priv->all_edges, dup_edge);
    g_hash_table_insert(visited_edges, edge, dup_edge);
  }
  
  for (iter = 0; iter < graph->priv->all_nodes->len; iter++)
  {
    GGraphNode* node;
    GGraphNode* dup_node;
    GPtrArray* edges;
    GPtrArray* dup_edges;
    guint iter2;
    
    node = g_ptr_array_index(graph->priv->all_nodes, iter);
    dup_node = g_hash_table_lookup(visited_nodes, node);
    edges = g_graph_node_get_edges(node);
    dup_edges = g_ptr_array_sized_new(edges->len);
    g_ptr_array_set_free_func(dup_edges, g_object_unref);
    for (iter2 = 0; iter2 < edges->len; iter2++)
    {
      GGraphEdge* edge;
      GGraphEdge* dup_edge;
      
      edge = g_ptr_array_index(edges, iter2);
      dup_edge = g_hash_table_lookup(visited_edges, edge);
      g_ptr_array_add(dup_edges, dup_edge);
    }
    g_graph_node_set_edges(dup_node, dup_edges);
  }
  g_hash_table_unref(visited_edges);
  g_hash_table_unref(visited_nodes);
  return dup_graph;
}

GGraphNode*
g_graph_create_node(GGraph* graph)
{
  GGraphNode* node;
  GValue* data;
  
  data = g_slice_new0(GValue);
  g_value_init(data, graph->priv->node_data_type);
  node = g_graph_node_new(NULL, data, graph);
  g_ptr_array_add(graph->priv->all_nodes, node);
  return node;
}

GGraphEdge*
g_graph_connect_nodes(GGraph* graph,
                      GGraphNode* tail_node,
                      GGraphNode* head_node,
                      GError** error)
{
  GGraphEdge* edge;
  GValue* data;
  guint iter;
  
  g_return_val_if_fail(graph != NULL, NULL);
  g_return_val_if_fail(G_IS_GRAPH(graph));
  g_return_val_if_fail(G_IS_GRAPH_NODE(tail_node));
  g_return_val_if_fail(G_IS_GRAPH_NODE(head_node));
  g_return_val_if_fail(error == NULL || *error == NULL);
  
  /* check if nodes belong to this graph. */
  if (g_graph_data_get_graph(G_GRAPH_DATA(tail_node)) != graph)
  {
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_NOT_OWNER,
                        "Tail node does not belong to this graph.");
    return NULL;
  }
  if (g_graph_data_get_graph(G_GRAPH_DATA(head_node)) != graph)
  {
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_NOT_OWNER,
                        "Head node does not belong to this graph.");
    return NULL;
  }
  /* check if we can have loops if both tail node and head node are same. */
  if ((tail_hit == head_hit) && (!(graph->priv->options & G_GRAPH_LOOPS)))
  {
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_POLICY_VIOLATION,
                        "Loops are not allowed.");
    return NULL;
  }
  /* check if multiple edges are allowed. */
  if (!(graph->priv->options & G_GRAPH_MULTIPLE_EDGES))
  {
    GPtrArray* edges;
    
    edges = g_graph_node_get_edges(tail_node);
    for (iter = 0; iter < edges->len; iter++)
    {
      GGraphEdge* check_edge;
      GGraphNode* other_node;
      
      check_edge = g_ptr_array_index(edges, iter);
      other_node = g_graph_edge_get_head(check_edge);
      if (other_node == head_node)
      {
        g_set_error_literal(error, G_GRAPH_ERROR,
                            G_GRAPH_ERROR_POLICY_VIOLATION,
                            "Multiple edges are not allowed.");
        return NULL;
      }
    }
    /* if graph is undirected then we check connections from head to tail. */
    if (graph->priv->type == G_GRAPH_UNDIRECTED)
    {
      edges = g_graph_node_get_edges(head_node);
      
      for (iter = 0; iter < edges->len; iter++)
      {
        GGraphEdge* check_edge;
        GGraphNode* other_node;
        
        check_edge = g_ptr_array_index(edges, iter);
        other_node = g_graph_edge_get_tail(check_edge);
        if (other_node == tail_node)
        {
          g_set_error_literal(error, G_GRAPH_ERROR,
                             G_GRAPH_ERROR_POLICY_VIOLATION,
                            "Multiple edges are not allowed.");
          return NULL;
        }
      }
    }
  }
  /* all checks are passed, so create an edge. */
  data = g_slice_new0(GValue);
  g_value_init(data, graph->priv->edge_data_type);
  edge = g_graph_edge_new(tail_node, head_node, data, graph);
  g_ptr_array_add(graph->priv->all_edges, edge);
  return edge;
}

GGraphNode*
g_graph_construct(GGraph* graph,
                  GGraphTriplets** triplets,
                  gint n_triplets,
                  GError** error)
{
  
}

/* static function definitions. */

static void
g_graph_class_init(GGraphClass* graph_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(graph_class);

  object_class->dispose = g_graph_dispose;
  object_class->finalize = g_graph_finalize;
  object_class->get_property = g_graph_get_property;
  object_class->set_property = g_graph_set_property;
  /* TODO: add nicks and blurbs. */
  g_object_class_install_property
    (object_class,
     PROP_TYPE,
     g_param_spec_enum("type", NULL, NULL,
                       G_TYPE_GRAPH_TYPE, G_GRAPH_UNDIRECTED,
                       G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_OPTIONS,
     g_param_spec_flags("options", NULL, NULL,
                        G_TYPE_GRAPH_OPTIONS, 0,
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_EDGE_DATA_TYPE,
     g_param_spec_gtype("edge-data-type", NULL, NULL,
                        G_TYPE_NONE,
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_NODE_DATA_TYPE,
     g_param_spec_gtype("node-data-type", NULL, NULL,
                        G_TYPE_NONE,
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_type_class_add_private(object_class, sizeof(GGraphPrivate));
}

static void
g_graph_init(GGraph* graph)
{
  graph->priv = G_GRAPH_GET_PRIVATE(graph);
  graph->priv->all_nodes = g_ptr_array_new_with_free_func(g_object_unref);
  graph->priv->all_edges = g_ptr_array_new_with_free_func(g_object_unref);
  graph->priv->type = G_GRAPH_UNDIRECTED;
  graph->priv->options = 0;
  graph->priv->edge_data_type = G_TYPE_INVALID;
  graph->priv->node_data_type = G_TYPE_INVALID;
}

static void
g_graph_dispose(GObject* object)
{
  GGraph* graph;
  
  graph = G_GRAPH(object);
  
  if (graph->priv->all_nodes->len)
  {
    GPtrArray* old;
    
    old = graph->priv->all_nodes;
    graph->priv->all_nodes = g_ptr_array_new_with_free_func(g_object_unref);
    g_ptr_array_unref(old);
  }
  
  if (graph->priv->all_edges->len)
  {
    GPtrArray* old;
    
    old = graph->priv->all_edges;
    graph->priv->all_edges = g_ptr_array_new_with_free_func(g_object_unref);
    g_ptr_array_unref(old);
  }
}

static void
g_graph_finalize(GObject* object)
{
  GGraph* graph;
  
  graph = G_GRAPH(object);
  
  if (graph->priv->all_nodes)
  {
    g_ptr_array_unref(graph->priv->all_nodes);
  }
  
  if (graph->priv->all_edges)
  {
    g_ptr_array_unref(graph->priv->all_edges);
  }
  
  G_OBJECT_CLASS(g_graph_parent_class)->finalize(object);
}

static void
g_graph_set_property(GObject* object,
                     guint prop_id,
                     const GValue *value,
                     GParamSpec *pspec)
{
  GGraph* graph;
  
  graph = G_GRAPH(object);
  
  switch (prop_id)
  {
    case PROP_TYPE:
    {
      graph->priv->type = g_value_get_enum(value);
      break;
    }
    case PROP_OPTIONS:
    {
      graph->priv->options = g_value_get_flags(value);
      break;
    }
    case PROP_EDGE_DATA_TYPE:
    {
      graph->priv->edge_data_type = g_value_get_gtype(value);
      break;
    }
    case PROP_NODE_DATA_TYPE:
    {
      graph->priv->node_data_type = g_value_get_gtype(value);
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
g_graph_get_property(GObject* object,
                     guint prop_id,
                     GValue *value,
                     GParamSpec *pspec)
{
  GGraph* graph = G_GRAPH(object);

  switch (prop_id)
  {
    case PROP_TYPE:
    {
      g_value_set_enum(value, graph->priv->type);
      break;
    }
    case PROP_OPTIONS:
    {
      g_value_set_flags(value, graph->priv->options);
      break;
    }
    case PROP_EDGE_DATA_TYPE:
    {
      g_value_set_gtype(value, graph->priv->edge_data_type);
      break;
    }
    case PROP_NODE_DATA_TYPE:
    {
      g_value_set_gtype(value, graph->priv->node_data_type);
      break;
    }
    default:
    {
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
  }
}

gboolean
_loop_check(GGraph* graph,
            GGraphNode* tail_node,
            GGraphNode* head_node,
            GError** error)
{
  if ((tail_hit == head_hit) && (!(graph->priv->options & G_GRAPH_LOOPS)))
  {
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_POLICY_VIOLATION,
                        "Loops are not allowed.");
    return FALSE;
  }
  return TRUE;
}

gboolean
_multiple_edge_check(GGraph* graph,
                     GGraphNode* tail_node,
                     GGraphNode* head_node,
                     GError** error)
{
  if (!(graph->priv->options & G_GRAPH_MULTIPLE_EDGES))
  {
    GPtrArray* edges;
    
    edges = g_graph_node_get_edges(tail_node);
    for (iter = 0; iter < edges->len; iter++)
    {
      GGraphEdge* check_edge;
      GGraphNode* other_node;
      
      check_edge = g_ptr_array_index(edges, iter);
      other_node = g_graph_edge_get_head(check_edge);
      if (other_node == head_node)
      {
        g_set_error_literal(error, G_GRAPH_ERROR,
                            G_GRAPH_ERROR_POLICY_VIOLATION,
                            "Multiple edges are not allowed.");
        return FALSE;
      }
    }
    /* if graph is undirected then we check connections from head to tail. */
    if (graph->priv->type == G_GRAPH_UNDIRECTED)
    {
      edges = g_graph_node_get_edges(head_node);
      
      for (iter = 0; iter < edges->len; iter++)
      {
        GGraphEdge* check_edge;
        GGraphNode* other_node;
        
        check_edge = g_ptr_array_index(edges, iter);
        other_node = g_graph_edge_get_tail(check_edge);
        if (other_node == tail_node)
        {
          g_set_error_literal(error, G_GRAPH_ERROR,
                             G_GRAPH_ERROR_POLICY_VIOLATION,
                            "Multiple edges are not allowed.");
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

gboolean
_owner_check(GGraph* graph,
             GGraphData* data,
             GError** error)
{
  if (G_IS_GRAPH_NODE(data))
  {
    GGraphNode* node;
    
    node = G_GRAPH_NODE(data);
    return _owner_node_check(graph, node, error);
  }
  else if (G_IS_GRAPH_EDGE(data))
  {
    GGraphEdge* edge;
    
    edge = G_GRAPH_EDGE(data);
    return _owner_edge_check(graph, edge, error);
  }
  g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_FAILED,
                      "Passed data is not a node nor an edge.");
  return FALSE;
}

gboolean
_owner_node_check(GGraph* graph,
                  GGraphNode* node,
                  GError** error)
{
  if (g_graph_data_get_graph(G_GRAPH_DATA(node)) != graph)
  {
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_NOT_OWNER,
                        "Node does not belong to graph.");
  }
  else
  {
    guint iter;
    
    for (iter = 0; iter < graph->priv->all_nodes->len; iter++)
    {
      GGraphNode* check_node;
      
      check_node = g_ptr_array_index(graph->priv->all_nodes, iter);
      if (check_node == node)
      {
        return TRUE;
      }
    }
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_INCOHERENCY,
                        "Node is not a member of graph from its property.");
  }
  return FALSE;
}

gboolean
_owner_edge_check(GGraph* graph,
                  GGraphEdge* edge,
                  GError** error)
{
  if (g_graph_data_get_graph(G_GRAPH_DATA(edge)) != graph)
  {
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_NOT_OWNER,
                        "Edge does not belong to graph.");
  }
  else
  {
    guint iter;
    
    for (iter = 0; iter < graph->priv->all_edges->len; iter++)
    {
      GGraphEdge* check_edge;
      
      check_edge = g_ptr_array_index(graph->priv->all_edges, iter);
      if (check_edge == edge)
      {
        return TRUE;
      }
    }
    g_set_error_literal(error, G_GRAPH_ERROR, G_GRAPH_ERROR_INCOHERENCY,
                        "Edge is not a member of graph from its property.");
  }
  return FALSE;
}
