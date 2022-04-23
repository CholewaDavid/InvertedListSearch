#ifndef _TEST_WRAPPER_HPP
#define _TEST_WRAPPER_HPP

#include <vector>
#include "tester.hpp"

class TestWrapper{
    public:
        TestWrapper();
        TestWrapper(std::vector<std::vector<int>*> scenario, std::string scenarioName);
        ~TestWrapper();

        std::vector<std::vector<int>*> testScenario;
        std::vector<TesterResult> results;
        std::string name;
    private:

};


#endif