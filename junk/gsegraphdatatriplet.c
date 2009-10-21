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
 * SECTION: GSEGraphDataTriplet
 * @title: Data triplets
 * @short_description: data triplets for constructing graphs.
 * @include: libggraph.h
 * @see_also: #GSEGraph, g_segraph_construct()
 *
 * Data triplets are used primarly for constructing graphs by
 * g_segraph_construct(). They contain all necessary data to create a pair of
 * nodes - nodes data and edge data.
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
 * If you want to provide free functions for node and edge data then use
 * g_segraph_data_triplet_free_with_funcs() and
 * g_segraph_data_triplet_free_v_with_funcs().
 *
 * <note>
 *   <para>
 *     Use g_segraph_data_triplet_free_v() or
 *     g_segraph_data_triplet_free_v_with_funcs() only when you created the
 *     array with g_malloc() or function using it. That is - the array is freed
 *     with g_free().
 *   </para>
 * </note>
 */

/* function definitions */

/**
 * g_segraph_data_triplet_new:
 * @first: first data.
 * @second: second data.
 * @data: connection data.
 * @connected: connection option.
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
  GSEGraphDataTriplet* data_triplet = g_slice_new(GSEGraphDataTriplet);
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
  g_slice_free(GSEGraphDataTriplet, data_triplet);
}

/**
 * g_segraph_data_triplet_free_v:
 * @data_triplets: #GSEGraphDataTriplet array to free.
 * @count: length of @data_triplets.
 * @free_array: whether to free an array.
 *
 * Frees all #GSEGraphDataTriplet in array and then if @free_array is %TRUE,
 * frees the array itself using g_free(). If count is -1 or less, it is assumed
 * that @data_triplets is %NULL terminated.
 */
void
g_segraph_data_triplet_free_v(GSEGraphDataTriplet** data_triplets,
                              gint count,
                              gboolean free_array);
{
  gint iter;
  
  if ((!data_triplets) || (!count))
  {
    return;
  }
  
  if (count < 0)
  {
    count = 0;
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
