#pragma once

#include <filesystem>

int main();
void initialize_playlist(const std::filesystem::path &path);
void load_state();
int run_loop();
