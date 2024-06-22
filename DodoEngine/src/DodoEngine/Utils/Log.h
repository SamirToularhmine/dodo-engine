#include <print>

#define DODO_INFO(x) std::println(x)
#define DODO_WARN(x) std::println(x)
#define DODO_ERROR(x) std::println(x)
#define DODO_CRITICAL(x) std::println(x); exit(1)