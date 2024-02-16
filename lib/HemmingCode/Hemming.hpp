#pragma once

#include "../Archiver/Archiver.hpp"

#include <iostream>
#include <vector>
#include <stdint.h>
#include <fstream> 

class CatalogHeader;

std::vector<CatalogHeader> ReadCatalog(std::string& archive_path);

std::ofstream& WriteFile(std::ofstream& archive, std::string& path, uint64_t& offset);
std::ofstream& WriteCatalog(std::ofstream& archive, std::vector<CatalogHeader>& catalog);

std::ifstream& DecodeFile(std::ifstream& archive, uint64_t file_offset);
