float32_t Noise(float32_t2 coord)
{

	return frac(sin(dot(coord, float32_t2(8.7819f, 3.255f))) * 437.645);

}