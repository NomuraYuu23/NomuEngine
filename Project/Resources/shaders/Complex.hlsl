
float2 ComplexConjugate(float2 cmp) {

	return float2(cmp.x, -cmp.y);

}

float ComplexSquare(float2 cmp) {

	return cmp.x * cmp.x + cmp.y * cmp.y;

}

float ComplexNormalize(float2 cmp) {

	return sqrt(ComplexSquare(cmp));

}

float2 ComplexAdd(float2 cmp1, float2 cmp2) {

	return float2(cmp1.x + cmp2.x, cmp1.y + cmp2.y);

}

float2 ComplexSub(float2 cmp1, float2 cmp2) {

	return float2(cmp1.x - cmp2.x, cmp1.y - cmp2.y);

}

float2 ComplexMul(float2 cmp1, float2 cmp2) {

	return float2(
		cmp1.x * cmp2.x - cmp1.y * cmp2.y,
		cmp1.y * cmp2.x + cmp1.x * cmp2.y);

}

float2 ComplexDiv(float2 cmp1, float2 cmp2) {

	float2 cmp = ComplexMul(cmp1, ComplexConjugate(cmp2));
	float sqr = ComplexSquare(cmp2);
	return float2(cmp.x / sqr, cmp.y / sqr);

}

float2 ComplexPolar(float amp, float phase) {

	return float2(amp * cos(phase), amp * sin(phase));

}
