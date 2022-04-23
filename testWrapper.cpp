#include "testWrapper.hpp"

TestWrapper::TestWrapper(){
    
}

TestWrapper::TestWrapper(std::vector<std::vector<int>*> scenario, std::string scenarioName){
    this->testScenario = scenario;
    this->name = scenarioName;
}

TestWrapper::~TestWrapper(){
    for(int i = 0; i < this->testScenario.size(); i++){
        delete this->testScenario.at(i);
    }
}