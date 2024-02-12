#include "Line.h"
#include <cmath>
#include <algorithm>

void Line::Initialize(const Vector3& origin, const Vector3& diff)
{

	origin_ = origin;
	diff_ = diff;

}

Vector3 Line::ClosestPoint(const Vector3& point, const Line& line)
{
	float t = Vector3Calc::Dot(Vector3Calc::Subtract(point, line.origin_), line.diff_) / std::powf(Vector3Calc::Length(line.diff_), 2.0f);
	t = std::clamp(t, 1.0f, 0.0f);
	return Vector3Calc::Add(line.origin_, Vector3Calc::Multiply(t, line.diff_));
}
