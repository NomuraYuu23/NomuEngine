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

	float32_t4 flareColor; // フレアの色
	float32_t2 flareSize; // フレアの大きさ
	float32_t2 flarePosition; // フレアの位置

	float32_t4 paraColor; // パラの色
	float32_t2 paraSize; // パラの大きさ
	float32_t2 paraPosition; // パラの位置

};

// 定数データ
ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);

// 速度データ
struct VelocityParameters {
	float32_t2 values; // 値
};

// 速度データ
ConstantBuffer<VelocityParameters> gVelocityConstants: register(b1);

// 衝撃波データ
struct ShockWaveParameters {

	float32_t2 center; // 中心
	float32_t distortion; // 歪み
	float32_t radius; // 半径
	float32_t thickness; // 厚み

};

// 衝撃波データ
ConstantBuffer<ShockWaveParameters> gShockWaveConstants: register(b2);

// ソース0
Texture2D<float32_t4> sourceImage0 : register(t0);
// ソース1
Texture2D<float32_t4> sourceImage1 : register(t1);
// 行先
RWTexture2D<float32_t4> destinationImage0 : register(u0);

// コピー
[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainCopy(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {
		
		destinationImage0[dispatchId.xy] = sourceImage0[dispatchId.xy];

	}

}

// クリア(グリーン)
[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainClear(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = gComputeConstants.clearColor;

	}

}

// 明度の高いところを白で書き込む、それ以外は黒を書き込む
float32_t4 BinaryThreshold(float32_t4 input) {

	float32_t3 col = float32_t3(0.0, 0.0, 0.0);

	if ((input.r + input.g + input.b) / 3.0 > gComputeConstants.threshold) {
		col = float32_t3(1.0, 1.0, 1.0);
	}

	return float32_t4(col, 1.0f);

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBinaryThreshold(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = BinaryThreshold(input);

	}

}

// ガウシアンブラー
float32_t4 GaussianBlur(float32_t2 index, float32_t2 dir) {

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
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = GaussianBlur(dispatchId.xy, float32_t2(1.0f, 0.0f));

	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = GaussianBlur(dispatchId.xy, float32_t2(0.0f, 1.0f));

	}

}

// ブルーム
float32_t4 Bloom(float32_t2 index, float32_t2 dir) {

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
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = Bloom(dispatchId.xy, float32_t2(1.0f, 0.0f));

	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = Bloom(dispatchId.xy, float32_t2(0.0f, 1.0f));

	}

}

// 明度の高いところをその色で書き込む、それ以外は透明を書き込む
float32_t4 BrightnessThreshold(float32_t4 input) {

	float32_t4 col = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

	if ((input.r + input.g + input.b) / 3.0f > gComputeConstants.threshold) {
		col = input;
	}

	return col;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBrightnessThreshold(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = BrightnessThreshold(input);

	}

}

// ブラー画像との合成
float32_t4 BlurAdd(float32_t4 input0, float32_t4 input1) {

	float32_t alphaSum = input0.a + input1.a;

	if(alphaSum != 0.0f) {
		float32_t a1 = input0.a / alphaSum;
		float32_t a2 = input1.a / alphaSum;

		float32_t3 col = input0.rgb * a1 + input1.rgb * a2;
		
		return float32_t4(col, min(alphaSum, 1.0f));
	}

	return float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBlurAdd(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input0 = sourceImage0[dispatchId.xy];
		float32_t4 input1 = sourceImage1[dispatchId.xy];
		destinationImage0[dispatchId.xy] = BlurAdd(input0, input1);

	}

}

// 画像の上に画像を書き込む
float32_t4 Overwrite(float32_t4 input0, float32_t4 input1) {

	if (input1.a == 1.0f) {
		return input1;
	}
	else if (input1.a == 0.0f) {
		return input0;
	}

	float32_t alphaOut = 1.0f - input1.a;

	alphaOut = min(alphaOut, input0.a);

	if (input0.a != 0.0f) {
		float32_t a = min(alphaOut / input0.a, 1.0f);

		float32_t4 color =
			float32_t4(
				input0.r * a,
				input0.g * a,
				input0.b * a,
				alphaOut);

		return input1 + color;

	}

	return input1;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainOverwrite(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input0 = sourceImage0[dispatchId.xy];
		float32_t4 input1 = sourceImage1[dispatchId.xy];
		destinationImage0[dispatchId.xy] = Overwrite(input0, input1);

	}

}

// レンダーターゲット画像の書き込まれていない部分を透明に
float32_t4 RTTCorrection(float32_t4 input) {

	float32_t4 clear = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

	if (input.r == gComputeConstants.clearColor.r &&
		input.g == gComputeConstants.clearColor.g &&
		input.b == gComputeConstants.clearColor.b &&
		input.a == gComputeConstants.clearColor.a) {
		return clear;
	}

	return input;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainRTTCorrection(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = RTTCorrection(input);

	}

}

// モーションブラー
float32_t4 MotionBlur(float32_t2 index) {

	// ブラーかからない
	if (gVelocityConstants.values.x == 0 && 
		gVelocityConstants.values.y == 0) {
		return sourceImage0[index];
	}

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

	for (int32_t i = 0; i < gComputeConstants.kernelSize / 2; i++) {

		// インデックス
		indexTmp = index;

		indexTmp.x += float32_t(i) * gVelocityConstants.values.x;
		indexTmp.y += float32_t(i) * gVelocityConstants.values.y;
		if ((indexTmp.x < 0.0f) || (indexTmp.y < 0.0f)) {
			continue;
		}

		input = sourceImage0[indexTmp];

		if ((input.a == 0.0f) && (i != 0)) {
			continue;
		}

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);
		
		// outputに加算
		output += input * weight;
		// 重みの合計に加算
		weightSum += weight;

	}

	// 重みの合計分割る
	output *= (1.0f / weightSum);

	// 代入
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainMotionBlur(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = MotionBlur(dispatchId.xy);

	}

}

// ホワイトノイズ
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

// 走査線
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

// RGBずらし
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

// 樽状湾曲
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

// ビネット
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

// グリッチ
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

// 放射状ブラー
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

// 衝撃波
void ShockWave(float32_t2 index) {

	// uv
	float32_t2 texcoord = float32_t2(
		index.x / gComputeConstants.threadIdTotalX,
		index.y / gComputeConstants.threadIdTotalY);
	
	// 比率
	float32_t ratio = float32_t(gComputeConstants.threadIdTotalY) / float32_t(gComputeConstants.threadIdTotalX);

	// テクスチャ比率に依存しない真円
	float32_t2 scaleUV = (texcoord - float32_t2(0.5f, 0.0f)) / float32_t2(ratio, 1.0f) + float32_t2(0.5f, 0.0f);

	// 中心を基準にした位置
	float32_t2 position = scaleUV - gShockWaveConstants.center;

	// マスク
	float32_t mask =
		(1.0f - smoothstep(gShockWaveConstants.radius - 0.1f, gShockWaveConstants.radius, length(position))) *
		smoothstep(gShockWaveConstants.radius - gShockWaveConstants.thickness - 0.1f, gShockWaveConstants.radius - gShockWaveConstants.thickness, length(position));

	// 歪み
	float32_t2 distortion = normalize(position) * gShockWaveConstants.distortion * mask;
	
	// 新しいインデックス
	float32_t2 newIndex = texcoord - distortion;
	newIndex.x *= gComputeConstants.threadIdTotalX;
	newIndex.y *= gComputeConstants.threadIdTotalY;
	
	// 出力
	destinationImage0[index] = sourceImage0[newIndex];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainShockWave(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		ShockWave(dispatchId.xy);

	}

}

void FlarePara(float32_t2 index) {

	// アウトプットにソース画像を入れる
	float32_t4 output = sourceImage0[index];

	// フレアの距離
	float32_t2 flareLength = index - gComputeConstants.flarePosition;
	flareLength.x = flareLength.x / gComputeConstants.flareSize.x / gComputeConstants.threadIdTotalY;
	flareLength.y = flareLength.y / gComputeConstants.flareSize.y / gComputeConstants.threadIdTotalY;

	// フレア
	float32_t flare = 1.0f - clamp(length(flareLength), 0.0f, 1.0f);

	// パラの距離
	float32_t2 paraLength = index - gComputeConstants.paraPosition;
	paraLength.x = paraLength.x / gComputeConstants.paraSize.x / gComputeConstants.threadIdTotalY;
	paraLength.y = paraLength.y / gComputeConstants.paraSize.y / gComputeConstants.threadIdTotalY;

	// パラ
	float32_t para = 1.0f - clamp(length(paraLength), 0.0f, 1.0f);
	
	// 出力
	output.rgb *= lerp(float32_t3(1.0f, 1.0f, 1.0f), gComputeConstants.paraColor.rgb, para) * gComputeConstants.paraColor.a;
	output.rgb += lerp(float32_t3(0.0f, 0.0f, 0.0f), gComputeConstants.flareColor.rgb, flare) * gComputeConstants.flareColor.a;
	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainFlarePara(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		FlarePara(dispatchId.xy);

	}

}

void Reduction(float32_t2 index) {

	// 定数
	float32_t ratio = 8.0f;
	
	// 新しいインデックス
	float32_t2 newIndex = index / ratio;

	destinationImage0[newIndex] = sourceImage0[index];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainReduction(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Reduction(dispatchId.xy);

	}

}

void Expansion(float32_t2 index) {

	// 定数
	float32_t ratio = 8.0f;

	// 新しいインデックス
	float32_t2 newIndex = index / ratio;

	destinationImage0[index] = sourceImage0[newIndex];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainExpansion(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Expansion(dispatchId.xy);

	}

}