#include "Segment.h"
#include <cmath>
#include <algorithm>

void Segment::Initialize(const Vector3& origin, const Vector3& diff)
{

	origin_ = origin;
	diff_ = diff;

}

Vector3 Segment::ClosestPoint(const Vector3& point, const Segment& segment)
{
	float t = Vector3Calc::Dot(Vector3Calc::Subtract(point, segment.origin_), segment.diff_) / std::powf(Vector3Calc::Length(segment.diff_), 2.0f);
	t = std::clamp(t, 1.0f, 0.0f);
	return Vector3Calc::Add(segment.origin_, Vector3Calc::Multiply(t, segment.diff_));
}
