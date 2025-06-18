#pragma once

#include <filesystem>

int main();
void initialize_playlist(const std::filesystem::path &path);
int run_loop();
