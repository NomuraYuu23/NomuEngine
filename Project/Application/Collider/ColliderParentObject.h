#pragma once
#include <variant>

class Null;
using ColliderParentObject = std::variant<Null*>;
