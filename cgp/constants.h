#ifndef PCGP_CONSTANTS_H
#define PCGP_CONSTANTS_H

#define SEED 10

/** Available node functions */
#define AND 10
#define OR 11
#define XOR 12
#define NOT 14
#define NAND 15
#define NOR 16
#define XNOR 17

#define CONST_PI 3.14159265359

/** Graph parameters */
#define MAX_NODES 500
#define MAX_OUTPUTS 1
#define MAX_ARITY 2

#define TESTVAR MAX_OUTPUTS/2 + MAX_OUTPUTS%2

/** Genetic parameters */
#ifndef NUM_INDIV
#define NUM_INDIV (25)
#endif // NUM_INDIV

#ifndef PROB_CROSS
#define PROB_CROSS 0.9
#endif // PROB_CROSS

#ifndef PROB_MUT
#define PROB_MUT 0.05
#endif // PROB_MUT

#define NUM_INDIV_POP 5
#define NUM_EXECUTIONS 5
#define NUM_GENERATIONS 50000
#define NUM_EVALUATIONS 2.40e+007

#define PARALLEL    1

/**
 * How the Chromosome is build:
 *
 *           _____________________________________________________________
 * nodes:   |( F0 | I0 | I1 )|( F2 | I0' | I1' )| ... |( Fn | I0n | I1n )|
 *          -------------------------------------------------------------
 *          Fn -> Value coded with a function from the functions set
 *          I0, I1 -> Inputs of the function. Can be wither a Variable from the dataset or any node with a smaller index
 *          -> This way, each node occupies 3 spaces in the array.
 *
 *           _____________________
 * outputs: | O0 | O1 | ... | On |
 *          ---------------------
 *          On -> index of the node from which the output is taken
 *
 * fitness: sum of
 */

typedef struct
{
    unsigned short int function;
    unsigned short int maxInputs;
    unsigned short int inputs[MAX_ARITY];
    int active;
} Node;

typedef struct
{
    Node nodes[MAX_NODES];
    unsigned short int output[MAX_OUTPUTS];
    unsigned short int activeNodes[MAX_NODES];
    unsigned short int numActiveNodes;
    unsigned short int fitness;
} Chromosome;

typedef struct
{
    unsigned short int function;
    unsigned short int maxInputs;
    unsigned short int inputs[MAX_ARITY];
    unsigned short int originalIndex;
} ActiveNode;

typedef struct
{
    ActiveNode nodes[MAX_NODES];
    unsigned short int output[MAX_OUTPUTS];
    unsigned short int numActiveNodes;
} ActiveChromosome;


typedef struct
{
    unsigned short int N; //inputs
    unsigned short int O; //outputs
    unsigned short int M; //dataset size

    unsigned short int NUM_FUNCTIONS;
    unsigned short int* functionSet;

    char** labels;
} Parameters;

typedef struct
{
    /** Number of inputs */
    unsigned short int N;
    /** Number of outputs */
    unsigned short int O;
    /** Number of entries */
    unsigned short int M;

    unsigned short int** data;
    unsigned short int** output;
} Dataset;

#endif //PCGP_CONSTANTS_H
