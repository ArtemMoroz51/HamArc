#pragma once

#include <iostream>
#include <vector>
#include <string>

int RedutantsAmountCount();
struct UserInput {
public:
    std::vector<std::string> fileNames;
    std::vector<std::string> archiveNames;

    bool create = false;
    bool list = false;
    bool extract = false;
    bool append = false;
    bool delete_ = false;
    bool concatenate = false;
    inline static const  int data_len = 20;
    inline static const int redutants_amount = RedutantsAmountCount();

public:
    void Parse(int argc, char* argv[]);
};
