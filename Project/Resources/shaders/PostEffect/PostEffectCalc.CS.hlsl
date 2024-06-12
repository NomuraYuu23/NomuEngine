float32_t Gauss(in const float32_t i, in const float32_t sigma) {

	return rcp(2.0f * PI * sigma * sigma) * exp(-(i * i) * rcp(2.0f * sigma * sigma));

}

float32_t Noise(in const float32_t2 coord)
{

	return frac(sin(dot(coord, float32_t2(8.7819f, 3.255f))) * 437.645);

}

float32_t2 GetTexcoord(in const float32_t2 index, in const float32_t2 textureSize) {

	return float32_t2(
		index.x * rcp(textureSize.x),
		index.y * rcp(textureSize.y) );

}

float32_t Luminance(float32_t3 v) {
	return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}
