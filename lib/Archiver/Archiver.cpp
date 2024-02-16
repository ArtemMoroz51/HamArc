#include "Archiver.hpp" 

void App::Run(int32_t argc, char** argv){
    UserInput input;
    input.Parse(argc, argv);
    
    if (input.create) {
        std::cout << input.archiveNames.front();
        CreateArchive(input.fileNames, input.archiveNames.front());
    }
    if (input.list) {
        PrintCatalog(input.archiveNames.front());
    }
    if (input.extract) {
        ExtractFile(input.fileNames.front(), input.archiveNames.front());
    }
    if (input.append) {
        AppendFile(input.fileNames.front(), input.archiveNames.front());
    }
}

void App::CreateArchive(std::vector<std::string>& file_paths, std::string& archive_name){
  std::ofstream archive(archive_name, std::ios::out | std::ios::binary);

  uint64_t catalog_offset = 8;
  archive.write((char*)&catalog_offset, sizeof(catalog_offset));
  
  std::vector<CatalogHeader> catalog;
  
  for(int i = 0; i < file_paths.size(); i++){
    std::cout << "encoding: " <<  file_paths[i] << '\n';
    catalog.push_back(CatalogHeader{file_paths[i], catalog_offset});
    WriteFile(archive, file_paths[i], catalog_offset);
  }

  archive.seekp(0, std::ios::beg);
  archive.write((char*)&catalog_offset, sizeof(catalog_offset));
  archive.seekp(catalog_offset, std::ios::beg);

  WriteCatalog(archive, catalog); 
  
}

void App::ExtractFile(std::string& file_name, std::string& archive_path){
  std::ifstream archive(archive_path, std::ios::binary | std::ios::in);

  uint64_t catalog_offset = 0;
  archive.read((char*)&catalog_offset, sizeof(catalog_offset));
  
  archive.seekg(catalog_offset, std::ios::beg);

  uint64_t catalog_size = 0;

  archive.read((char*)&catalog_size, sizeof(catalog_size));

  uint64_t path_size;
  uint64_t file_offset;
  std::string current_path;

  for(int i = 0; i < catalog_size; i++){
    
    archive.read((char*)&path_size, sizeof(path_size));
    current_path.resize(path_size);
    archive.read((char*)current_path.c_str(), path_size);
    archive.read((char*)&file_offset, sizeof(file_offset));
    
    if(current_path == file_name){
      DecodeFile(archive, file_offset);
      break;
    }
  } 
}

void App::PrintCatalog(std::string& archive_path){
  std::vector<CatalogHeader> catalog = ReadCatalog(archive_path);

  for(uint64_t i = 0; i < catalog.size(); i++)
    std::cout << i + 1 << ") " <<  catalog[i].path << '\n';

}

void App::AppendFile(std::string& file_name, std::string& archive_name){
  std::vector<CatalogHeader> catalog = ReadCatalog(archive_name);
  
  std::ifstream arc(archive_name, std::ios::in | std::ios::binary );
  
  uint64_t catalog_offset;
  arc.read((char*)&catalog_offset, sizeof(catalog_offset));
  arc.close();

  std::ofstream archive(archive_name, std::ios::out | std::ios::binary);
  
  archive.seekp(catalog_offset, std::ios::beg);
  

  catalog.push_back(CatalogHeader{file_name, catalog_offset});
  
  WriteFile(archive, file_name, catalog_offset);

  archive.seekp(0, std::ios::beg);
  archive.write((char*)&catalog_offset, sizeof(catalog_offset));
  archive.seekp(catalog_offset, std::ios::beg);
  
  WriteCatalog(archive, catalog); 
    
}

