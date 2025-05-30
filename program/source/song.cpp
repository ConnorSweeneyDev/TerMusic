#include "song.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "sqlite/sqlite3.h"

namespace tuim
{
  Song::Song(const std::filesystem::path &i_path, const std::string &i_artist, const std::string &i_title,
             const double &i_mean_volume)
    : path(i_path), artist(i_artist), title(i_title), mean_volume(i_mean_volume) {};

  void Song::handle_query(std::vector<Song> &results, sqlite3_stmt *stmt,
                          std::unordered_map<std::string, int> &column_indices)
  {
    const char *path_text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_indices[table_columns[0]]));
    const char *artist_text =
      reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_indices[table_columns[1]]));
    const char *title_text =
      reinterpret_cast<const char *>(sqlite3_column_text(stmt, column_indices[table_columns[2]]));
    const double mean_volume_double = sqlite3_column_double(stmt, column_indices[table_columns[3]]);

    const std::filesystem::path current_path = path_text ? path_text : "";
    const std::string current_artist = artist_text ? artist_text : "";
    const std::string current_title = title_text ? title_text : "";
    const double current_mean_volume = mean_volume_double;
    results.emplace_back(current_path, current_artist, current_title, current_mean_volume);
  }
}
