#include "ggraphdataquadruplet.h"

/**
 * SECTION: ggraphdataquadruplet
 * @title: Data quadruplets
 * @short_description: data quadruplets for constructing graphs.
 * @include: libggraph.h
 * @see_also: #GGraph, g_graph_construct()
 *
 * Data quadruplets are used primarly for constructing graphs by
 * g_graph_construct(). They contain all necessary data to create a pair of
 * nodes - nodes data, edge data and option describing relation between them.
 *
 * To create new data quadruplet, use g_graph_data_quadruplet_new().
 *
 * To copy it, use g_graph_data_quadruplet_copy().
 *
 * To free it, use g_graph_data_quadruplet_free().
 *
 * Convenient function for freeing an array of data quadruplets is also
 * provided - g_graph_data_quadruplet_free_v().
 */

/* function definitions */

/**
 * g_graph_data_quadruplet_new:
 * @first: first data.
 * @second: second data.
 * @data: connection data.
 * @connected: connection option.
 *
 * Creates new data quadruplet.
 *
 * Returns: newly created #GGraphDataQuadruplet.
 */
GGraphDataQuadruplet*
g_graph_data_quadruplet_new(gpointer first,
                            gpointer second,
                            gpointer data,
                            GGraphEdgeConnected connected)
{
  GGraphDataQuadruplet* data_quadruplet = g_slice_new(GGraphDataQuadruplet);
  data_quadruplet->first = first;
  data_quadruplet->second = second;
  data_quadruplet->data = data;
  data_quadruplet->connected = connected;
  return data_quadruplet;
}

/**
 * g_graph_data_quadruplet_copy:
 * @data_quadruplet: a quadruplet.
 *
 * Creates a copy of @data_quadruplet.
 *
 * Returns: newly created #GGraphDataQuadruplet.
 */
GGraphDataQuadruplet*
g_graph_data_quadruplet_copy(GGraphDataQuadruplet* data_quadruplet)
{
  return g_graph_data_quadruplet_new(data_quadruplet->first, data_quadruplet->second, data_quadruplet->data, data_quadruplet->connected);
}

/**
 * g_graph_data_quadruplet_free:
 * @data_quadruplet: data quadruplet to free.
 *
 * Frees memory allocated to data quadruplet.
 */
void
g_graph_data_quadruplet_free(GGraphDataQuadruplet* data_quadruplet)
{
  g_slice_free(GGraphDataQuadruplet, data_quadruplet);
}

/**
 * g_graph_data_quadruplet_free_v:
 * @data_quadruplets: #GGraphDataQuadruplet array to free.
 * @count: length of @data_quadruplets.
 *
 * Frees all #GGraphDataQuadruplet in array and then frees the array itself using
 * g_free(). If count is -1, it is assumed that @data_quadruplets is %NULL
 * terminated.
 */
void
g_graph_data_quadruplet_free_v(GGraphDataQuadruplet** data_quadruplets,
                               gint count)
{
  gint iter;
  
  if ((data_quadruplets == NULL) || (count == 0))
  {
    return;
  }
  
  if (count == -1)
  {
    count = 0;
    while (data_quadruplets[count])
    {
      count++;
    }
  }
  
  for (iter = 0; iter < count; iter++)
  {
    g_graph_data_quadruplet_free(data_quadruplets[iter]);
  }
  g_free(data_quadruplets);
}
