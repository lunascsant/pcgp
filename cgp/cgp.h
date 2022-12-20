#ifndef PCGP_CIRCUIT_H
#define PCGP_CIRCUIT_H

#include "utils.h"
#include "stack.h"

void newNode(Chromosome* c, Parameters* params, int index);
void activateNodes(Chromosome* c, Parameters* p);
void circuitGenerator(Chromosome* c, Parameters* params);

void evaluateCircuit(Chromosome* c, Dataset* data);

void runCircuit(Chromosome* c, Dataset* dataset, int index);

void initializePopulation(Chromosome* pop, Parameters* p);
void evaluatePopulation(Chromosome* pop, Dataset* dataset, int validation, int bestIndex[]);

Chromosome *mutateSAM(Chromosome *c, Parameters *p);

Chromosome *CGP(Dataset *training, Parameters *params, std::ofstream& factivel_file);
Chromosome *PCGP(Dataset* training, Parameters* params, std::ofstream& factivel_file);

void printChromosome(Chromosome *c, Parameters *p);
void printFile(Chromosome *c, Parameters *p, std::ofstream& factivel_file);


#endif //PCGP_CIRCUIT_H
