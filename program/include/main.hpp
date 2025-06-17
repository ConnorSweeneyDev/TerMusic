#pragma once

#include <filesystem>

void handle_args(int argc, char *argv[]);
void initialize_playlist(const std::filesystem::path &path);
int run_loop();
int main(int argc, char *argv[]);
