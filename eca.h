#ifndef ECA_H
#define ECA_H

#include <vector>

class ECA
{
public:
    ECA();
    ECA(std::vector<bool> init_state);
    ECA(std::vector<bool> init_state, std::vector<bool> rule);

    std::vector< std::vector<bool> > getState();
    void computeMultipleGeneration(unsigned int num_generation);


private:
    std::vector<bool> getNextGeneration();
    bool useRule(bool left_cell, bool cell, bool right_cell);

    std::vector<bool> rule;
    std::vector<std::vector<bool>> state;

};

#endif // ECA_H
