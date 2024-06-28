#pragma once

#include <print>

#define DODO_INFO(x, ...) std::println(x,## __VA_ARGS__)
#define DODO_WARN(x, ...) std::println(x, ##__VA_ARGS__)
#define DODO_ERROR(x, ...) std::println(x, ##__VA_ARGS__)
#define DODO_CRITICAL(x, ...) std::println(x, ##__VA_ARGS__); exit(1)