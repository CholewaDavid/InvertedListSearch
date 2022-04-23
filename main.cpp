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
#include "testWrapper.hpp"

using namespace std;


vector<int*> loadData(string filename, vector<int>* dataLengths);
bool checkTestResults(vector<TesterResult> results, string testType);
vector<BsrArrays*> encodeBsr(vector<int*>* data, vector<int>* dataLengths);
void writeTestResults(int durationLoadData, int durationBsr, vector<TestWrapper*>* testWrappers, string path);
void writeTestResult(ofstream* outputStream, TesterResult* testerResult);


const int TEST_COUNT_RANDOM = 500000;
const int TEST_COUNT_RANDOM_DIST = 1000;
const int TEST_TOP_RANDOM = 100;
const int TEST_MULTIPLE_TERMS_LIMIT = 20;
const int TEST_TOP_PAIRS = 20;
const int BUFFER_SIZE = 1024*4;
const int LIMIT_DATA_TERMS = -1;
const string PATH_DUMP = "../outputDump";
const string PATH_RESULT = "testResults.csv";
const bool FRAGMENTED_TIMER = true;

int main(){
    chrono::time_point<chrono::system_clock> timeStart;
    chrono::time_point<chrono::system_clock> timeStop; 
    int durationLoadData;
    int durationBsrGeneration;
    int fullTestingTime;

    //Reading data
    cout<<"Loading data..."<<endl;
    vector<int> dataLengths;
    timeStart = chrono::system_clock::now(); 
    vector<int*> data = loadData(PATH_DUMP, &dataLengths); 
    timeStop = chrono::system_clock::now(); 
    durationLoadData = chrono::duration_cast<chrono::milliseconds>(timeStop - timeStart).count();

    //Generating BSR formated data
    cout<<"Creating BSR data"<<endl;      
    timeStart = chrono::system_clock::now(); 
    vector<BsrArrays*> dataBsr = encodeBsr(&data, &dataLengths);
    timeStop = chrono::system_clock::now(); 
    durationBsrGeneration = chrono::duration_cast<chrono::milliseconds>(timeStop - timeStart).count();

    Tester tester;  
    vector<IntersectionAlg*> algorithms;
    vector<IntersectionAlgBsr*> algorithmsBsr;
    vector<TestWrapper*> testWrappers;

    //Algorithms
    algorithms.push_back(new QFilter());
    algorithms.push_back(new ScalarMerge());    
    algorithms.push_back(new ScalarGalloping());     
    algorithms.push_back(new SimdGalloping());    
    algorithms.push_back(new Shuffling());     
    algorithms.push_back(new BMiss());      
    algorithms.push_back(new BMissSTTNI());
    algorithms.push_back(new HieraInter());   

    algorithmsBsr.push_back(new QFilterBsr());
    algorithmsBsr.push_back(new ScalarMergeBsr()); 
    algorithmsBsr.push_back(new ScalarGallopingBsr());
    algorithmsBsr.push_back(new SimdGallopingBsr());
    algorithmsBsr.push_back(new ShufflingBsr());


    //Creating test cases
    cout<<"Generating test scenarios"<<endl<<endl;
    //testWrappers.push_back(new TestWrapper(tester.generateRandomPairTerms(TEST_COUNT_RANDOM, data.size()), "Random pairs"));
    //testWrappers.push_back(new TestWrapper(tester.generateRandomDistPairTerms(TEST_COUNT_RANDOM_DIST, &data, &dataLengths), "Random distribution pairs"));
    //testWrappers.push_back(new TestWrapper(tester.generateTopPairs(TEST_TOP_PAIRS), "Top pairs"));
    testWrappers.push_back(new TestWrapper(tester.generateTopRandomPair(TEST_TOP_RANDOM, data.size()), "Top random pairs"));

    //vector<vector<int>*> randomDistMultipleTerms = tester.generateRandomDistMultipleTerms(TEST_COUNT_RANDOM_DIST, &data, &dataLengths, TEST_MULTIPLE_TERMS_LIMIT);    

    cout<<"Testing"<<endl<<endl;
    //Testing regular algorithms
    for(int i = 0; i < algorithms.size(); i++){
        IntersectionAlg* alg = algorithms.at(i);
        cout<<alg->getName()<<endl;

        for(int j = 0; j < testWrappers.size(); j++){
            timeStart = chrono::system_clock::now(); 
            TesterResult testResultPairs = tester.testIntersection(&data, &dataLengths, &testWrappers[j]->testScenario, alg);
            timeStop = chrono::system_clock::now();            
            if(!FRAGMENTED_TIMER){
                fullTestingTime = chrono::duration_cast<chrono::milliseconds>(timeStop - timeStart).count();
                testResultPairs.duration = fullTestingTime;
            }
            testWrappers[j]->results.push_back(testResultPairs);
            cout<<testWrappers.at(j)->name<<" "<<testResultPairs.duration<< "ms"<<endl;
        }
        
        cout<<endl<<endl;
    }

    //Testing BSR algorithms
    for(int i = 0; i < algorithmsBsr.size(); i++){
        IntersectionAlgBsr* alg = algorithmsBsr.at(i);
        cout<<alg->getName()<<endl;

        for(int j = 0; j < testWrappers.size(); j++){
            timeStart = chrono::system_clock::now(); 
            TesterResult testResultPairs = tester.testIntersection(&dataBsr, &testWrappers[j]->testScenario, alg);
            timeStop = chrono::system_clock::now(); 
            if(!FRAGMENTED_TIMER){
                fullTestingTime = chrono::duration_cast<chrono::milliseconds>(timeStop - timeStart).count();
                testResultPairs.duration = fullTestingTime;
            }
            testWrappers[j]->results.push_back(testResultPairs);
            cout<<testWrappers.at(j)->name<<" "<<testResultPairs.duration<< "ms"<<endl;
        }

        cout<<endl<<endl;

    }

    //Processing results
    cout<<"Checking results"<<endl;
    for(int i = 0; i < testWrappers.size(); i++){
        checkTestResults(testWrappers[i]->results, testWrappers[i]->name);
    }

    cout<<"Writing testing results to file: "<<PATH_RESULT<<endl;
    writeTestResults(durationLoadData, durationBsrGeneration, &testWrappers, PATH_RESULT);


    //Deleting
    for(int i = 0; i < data.size(); i++){
        delete data.at(i);
        delete dataBsr.at(i);
    }
    
    for(int i = 0; i < testWrappers.size(); i++){
        delete testWrappers.at(i);
    }
    for(int i = 0; i < algorithms.size(); i++){
        delete algorithms.at(i);
    }
    for(int i = 0; i < algorithmsBsr.size(); i++){
        delete algorithmsBsr.at(i);
    }

    return 0;
}

vector<int*> loadData(string filename, vector<int>* dataLengths){
    ifstream file(filename, ios::binary);
    char buffer[BUFFER_SIZE];

    vector<int*> output;
    int documentCount = 0;
    int currentDocumentCount = 0;
    int processedTerms = 0;
    int* currentTermArray;   

    while(file.read(buffer, BUFFER_SIZE)){
        int readChars = file.gcount();
        for(int i = 0; i < readChars; i+=4){
            char* binaryValue = new char[4];
            for(int j = 0; j < 4; j++){
                binaryValue[j] = buffer[i+j];
            }
            int value = *(reinterpret_cast<int*>(binaryValue));
            delete[] binaryValue;

            if(currentDocumentCount >= documentCount){
                if(LIMIT_DATA_TERMS > 0 && processedTerms >= LIMIT_DATA_TERMS)
                    return output;
                documentCount = value;  
                currentTermArray = new int[documentCount];
                
                if(documentCount > 0){
                    dataLengths->push_back(documentCount);                
                    output.push_back(currentTermArray);
                }

                currentDocumentCount = 0;                                              
                processedTerms++;                
            }
            else{                
                currentTermArray[currentDocumentCount] = value;
                currentDocumentCount++;
            }
        }
    }
    return output;
}

bool checkTestResults(vector<TesterResult> results, string testType){
    bool result = true;
    cout<<"Checking result sizes, test type: "<<testType<<endl;
    for(int i = 1; i < results.size(); i++){
        if(!Tester::checkIntersectionResultSizes(results.at(0), results.at(i))){
            cout<<"Mismatched result sizes, algorithms: " + results.at(0).algorithm + ", " + results.at(i).algorithm<<endl;        
            result = false;
        }        
    }
    return result;
}

vector<BsrArrays*> encodeBsr(vector<int*>* data, vector<int>* dataLengths){
    vector<BsrArrays*> bsrData;
    for(int i = 0; i < data->size(); i++){
        int* array = data->at(i);
        bsrData.push_back(BsrEncoder::generateBsrArrays(array, dataLengths->at(i)));
    }
    return bsrData;
}

void writeTestResults(int durationLoadData, int durationBsr, vector<TestWrapper*>* testWrappers, string path){
    ofstream output;
    output.open(path);
    if(!output){
        cout<<"Failed to open path: "<<path<<endl;
        return;
    }

    output<<"Test settings:"<<endl;
    output<<"TEST_COUNT_RANDOM,"<<TEST_COUNT_RANDOM<<endl;
    output<<"TEST_COUNT_RANDOM_DIST,"<<TEST_COUNT_RANDOM_DIST<<endl;
    output<<"TEST_TOP_PAIRS,"<<TEST_TOP_PAIRS<<endl;
    output<<"TEST_TOP_RANDOM,"<<TEST_TOP_RANDOM<<endl;
    output<<"LIMIT_DATA_TERMS,"<<LIMIT_DATA_TERMS<<endl;
    if(FRAGMENTED_TIMER)
        output<<"FRAGMENTED_TIMER,TRUE"<<endl;
    else
        output<<"FRAGMENTED_TIMER,FALSE"<<endl;

    output<<"Data loading duration,"<<durationLoadData<<endl;
    output<<"BSR generation duration,"<<durationBsr<<endl;
    output<<endl;

    for(int i = 0; i < testWrappers->size(); i++){
        output<<testWrappers->at(i)->name<<endl;
        for(int j = 0; j < testWrappers->at(i)->results.size(); j++){
            TesterResult currentResult = testWrappers->at(i)->results.at(j);
            writeTestResult(&output, &currentResult);
        }
        output<<endl;
    }

    output.close();
}

void writeTestResult(ofstream* outputStream, TesterResult* testerResult){
    *outputStream<<testerResult->algorithm<<","<<testerResult->duration<<endl;
}
