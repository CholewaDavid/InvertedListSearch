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

int Tester::randomPairIntersection(vector<vector<int>*>* data, IntersectionAlg* algorithm, int count){
    srand(time(0));
    int duration = 0;

    for(int i = 0; i < count; i++){
        int indexArrayA = rand() % data->size();
        int indexArrayB = rand() % data->size();
        int indexes[2] = {indexArrayA, indexArrayB};

        duration += this->intersect(data, indexes, 2, algorithm);
    }
    return duration;
}

int Tester::randomDistPairIntersection(vector<vector<int>*>* data, IntersectionAlg* algorithm, int count){
    srand(time(0));
    long* termLengthArray = new long[data->size()];
    long maxValue = this->getTermLengthArray(data, termLengthArray);

    int duration = 0;

    for(int i = 0; i < count; i++){        
        int valueA = rand() % maxValue;
        int valueB = rand() % maxValue;        
        int indexArrayA = this->getTermFromDistribution(termLengthArray, data->size(), valueA);
        int indexArrayB = this->getTermFromDistribution(termLengthArray, data->size(), valueB);
        int indexes[2] = {indexArrayA, indexArrayB};
        duration += this->intersect(data, indexes, 2, algorithm);
    }
    delete termLengthArray;

    return duration;
}

int Tester::randomDistMultipleIntersections(vector<vector<int>*>* data, IntersectionAlg* algorithm, int count, int maxTermCount){
    srand(time(0));
    long* termLengthArray = new long[data->size()];
    long maxValue = this->getTermLengthArray(data, termLengthArray);

    int duration = 0;

    for(int i = 0; i < count; i++){
        int termCount = rand() % (maxTermCount - 2) + 2;
        
        int* indexes = new int[termCount];
        for(int j = 0; j < termCount; j++){
            int value = rand() % maxValue;
            int indexArray = this->getTermFromDistribution(termLengthArray, data->size(), value);
            indexes[j] = indexArray;
        }
                
        duration += this->intersect(data, indexes, termCount, algorithm);
        delete indexes;
    }

    delete termLengthArray;

    return duration;
}

long Tester::getTermLengthArray(vector<vector<int>*>* data, long* result){
    long currentLength = 0;
    int resultSize = data->size();
    for(int i = 0; i < resultSize; i++){
        currentLength += data->at(i)->size();
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

int Tester::intersect(vector<vector<int>*>* data, int* terms, int termsLength, IntersectionAlg* algorithm){    
    int duration = 0;
    chrono::time_point<chrono::system_clock> timeStart;
    chrono::time_point<chrono::system_clock> timeStop;     

    int progressArraySize = data->at(terms[0])->size();
    int* progressArray = new int[progressArraySize];
    for(int i = 0; i < progressArraySize; i++){
        progressArray[i] = data->at(terms[0])->at(i);
    }

    for(int i = 1; i < termsLength; i++){
        int indexArrayNew = terms[i];
        int arrayNewSize = data->at(indexArrayNew)->size();
        int* arrayNew = new int[arrayNewSize];

        for(int j = 0; j < arrayNewSize; j++){
            arrayNew[j] = data->at(indexArrayNew)->at(j);
        }

        int sizeResult = arrayNewSize;
        if(arrayNewSize < progressArraySize)
            sizeResult = progressArraySize;

        int* arrayAlgResult;
        align_malloc((void**)&arrayAlgResult, 32, sizeof(int) * sizeResult);
        int arrayAlgResultSize;

        timeStart = chrono::system_clock::now(); 
        arrayAlgResultSize = algorithm->intersect(progressArray, progressArraySize, arrayNew, arrayNewSize, arrayAlgResult);
        timeStop = chrono::system_clock::now();

        duration += chrono::duration_cast<chrono::milliseconds>(timeStop - timeStart).count();

        delete progressArray;
        delete arrayNew;

        progressArray = new int[arrayAlgResultSize];
        for(int j = 0; j < arrayAlgResultSize; j++){
            progressArray[j] = arrayAlgResult[j];
        }
        progressArraySize = arrayAlgResultSize;

        free(arrayAlgResult);
    } 
    
    delete progressArray;

    return duration;
}