#pragma once

#include <memory>

#define DODO_BEGIN_NAMESPACE namespace dodo {
#define DODO_END_NAMESPACE }

#define DODO_ENGINE_NAME "Dodo Engine"

DODO_BEGIN_NAMESPACE

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T>
using Ptr = std::unique_ptr<T>;

DODO_END_NAMESPACE