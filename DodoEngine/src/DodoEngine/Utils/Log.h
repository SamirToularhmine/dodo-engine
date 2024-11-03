#pragma once

#include <print>

#define DODO_INFO(...) std::println(__VA_ARGS__)
#define DODO_WARN(...) std::println(__VA_ARGS__)
#define DODO_ERROR(...) std::println(__VA_ARGS__)
#define DODO_CRITICAL(...) std::println(__VA_ARGS__); exit(1)