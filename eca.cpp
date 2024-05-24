#include "eca.h"

ECA::ECA() {
    this->state.reserve(1);
    for (int i=0; i < 8; ++i){
        this->rule.push_back(false);
    }
}

ECA::ECA(std::vector<bool> init_state, std::vector<bool> rule){
    this->state.push_back(init_state);
    this->rule = rule;
}

std::vector<std::vector<bool> > ECA::getState()
{
    return this->state;
}

void ECA::computeMultipleGeneration(unsigned int num_generation)
{
    for (unsigned int gen = 0; gen < num_generation; ++gen){
        std::vector<bool> next_gen = getNextGeneration();
        state.push_back(next_gen);
    }
}

std::vector<bool> ECA::getNextGeneration()
{
    std::vector<bool> current_gen = state.back();
    std::vector<bool> next_gen{};

    for (unsigned int i=0; i < current_gen.size(); ++i){
        bool left_cell, cell, right_cell;

        //if cell is at beginning of list, use cell at end of list as left neighbour
        if(i == 0)
            left_cell = current_gen.back();
        else
            left_cell = current_gen.at(i - 1);

        cell = current_gen.at(i);

        //if cell is at end of list, use cell at beginning of list as right neighbour
        if(i == current_gen.size() - 1)
            right_cell = current_gen.at(0);
        else
            right_cell = current_gen.at(i + 1);

        //calculate if cell is alive and write new state to the new generation
        next_gen.push_back(useRule(left_cell, cell, right_cell));
    }
    return next_gen;
}

//tests which of the 8 rules applies and reads it from the ruleset
bool ECA::useRule(bool left_cell, bool cell, bool right_cell) {

    if(left_cell && cell && right_cell)
        return rule.at(0);
    if(left_cell && cell && !right_cell)
        return rule.at(1);
    if(left_cell && !cell && right_cell)
        return rule.at(2);
    if(left_cell && !cell && !right_cell)
        return rule.at(3);
    if(!left_cell && cell && right_cell)
        return rule.at(4);
    if(!left_cell && cell && !right_cell)
        return rule.at(5);
    if(!left_cell && !cell && right_cell)
        return rule.at(6);
    if(!left_cell && !cell && !right_cell)
        return rule.at(7);

    return false;
}
