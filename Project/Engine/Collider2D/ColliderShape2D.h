#pragma once
#include <variant>
#include "Circle.h"
#include "Box.h"

using ColliderShape2D = std::variant<Box*, Circle*>;
