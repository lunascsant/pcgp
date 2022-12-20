#include "cgp.h"
#include <omp.h>

void newNode(Chromosome* c, Parameters* params, int index){
    /** set node function */
    c->nodes[index].function = params->functionSet[randomFunction(params)];


    c->nodes[index].maxInputs = getFunctionInputs(c->nodes[index].function);

    /** set node inputs */
    for(int pos = 0; pos < MAX_ARITY; pos++){
        c->nodes[index].inputs[pos] = randomInput(params, index);
    }

    /** set to unactive as default */
    c->nodes[index].active = 0;
}

/*
	used by qsort in sortIntArray
*/
static int cmpInt(const void * a, const void * b) {
    return ( *(int*)a - * (int*)b );
}

void sortActiveArray(unsigned short int *array, const int length) {
    qsort(array, length, sizeof(int), cmpInt);
}

void activateNodes(Chromosome* c, Parameters* p){

    int i, j;
    int alreadyEvaluated[MAX_NODES];
    for(i = 0; i < MAX_NODES; i++) {
        alreadyEvaluated[i] = -1;
        c->activeNodes[i] = MAX_NODES + 1;
        c->nodes[i].active = 0;
    }
    c->numActiveNodes = 0;

    Stack s;
    s.topIndex = -1;


    for(i = 0; i < MAX_OUTPUTS; i++) {
        int nodeIndex = c->output[i];
        push(&s, nodeIndex);

        while(s.topIndex != -1) {
            int node = pop(&s);
            if( c->nodes[node].active == 0) {
                for (j = 0; j < MAX_ARITY; j++) {
                    if (c->nodes[node].inputs[j] >= p->N) {
                        push(&s, c->nodes[node].inputs[j] - p->N);
                    }
                }
                c->activeNodes[c->numActiveNodes] = node;
                c->nodes[node].active = 1;
                c->numActiveNodes++;
            }

        }
    }
    sortActiveArray(c->activeNodes, c->numActiveNodes);
}

void circuitGenerator(Chromosome* c, Parameters* params){
    int i;

    for(i = 0; i < MAX_NODES; i++){
        newNode(c, params, i);
    }

    for(i = 0; i < MAX_OUTPUTS; i++){
        c->output[i] = randomOutputIndex();
    }

    c->fitness = 0.0;
    c->numActiveNodes = 0;
    activateNodes(c, params);
}

void initializePopulation(Chromosome* pop, Parameters* p) {
    for(int i = 0; i < NUM_INDIV; i++){
        circuitGenerator(&pop[i], p);
    }
}


void evaluateCircuit(Chromosome* c, Dataset* data) {
    int i;
    c->fitness = 0.0;
    for(i = 0; i < data->M; i++){
        runCircuit(c, data, i);
    }
 
}

float executeFunction(Chromosome* c, int node, ExStack* exStack){
    int i;
    float result, sum;
    int inputs = c->nodes[node].maxInputs;

    int r1, r2;



    switch (c->nodes[node].function){
        case AND:
            result = 1;
            for(i = 0; i < inputs; i++){
                if(popEx(exStack) == 0){
                    result = 0;
                }
            }
           // std::cout << "AND " << result << std::endl;
            break;
        case OR:
            result = 0;
            for(i = 0; i < inputs; i++){
                if(popEx(exStack) == 1){
                    result = 1;
                }
            }
           // std::cout << "OR " << result << std::endl;
            break;
        case XOR:
            result = 0;
            for(i = 0; i < inputs; i++){
                if(popEx(exStack) == 1){
                    result += 1;
                }
            }
            if(result != 1){
                result = 0;
            }
            //r1 = popEx(exStack);
            //r2 = popEx(exStack);
            //std::cout << "valor r1: " << r1 << " valor r2: " << r2 << std::endl;


            //std::cout << "XOR " << result << std::endl;
            break;
        case NAND:
            result = 0;
            for(i = 0; i < inputs; i++){
                if(popEx(exStack) == 0){
                    result = 1;
                }
            }
            //std::cout << "NAND " << result << std::endl;
            break;
        case NOR:
            result = 1;
            for(i = 0; i < inputs; i++){
                if(popEx(exStack) == 1){
                    result = 0;
                }
            }
            //std::cout << "NOR " << result << std::endl;
            break;
        case XNOR:
            result = 0;
            for(i = 0; i < inputs; i++){
                if(popEx(exStack) == 1){
                    result += 1;
                }
            }
            if(result == 1){
                result = 0;
            } else {
                result = 1;
            }
            //std::cout << "XNOR " << result << std::endl;
            break;
        case NOT:
            result = 0;
            if(popEx(exStack) == 0){
                result = 1;
            }
           // std::cout << "NOT " << result << std::endl;
            break;
        default:
            break;
    }

    return result;
}

void runCircuit(Chromosome* c, Dataset* dataset, int index){

    /*std::cout << "RODANDO UM INDIVIDUO" << std::endl;*/
    int i;
    float maxPredicted = -DBL_MAX;
    int predictedClass = 0;
    int correctClass = 0;

    float executionOut[MAX_OUTPUTS];
    float alreadyEvaluated[MAX_NODES];
    int inputsEvaluatedAux[MAX_NODES];

    for(i = 0; i < MAX_NODES; i++){
        alreadyEvaluated[i] = -DBL_MAX;
        inputsEvaluatedAux[i] = 0;
        //c->nodes[i].inputsEvaluated = 0;
    }

    Stack s;
    s.topIndex = -1;

    ExStack exStack;
    exStack.topIndex = -1;


    for( i = 0; i < MAX_OUTPUTS; i++) {
        int nodeIndex = c->output[i];
        push(&s, nodeIndex);

        while(s.topIndex != -1) {
            int node = pop(&s);

            for (int j = inputsEvaluatedAux[node]; j < c->nodes[node].maxInputs; j++) {
                if (c->nodes[node].inputs[j] >= dataset->N) { 
                    int refIndex = c->nodes[node].inputs[j] - dataset->N;

                    if(alreadyEvaluated[refIndex] > -DBL_MAX) {
                        inputsEvaluatedAux[node]++;
                        pushEx(&exStack, alreadyEvaluated[refIndex]);
                    } else {
                        push(&s, node); 
                        push(&s, refIndex); 
                        break;
                    }
                } else {
                    inputsEvaluatedAux[node]++;
                    pushEx(&exStack, dataset->data[index][c->nodes[node].inputs[j]]);
                }
            }

            if(inputsEvaluatedAux[node] == c->nodes[node].maxInputs){

                if(!(alreadyEvaluated[node] > -DBL_MAX)) {

                    alreadyEvaluated[node] = executeFunction(c, node, &exStack);
                }
                
            }

        }
        executionOut[i] = alreadyEvaluated[nodeIndex];

        if(dataset->output[index][i] == executionOut[i]) {
            (c->fitness)++;
        }
        
    }
}

void evaluatePopulation(Chromosome* pop, Dataset* dataset, int validation, int bestIndex[]){
    int i, j, k, l;
    float bestFitness[NUM_EXECUTIONS];
    int bestActiveNodes[NUM_EXECUTIONS];
    int feasibles[NUM_EXECUTIONS];

    for(k = 0; k < NUM_EXECUTIONS; k++) {
        bestFitness[k] = 0;
        bestActiveNodes[k] = 9999;
        bestIndex[k] = -1;
        feasibles[k] = 0;
    }

    for(j = 0; j < NUM_INDIV; j++) {
        evaluateCircuit(&pop[j], dataset);
    }
    std::cout << std::endl;

    int prox = 0;
    for(l = 0; l < NUM_EXECUTIONS; l++) {
        for(i = prox; i < prox + NUM_INDIV_POP; i++) {
            if (pop[i].fitness == dataset->M) {
                feasibles[l] += 1;
            }
        }
        prox = i;
    }

    std::vector<int> equalFitness;
    int indBest = -1;

    prox = 0;
    for(l = 0; l < NUM_EXECUTIONS; l++) {
        if(feasibles[l] > 0) {
            if(feasibles[l] == 1) {
                for(i = prox; i < prox + NUM_INDIV_POP; i++) {
                    if(pop[i].fitness == dataset->M){
                        bestFitness[l] = pop[i].fitness;
                        bestActiveNodes[l] = pop[i].numActiveNodes;
                        bestIndex[l] = i;
                    }
                }
            } else if (feasibles[l] > 1) {
                for(i = prox; i < prox + NUM_INDIV_POP; i++) {
                    if(pop[i].fitness == dataset->M){
                        if(pop[i].numActiveNodes < bestActiveNodes[l]) {
                            bestFitness[l] = pop[i].fitness;
                            bestActiveNodes[l] = pop[i].numActiveNodes;
                            bestIndex[l] = i;
                        }
                    }
                }
            }
        } else {
            for(i = prox; i < prox + NUM_INDIV_POP; i++) {
                if(pop[i].fitness > bestFitness[l]) {
                    bestFitness[l] = pop[i].fitness;
                    bestActiveNodes[l] = pop[i].numActiveNodes;
                    bestIndex[l] = i;
                    equalFitness.clear();
                    equalFitness.push_back(i);
                } else if (pop[i].fitness == bestFitness[l]) {
                    equalFitness.push_back(i);
                }
            }

            if(!equalFitness.empty()) {
                if(equalFitness.size() == 1) {
                    bestFitness[l] = pop[equalFitness.at(0)].fitness;
                    bestActiveNodes[l] = pop[equalFitness.at(0)].numActiveNodes;
                    bestIndex[l] = equalFitness.at(0);
                } else {
                    indBest = rand() % (equalFitness.size() - 1);
                    bestFitness[l] = pop[equalFitness.at(indBest)].fitness;
                    bestActiveNodes[l] = pop[equalFitness.at(indBest)].numActiveNodes;
                    bestIndex[l] = equalFitness.at(indBest);
                }
            }
        }
        prox = i;

        equalFitness.clear();
    }
}


void copyNode(Node* n, ActiveNode* an){
    an->function = n->function;
    an->maxInputs = n->maxInputs;
    for(int i = 0; i < MAX_ARITY; i++){
        an->inputs[i] = n->inputs[i];
    }
}

void copyActiveNodes(Chromosome *c, ActiveChromosome* ac){

    for(int i = 0; i < NUM_INDIV; i++ ){

        int numActiveNodes = c[i].numActiveNodes;
        ac[i].numActiveNodes = numActiveNodes;


        for(int j = 0; j < numActiveNodes; j++){
            int currentActive = c[i].activeNodes[j];

            copyNode(&(c[i].nodes[currentActive]), &(ac[i].nodes[j]));
            ac[i].nodes[j].originalIndex = currentActive;
        }


        for(int j = 0; j < MAX_OUTPUTS; j++){
            ac[i].output[j] = c[i].output[j];
        }


    }
}

Chromosome *mutateSAM(Chromosome *c, Parameters *p) {
    int i, j, inputOrFunction, nodeOrOutput;
    int activeSelected = 0;

    nodeOrOutput = randomInterval(0, MAX_NODES + p->O - 1);

    if (nodeOrOutput > MAX_NODES - 1) {
        activeSelected = 1;
        c->output[0] = randomInterval(0, MAX_NODES - 1);
    } else {
        while(activeSelected == 0) {
            i = randomInterval(0, MAX_NODES - 1);
            if(c->nodes[i].active == 1) {
                activeSelected = 1;
            }
            inputOrFunction = randomInterval(0, 1);
            if (!inputOrFunction) {
                c->nodes[i].function = p->functionSet[randomFunction(p)];
                c->nodes[i].maxInputs = getFunctionInputs(c->nodes[i].function);
            } else {
                j = randomInterval(0, 1);
                c->nodes[i].inputs[j] = randomInput(p, i);
            }
        }
    }

    activateNodes(c, p);
    return  c;
}

Chromosome* CGP(Dataset *training, Parameters *params, std::ofstream& factivel_file) {
    Chromosome *current_pop;
    current_pop = new Chromosome[NUM_INDIV];

    int factivel = 0;
    Chromosome* best_train = new Chromosome[NUM_EXECUTIONS];
    Chromosome mutated_best;

    initializePopulation(current_pop, params);

    int bestTrain[NUM_EXECUTIONS];
    evaluatePopulation(current_pop, training, 0, bestTrain);

    int bestActiveNodes[NUM_EXECUTIONS];
    int feasibles[NUM_EXECUTIONS];

    for(int k = 0; k < NUM_EXECUTIONS; k++) {
        bestActiveNodes[k] = 9999;
        feasibles[k] = 0;
        best_train[k] = current_pop[bestTrain[k]];
    }

    int prox, it;
    std::vector<int> equalFitness;
    int indBest = -1;
    int iterations = 0;
    while(stopCriteria(iterations)) {
        int group = -1;
        for(int k = 0; k < NUM_INDIV; k++){
            if(k % NUM_EXECUTIONS == 0) {
                group++;
            }
            current_pop[k] = best_train[group];
            mutateSAM(&current_pop[k], params);
        }

        it = 0;
        prox = 0;

        for(int l = 0; l < NUM_EXECUTIONS; l++) {
            for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                evaluateCircuit(&current_pop[it], training);
            }
            prox = it;
        }


        it = 0;
        prox = 0;

        for(int l = 0; l < NUM_EXECUTIONS; l++) {
            for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                if (current_pop[it].fitness == params->M) {
                    feasibles[l] += 1;
                }
            }
            prox = it;
        }

        it = 0;
        prox = 0;

        for(int l = 0; l < NUM_EXECUTIONS; l++) {
            if(feasibles[l] > 0) {
                if(feasibles[l] == 1) {
                    for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                        if(current_pop[it].fitness == params->M){
                            best_train[l] = current_pop[it];
                            bestActiveNodes[l] = current_pop[it].numActiveNodes;
                        }
                    }
                } else if (feasibles[l] > 1) {
                    for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                        if(current_pop[it].fitness == params->M){
                            if(current_pop[it].numActiveNodes < bestActiveNodes[l]) {
                                best_train[l] = current_pop[it];
                                bestActiveNodes[l] = current_pop[it].numActiveNodes;
                            }
                        }
                    }
                }
            } else {
                for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                    if(current_pop[it].fitness > best_train[l].fitness){
                        best_train[l] = current_pop[it];
                        bestActiveNodes[l] = current_pop[it].numActiveNodes;
                        equalFitness.clear();
                        equalFitness.push_back(it);
                    } else if (current_pop[it].fitness == best_train[l].fitness) {
                        equalFitness.push_back(it);
                    }
                }

                if(!equalFitness.empty()) {
                    if(equalFitness.size() == 1) {
                        bestActiveNodes[l] = current_pop[equalFitness.at(0)].numActiveNodes;
                        best_train[l] = current_pop[equalFitness.at(0)];
                    } else {
                        indBest = rand() % (equalFitness.size() - 1);
                        bestActiveNodes[l] = current_pop[equalFitness.at(indBest)].numActiveNodes;
                        best_train[l] = current_pop[equalFitness.at(indBest)];
                    }
                }
            }

            equalFitness.clear();
            prox = it;
        }

        for(int k = 0; k < NUM_EXECUTIONS; k++) {
            bestActiveNodes[k] = 9999;
            feasibles[k] = 0;
        }

        equalFitness.clear();
        indBest = -1;

        iterations++;
    }

    return best_train;
}

Chromosome* PCGP(Dataset *training, Parameters *params, std::ofstream& factivel_file) {
    Chromosome *current_pop;
    current_pop = new Chromosome[NUM_INDIV];

    int factivel = 0;
    Chromosome* best_train = new Chromosome[NUM_EXECUTIONS];
    Chromosome mutated_best;

    initializePopulation(current_pop, params);

    int bestTrain[NUM_EXECUTIONS];
    evaluatePopulation(current_pop, training, 0, bestTrain);

    int bestActiveNodes[NUM_EXECUTIONS];
     int feasibles[NUM_EXECUTIONS];

    for(int k = 0; k < NUM_EXECUTIONS; k++) {
        bestActiveNodes[k] = 9999;
        feasibles[k] = 0;
        best_train[k] = current_pop[bestTrain[k]];
    }

    int prox, it;
    std::vector<int> equalFitness;
    int indBest = -1;
    int iterations = 0;
    while(stopCriteria(iterations)) {
        int group = -1;
        for(int k = 0; k < NUM_INDIV; k++){
            if(k % NUM_EXECUTIONS == 0) {
                group++;
            }
            current_pop[k] = best_train[group];
            mutateSAM(&current_pop[k], params);
        }

        it = 0;
        prox = 0;

        #pragma omp parallel for num_threads(4)
        for(int l = 0; l < NUM_EXECUTIONS * NUM_INDIV_POP; l++) {
            evaluateCircuit(&current_pop[l], training);
        }


        it = 0;
        prox = 0;

        for(int l = 0; l < NUM_EXECUTIONS; l++) {
            for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                if (current_pop[it].fitness == params->M) {
                    feasibles[l] += 1;
                }
            }
            prox = it;
        }

        it = 0;
        prox = 0;

        for(int l = 0; l < NUM_EXECUTIONS; l++) {
            if(feasibles[l] > 0) {
                if(feasibles[l] == 1) {
                    for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                        if(current_pop[it].fitness == params->M){
                            best_train[l] = current_pop[it];
                            bestActiveNodes[l] = current_pop[it].numActiveNodes;
                        }
                    }
                } else if (feasibles[l] > 1) {
                    for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                        if(current_pop[it].fitness == params->M){
                            if(current_pop[it].numActiveNodes < bestActiveNodes[l]) {
                                best_train[l] = current_pop[it];
                                bestActiveNodes[l] = current_pop[it].numActiveNodes;
                            }
                        }
                    }
                }
            } else {
                for(it = prox; it < prox + NUM_INDIV_POP; it++) {
                    if(current_pop[it].fitness > best_train[l].fitness){
                        best_train[l] = current_pop[it];
                        bestActiveNodes[l] = current_pop[it].numActiveNodes;
                        equalFitness.clear();
                        equalFitness.push_back(it);
                    } else if (current_pop[it].fitness == best_train[l].fitness) {
                        equalFitness.push_back(it);
                    }
                }

                if(!equalFitness.empty()) {
                    if(equalFitness.size() == 1) {
                        bestActiveNodes[l] = current_pop[equalFitness.at(0)].numActiveNodes;
                        best_train[l] = current_pop[equalFitness.at(0)];
                    } else {
                        indBest = rand() % (equalFitness.size() - 1);
                        bestActiveNodes[l] = current_pop[equalFitness.at(indBest)].numActiveNodes;
                        best_train[l] = current_pop[equalFitness.at(indBest)];
                    }
                }
            }

            equalFitness.clear();
            prox = it;
        }

        for(int k = 0; k < NUM_EXECUTIONS; k++) {
            bestActiveNodes[k] = 9999;
        }

        for(int k = 0; k < NUM_EXECUTIONS; k++) {
            feasibles[k] = 0;
        }

        equalFitness.clear();
        indBest = -1;

        iterations++;
    }

    return best_train;
}

void printChromosome(Chromosome *c, Parameters *p) {
    for(int i = 0; i < MAX_NODES; i++) {
       if (c->nodes[i].active) {
           std::cout << "No" << i + p->N << " - " << c->nodes[i].inputs[0] << " " << c->nodes[i].inputs[1] << " " << c->nodes[i].function << std::endl;
       }
    }
    std::cout <<"Outputs: " << c->output[0] + p->N << std::endl;
    std::cout << "- FIM -" << std::endl;
}

void printFile(Chromosome *c, Parameters *p, std::ofstream& factivel_file) {
    for(int i = 0; i < MAX_NODES; i++) {
        if (c->nodes[i].active) {
            factivel_file << "Node" << i + p->N << " " << c->nodes[i].inputs[0]
            << " " << c->nodes[i].inputs[1] << " " <<  c->nodes[i].function << "\n";
        }
    }
    factivel_file << "Output " << c->output[0] + p->N << "\n";
    factivel_file << "\n";
}

