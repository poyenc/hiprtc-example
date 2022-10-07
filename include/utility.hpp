#include <ios>
#include <string>
#include <string_view>

namespace poyenc {
std::string read_file_content(std::string_view path, std::ios_base::openmode mode = std::ios_base::in);
void write_file_content(std::string_view path, std::string_view content, std::ios_base::openmode mode = std::ios_base::out);
}  // namespace poyenc
