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
 * g_sgraph_data_get_first:
 * @data_pair: data pair.
 *
 * Gets first member of @data_pair.
 *
 * Returns: first member of @data_pair.
 */
gpointer
g_sgraph_data_pair_get_first(GSGraphDataPair* data_pair)
{
  return data_pair->first;
}

/**
 * g_sgraph_data_get_second:
 * @data_pair: data pair.
 *
 * Gets second member of @data_pair.
 *
 * Returns: second member of @data_pair.
 */
gpointer
g_sgraph_data_pair_get_second(GSGraphDataPair* data_pair)
{
  return data_pair->second;
}
