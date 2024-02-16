#include "InputParser.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
void UserInput::Parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];
        if ((std::strcmp(arg, "--create") == 0) || (std::strcmp(arg, "-c") == 0)) {
            this->create = true;
        }
        else  if ((std::strcmp(arg, "--list") == 0) || (std::strcmp(arg, "-l") == 0)) {
            this->list = true;
        }
        else if ((std::strcmp(arg, "--extract") == 0) || (std::strcmp(arg, "-x") == 0)) {
            this->extract = true;
        }
        else if ((std::strcmp(arg, "--append") == 0) || (std::strcmp(arg, "-a") == 0)) {
            this->append = true;
        }
        else if ((std::strcmp(arg, "--delete") == 0) || (std::strcmp(arg, "-d") == 0)) {
            this->delete_ = true;
        }
        else if ((std::strcmp(arg, "--concatenate") == 0) || (std::strcmp(arg, "-A") == 0)) {
            this->concatenate = true;
        }
        else if ((std::strcmp(arg, "--file") == 0) || (std::strcmp(arg, "-f=") == 0)) {
            std::string input_str(argv[i + 1]);
            this->archiveNames.push_back(input_str);
        }
        else {
            this->fileNames.push_back(std::string(arg));
        }
    }

}
int Power_Two(int power) {
    return (1 << power);
}

int RedutantsAmountCount() {
    int redutants_amount = 2;
    while (Power_Two(redutants_amount) < UserInput::data_len + redutants_amount + 1) {
        redutants_amount++;
    }
    return redutants_amount;
}