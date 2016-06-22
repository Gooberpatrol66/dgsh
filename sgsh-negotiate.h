#ifndef SGSH_NEGOTIATE_H
#define SGSH_NEGOTIATE_H

/* Negotiation protocol states */
enum prot_state {
	PS_COMPLETE,		/* Negotiation process is complete */
	PS_NEGOTIATION,		/* Negotiation phase */
	PS_NEGOTIATION_END,	/* End of negotiation phase */
	PS_RUN,			/* Share solution; prepare to run */
	PS_ERROR,		/* Error in negotiation process */
};

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
enum prot_state sgsh_negotiate(
		/* Input */
		const char *tool_name, /* Tool name */
		int channels_required, /* How many input channels can take */
		int channels_provided, /* How many output channels can provide
					*/
		/* Output: to fill */
		int **input_fds,	/* Input file descriptors */
		int *n_input_fds,	/* Number of input file descriptors */
		int **output_fds,	/* Output file descriptors */
		int *n_output_fds);	/* Number of output file descriptors */

/*
 * Results of operations
 * Also negative values signify a failed operation's errno value
 */
enum op_result {
	OP_SUCCESS,		/* Successful */
	OP_ERROR,		/* Unresolvable error due to I/O problem
				 * constraints provided by the processes
				 * on the sgsh graph or memory constraints
				 * of the systems.
				 */
	OP_EXISTS,		/* Node or edge already registered with the
				 * sgsh graph.
				 */
	OP_CREATE,		/* Node ar edge registered with the sgsh 
				 * graph.
				 */
	OP_NOOP,		/* No operation when trying to add an edge
				 * on a graph with just one node at the time.
				 */
	OP_RETRY,		/* Not all constraints of an I/O constraint
				 * problem have been satisfied yet.
				 * Retry by leveraging flexible constraints.
				 */
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


enum op_result validate_input(
		int channels_required,
		int channels_provided,
		const char *tool_name);
enum op_result alloc_node_connections(
		struct sgsh_edge **nc_edges,
		int nc_n_edges,
		int type,
		int node_index);
enum op_result make_compact_edge_array(
		struct sgsh_edge **nc_edges,
		int nc_n_edges,
		struct sgsh_edge **p_edges);
enum op_result reallocate_edge_pointer_array(
		struct sgsh_edge ***edge_array,
		int n_elements);
#endif

#endif
