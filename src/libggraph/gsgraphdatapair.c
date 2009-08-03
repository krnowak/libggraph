#include "gsgraphdatapair.h"

/* function definitions */

/**
 * g_sgraph_data_pair_new:
 * @first: first data.
 * @second: second data.
 *
 * Creates new data pair.
 *
 * Returns: newly created #GSGraphDataPair.
 */
GSGraphDataPair*
g_sgraph_data_pair_new(gpointer first,
                       gpointer second)
{
  GSGraphDataPair* data_pair = g_slice_new(GSGraphDataPair);
  data_pair->first = first;
  data_pair->second = second;
  return data_pair;
}

/**
 * g_sgraph_data_pair_copy:
 * @data_pair: first data.
 *
 * Creates a copy of @data_pair.
 *
 * Returns: newly created #GSGraphDataPair.
 */
GSGraphDataPair*
g_sgraph_data_pair_copy(GSGraphDataPair* data_pair)
{
  return g_sgraph_data_pair_new(data_pair->first, data_pair->second);
}

/**
 * g_sgraph_data_pair_free:
 * @data_pair: data pair to free.
 *
 * Frees memorty allocated to data pair.
 */
void
g_sgraph_data_pair_free(GSGraphDataPair* data_pair)
{
  g_slice_free(GSGraphDataPair, data_pair);
}

/**
 * g_sgraph_data_pair_free_v:
 * @data_pairs: #GSGraphDataPair array to free.
 * @count: length of @data_pairs.
 *
 * Frees all #GSGraphDataPair in array and then frees the array itself using
 * g_free(). If count is -1, it is assumed that @data_pairs is %NULL terminated.
 */
void
g_sgraph_data_pair_free_v(GSGraphDataPair** data_pairs,
                          gint count)
{
  gint iter;
  
  if ((data_pairs == NULL) || (count == 0))
  {
    return;
  }
  
  if (count == -1)
  {
    count = 0;
    while (data_pairs[count])
    {
      count++;
    }
  }
  
  for (iter = 0; iter < count; iter++)
  {
    g_sgraph_data_pair_free(data_pairs[iter]);
  }
  g_free(data_pairs);
}
