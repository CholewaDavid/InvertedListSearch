#ifndef TESTER_HPP
#define TESTER_HPP

#include <vector>
#include "intersectionAlg.hpp"
#include "intersectionAlgBsr.hpp"
#include "roaring.hpp"


class TesterResult{
    public:                
        TesterResult();       

        double duration;
        std::vector<int> resultLengths;
        std::string algorithm;
};

class Tester{
    public:
        //Checks lengths of results
        static bool checkIntersectionResultSizes(TesterResult reference, TesterResult result);

        //Generate testing data for random pair intersection
        std::vector<std::vector<int>*> generateRandomPairTerms(int count, int termCount);

        //Generate testing data for distributed random pair intersection
        std::vector<std::vector<int>*> generateRandomDistPairTerms(int count, std::vector<int*>* data, std::vector<int>* dataLengths);

        //Generate testing data for distributed random multiple intersection
        std::vector<std::vector<int>*> generateRandomDistMultipleTerms(int count, std::vector<int*>* data, std::vector<int>* dataLengths, int maxTermCount);

        //Generate testing data from the top "count" terms
        std::vector<std::vector<int>*> generateTopPairs(int count);

        //Generate testing pairs of top one term and random second term
        std::vector<std::vector<int>*> generateTopRandomPair(int count, int dataSize);

        //Intersections of terms
        TesterResult testIntersection(std::vector<int*>* data, std::vector<int>* dataLengths, std::vector<std::vector<int>*>* terms, IntersectionAlg* algorithm); 

        //Intersections of terms for BSR encoding
        TesterResult testIntersection(std::vector<BsrArrays*>* data, std::vector<std::vector<int>*>* terms, IntersectionAlgBsr* algorithm);            
                
    private:
        //Generates array of term lengths for distributed probabilities; returns max value
        long getTermLengthArray(std::vector<int>* dataLengths, long* result);

        //Finds the term based on distributed probability
        int getTermFromDistribution(long* lengthArray, int lengthArraySize, int value);

        //Intersects the given terms
        TesterResult intersect(std::vector<int*>* data, std::vector<int>* dataLengths, int* terms, int termsLength, IntersectionAlg* algorithm);

        //Intersects the pair of given terms for BSR
        TesterResult intersectPairsBsr(std::vector<BsrArrays*>* data, int* terms, int termsLength, IntersectionAlgBsr* algorithm);
};

#endif