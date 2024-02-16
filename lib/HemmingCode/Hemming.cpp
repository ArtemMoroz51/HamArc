#include "Hemming.hpp"

inline int PowerTwo(int power) { return 1 << power; }
inline bool IsPowerTwo(int x) { return x && (!(x & (x - 1))); }

// 0000(1111) info bits are at right side
bool* Encode4Bits(uint8_t bits){
  bool* encoded = new bool[8]{0};
  bool total_parity = 0;

  for (int32_t bit = 0, place = 3; bit < 4; bit++, place++) {
    bool cur_bit = (bits << bit) & 0b1000;
    
    if (IsPowerTwo(place)) place++;
    encoded[place] = cur_bit;

    if (cur_bit) {
      total_parity ^= 1;
      for (int8_t add = 0; add < 7; add++) {
          encoded[PowerTwo(add)] ^= (bool)(place & (1 << add));
          total_parity ^= (bool)(place & (1 << add));
      }
    }
  }

  encoded[0] = total_parity;

  return encoded;
}

char BitsToChar(bool* data){
  char result = 0;
  for(uint32_t i = 0; i < 8; i++)
    result |= (0x80 >> i) * data[i];
  
  return result;
}

std::ofstream& EncodeByte(std::ofstream& out_stream, uint8_t byte){
  bool* first_byte = Encode4Bits((byte >> 4) & 0b1111);
  bool* second_byte = Encode4Bits(byte & 0b1111);
  
  char encoded_first = BitsToChar(first_byte);
  char encoded_second = BitsToChar(second_byte);
   
  out_stream.write(&encoded_first, sizeof(encoded_first));
  out_stream.write(&encoded_second, sizeof(encoded_second));
  
  delete[] first_byte;
  delete[] second_byte;

  return out_stream;
}

std::ofstream& WriteFile(std::ofstream& archive, std::string& file_path, uint64_t& offset){
  std::ifstream file(file_path, std::ios::in | std::ios::binary);
 
  uint64_t path_size = file_path.length();
  uint64_t encoded_size = 0;
  uint64_t header_size = sizeof(encoded_size) + sizeof(path_size) + path_size;
  
  archive.write((char*)&path_size, sizeof(path_size));
  archive.write(file_path.c_str(), path_size);
  
  uint64_t size_position = archive.tellp();
  
  archive.write((char*)&encoded_size, sizeof(encoded_size));
  
  offset += header_size;

  char buffer;
  while(file.read(&buffer, sizeof(buffer))){
    EncodeByte(archive, buffer);
    offset += 2;
  }
  
  uint64_t end_position = archive.tellp();

  archive.seekp(size_position, std::ios::beg);
  
  uint64_t file_size = end_position - size_position - sizeof(size_position); 
  
  archive.write((char*)&file_size, sizeof(file_size));
  archive.seekp(end_position, std::ios::beg); 

  return archive;
}

std::vector<CatalogHeader> ReadCatalog(std::string& archive_path){
  std::vector<CatalogHeader> catalog;
  
  std::ifstream archive(archive_path, std::ios::in | std::ios::binary);

  uint64_t catalog_offset;

  archive.read((char*)&catalog_offset, sizeof(catalog_offset));

  archive.seekg(catalog_offset, std::ios::beg);

  uint64_t catalog_size;

  archive.read((char*)&catalog_size, sizeof(catalog_size));
  
  uint64_t path_size;
  std::string path;
  uint64_t file_offset;

  for(uint64_t i = 0; i < catalog_size; i++){
    archive.read((char*)&path_size, sizeof(path_size));
    path.resize(path_size);

    archive.read((char*)path.c_str(), path_size);
    archive.read((char*)&file_offset, sizeof(file_offset));

    catalog.push_back(CatalogHeader{path, file_offset});
  }
  
  return catalog;
}

std::ofstream& WriteCatalog(std::ofstream& archive, std::vector<CatalogHeader>& catalog){
  
  uint64_t catalog_size = catalog.size();

  archive.write((char*)&catalog_size, sizeof(catalog_size));
  
  for(int i = 0; i < catalog.size(); i++){
    uint64_t path_size = catalog[i].path.length();
    uint64_t file_offset = catalog[i].offset;
    
    archive.write((char*)&path_size, sizeof(path_size));
    archive.write(catalog[i].path.c_str(), path_size);
    archive.write((char*)&file_offset, sizeof(file_offset)); 
 
  }  

  return archive;
}

bool* ByteToBool(char buffer){
  bool* bits = new bool[8];
  for(int i = 0; i < 8; i++)
    bits[i] = (0x80 >> i) & buffer;

  return bits;
}

void FixError(bool* bits){
  uint32_t error_place = 0;
  bool total_parity = 0;
  
  for(int32_t i = 1; i < 8; i++){
    if(bits[i]){
      error_place ^= i;
      total_parity ^= 1;
    }
  }  

  if(error_place){
    bits[error_place] = !bits[error_place];
    total_parity ^= 1;
  }

  if(total_parity != bits[0]){
    std::cout << "file is too damaged\n";
    exit(0);
  }
}

char BitsToByte(bool* buffer1, bool* buffer2){
  char merged = 0;
  
  for(int32_t i = 0; i < 4; i++){
    merged |= buffer1[i] * (0x80 >> i);
    merged |= buffer2[i] * (0x8 >> i);
  }

  return merged;
}

char DecodeBytes(char buffer1, char buffer2){
  char decoded = 0;
  
  bool* first_bits = ByteToBool(buffer1);
  bool* second_bits = ByteToBool(buffer2);
   
  FixError(first_bits);
  FixError(second_bits);
  
  bool* decoded_first = new bool[4]{0};
  bool* decoded_second = new bool[4]{0};

  for(int32_t i = 3, j = 0; i < 8; i++){
    if(IsPowerTwo(i)) continue;
    decoded_first[j] = first_bits[i];
    j++;
  }
  
  for(int32_t i = 3, j = 0; i < 8; i++){
    if(IsPowerTwo(i)) continue;
    decoded_second[j] = second_bits[i];
    j++;
  }

  decoded = BitsToByte(decoded_first, decoded_second); 
  
  delete[] decoded_first;
  delete[] decoded_second;
  delete[] first_bits;
  delete[] second_bits; 

  return decoded;
}

std::ifstream& DecodeFile(std::ifstream& archive, uint64_t file_offset){
  
  uint64_t path_size;
  
  archive.seekg(file_offset, std::ios::beg);
  
  archive.read((char*)&path_size, sizeof(path_size));

  
  std::string path;
  path.resize(path_size);

  archive.read((char*)path.c_str(), path_size);

  uint64_t file_size;
  archive.read((char*)&file_size, sizeof(file_size));
  
  std::ofstream file(path, std::ios::out | std::ios::binary);

  char buffer1;
  char buffer2;
  char decoded;

  for(uint64_t i = 0; i < file_size/2; i++){
    archive.read((char*)&buffer1, sizeof(buffer1));
    archive.read((char*)&buffer2, sizeof(buffer2));
    
    decoded = DecodeBytes(buffer1, buffer2);
    file.write(&decoded, sizeof(decoded));
  }

  return archive;
}
