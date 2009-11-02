/*
 * Copyright (C) 2009 Krzesimir Nowak <qdlacz@gmail.com>
 *
 * This file is part of libggraph.
 *
 * libggraph is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libggraph is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libggraph.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gsegraphdatatriplet.h"

/**
 * SECTION: gsegraphdatatriplet
 * @title: Data triplets
 * @short_description: data triplets for constructing graphs.
 * @include: gsgraph/gsegraph.h
 * @see_also: #GSEGraphWhole, g_segraph_whole_construct()
 *
 * Data triplets are used primarly for constructing graphs by
 * g_segraph_whole_construct(). They contain all necessary data to create a pair
 * of nodes - nodes data and edge data.
 *
 * To create new data triplet, use g_segraph_data_triplet_new().
 *
 * To copy it, use g_segraph_data_triplet_copy().
 *
 * To free it, use g_segraph_data_triplet_free().
 *
 * Convenient function for freeing an array of data triplets is also
 * provided - g_segraph_data_triplet_free_v().
 *
 * <note>
 *   <para>
 *     If data in triplet was allocated dynamically then upon freeing triplet
 *     that was used in g_segraph_whole_construct() do not free its data - they
 *     are holded by created nodes and edges.
 *   </para>
 * </note>
 */

/* public function definitions */

/**
 * g_segraph_data_triplet_new:
 * @first: first data.
 * @second: second data.
 * @data: connection data.
 *
 * Creates new data triplet.
 *
 * Returns: newly created #GSEGraphDataTriplet.
 */
GSEGraphDataTriplet*
g_segraph_data_triplet_new(gpointer first,
                           gpointer second,
                           gpointer edge)
{
  GSEGraphDataTriplet* data_triplet;

  g_return_val_if_fail((first != NULL) || (second != NULL), NULL);

  data_triplet = g_slice_new(GSEGraphDataTriplet);
  data_triplet->first = first;
  data_triplet->second = second;
  data_triplet->edge = edge;
  return data_triplet;
}

/**
 * g_segraph_data_triplet_copy:
 * @data_triplet: a triplet.
 *
 * Creates a shallow copy of @data_triplet.
 *
 * Returns: newly created #GSEGraphDataTriplet.
 */
GSEGraphDataTriplet*
g_segraph_data_triplet_copy(GSEGraphDataTriplet* data_triplet)
{
  g_return_val_if_fail(data_triplet != NULL, NULL);

  return g_segraph_data_triplet_new(data_triplet->first, data_triplet->second, data_triplet->edge);
}

/**
 * g_segraph_data_triplet_free:
 * @data_triplet: data triplet to free.
 *
 * Frees memory allocated to data triplet.
 */
void
g_segraph_data_triplet_free(GSEGraphDataTriplet* data_triplet)
{
  g_return_if_fail(data_triplet != NULL);

  g_slice_free(GSEGraphDataTriplet, data_triplet);
}

/**
 * g_segraph_data_triplet_free_v:
 * @data_triplets: #GSEGraphDataTriplet array to free.
 * @count: length of @data_triplets.
 * @free_array: whether to free an array.
 *
 * Frees all #GSEGraphDataTriplet in array and then if @free_array is %TRUE,
 * frees the array itself using g_free(). If count is 0, it is assumed that
 * @data_triplets is %NULL terminated.
 */
void
g_segraph_data_triplet_free_v(GSEGraphDataTriplet** data_triplets,
                              guint count,
                              gboolean free_array)
{
  guint iter;

  g_return_if_fail(data_triplets != NULL);

  if (!count)
  {
    while (data_triplets[count])
    {
      count++;
    }
  }

  for (iter = 0; iter < count; iter++)
  {
    g_segraph_data_triplet_free(data_triplets[iter]);
  }
  if (free_array)
  {
    g_free(data_triplets);
  }
}

/**
 * g_segraph_data_triplet_is_valid:
 * @data_triplet: a triplet.
 *
 * Checks if @data_triplet is valid, that is - if at least one of first and
 * second member of @data_triplet is not %NULL.
 *
 * Returns: %TRUE if @data_triplet is valid, otherwise %FALSE.
 */
gboolean
g_segraph_data_triplet_is_valid(GSEGraphDataTriplet* data_triplet)
{
  g_return_val_if_fail(data_triplet != NULL, FALSE);

  return ((data_triplet->first != NULL) || (data_triplet->second != NULL));
}
