#ifndef TESTER_HPP
#define TESTER_HPP

#include <vector>
#include "intersectionAlg.hpp"


class Tester{
    public:
        //Intersections of randomly selected pairs of terms
        int randomPairIntersection(std::vector<std::vector<int>*>* data, IntersectionAlg* algorithm, int count);

        //Intersections of randomly selected pairs of terms; distributive randomness based on the number of elements of given term
        int randomDistPairIntersection(std::vector<std::vector<int>*>* data, IntersectionAlg* algorithm, int count);

        //Intersections of randomly selected multiple terms; number of terms selected randomly; term selection by distributive randomness
        int randomDistMultipleIntersections(std::vector<std::vector<int>*>* data, IntersectionAlg* algorithm, int count, int maxTermCount);                

        
    private:
        //Generates array of term lengths for distributed probabilities; returns max value
        long getTermLengthArray(std::vector<std::vector<int>*>* data, long* result);

        //Finds the term based on distributed probability
        int getTermFromDistribution(long* lengthArray, int lengthArraySize, int value);

        //Intersects the given terms, returns duration
        int intersect(std::vector<std::vector<int>*>* data, int* terms, int termsLength, IntersectionAlg* algorithm);
};

#endif