#include "PostEffect.hlsli"

#include "PostEffectCalc.CS.hlsl"

// 定数データ
struct ComputeParameters {

	uint32_t threadIdOffsetX; // スレッドのオフセットX
	uint32_t threadIdTotalX; // スレッドの総数X
	uint32_t threadIdOffsetY; // スレッドのオフセットY
	uint32_t threadIdTotalY; // スレッドの総数Y
	uint32_t threadIdOffsetZ; // スレッドのオフセットZ
	uint32_t threadIdTotalZ; // スレッドの総数Z
	
	float32_t4 clearColor; // クリアするときの色
	
	float32_t threshold; // しきい値
	
	int32_t kernelSize; // カーネルサイズ
	float32_t sigma; // 標準偏差
	
	float32_t time; // 時間
	
	float32_t2 rShift; // Rずらし
	float32_t2 gShift; // Gずらし
	float32_t2 bShift; // Bずらし
	
	float32_t distortion; // 歪み
	
	float32_t vignetteSize; // ビネットの大きさ

	float32_t horzGlitchPase; //水平
	float32_t vertGlitchPase; //垂直
	float32_t glitchStepValue; // グリッチのステップ値

	int32_t radialBlurSamples; // 放射状ブラーのサンプル回数
	float32_t2 radialBlurCenter; // 放射状ブラーの中心座標
	float32_t radialBlurStrength; // 放射状ブラーの広がる強さ
	float32_t radialBlurMask; // 放射状ブラーが適用されないサイズ

};

// 定数データ
ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);

// 速度データ
struct VelocityParameters {
	float32_t2 values; // 値
};

// 速度データ
ConstantBuffer<VelocityParameters> gVelocityConstants: register(b1);

// ソース0
Texture2D<float32_t4> sourceImage0 : register(t0);
// ソース1
Texture2D<float32_t4> sourceImage1 : register(t1);
// 行先
RWTexture2D<float32_t4> destinationImage0 : register(u0);

void Copy(float32_t2 index) {

	destinationImage0[index] = sourceImage0[index];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainCopy(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Copy(dispatchId.xy);

	}

}

void Clear(float32_t2 index) {

	destinationImage0[index] = gComputeConstants.clearColor;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainClear(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Clear(dispatchId.xy);

	}

}

void BinaryThreshold(float32_t2 index) {

	float32_t3 input = sourceImage0[index].rgb;

	float32_t3 col = float32_t3(0.0, 0.0, 0.0);

	if ((input.r + input.g + input.b) / 3.0 > gComputeConstants.threshold) {
		col = float32_t3(1.0, 1.0, 1.0);
	}

	destinationImage0[index] = float32_t4(col, 1.0f);

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBinaryThreshold(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		BinaryThreshold(dispatchId.xy);

	}

}

float32_t Gauss(float32_t i, float32_t sigma) {

	return 1.0f / (2.0f * PI * sigma * sigma) * exp(-(i * i) / (2.0f * sigma * sigma));

}

void GaussianBlur(float32_t2 index, float32_t2 dir) {

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };

	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	for (int32_t i = -gComputeConstants.kernelSize / 2; i < gComputeConstants.kernelSize / 2; i += 2) {

		// インデックス
		indexTmp = index;

		indexTmp.x += (float32_t(i) + 0.5f) * dir.x;
		indexTmp.y += (float32_t(i) + 0.5f) * dir.y;

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// outputに加算
		output += sourceImage0[indexTmp] * weight;

		// 重みの合計に加算
		weightSum += weight;
	}

	// 重みの合計分割る
	output *= (1.0f / weightSum);

	// 代入
	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		GaussianBlur(dispatchId.xy, float32_t2(1.0f, 0.0f));

	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		GaussianBlur(dispatchId.xy, float32_t2(0.0f, 1.0f));

	}

}

void Bloom(float32_t2 index, float32_t2 dir) {

	// 入力色
	float32_t4 input = { 0.0f,0.0f,0.0f,0.0f };
	
	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };

	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	for (int32_t i = -gComputeConstants.kernelSize / 2; i < gComputeConstants.kernelSize / 2; i += 2) {

		// インデックス
		indexTmp = index;

		indexTmp.x += (float32_t(i) + 0.5f) * dir.x;
		indexTmp.y += (float32_t(i) + 0.5f) * dir.y;

		input = sourceImage0[indexTmp];

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// 色確認
		if (((input.r + input.g + input.b) / 3.0 > gComputeConstants.threshold) || 
			(i <= 1 && i >= -1 )) {
			// outputに加算
			output += input * weight;
			// 重みの合計に加算
			weightSum += weight;
		}
	}


	// 重みの合計分割る
	output *= (1.0f / weightSum);

	// 代入
	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Bloom(dispatchId.xy, float32_t2(1.0f, 0.0f));

	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Bloom(dispatchId.xy, float32_t2(0.0f, 1.0f));

	}

}

void BrightnessThreshold(float32_t2 index) {

	float32_t4 input = sourceImage0[index];

	float32_t4 col = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

	if ((input.r + input.g + input.b) / 3.0f > gComputeConstants.threshold) {
		col = input;
	}

	destinationImage0[index] = col;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBrightnessThreshold(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		BrightnessThreshold(dispatchId.xy);

	}

}

void BlurAdd(float32_t2 index) {

	float32_t3 input1 = sourceImage0[index].rgb;
	float32_t3 input2 = sourceImage1[index].rgb;

	float32_t alphaSum = sourceImage0[index].a + sourceImage1[index].a;

	if (alphaSum == 0.0f) {
		destinationImage0[index] = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else {
		float32_t a1 = sourceImage0[index].a / alphaSum;
		float32_t a2 = sourceImage1[index].a / alphaSum;

		float32_t3 col = input1 * a1 + input2 * a2;
		destinationImage0[index] = float32_t4(col, min(alphaSum, 1.0f));
	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBlurAdd(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		BlurAdd(dispatchId.xy);

	}

}

void Overwrite(float32_t2 index) {

	float32_t4 input = sourceImage1[index];
	float32_t4 output = sourceImage0[index];

	if (input.a == 1.0f) {
		destinationImage0[index] = input;
	}
	else if (input.a == 0.0f) {
		destinationImage0[index] = output;
	}
	else {
		
		float32_t alphaOut = 1.0f - input.a;

		alphaOut = min(alphaOut, output.a);

		destinationImage0[index] = input;

		if (output.a != 0.0f) {
			float32_t a = min(alphaOut / output.a, 1.0f);

			float32_t4 color =
				float32_t4(
					output.r * a,
					output.g * a,
					output.b * a,
					alphaOut);

			destinationImage0[index] += color;

		}

	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainOverwrite(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Overwrite(dispatchId.xy);

	}

}

void RTTCorrection(float32_t2 index) {


	float32_t4 output = sourceImage0[index];

	if (output.r == gComputeConstants.clearColor.r &&
		output.g == gComputeConstants.clearColor.g && 
		output.b == gComputeConstants.clearColor.b &&
		output.a == gComputeConstants.clearColor.a) {
		output = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainRTTCorrection(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		RTTCorrection(dispatchId.xy);

	}

}

void MotionBlur(float32_t2 index) {

	// 入力色
	float32_t4 input = { 0.0f,0.0f,0.0f,0.0f };

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };

	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	// 速度
	float32_t x = gVelocityConstants.values.x;
	float32_t y = gVelocityConstants.values.y;

	//y = ax;
	// x != 0 
	float32_t coefficient = 0.0f;

	float32_t2 dir = float32_t2(0.0f, 0.0f);
	if (x != 0) {
		coefficient = y / x;
		dir.x = x;
		dir.y = dir.x * coefficient * -1.0f;
	}
	else if (y != 0.0f) {
		dir.x = 0.0f;
		dir.y = y * -1.0f;
	}
	else {
		destinationImage0[index] = sourceImage0[index];
		return;
	}

	for (int32_t i = 0; i < gComputeConstants.kernelSize / 2; i++) {

		// インデックス
		indexTmp = index;

		indexTmp.x += float32_t(i) * dir.x;
		indexTmp.y += float32_t(i) * dir.y;
		if ((indexTmp.x < 0.0f) || (indexTmp.y < 0.0f)) {
			continue;
		}

		input = sourceImage0[indexTmp];

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// 色確認
		if ((input.a != 0.0f) ||
			(i == 0)) {
			// outputに加算
			output += input * weight;
			// 重みの合計に加算
			weightSum += weight;
		}
	}


	// 重みの合計分割る
	output *= (1.0f / weightSum);

	// 代入
	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainMotionBlur(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		MotionBlur(dispatchId.xy);

	}

}

void WhiteNoise(float32_t2 index) {

	float32_t4 output = sourceImage0[index];

	float32_t2 texcoord = float32_t2(
		index.x / gComputeConstants.threadIdTotalX,
		index.y / gComputeConstants.threadIdTotalY);

	float32_t noise = Noise(texcoord * gComputeConstants.time) - 0.5f;

	output.rgb += noise;

	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainWhiteNoise(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		WhiteNoise(dispatchId.xy);

	}

}

void ScanLine(float32_t2 index) {

	float32_t4 output = sourceImage0[index];

	float32_t2 texcoord = float32_t2(
		index.x / gComputeConstants.threadIdTotalX,
		index.y / gComputeConstants.threadIdTotalY);

	float32_t sinv = sin(texcoord.y * 2.0f + gComputeConstants.time * -0.1f);
	float32_t steped = step(0.99f, sinv * sinv);

	output.rgb -= (1.0f - steped) * abs(sin(texcoord.y * 50.0f + gComputeConstants.time)) * 0.05f;

	output.rgb -= (1.0f - steped) * abs(sin(texcoord.y * 100.0f - gComputeConstants.time * 2.0f)) * 0.08f;

	output.rgb += steped * 0.1f;

	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainScanLine(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		ScanLine(dispatchId.xy);

	}

}

void RGBShift(float32_t2 index) {


	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	output.r = sourceImage0[index + gComputeConstants.rShift].r;
	output.g = sourceImage0[index + gComputeConstants.gShift].g;
	output.b = sourceImage0[index + gComputeConstants.bShift].b;
	output.a = sourceImage0[index].a;

	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainRGBShift(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		RGBShift(dispatchId.xy);

	}

}

void BarrelCurved(float32_t2 index) {

	float32_t2 texcoord = float32_t2(
		index.x / gComputeConstants.threadIdTotalX,
		index.y / gComputeConstants.threadIdTotalY);

	texcoord -= float32_t2(0.5f, 0.5f);
	float32_t distPower = pow(length(texcoord), gComputeConstants.distortion);
	texcoord *= float32_t2(distPower, distPower);
	texcoord += float32_t2(0.5f, 0.5f);

	float32_t2 newIndex = float32_t2(
		texcoord.x * gComputeConstants.threadIdTotalX,
		texcoord.y * gComputeConstants.threadIdTotalY);

	destinationImage0[index] = sourceImage0[newIndex];
}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBarrelCurved(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		BarrelCurved(dispatchId.xy);

	}

}

void Vignette(float32_t2 index) {

	float32_t2 texcoord = float32_t2(
		index.x / gComputeConstants.threadIdTotalX,
		index.y / gComputeConstants.threadIdTotalY);

	float32_t vignette = length(float32_t2(0.5f, 0.5f) - texcoord);

	vignette = clamp(vignette - gComputeConstants.vignetteSize, 0.0f, 1.0f);

	destinationImage0[index] = sourceImage0[index];
	destinationImage0[index].rgb -= vignette;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainVignette(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Vignette(dispatchId.xy);

	}

}

void Glitch(float32_t2 index) {

	float32_t2 texcoord = float32_t2(
		index.x / gComputeConstants.threadIdTotalX,
		index.y / gComputeConstants.threadIdTotalY);

	float32_t horzNoise = Noise(
		float32_t2(
			floor((texcoord.y) / gComputeConstants.horzGlitchPase) * gComputeConstants.horzGlitchPase,
			gComputeConstants.time * 0.2f));

	float32_t vertNoise = Noise(
		float32_t2(
			floor((texcoord.x) / gComputeConstants.vertGlitchPase) * gComputeConstants.vertGlitchPase,
			gComputeConstants.time * 0.1f));

	float32_t horzGlitchStrength = horzNoise / gComputeConstants.glitchStepValue;

	float32_t vertGlitchStrength = vertNoise / gComputeConstants.glitchStepValue;

	horzGlitchStrength = vertGlitchStrength * 2.0f - 1.0f;
	vertGlitchStrength = horzGlitchStrength * 2.0f - 1.0f;

	float32_t horz = step(horzNoise, gComputeConstants.glitchStepValue) * horzGlitchStrength;
	float32_t vert = step(vertNoise, gComputeConstants.glitchStepValue * 2.0f) * vertGlitchStrength;

	float32_t sinv = sin(texcoord.y * 2.0f - gComputeConstants.time * -0.1f);
	float32_t steped = 1.0f - step(0.99f, sinv * sinv);
	float32_t timeFrac = steped * step(0.8f, frac(gComputeConstants.time));

	float32_t2 newIndex = index + timeFrac * (horz + vert);

	destinationImage0[index] = sourceImage0[newIndex];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGlitch(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Glitch(dispatchId.xy);

	}

}

void RadialBlur(float32_t2 index) {

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// uv
	float32_t2 texcoord = float32_t2(
		index.x / gComputeConstants.threadIdTotalX,
		index.y / gComputeConstants.threadIdTotalY);

	// 中心を基準にした位置
	float32_t2 position = texcoord - gComputeConstants.radialBlurCenter;

	// 中心からの距離
	float32_t distance = length(position);
	float32_t factor = gComputeConstants.radialBlurStrength / float32_t(gComputeConstants.radialBlurSamples) * distance;
	
	// ブラーが適用されない範囲を計算, 0.1の範囲をぼかす
	factor *= smoothstep(gComputeConstants.radialBlurMask - 0.1f, gComputeConstants.radialBlurMask, distance);
	
	// 新しいインデックス
	float32_t2 newIndex = float32_t2(0.0f, 0.0f);
	// サンプル分回す
	for (int32_t i = 0; i < gComputeConstants.radialBlurSamples; ++i) {
		float32_t uvOffset = 1.0f - factor * float32_t(i);
		newIndex = position * uvOffset + gComputeConstants.radialBlurCenter;
		newIndex.x *= gComputeConstants.threadIdTotalX;
		newIndex.y *= gComputeConstants.threadIdTotalY;
		output += sourceImage0[newIndex];
	}

	// 平均を求める
	output /= float32_t(gComputeConstants.radialBlurSamples);
	// 出力
	destinationImage0[index] = output;

}


[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainRadialBlur(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		RadialBlur(dispatchId.xy);

	}

}
