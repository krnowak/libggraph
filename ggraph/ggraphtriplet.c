#include <ggraphtriplet.h>

GType
g_graph_triplet_get_type(void)
{
  static GType type = G_TYPE_INVALID;
  
  if (G_UNLIKELY(type == G_TYPE_INVALID))
  {
    type = g_boxed_type_register_static("GGraphTriplet",
                                        (GBoxedCopyFunc)g_graph_triplet_copy,
                                        (GBoxedFreeFunc)g_graph_triplet_free);
  }
  return type;
}

GGraphTriplet*
g_graph_triplet_new(GValue* tail_data
                    GValue* head_data,
                    GValue* edge_data)
{
  GGraphTriplet* triplet;
  GValue* data;
  guint iter;
  
  triplet = g_slice_new0(GGraphTriplet);
  if (tail_data && G_VALUE_TYPE(tail_data) != G_TYPE_INVALID)
  {
    data = g_slice_new0(GValue);
    g_value_init(data, G_VALUE_TYPE(tail_data));
    g_value_copy(tail_data, data);
    triplet->tail_data = data;
  }
  
  if (head_data && G_VALUE_TYPE(head_data) != G_TYPE_INVALID)
  {
    data = g_slice_new0(GValue);
    g_value_init(data, G_VALUE_TYPE(head_data));
    g_value_copy(tail_data, data);
    triplet->edge_data = data;
  }
  
  if (edge_data && G_VALUE_TYPE(edge_data) != G_TYPE_INVALID)
  {
    data = g_slice_new0(GValue);
    g_value_init(data, G_VALUE_TYPE(edge_data));
    g_value_copy(tail_data, data);
    triplet->edge_data = data;
  }
  return triplet;
}

GGraphTriplet*
g_graph_triplet_copy(const GGraphTriplet* triplet)
{
  GGraphTriplet* dup_triplet;
  GValue* dup_data;
  
  if (!triplet)
  {
    return NULL;
  }
  
  dup_triplet = g_slice_new0(GGraphTriplet);
  if (triplet->tail_data && G_VALUE_TYPE(triplet->tail_data) != G_TYPE_INVALID)
  {
    data = g_slice_new0(GValue);
    g_value_init(data, G_VALUE_TYPE(triplet->tail_data));
    g_value_copy(triplet->tail_data, data);
    dup_triplet->tail_data = data;
  }
  
  if (triplet->head_data && G_VALUE_TYPE(triplet->head_data) != G_TYPE_INVALID)
  {
    data = g_slice_new0(GValue);
    g_value_init(data, G_VALUE_TYPE(triplet->head_data));
    g_value_copy(triplet->head_data, data);
    dup_triplet->head_data = data;
  }
  
  if (triplet->edge_data && G_VALUE_TYPE(triplet->edge_data) != G_TYPE_INVALID)
  {
    data = g_slice_new0(GValue);
    g_value_init(data, G_VALUE_TYPE(triplet->edge_data));
    g_value_copy(triplet->edge_data, data);
    dup_triplet->edge_data = data;
  }
  
  return dup_triplet;
}

void
g_graph_triplet_free(GGraphTriplet* triplet)
{
  if (!triplet)
  {
    return;
  }
  
  if (triplet->tail_data)
  {
    if (G_VALUE_TYPE(triplet->tail_data) != G_TYPE_INVALID)
    {
      g_value_unset(triplet->tail_data);
    }
    g_slice_free(GValue, triplet->head_data);
  }
  
  if (triplet->head_data)
  {
    if (G_VALUE_TYPE(triplet->head_data) != G_TYPE_INVALID)
    {
      g_value_unset(triplet->head_data);
    }
    g_slice_free(GValue, triplet->head_data);
  }
  
  if (triplet->edge_data)
  {
    if (G_VALUE_TYPE(triplet->edge_data) != G_TYPE_INVALID)
    {
      g_value_unset(triplet->edge_data);
    }
    g_slice_free(GValue, triplet->edge_data);
  }
  
  g_slice_free(GGraphTriplet, triplet);
}

void
g_graph_triplet_freev(GGraphTriplet** triplets,
                      gint n_triplets)
{
  if (!triplets)
  {
    return;
  }
  
  if (n_triplets < 0)
  {
    n_triplets = 0;
    while (triplets[n_triplets])
    {
      n_triplets++;
    }
  }
  
  if (n_triplets)
  {
    n_triplets--;
    for (; n_triplets >= 0; n_triplets--)
    {
      g_graph_triplet_free(triplets[n_triplets]);
    }
  }
  g_free(triplets);
}
