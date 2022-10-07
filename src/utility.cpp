#include "utility.hpp"

#include <fstream>
#include <sstream>

namespace poyenc {

std::string read_file_content(std::string_view path, std::ios_base::openmode mode) {
  std::ifstream file(path.data(), mode);

  std::streampos file_size = file.tellg();
  file.seekg(0, std::ios::end);
  file_size = file.tellg() - file_size;
  file.seekg(0, std::ios::beg);

  std::string content(file_size, '\0');
  file.read(content.data(), file_size);

  return content;
}

void write_file_content(std::string_view path, std::string_view content, std::ios_base::openmode mode) {
  std::ofstream file(path.data(), mode);
  file.write(content.data(), content.size());
}

}  // namespace poyenc
