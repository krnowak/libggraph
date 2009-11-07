#if defined(G_DISABLE_SINGLE_INCLUDES) && !defined (_LIB_G_GRAPH_H_INSIDE_) && !defined (_LIB_G_GRAPH_COMPILATION_)
#error "Only <libggraph.h> can be included directly."
#endif

#ifndef _G_GRAPH_OPTIONS_H_
#define _G_GRAPH_OPTIONS_H_

G_BEGIN_DECLS

/* TODO: fix docs. */
/**
 * SECTION: ggraphedgeconnected
 * @title: Connection options
 * @short_description: describes possible relation between two connected nodes
 * @include: libggraph.h
 * @see_also: #GGraphEdge
 *
 * This enumeration is used in #GGraphDataQuadruplet and #GGraphEdge.
 */

/**
 * GGraphEdgeConnected:
 * @G_GRAPH_EDGE_CONNECTED_FIRST_TO_SECOND: First node is connected to second.
 * @G_GRAPH_EDGE_CONNECTED_SECOND_TO_FIRST: Second node is connected to first.
 * @G_GRAPH_EDGE_CONNECTED_BOTH: First and second are interconnected.
 *
 * This enumeration describes possible connection options in #GGraphEdge.
 */
typedef enum
{
  G_GRAPH_MULTIPLE_EDGES = 1 << 0,
  G_GRAPH_LOOPS = 1 << 1
} GGraphOptions;

G_END_DECLS

#endif /* _G_GRAPH_OPTIONS_H_ */
