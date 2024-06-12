#include "PostEffect.hlsli"

#include "LuminanceBasedOutline.CS.hlsl"

#include "PostEffectCalc.CS.hlsl"

// 定数データ
struct ComputeParameters {

	uint32_t threadIdOffsetX; // スレッドのオフセットX
	uint32_t threadIdTotalX; // スレッドの総数X
	uint32_t threadIdOffsetY; // スレッドのオフセットY
	uint32_t threadIdTotalY; // スレッドの総数Y

	uint32_t threadIdOffsetZ; // スレッドのオフセットZ
	uint32_t threadIdTotalZ; // スレッドの総数Z
	float32_t threshold; // しきい値
	float32_t time; // 時間

	float32_t4 clearColor; // クリアするときの色
	
	int32_t kernelSize; // カーネルサイズ
	float32_t sigma; // 標準偏差
	float32_t2 rShift; // Rずらし
	
	float32_t2 gShift; // Gずらし
	float32_t2 bShift; // Bずらし
	
	float32_t distortion; // 歪み
	float32_t vignetteSize; // ビネットの大きさ
	float32_t vignetteChange; // ビネットの変化
	float32_t horzGlitchPase; //水平

	float32_t vertGlitchPase; //垂直
	float32_t glitchStepValue; // グリッチのステップ値
	float32_t2 radialBlurCenter; // 放射状ブラーの中心座標

	int32_t radialBlurSamples; // 放射状ブラーのサンプル回数
	float32_t radialBlurStrength; // 放射状ブラーの広がる強さ
	float32_t radialBlurMask; // 放射状ブラーが適用されないサイズ
	float32_t colorLerpT; // 色変える系のLerpT
	
	float32_t2 colorSize; // 色変える系の大きさ
	float32_t2 colorPosition; // 色変える系の位置

	float32_t4 flareColor; // フレアの色
	
	float32_t2 flareSize; // フレアの大きさ
	float32_t2 flarePosition; // フレアの位置

	float32_t4 paraColor; // パラの色
	
	float32_t2 paraSize; // パラの大きさ
	float32_t2 paraPosition; // パラの位置

	float32_t4x4 projectionInverse; // プロジェクション逆行列

	uint32_t executionFlag;  // 実行フラグ(複数組み合わせたときのやつ)

};

// 定数データ
ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);

// 速度データ
struct VelocityParameters {
	float32_t2 values; // 値
};

// 速度データ
ConstantBuffer<VelocityParameters> gVelocityConstants0: register(b1);
ConstantBuffer<VelocityParameters> gVelocityConstants1: register(b2);
ConstantBuffer<VelocityParameters> gVelocityConstants2: register(b3);
ConstantBuffer<VelocityParameters> gVelocityConstants3: register(b4);

// 衝撃波データ
struct ShockWaveParameters {

	float32_t2 center; // 中心
	float32_t distortion; // 歪み
	float32_t radius; // 半径
	float32_t thickness; // 厚み

};

// 衝撃波データ
ConstantBuffer<ShockWaveParameters> gShockWaveConstants0: register(b5);
ConstantBuffer<ShockWaveParameters> gShockWaveConstants1: register(b6);
ConstantBuffer<ShockWaveParameters> gShockWaveConstants2: register(b7);
ConstantBuffer<ShockWaveParameters> gShockWaveConstants3: register(b8);

// ソース
Texture2D<float32_t4> sourceImage0 : register(t0);
Texture2D<float32_t4> sourceImage1 : register(t1);
Texture2D<float32_t4> sourceImage2 : register(t2);
Texture2D<float32_t4> sourceImage3 : register(t3);
Texture2D<float32_t4> sourceImage4 : register(t4);
Texture2D<float32_t4> sourceImage5 : register(t5);
Texture2D<float32_t4> sourceImage6 : register(t6);
Texture2D<float32_t4> sourceImage7 : register(t7);

// 深度値
Texture2D<float32_t4> depthTexture : register(t8);

// 行先
RWTexture2D<float32_t4> destinationImage0 : register(u0);
RWTexture2D<float32_t4> destinationImage1 : register(u1);

// 明度の高いところを白で書き込む、それ以外は黒を書き込む
float32_t4 BinaryThreshold(in const float32_t4 input) {

	float32_t3 col = float32_t3(0.0f, 0.0f, 0.0f);

	if ((input.r + input.g + input.b) * rcp(3.0f) > gComputeConstants.threshold) {
		col = float32_t3(1.0f, 1.0f, 1.0f);
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

// ブラー画像との合成
float32_t4 BlurAdd(in const float32_t4 input0, in const float32_t4 input1) {

	float32_t alphaSum = input0.a + input1.a;

	if (alphaSum != 0.0f) {
		float32_t a1 = input0.a * rcp(alphaSum);
		float32_t a2 = input1.a * rcp(alphaSum);

		float32_t3 col = input0.rgb * a1 + input1.rgb * a2;

		return float32_t4(col, min(alphaSum, 1.0f));
	}

	return float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

}

// レンダーターゲット画像の書き込まれていない部分を透明に
float32_t4 RTTCorrection(in const float32_t4 input) {

	float32_t4 clear = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

	if (input.r == gComputeConstants.clearColor.r &&
		input.g == gComputeConstants.clearColor.g &&
		input.b == gComputeConstants.clearColor.b &&
		input.a == gComputeConstants.clearColor.a) {
		return clear;
	}

	return input;

}

// ガウシアンブラー
float32_t4 GaussianBlurHorizontal(in const float32_t2 index) {

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };

	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	for (int32_t i = -gComputeConstants.kernelSize * rcp(2); i < gComputeConstants.kernelSize * rcp(2); i += 2) {

		// インデックス
		indexTmp = index;

		indexTmp.x += (float32_t(i) + 0.5f);

		if ((indexTmp.x < 0.0f) || (indexTmp.x >= float32_t(gComputeConstants.threadIdTotalX))) {
			continue;
		}

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// outputに加算
		output += sourceImage0[indexTmp] * weight;

		// 重みの合計に加算
		weightSum += weight;
	}

	// 重みの合計分割る
	output *= rcp(weightSum);

	// 代入
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage1[dispatchId.xy] = GaussianBlurHorizontal(dispatchId.xy);

	}

}

// ガウシアンブラー
float32_t4 GaussianBlurVertical(in const float32_t2 index) {

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };

	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	for (int32_t i = -gComputeConstants.kernelSize * rcp(2); i < gComputeConstants.kernelSize * rcp(2); i += 2) {

		// インデックス
		indexTmp = index;

		indexTmp.y += (float32_t(i) + 0.5f);

		if ((indexTmp.y < 0.0f) || (indexTmp.y >= float32_t(gComputeConstants.threadIdTotalY))) {
			continue;
		}

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// outputに加算
		output += destinationImage1[indexTmp] * weight;

		// 重みの合計に加算
		weightSum += weight;
	}

	// 重みの合計分割る
	output *= rcp(weightSum);

	// 代入
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = GaussianBlurVertical(dispatchId.xy);

	}

}

// ブルーム
float32_t4 Bloom(in const float32_t2 index, in const  float32_t2 dir) {

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

	for (int32_t i = -gComputeConstants.kernelSize * rcp(2); i < gComputeConstants.kernelSize * rcp(2); i += 2) {

		// インデックス
		indexTmp = index;

		indexTmp.x += (float32_t(i) + 0.5f) * dir.x;
		indexTmp.y += (float32_t(i) + 0.5f) * dir.y;
		
		if (dir.x == 1.0f) {
			input = sourceImage0[indexTmp];
		}
		else {
			input = destinationImage1[indexTmp];
		}

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// 色確認
		if (((input.r + input.g + input.b) * rcp(3.0f) > gComputeConstants.threshold)) {
			// outputに加算
			output += input * weight;
		}
		// 重みの合計に加算
		weightSum += weight;
	}

	// 重みの合計分割る
	output *= rcp(weightSum);

	// 代入
	return output;

}

// ブルーム
float32_t4 BloomHorizontal(in const float32_t2 index) {

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

	for (int32_t i = -gComputeConstants.kernelSize * rcp(2); i < gComputeConstants.kernelSize * rcp(2); i += 2) {

		// インデックス
		indexTmp = index;
		indexTmp.x += float32_t(i) + 0.5f;

		if ((indexTmp.x < 0.0f) || (indexTmp.x >= float32_t(gComputeConstants.threadIdTotalX))) {
			continue;
		}

		input = sourceImage0[indexTmp];

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// 色確認
		if (((input.r + input.g + input.b) * rcp(3.0f) > gComputeConstants.threshold)) {
			// outputに加算
			output += input * weight;
		}
		// 重みの合計に加算
		weightSum += weight;
	}

	// 重みの合計分割る
	output *= rcp(weightSum);

	// 代入
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage1[dispatchId.xy] = BloomHorizontal(dispatchId.xy);

	}

}

// ブルーム
float32_t4 BloomVertical(in const float32_t2 index) {

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

	for (int32_t i = -gComputeConstants.kernelSize * rcp(2); i < gComputeConstants.kernelSize * rcp(2); i += 2) {

		// インデックス
		indexTmp = index;
		indexTmp.y += float32_t(i) + 0.5f;

		if ((indexTmp.y < 0.0f) || (indexTmp.y >= float32_t(gComputeConstants.threadIdTotalY))) {
			continue;
		}

		input = destinationImage1[indexTmp];

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// 色確認
		if (((input.r + input.g + input.b) * rcp(3.0f) > gComputeConstants.threshold)) {
			// outputに加算
			output += input * weight;
		}
		// 重みの合計に加算
		weightSum += weight;
	}

	// 重みの合計分割る
	output *= rcp(weightSum);

	// 代入
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input0 = sourceImage0[dispatchId.xy];
		float32_t4 input1 = BloomVertical(dispatchId.xy);
		destinationImage0[dispatchId.xy] = BlurAdd(input0, input1);
	}

}

// モーションブラー
float32_t4 MotionBlur(in const float32_t2 index) {

	// ブラーかからない
	if (gVelocityConstants0.values.x == 0 &&
		gVelocityConstants0.values.y == 0) {
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

	for (int32_t i = 0; i < gComputeConstants.kernelSize * rcp(2); i++) {

		// インデックス
		indexTmp = index;

		indexTmp.x += float32_t(i) * gVelocityConstants0.values.x;
		indexTmp.y += float32_t(i) * gVelocityConstants0.values.y;
		if ((indexTmp.x < 0.0f) || (indexTmp.y < 0.0f) ||
			(indexTmp.x > float32_t(gComputeConstants.threadIdTotalX)) || (indexTmp.y > float32_t(gComputeConstants.threadIdTotalY))) {
			continue;
		}

		input = sourceImage0[indexTmp];

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);
		
		// outputに加算
		output += input * weight;
		// 重みの合計に加算
		weightSum += weight;

	}

	if (weightSum != 0.0f) {
		// 重みの合計分割る
		output *= rcp(weightSum);
	}

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
float32_t4 WhiteNoise(in const float32_t4 input, in const float32_t2 index) {

	float32_t4 output = input;

	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	float32_t noise = Noise(texcoord * gComputeConstants.time) - 0.5f;

	output.rgb += noise;

	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainWhiteNoise(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = WhiteNoise(input, dispatchId.xy);

	}

}

// 走査線
float32_t4 ScanLine(in const float32_t4 input, in const float32_t2 index) {

	float32_t4 output = input;

	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	float32_t sinv = sin(texcoord.y * 2.0f + gComputeConstants.time * -0.1f);
	float32_t steped = step(0.99f, sinv * sinv);

	output.rgb -= (1.0f - steped) * abs(sin(texcoord.y * 50.0f + gComputeConstants.time)) * 0.05f;

	output.rgb -= (1.0f - steped) * abs(sin(texcoord.y * 100.0f - gComputeConstants.time * 2.0f)) * 0.08f;

	output.rgb += steped * 0.1f;

	 return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainScanLine(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = ScanLine(input, dispatchId.xy);

	}

}

// RGBずらし (RGB)
float32_t3 RGBShift(in const float32_t2 index) {


	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	output.r = sourceImage0[index - gComputeConstants.rShift].r;
	output.g = sourceImage0[index - gComputeConstants.gShift].g;
	output.b = sourceImage0[index - gComputeConstants.bShift].b;

	return output.rgb;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainRGBShift(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t a = sourceImage0[dispatchId.xy].a;
		destinationImage0[dispatchId.xy] = float32_t4(RGBShift(dispatchId.xy), a);

	}

}

// 樽状湾曲
float32_t4 BarrelCurved(in const float32_t2 index) {

	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	texcoord -= float32_t2(0.5f, 0.5f);
	float32_t distPower = pow(length(texcoord), gComputeConstants.distortion);
	texcoord *= float32_t2(distPower, distPower);
	texcoord += float32_t2(0.5f, 0.5f);

	float32_t2 newIndex = float32_t2(
		texcoord.x * gComputeConstants.threadIdTotalX,
		texcoord.y * gComputeConstants.threadIdTotalY);

	return sourceImage0[newIndex];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBarrelCurved(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = BarrelCurved(dispatchId.xy);

	}

}

// ビネット
float32_t4 Vignette(in const float32_t4 input, in const float32_t2 index) {

	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	float32_t2 corrent = texcoord * (1.0f - texcoord.yx);

	float32_t vigntte = corrent.x * corrent.y * gComputeConstants.vignetteSize;

	vigntte = saturate(pow(vigntte, gComputeConstants.vignetteChange));

	float32_t4 output = input;
	output.rgb *= vigntte;

	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainVignette(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = Vignette(input, dispatchId.xy);

	}

}

// グリッチ
float32_t4 Glitch(in const float32_t2 index) {

	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	float32_t horzNoise = Noise(
		float32_t2(
			floor((texcoord.y) * rcp(gComputeConstants.horzGlitchPase)) * gComputeConstants.horzGlitchPase,
			gComputeConstants.time * 0.2f));

	float32_t vertNoise = Noise(
		float32_t2(
			floor((texcoord.x) * rcp(gComputeConstants.vertGlitchPase)) * gComputeConstants.vertGlitchPase,
			gComputeConstants.time * 0.1f));

	float32_t horzGlitchStrength = horzNoise * rcp(gComputeConstants.glitchStepValue);

	float32_t vertGlitchStrength = vertNoise * rcp(gComputeConstants.glitchStepValue);

	horzGlitchStrength = vertGlitchStrength * 2.0f - 1.0f;
	vertGlitchStrength = horzGlitchStrength * 2.0f - 1.0f;

	float32_t horz = step(horzNoise, gComputeConstants.glitchStepValue) * horzGlitchStrength;
	float32_t vert = step(vertNoise, gComputeConstants.glitchStepValue * 2.0f) * vertGlitchStrength;

	float32_t sinv = sin(texcoord.y * 2.0f - gComputeConstants.time * -0.1f);
	float32_t steped = 1.0f - step(0.99f, sinv * sinv);
	float32_t timeFrac = steped * step(0.8f, frac(gComputeConstants.time));

	float32_t2 newIndex = index + timeFrac * (horz + vert);

	return sourceImage0[newIndex];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGlitch(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = Glitch(dispatchId.xy);

	}

}

// 放射状ブラー
float32_t4 RadialBlur(in const float32_t2 index) {

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// uv
	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	// 中心を基準にした位置
	float32_t2 position = texcoord - gComputeConstants.radialBlurCenter;

	// 中心からの距離
	float32_t distance = length(position);
	float32_t factor = gComputeConstants.radialBlurStrength *  rcp(gComputeConstants.radialBlurSamples) * distance;
	
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
	output *= rcp(gComputeConstants.radialBlurSamples);
	// 出力
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainRadialBlur(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = RadialBlur(dispatchId.xy);

	}

}

// 衝撃波
float32_t4 ShockWave(in const float32_t2 index) {

	// uv
	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));
	
	// 比率
	float32_t ratio = float32_t(gComputeConstants.threadIdTotalY) * rcp(gComputeConstants.threadIdTotalX);

	// テクスチャ比率に依存しない真円
	float32_t2 scaleUV = (texcoord - float32_t2(0.5f, 0.0f)) * float32_t2(rcp(ratio), 1.0f) + float32_t2(0.5f, 0.0f);

	// 中心を基準にした位置
	float32_t2 position = scaleUV - gShockWaveConstants0.center;

	// マスク
	float32_t mask =
		(1.0f - smoothstep(gShockWaveConstants0.radius - 0.1f, gShockWaveConstants0.radius, length(position))) *
		smoothstep(gShockWaveConstants0.radius - gShockWaveConstants0.thickness - 0.1f, gShockWaveConstants0.radius - gShockWaveConstants0.thickness, length(position));

	// 歪み
	float32_t2 distortion = normalize(position) * gShockWaveConstants0.distortion * mask;
	
	// 新しいインデックス
	float32_t2 newIndex = texcoord - distortion;
	newIndex.x *= gComputeConstants.threadIdTotalX;
	newIndex.y *= gComputeConstants.threadIdTotalY;
	
	// 出力
	return sourceImage0[newIndex];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainShockWave(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] =  ShockWave(dispatchId.xy);

	}

}

float32_t4 FlarePara(in const float32_t4 input, in const float32_t2 index) {

	// アウトプットにソース画像を入れる
	float32_t4 output = input;

	// uv
	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	// フレアの距離
	float32_t2 flareLength = texcoord - gComputeConstants.flarePosition;
	flareLength.x = flareLength.x * rcp(gComputeConstants.flareSize.x);
	flareLength.y = flareLength.y * rcp(gComputeConstants.flareSize.y);

	// フレア
	float32_t flare = 1.0f - clamp(length(flareLength), 0.0f, 1.0f);

	// パラの距離
	float32_t2 paraLength = texcoord - gComputeConstants.paraPosition;
	paraLength.x = paraLength.x * rcp(gComputeConstants.paraSize.x);
	paraLength.y = paraLength.y * rcp(gComputeConstants.paraSize.y);

	// パラ
	float32_t para = 1.0f - clamp(length(paraLength), 0.0f, 1.0f);
	
	// 出力
	output.rgb *= lerp(float32_t3(1.0f, 1.0f, 1.0f), gComputeConstants.paraColor.rgb, para) * gComputeConstants.paraColor.a;
	output.rgb += lerp(float32_t3(0.0f, 0.0f, 0.0f), gComputeConstants.flareColor.rgb, flare) * gComputeConstants.flareColor.a;
	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainFlarePara(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = FlarePara(input, dispatchId.xy);

	}

}

float32_t4 GrayScale(in const float32_t4 input, in const float32_t2 index) {

	float32_t value = dot(input.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));

	float32_t3 output = lerp(input.rgb, float32_t3(value, value, value), gComputeConstants.colorLerpT);

	// uv
	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	// 距離
	float32_t2 colorLength = texcoord - gComputeConstants.colorPosition;
	colorLength.x = colorLength.x * rcp(gComputeConstants.colorSize.x);
	colorLength.y = colorLength.y * rcp(gComputeConstants.colorSize.y);

	// 補間係数
	float32_t t = 1.0f - clamp(length(colorLength), 0.0f, 1.0f);

	// 出力
	output.rgb = lerp(input.rgb, output, t);

	return float32_t4(output, input.a);

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGrayScale(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {
		
		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = GrayScale(input, dispatchId.xy);

	}

}

float32_t4 Sepia(in const float32_t4 input, in const float32_t2 index) {

	float32_t value = dot(input.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));

	float32_t3 output = lerp(input.rgb, float32_t3(value, value * 74.0f * rcp(107.0f), value * 43.0f * rcp(107.0f)), gComputeConstants.colorLerpT);

	// uv
	float32_t2 texcoord = GetTexcoord(index, float32_t2(gComputeConstants.threadIdTotalX, gComputeConstants.threadIdTotalY));

	// 距離
	float32_t2 colorLength = texcoord - gComputeConstants.colorPosition;
	colorLength.x = colorLength.x * rcp(gComputeConstants.colorSize.x);
	colorLength.y = colorLength.y * rcp(gComputeConstants.colorSize.y);

	// 補間係数
	float32_t t = 1.0f - clamp(length(colorLength), 0.0f, 1.0f);

	// 出力
	output.rgb = lerp(input.rgb, output, t);

	return float32_t4(output, input.a);

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainSepia(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = Sepia(input, dispatchId.xy);

	}

}

float32_t4 Outline(in const float32_t2 index) {

	float32_t2 difference = float32_t2(0.0f, 0.0f);

	for (int32_t x = -1; x < 2; ++x) {
		for (int32_t y = -1; y < 2; ++y) {
			// uv
			float32_t2 indexTmp = index;
			indexTmp += float32_t2(float32_t(x), float32_t(y)) * gComputeConstants.sigma;

			float32_t ndcDepth = depthTexture[indexTmp].r;

			float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gComputeConstants.projectionInverse);

			float32_t viewZ = viewSpace.z * rcp(viewSpace.w);

			difference.x += viewZ * kPrewittHorizontalKernel[x + 1][y + 1];
			difference.y += viewZ * kPrewittVerticalKernel[x + 1][y + 1];

		}
	}

	float32_t weight = length(difference);
	weight = saturate(weight);

	float32_t4 output;
	output.rgb = (1.0f - weight) * sourceImage0[index].rgb;
	output.a = 1.0f;

	return output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainOutline(uint32_t3 dispatchId : SV_DispatchThreadID) {

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		destinationImage0[dispatchId.xy] = Outline(dispatchId.xy);

	}

}
