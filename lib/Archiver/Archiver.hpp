#pragma once

#include "../InputParser/InputParser.hpp"
#include "../HemmingCode/Hemming.hpp"

#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

class CatalogHeader{
  public:
    std::string path;
    uint64_t offset;
};


class App{
  public:
    void Run(int32_t argc, char** argv);
    void CreateArchive(std::vector<std::string>& file_paths, std::string& arhcive_name);
    void ExtractFile(std::string& file_name, std::string& archive_path);
    void AppendFile(std::string& file_name, std::string& archive_name);
    void DeleteFile(std::string& file_name, std::string& arhcive_path);
    void Merge(std::string& first_arch, std::string& second_arch);
    void PrintCatalog(std::string& archive_path); 
};
