#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <time.h>
#include <cstdlib>
#include "qFilter.hpp"
#include "qFilterBsr.hpp"
#include "scalarMerge.hpp"
#include "scalarMergeBsr.hpp"
#include "scalarGalloping.hpp"
#include "scalarGallopingBsr.hpp"
#include "simdGalloping.hpp"
#include "simdGallopingBsr.hpp"
#include "shuffling.hpp"
#include "shufflingBsr.hpp"
#include "bmiss.hpp"
#include "bmissSttni.hpp"
#include "hieraInter.hpp"
#include "roaring.hpp"
#include "tester.hpp"

using namespace std;

vector<vector<int>*> loadData(string filename);

const int TEST_COUNT_RANDOM = 200000;
const int TEST_COUNT_RANDOM_DIST = 200;
const int TEST_MULTIPLE_TERMS_LIMIT = 20;
const int BUFFER_SIZE = 1024*4;
const int LIMIT_DATA_TERMS = -1;

int main(){
    string path_dump = "outputDump";
    cout<<"Loading data..."<<endl;
    vector<vector<int>*> data = loadData(path_dump);                
    Tester tester;  
    vector<IntersectionAlg*> algorithms;

    algorithms.push_back(new QFilter());
    algorithms.push_back(new QFilterBsr());
    algorithms.push_back(new ScalarMerge());
    algorithms.push_back(new ScalarMergeBsr()); 
    algorithms.push_back(new ScalarGalloping()); 
    algorithms.push_back(new ScalarGallopingBsr());
    algorithms.push_back(new SimdGalloping());
    algorithms.push_back(new SimdGallopingBsr());
    algorithms.push_back(new Shuffling()); 
    algorithms.push_back(new ShufflingBsr());
    algorithms.push_back(new BMiss());      
    algorithms.push_back(new BMissSTTNI());
    algorithms.push_back(new HieraInter());   
    algorithms.push_back(new RoaringAlg());
     
    for(int i = 0; i < algorithms.size(); i++){
        IntersectionAlg* alg = algorithms.at(i);
        cout<<alg->getName()<<endl;

        //Random testing
        int durationPairs = tester.randomPairIntersection(&data, alg, TEST_COUNT_RANDOM);
        cout<<"Two terms, random selection: "<<durationPairs<< "ms"<<endl;

        //Distributed testing
        int durationDistPairs = tester.randomDistPairIntersection(&data, alg, TEST_COUNT_RANDOM_DIST);
        cout<<"Two terms, distributed probability: "<<durationDistPairs<< "ms"<<endl;

        //Multiple random terms
        int durationDistMultiple = tester.randomDistMultipleIntersections(&data, alg, TEST_COUNT_RANDOM_DIST, TEST_MULTIPLE_TERMS_LIMIT);
        cout<<"Multiple terms, distributed probability: "<<durationDistMultiple<< "ms"<<endl;

        cout<<endl<<endl;
    }

    for(int i = 0; i < data.size(); i++){
        delete data.at(i);
    }

    return 0;
}

vector<vector<int>*> loadData(string filename){
    ifstream file(filename, ios::binary);
    char buffer[BUFFER_SIZE];

    vector<vector<int>*> output;
    int documentCount = 0;
    int currentDocumentCount = 0;
    int processedTerms = 0;
    vector<int>* currentTermVector;   

    while(file.read(buffer, BUFFER_SIZE)){
        int readChars = file.gcount();
        for(int i = 0; i < readChars; i+=4){
            char* binaryValue = new char[4];
            for(int j = 0; j < 4; j++){
                binaryValue[j] = buffer[i+j];
            }
            int value = *(reinterpret_cast<int*>(binaryValue));
            delete binaryValue;

            if(currentDocumentCount >= documentCount){
                if(LIMIT_DATA_TERMS > 0 && processedTerms >= LIMIT_DATA_TERMS)
                    return output;
                currentTermVector = new vector<int>();
                output.push_back(currentTermVector);
                currentDocumentCount = 0;                
                documentCount = value;                
                processedTerms++;                
            }
            else{                
                currentTermVector->push_back(value);
                currentDocumentCount++;
            }
        }
    }
    return output;
}