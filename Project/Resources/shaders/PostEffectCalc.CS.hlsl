float32_t Gauss(float32_t i, float32_t sigma) {

	return 1.0f / (2.0f * PI * sigma * sigma) * exp(-(i * i) / (2.0f * sigma * sigma));

}

float32_t Noise(float32_t2 coord)
{

	return frac(sin(dot(coord, float32_t2(8.7819f, 3.255f))) * 437.645);

}