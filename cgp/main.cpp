#include <iostream>
#include <fstream>
#include "cgp.h"
#include "utils.h"

using namespace std;

void printFileFiveExe(Chromosome *c, Parameters *p, ofstream& factivel_file) {
    for(int i = 0; i < MAX_NODES; i++) {
        if (c->nodes[i].active) {
            factivel_file << "Node" << i + p->N << " " << c->nodes[i].inputs[0]
            << " " << c->nodes[i].inputs[1] << " " <<  c->nodes[i].function << "\n";
        }
    }
    factivel_file << "Output " << c->output[0] + p->N << "\n";
    factivel_file << "\n";
    factivel_file << "\n";
}


int main(int argc, char** argv) {

    // --------------- INICIO TRATAMENTO ARQUIVOS --------------

    std::string newSeed = argv[5];
    std::string geneNamesStr = argv[1];
    std::string argExe = argv[2];
    std::string argProblemName = argv[3];
    std::ifstream geneNamesFile(geneNamesStr);
    std::vector<std::string> geneNames;
    std::string gene;
    int numGenes = 0;

    while(std::getline (geneNamesFile, gene)) {
        geneNames.push_back(gene);
        numGenes++;
    }

    geneNamesFile.close();

    std::string currentGene = argv[4];

    std::vector<int> allNetworks;

    std::string datasetFile = currentGene + "_" + argProblemName + ".bin";

#if PARALLEL
    std::string unfeasiblesFile = "./executions_parallel/" + argExe + "/unfeasibles_" + argProblemName + ".txt";
    std::string rankedEdgesFile = "./executions_parallel/" + argExe + "/rankedEdges_" + argProblemName + ".csv";
    std::string nomeArquivo = currentGene + "_" + newSeed + "_" + argExe;
    std::string caminhoArquivo = "./executions_parallel/" + argExe + "/" + nomeArquivo + ".txt";

#else

    std::string unfeasiblesFile = "./executions_sequential/" + argExe + "/unfeasibles_" + argProblemName + ".txt";
    std::string rankedEdgesFile = "./executions_sequential/" + argExe + "/rankedEdges_" + argProblemName + ".csv";
    std::string nomeArquivo = currentGene + "_" + newSeed + "_" + argExe;
    std::string caminhoArquivo = "./executions_sequential/" + argExe + "/" + nomeArquivo + ".txt";

#endif

    std::ofstream rankedEdges;
    std::ofstream unfeasibles;
    std::ofstream factivelFile;

    rankedEdges.open(rankedEdgesFile, std::ios_base::app);
     if (!rankedEdges) {
        std::cout << "Error file ranked edges" << std::endl;
        exit(1);
    }

    unfeasibles.open(unfeasiblesFile, std::ios_base::app);
     if (!unfeasibles) {
        std::cout << "Error file unfeasibles" << std::endl;
        exit(1);
    }

    factivelFile.open(caminhoArquivo, std::ios::out);
    if (!factivelFile) {
        std::cout << "Error file" << std::endl;
        exit(1);
    }

    // --------------- FIM TRATAMENTO ARQUIVOS --------------

    // --------------- INICIO LEITURA DATASET --------------

    Parameters *params;
    params = new Parameters;

    Dataset fullData;
    readDataset(params, &fullData, datasetFile);
    std::cout << "-----------------PRINT DATASET-------------------" << std::endl;
    printDataset(&fullData);
    std::cout << "-----------------PRINT DATASET-------------------" << std::endl;

    // --------------- FIM LEITURA DATASET --------------

    // --------------- INICIO SEED --------------
    
    int seed = atoi(argv[5]);
    srand(seed);
    
    // --------------- FIM SEED --------------

    std::vector<int> rede;
    int countUnfeasible;
    std::vector<int> rede_local;

    Dataset* data = &fullData;

    // -------------- INICIO CGP --------------

#if PARALLEL
    Chromosome* executionBest = PCGP(data, params, factivelFile);

    for(int i = 0; i < NUM_EXECUTIONS; i++) {
        std::cout << "Fitness - exe " << i << " : " << executionBest[i].fitness << std::endl;
        if(executionBest[i].fitness == params->M)
            printFileFiveExe(&executionBest[i], params, factivelFile);
        else
            factivelFile << "Nao factivel\n\n";
    }

    countUnfeasible = 0;

    for(int i = 0; i < NUM_EXECUTIONS; i++) {
        if(executionBest[i].fitness != params->M) {
            countUnfeasible += 1;
            continue;
        }

        for(int j = 0; j < MAX_NODES; j++){
            if(executionBest[i].nodes[j].active == 1){
                for(int k = 0; k < MAX_ARITY; k++){
                    if(executionBest[i].nodes[j].inputs[k] < data->N){
                        auto search = find(rede_local.begin(), rede_local.end(), executionBest[i].nodes[j].inputs[k]);
                        if(search == rede_local.end()){
                            rede_local.push_back(executionBest[i].nodes[j].inputs[k]);
                        }
                    }
                }

            }
        }

        for(int & j : rede_local){
            rede.push_back(j);
        }

        rede_local.clear();
    }

#else
    Chromosome* executionBest = CGP(data, params, factivelFile);

    for(int i = 0; i < NUM_EXECUTIONS; i++) {
        std::cout << "Fitness - exe " << i << " : " <<executionBest[i].fitness << std::endl;
        if(executionBest[i].fitness == params->M)
            printFileFiveExe(&executionBest[i], params, factivelFile);
        else
            factivelFile << "Nao factivel\n\n";
    }

    countUnfeasible = 0;

    for(int i = 0; i < NUM_EXECUTIONS; i++) {
        if(executionBest[i].fitness != params->M) {
            countUnfeasible += 1;
            continue;
        }

        for(int j = 0; j < MAX_NODES; j++){
            if(executionBest[i].nodes[j].active == 1){
                for(int k = 0; k < MAX_ARITY; k++){
                    if(executionBest[i].nodes[j].inputs[k] < data->N){
                        auto search = find(rede_local.begin(), rede_local.end(), executionBest[i].nodes[j].inputs[k]);
                        if(search == rede_local.end()){
                            rede_local.push_back(executionBest[i].nodes[j].inputs[k]);
                        }
                    }
                }

            }
        }

        for(int & j : rede_local){
            rede.push_back(j);
        }

        rede_local.clear();
    }

#endif

    std::vector<float> counting;

    for(int i = 0; i < fullData.N; i++){
        float counted = std::count(rede.begin(), rede.end(), i);
        counting.push_back(counted/NUM_EXECUTIONS);
    }

    for(int i = 0; i < geneNames.size(); i++) {
        if(counting.at(i) != 0) {
            rankedEdges << geneNames[i] << "\t" << currentGene << "\t" << counting.at(i) << "\n";
        }
    }

    if(countUnfeasible == NUM_EXECUTIONS) {
        unfeasibles << currentGene << "\n";
    }

    delete params;
    factivelFile.close();
    rankedEdges.close();
    unfeasibles.close();

    return 0;
}