#include <string.h>

#include "ggraphdata.h"
#include "ggraph.h"

/* internal types. */

#define G_GRAPH_DATA_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), G_TYPE_GRAPH_EDGE, GGraphDataPrivate))

struct _GGraphDataPrivate
{
  GValue* data;
  GGraph* graph;
  gboolean is_marked;
  GGraph* marked_graph;
};

enum
{
  PROP_0,
  PROP_DATA,
  PROP_GRAPH,
  PROP_IS_MARKED,
  PROP_MARKED_GRAPH
};

/* static function declarations. */

static void
g_graph_data_dispose(GObject* object);

static void
g_graph_data_finalize(GObject* object);

static void
g_graph_data_set_property(GObject* object,
                          guint prop_id,
                          const GValue *value,
                          GParamSpec *pspec);

static void
g_graph_data_get_property(GObject* object,
                          guint prop_id,
                          GValue *value,
                          GParamSpec *pspec);

G_DEFINE_TYPE(GGraphData, g_graph_data, G_TYPE_OBJECT)

/* function definitions. */

GGraphData*
g_graph_data_new(GValue* data,
                 GGraph* graph);
{
  GGraphData* data;
  
  data = g_object_new(G_TYPE_GRAPH_DATA,
                      "data", data,
                      "graph", graph,
                      NULL);
  
  return G_GRAPH_DATA(data);
}

GType
g_graph_data_get_data_type(GGraphData* data)
{
  g_return_val_if_fail(G_IS_GRAPH_DATA(data), G_TYPE_INVALID);
  
  return G_VALUE_TYPE(data->priv->data);
}

const GValue*
g_graph_data_get_data(GGraphData* data)
{
  g_return_val_if_fail(G_IS_GRAPH_DATA(data), NULL);
  
  if (!data->priv->data || G_VALUE_TYPE(data->priv->data) == G_TYPE_INVALID)
  {
    return NULL;
  }
  
  return data->priv->data;
}

void
g_graph_data_set_data(GGraphData* data,
                      const GValue* value)
{
  g_return_val_if_fail(G_IS_GRAPH_DATA(data));
  
  {
    GValue* old;
    
    old = data->priv->data;
    
    if ((old == value) ||
        ((!value || G_VALUE_TYPE(value) == G_TYPE_INVALID) &&
         (!old || G_VALUE_TYPE(old) == G_TYPE_INVALID)))
    {
      return;
    }
  }
  
  if (G_VALUE_TYPE(data->priv->data) != G_TYPE_INVALID)
  {
    GValue old;
    
    memset(&old, 0, sizeof(GValue));
    g_value_init(&old, G_VALUE_TYPE(data->priv->data));
    g_value_copy(data->priv->data, &old);
    g_value_unset(data->priv->data);
    g_value_unset(&old);
  }
  
  if (value && (G_VALUE_TYPE(value) != G_TYPE_INVALID))
  {
    g_value_init(data->priv->data, G_VALUE_TYPE(value));
    g_value_copy(value, data->priv->data);
  }
  
  g_object_notify(G_OBJECT(data), "data");
}

GGraph*
g_graph_data_get_graph(GGraphData* data)
{
  g_return_val_if_fail(G_IS_GRAPH_DATA(data), NULL);
  
  return data->priv->graph;
}

void
g_graph_data_set_graph(GGraphData* data,
                       GGraph* graph)
{
  GGraph* old;
  
  g_return_if_fail(G_IS_GRAPH_DATA(data));
  g_return_if_fail((graph == NULL) || (G_IS_GRAPH(graph)));
  
  old = data->priv->graph;
  
  if (old == graph)
  {
    return;
  }
  
  if (graph)
  {
    data->priv->graph = g_object_ref(graph);
  }
  else
  {
    data->priv->graph = NULL;
  }
  
  if (old)
  {
    g_object_unref(old);
  }
  
  g_object_notify(data, "graph");
}

gboolean
g_graph_data_get_marked(GGraphData* data)
{
  g_return_val_if_fail(G_IS_GRAPH_DATA(data), NULL);
  
  return data->priv->is_marked;
}

void
g_graph_data_set_marked(GGraphData* data,
                        gboolean marked)
{
  g_return_if_fail(G_IS_GRAPH_DATA(data));
  
  if (data->priv->is_marked != marked)
  {
    data->priv->is_marked = marked;
    g_object_notify(data, "is-marked");
  }
}

GGraph*
g_graph_data_get_marked_graph(GGraphData* data)
{
  g_return_val_if_fail(G_IS_GRAPH_DATA(data), NULL);
  
  return data->priv->marked_graph;
}

void
g_graph_data_set_marked_graph(GGraphData* data,
                              GGraph* graph)
{
  GGraph* old;
  
  g_return_if_fail(G_IS_GRAPH_DATA(data));
  g_return_if_fail(graph == NULL || G_IS_GRAPH(graph));
  
  if (data->priv->marked == graph)
  {
    return;
  }
  
  old = data->priv->marked_graph;
  
  if (graph)
  {
    data->priv->marked_graph = g_object_ref(graph);
  }
  else
  {
    data->priv->marked_graph = NULL;
  }
  
  if (old)
  {
    g_object_unref(old);
  }
  
  g_object_notify(data, "marked-graph");
}

/* static function definitions. */

static void
g_graph_data_class_init(GGraphDataClass* data_class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(data_class);

  object_class->finalize = g_graph_data_finalize;
  object_class->dispose = g_graph_data_dispose;
  object_class->get_property = g_graph_data_get_property;
  object_class->set_property = g_graph_data_set_property;
  /* TODO: add nicks and blurbs. */
  g_object_class_install_property
    (object_class,
     PROP_DATA,
     g_param_spec_boxed("data", NULL, NULL,
                        G_TYPE_VALUE,
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_GRAPH,
     g_param_spec_object("graph", NULL, NULL,
                         G_TYPE_GRAPH,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_IS_MARKED,
     g_param_spec_boolean("is-marked", NULL, NULL,
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class,
     PROP_MARKED_GRAPH,
     g_param_spec_object("marked-graph", NULL, NULL,
                         G_TYPE_GRAPH,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_type_class_add_private(object_class, sizeof(GGraphDataPrivate));
}

static void
g_graph_data_init(GGraphData* data)
{
  data->priv = G_GRAPH_DATA_GET_PRIVATE(data);
  data->priv->data = g_slice_new0(GValue);
  data->priv->graph = NULL;
  data->priv->is_marked = FALSE;
  data->priv->marked_graph = NULL;
}

static void
g_graph_data_dispose(GObject* object)
{
  GGraphData* data;
  
  data = G_GRAPH_DATA(object);
  
  if (G_VALUE_TYPE(data->priv->data) != G_TYPE_INVALID)
  {
    GValue old;
    
    memset(&old, 0, sizeof(GValue));
    g_value_init(&old, G_VALUE_TYPE(data->priv->data));
    g_value_copy(data->priv->data, &old);
    g_value_unset(data->priv->data);
    g_value_unset(&old);
  }
  
  if (data->priv->graph)
  {
    GGraph* old;
    
    old = data->priv->graph;
    data->priv->graph = NULL;
    g_object_unref(old);
  }
  
  data->priv->is_marked = FALSE;
  
  if (data->priv->marked_graph)
  {
    GGraph* old;
    
    old = data->priv->marked_graph;
    data->priv->marked_graph = NULL;
    g_object_unref(old);
  }
  
  G_OBJECT_CLASS(g_graph_data_parent_class)->dispose(object);
}

static void
g_graph_data_finalize(GObject* object)
{
  GGraphData* data;
  
  data = G_GRAPH_DATA(object);
  
  if (data->priv->data)
  {
    g_slice_free(GValue, data->priv->data);
    data->priv->data = NULL;
  }
  
  G_OBJECT_CLASS(g_graph_data_parent_class)->finalize(object);
}

static void
g_graph_data_set_property(GObject* object,
                          guint prop_id,
                          const GValue *value,
                          GParamSpec *pspec)
{
  GGraphData* data;
  
  data = G_GRAPH_DATA(object);
  
  switch (prop_id)
  {
    case PROP_DATA:
    {
      g_graph_data_set_data(data, g_value_get_boxed(value));
      break;
    }
    case PROP_GRAPH:
    {
      g_graph_data_set_graph(data, g_value_get_object(value));
      break;
    }
    case PROP_IS_MARKED:
    {
      g_graph_data_set_marked(data, g_value_get_boolean(value));
      break;
    }
    case PROP_MARKED_GRAPH:
    {
      g_graph_data_set_marked_graph(data, g_value_get_object(value));
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
g_graph_data_get_property(GObject* object,
                          guint prop_id,
                          GValue *value,
                          GParamSpec *pspec)
{
  GGraphData* data;
  
  data = G_GRAPH_DATA(object);

  switch (prop_id)
  {
    case PROP_DATA:
    {
      g_value_set_boxed(value, data->priv->data);
      break;
    }
    case PROP_GRAPH:
    {
      g_value_set_object(value, data->priv->graph);
      break;
    }
    case PROP_IS_MARKED:
    {
      g_value_set_boolean(value, data->priv->is_marked);
      break;
    }
    case PROP_MARKED_GRAPH:
    {
      g_value_set_object(value, data->priv->marked_graph);
      break;
    }
    default:
    {
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
  }
}
