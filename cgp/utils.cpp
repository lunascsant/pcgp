#include "utils.h"
#include <iostream>

int randomInput(Parameters *p, int index) {
    return (rand() % (p->N + index));
}

int randomOutputIndex(){
    return (rand() % MAX_NODES);
}

int randomFunction(Parameters *p) {
    return (rand() % (p->NUM_FUNCTIONS));
}

int randomInterval(int inf_bound, int sup_bound) {
    return rand() % (sup_bound - inf_bound + 1) + inf_bound;
}

int getFunctionInputs(int function){
    switch (function) {
        case AND:
        case OR:
        case XOR:
        case NAND:
        case NOR:
        case XNOR:
            return MAX_ARITY;
        case NOT:
            return 1;
        default:
            break;
    }
}

bool IsPowerOf2( int n ){
    return (n & -n) == n;
}

int NextPowerOf2(int n){
    n--;
    n |= n >> 1;  // handle  2 bit numbers
    n |= n >> 2;  // handle  4 bit numbers
    n |= n >> 4;  // handle  8 bit numbers
    n |= n >> 8;  // handle 16 bit numbers
    n |= n >> 16; // handle 32 bit numbers
    n++;

    return n;
}

std::string ToString( double t ){
    std::stringstream ss; ss << std::setprecision(32) << t; return ss.str();
}

void readDataset(Parameters* params, Dataset* fulldata, std::string filename){
    std::fstream arq;
    int i, j, k;

    std::cout << "Lendo Dados Arquivo... " << filename << std::endl;
    arq.open(filename, std::fstream::in | std::fstream::binary);

    char* buffer;
    long size;
    arq.seekg(0, std::ios::end);
    size = arq.tellg();
    arq.seekg (0, std::ios::beg);
    buffer = new char [size];
    arq.read(buffer, size);

    if (!arq) {
        std::cout << "An error occurred!" << std::endl;
        arq.close();
        delete[] buffer;
        exit(1);
    }
    
    arq.close();

    params->N = buffer[0];
    params->O = buffer[1];
    params->M = buffer[2];

    unsigned short int M = params->M;
    unsigned short int N = params->N;
    unsigned short int O = params->O;
    std::cout << M << " " << N << " " << O << std::endl;

    fulldata->M = M;
    fulldata->N = N;
    fulldata->O = O;

    (fulldata->data) = new unsigned short int* [(M)];
    for(i = 0; i < (M); i++){
        (fulldata->data)[i] = new unsigned short int [(N)];
    }

    (fulldata->output) = new unsigned short int* [(M)];
    for(i = 0; i < (M); i++) {
        (fulldata->output)[i] = new unsigned short int[(O)];
    }

    (params->labels) = new char* [(N + O)];
    for(i = 0; i < (N + O); i++){
        (params->labels)[i] = new char [10];
    }

    //LABELS
    for(i = 0; i < params->N; i++){
        std::stringstream ss;
        std::string str;
        ss << "i";
        ss << i;
        ss >> str;
        strcpy((params->labels)[i], (str.c_str()));
    }
    for(; i < params->N+params->O; i++){
        std::stringstream ss;
        std::string str;
        ss << "o";
        ss << i;
        ss >> str;
        strcpy((params->labels)[i], (str.c_str()));
    }


    /** Read the dataset */
    // 3 infos (inputs, outputs, lines)
    k = 3;
    for(i = 0; i < (M); i++) {
        for(j = 0; j < (N); j++) {
            (fulldata->data)[i][j] = buffer[k];
            k++;
        }
    }

    for(i = 0; i < (M); i++) {
        for(j = 0; j < (O); j++) {
            (fulldata->output)[i][j] = buffer[k];
            k++;
        }
    }

    params->NUM_FUNCTIONS = 7;
    (params->functionSet) = new unsigned short int [params->NUM_FUNCTIONS];

    i = 0;

    (params->functionSet)[0] = AND;
    (params->functionSet)[1] = OR;
    (params->functionSet)[2] = XOR;
    (params->functionSet)[3] = NAND;
    (params->functionSet)[4] = NOR;
    (params->functionSet)[5] = XNOR;
    (params->functionSet)[6] = NOT;

    delete[] buffer;
}

void printDataset(Dataset* data){
    int i, j;

    std::cout << "Dataset" << std::endl; 
    for(i = 0; i < data->M; i++){
        std::cout << i << " - ";
        for(j = 0; j < data->N; j++) {
            std::cout << data->data[i][j] << " ";
        }
        std::cout << "| ";
        for(j = 0; j < data->O; j++) {
            std::cout << data->output[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

bool stopCriteria(int it){
    return it < NUM_GENERATIONS;
}