#pragma once
#include <variant>
#include "../Collider/AABB/AABB.h"
#include "../Collider/OBB/OBB.h"
#include "../Collider/Plane/Plane.h"
#include "../Collider/Sphere/Sphere.h"
#include "../Collider/Triangle/Triangle.h"

using ColliderShape = std::variant<AABB*, OBB*, Plane*, Sphere*, Triangle*>;