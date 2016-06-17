#ifndef SGSH_NEGOTIATE_H
#define SGSH_NEGOTIATE_H

#define PROT_STATE_COMPLETE 0
#define PROT_STATE_NEGOTIATION 1
#define PROT_STATE_NEGOTIATION_END 2
#define PROT_STATE_SOLUTION_SHARE 3
#define PROT_STATE_END_AFTER_WRITE 6
#define PROT_STATE_ERROR 7

#include <sys/socket.h> /* struct cmsghdr */
union fdmsg {
	struct cmsghdr h;
	char buf[CMSG_SPACE(sizeof(int))];
};

#ifdef UNIT_TESTING
#define STATIC
#else
#define STATIC static
#endif

/* Each tool in the sgsh graph calls sgsh_negotiate() to take part in
 * peer-to-peer negotiation. A message block is circulated among tools
 * and is filled with tools' I/O requirements. When all requirements are in
 * place, an algorithm runs that tries to find a solution that satisfies
 * all requirements. If a solution is found, pipes are allocated and
 * set up according to the solution. The appropriate file descriptors
 * are provided to each tool and the negotiation phase ends.
 * The function's return value signifies success or failure of the
 * negotiation phase.
 */
int sgsh_negotiate(const char *tool_name, /* Input. */
                    int channels_required, /* How many input channels can take. */
                    int channels_provided, /* How many output channels can
						provide. */
                                     /* Output: to fill. */
                    int **input_fds,  /* Input file descriptors. */
                    int *n_input_fds, /* Number of input file descriptors. */
                    int **output_fds, /* Output file descriptors. */
                    int *n_output_fds); /* Number of output file descriptors. */

/*
 * Results of operations
 * Also negative values signify a failed operation's errno value
 */
enum op_result {
	OP_SUCCESS,
	OP_ERROR,
	OP_QUIT,
	OP_EXISTS,
	OP_CREATE,
	OP_NOOP,
	OP_RETRY,
};

#ifdef UNIT_TESTING

/* Models an I/O connection between tools on an sgsh graph. */
struct sgsh_edge {
        int from; /* Index of node on the graph where data comes from (out). */
        int to; /* Index of node on the graph that receives the data (in). */
        int instances; /* Number of instances of an edge. */
	int from_instances; /* Number of instances the origin node of an edge can provide. */
	int to_instances; /* Number of instances the destination of an edge can require. */
};


int validate_input(int channels_required, int channels_provided, const char *tool_name);
int alloc_node_connections(struct sgsh_edge **nc_edges, int nc_n_edges, int type, int node_index);
int make_compact_edge_array(struct sgsh_edge **nc_edges, int nc_n_edges, struct sgsh_edge **p_edges);
int reallocate_edge_pointer_array(struct sgsh_edge ***edge_array, int n_elements);
int assign_edge_instances(struct sgsh_edge **edges, int n_edges, int this_node_channels, int is_edge_incoming, int n_edges_unlimited_constraint, int instances_to_each_unlimited, int remaining_free_channels, int total_instances);
#endif

#endif
