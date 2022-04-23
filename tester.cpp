#include "tester.hpp"
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <ctime>
#include <cmath>

using namespace std;

void align_malloc(void **memptr, size_t alignment, size_t size)
{
    int malloc_flag = posix_memalign(memptr, alignment, size);
}

vector<vector<int>*> Tester::generateRandomPairTerms(int count, int termCount){    
    vector<vector<int>*> output;
    for(int i = 0; i < count; i++){
        int indexArrayA = rand() % termCount;
        int indexArrayB = rand() % termCount;
        vector<int>* currentTerms = new vector<int>();
        currentTerms->push_back(indexArrayA);
        currentTerms->push_back(indexArrayB);
        output.push_back(currentTerms);
    }
    return output;
}

vector<vector<int>*> Tester::generateRandomDistPairTerms(int count, std::vector<int*>* data, std::vector<int>* dataLengths){   
    vector<vector<int>*> output;
    long* termLengthArray = new long[data->size()];
    long maxValue = this->getTermLengthArray(dataLengths, termLengthArray);

    int duration = 0;

    for(int i = 0; i < count; i++){        
        int valueA = rand() % maxValue;
        int valueB = rand() % maxValue;        
        int indexArrayA = this->getTermFromDistribution(termLengthArray, data->size(), valueA);
        int indexArrayB = this->getTermFromDistribution(termLengthArray, data->size(), valueB);        
        vector<int>* currentTerms = new vector<int>();
        currentTerms->push_back(indexArrayA);
        currentTerms->push_back(indexArrayB);
        output.push_back(currentTerms);
    }
    delete termLengthArray;

    return output;
}

std::vector<std::vector<int>*> Tester::generateRandomDistMultipleTerms(int count, std::vector<int*>* data, std::vector<int>* dataLengths, int maxTermCount){    
    vector<vector<int>*> output;
    long* termLengthArray = new long[data->size()];
    long maxValue = this->getTermLengthArray(dataLengths, termLengthArray);

    int duration = 0;

    for(int i = 0; i < count; i++){
        int termCount = rand() % (maxTermCount - 2) + 2;
        vector<int>* currentTerms = new vector<int>();

        for(int j = 0; j < termCount; j++){
            int value = rand() % maxValue;
            int indexArray = this->getTermFromDistribution(termLengthArray, data->size(), value);
            currentTerms->push_back(indexArray);
        }
        output.push_back(currentTerms);
    }

    delete termLengthArray;

    return output;
}

std::vector<std::vector<int>*> Tester::generateTopPairs(int count){
    vector<vector<int>*>output;
    for(int i = 0; i < count - 1; i++){
        for(int j = i; j < count; j++){
            vector<int>* currentTerms = new vector<int>();
            currentTerms->push_back(i);
            currentTerms->push_back(j);
            output.push_back(currentTerms);
        }
    }
    return output;
}

std::vector<std::vector<int>*> Tester::generateTopRandomPair(int count, int dataSize){
    vector<vector<int>*> output;
    for(int i = 0; i < count; i++){
        int indexArrayA = 0;
        int indexArrayB = rand() % dataSize;
        vector<int>* currentTerms = new vector<int>();
        currentTerms->push_back(indexArrayA);
        currentTerms->push_back(indexArrayB);
        output.push_back(currentTerms);
    }
    return output;
}

TesterResult Tester::testIntersection(std::vector<int*>* data, std::vector<int>* dataLengths, std::vector<std::vector<int>*>* terms, IntersectionAlg* algorithm){
    srand(time(0));
    
    TesterResult output, tempOutput;
    output.algorithm = algorithm->getName();

    for(int i = 0; i < terms->size(); i++){
        int termCount = terms->at(i)->size();        
        int* indexes = new int[termCount];
        for(int j = 0; j < termCount; j++){
            indexes[j] = terms->at(i)->at(j);
        }

        tempOutput = this->intersect(data, dataLengths, indexes, termCount, algorithm);
        output.duration += tempOutput.duration;
        output.resultLengths.push_back(tempOutput.resultLengths.at(0));
        delete[] indexes;
    }

    return output;
}

TesterResult Tester::testIntersection(std::vector<BsrArrays*>* data, std::vector<std::vector<int>*>* terms, IntersectionAlgBsr* algorithm){
    srand(time(0));
    
    TesterResult output, tempOutput;
    output.algorithm = algorithm->getName();

    for(int i = 0; i < terms->size(); i++){
        int termCount = terms->at(i)->size();        
        int* indexes = new int[termCount];
        for(int j = 0; j < termCount; j++){
            indexes[j] = terms->at(i)->at(j);
        }
                
        tempOutput = this->intersectPairsBsr(data, indexes, termCount, algorithm);
        output.duration += tempOutput.duration;
        output.resultLengths.push_back(tempOutput.resultLengths.at(0));
        delete indexes;
    }

    return output;
}

bool Tester::checkIntersectionResultSizes(TesterResult reference, TesterResult result){
    if(reference.resultLengths.size() != result.resultLengths.size())
        return false;
    for(int j = 0; j < reference.resultLengths.size(); j++){
        if(reference.resultLengths.at(j) != result.resultLengths.at(j)){
            return false;
        }
    }
    return true;
}

long Tester::getTermLengthArray(vector<int>* dataLengths, long* result){
    long currentLength = 0;
    int resultSize = dataLengths->size();
    for(int i = 0; i < resultSize; i++){
        currentLength += dataLengths->at(i);
        result[i] = currentLength;
    }
    return currentLength;
}

int Tester::getTermFromDistribution(long* lengthArray, int lengthArraySize, int value){
    int left = 0;
    int right = lengthArraySize - 1;
    while(left <= right){
        int middle = floor((left + right) / 2);
        if(middle == 0 && value < lengthArray[middle])
            return middle;
        if(value > lengthArray[middle] && ((middle == (lengthArraySize - 1)) || value <= lengthArray[middle + 1]))
            return middle;        
        if(value > lengthArray[middle])
            left = middle + 1;
        else
            right = middle - 1;
    }    
    return -1;
}

TesterResult Tester::intersect(std::vector<int*>* data, std::vector<int>* dataLengths, int* terms, int termsLength, IntersectionAlg* algorithm){    
    TesterResult output;

    chrono::time_point<chrono::system_clock> timeStart;
    chrono::time_point<chrono::system_clock> timeStop;     

    int progressArraySize = dataLengths->at(terms[0]);
    int* progressArray = new int[progressArraySize];
    for(int i = 0; i < progressArraySize; i++){
        progressArray[i] = data->at(terms[0])[i];
    }

    for(int i = 1; i < termsLength; i++){
        int indexArrayNew = terms[i];
        int arrayNewSize = dataLengths->at(indexArrayNew);
        int* arrayNew = data->at(indexArrayNew);

        int* arrayAlgResult;
        align_malloc((void**)&arrayAlgResult, 32, sizeof(int) * std::min(arrayNewSize, progressArraySize));
        int arrayAlgResultSize;

        timeStart = chrono::system_clock::now(); 
        arrayAlgResultSize = algorithm->intersect(progressArray, progressArraySize, arrayNew, arrayNewSize, arrayAlgResult);
        timeStop = chrono::system_clock::now();

        output.duration += chrono::duration_cast<chrono::nanoseconds>(timeStop - timeStart).count();

        delete[] progressArray;

        progressArray = new int[arrayAlgResultSize];
        for(int j = 0; j < arrayAlgResultSize; j++){
            progressArray[j] = arrayAlgResult[j];
        }
        progressArraySize = arrayAlgResultSize;

        free(arrayAlgResult);
    } 
    
    output.resultLengths.push_back(progressArraySize);
    output.duration = output.duration / 1000000;

    delete[] progressArray;

    return output;
}

TesterResult Tester::intersectPairsBsr(vector<BsrArrays*>* data, int* terms, int termsLength, IntersectionAlgBsr* algorithm){    
    TesterResult output;

    chrono::time_point<chrono::system_clock> timeStart;
    chrono::time_point<chrono::system_clock> timeStop;   

    int arrayAlgResultSize;  

    for(int i = 1; i < termsLength; i++){
        BsrArrays* bsrArrayA = data->at(terms[0]);
        BsrArrays* bsrArrayB = data->at(terms[1]);

        int* arrayAlgResult;
        align_malloc((void**)&arrayAlgResult, 32, sizeof(int) * std::min(bsrArrayA->size, bsrArrayB->size));
        
        timeStart = chrono::system_clock::now(); 
        arrayAlgResultSize = algorithm->intersect(bsrArrayA, bsrArrayB, arrayAlgResult);
        timeStop = chrono::system_clock::now();

        output.duration += chrono::duration_cast<chrono::nanoseconds>(timeStop - timeStart).count();

        free(arrayAlgResult);
    } 
    
    output.resultLengths.push_back(arrayAlgResultSize);
    output.duration = output.duration / 1000000;

    return output;
}

TesterResult::TesterResult(){
    this->duration = 0;
    this->resultLengths = vector<int>();
    this->algorithm = "";
}