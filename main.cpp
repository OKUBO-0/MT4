#include <Novice.h>
#include <cmath>
#include <array>

const char kWindowTitle[] = "LE2C_04_オオクボ_タク";

// クォータニオン構造体
struct Quaternion {
    float x, y, z, w;
};

// 軸と角度から回転クォータニオンを作成
Quaternion MakeRotateAxisAngleQuaternion(const std::array<float, 3>& axis, float angle) {
    float halfAngle = angle * 0.5f;
    float sinHalfAngle = std::sin(halfAngle);

    return {
        axis[0] * sinHalfAngle,
        axis[1] * sinHalfAngle,
        axis[2] * sinHalfAngle,
        std::cos(halfAngle)
    };
}

// 球面線形補間（Slerp）
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
    // q0とq1の内積を計算
    float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;

    Quaternion q1Adjusted = q1;

    // 内積が負の場合、q1を反転して近い回転を選択
    if (dot < 0.0f) {
        q1Adjusted.x = -q1.x;
        q1Adjusted.y = -q1.y;
        q1Adjusted.z = -q1.z;
        q1Adjusted.w = -q1.w;
        dot = -dot;
    }

    // なす角を計算
    float theta = std::acos(dot);

    // 補間係数を計算
    float sinTheta = std::sin(theta);
    float scale0 = std::sin((1.0f - t) * theta) / sinTheta;
    float scale1 = std::sin(t * theta) / sinTheta;

    // クォータニオンを補間
    return {
        scale0 * q0.x + scale1 * q1Adjusted.x,
        scale0 * q0.y + scale1 * q1Adjusted.y,
        scale0 * q0.z + scale1 * q1Adjusted.z,
        scale0 * q0.w + scale1 * q1Adjusted.w
    };
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

    // 回転クォータニオンを作成
    Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
    Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.0f, 0.71f }, 3.141592f);

    // 補間結果を計算
    Quaternion interpolates[5];
    float tValues[5] = { 0.0f, 0.3f, 0.5f, 0.7f, 1.0f };
    for (int i = 0; i < 5; i++) {
        interpolates[i] = Slerp(rotation0, rotation1, tValues[i]);
    }

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
        
        /// 描画処理
        for (int i = 0; i < 5; i++) {
            Novice::ScreenPrintf(
                50, 50 + i * 20,
                "%.2f %.2f %.2f %.2f : interpolate%d, Slerp(q0, q1, %.1ff)",
                interpolates[i].x,
                interpolates[i].y,
                interpolates[i].z,
                interpolates[i].w,
                i,
                tValues[i]
            );
        }

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

//球面線形補間を実装せよ
//
////球面辣形補間
//Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
//擬似コード
//float dot = ...; //q0とq1の内積
//if (dot < 0) {
//	q0 = -q0;//もう片方の回転を利用する
//	dot = -dot;//内積も反転
//}
////なす角を求める
//float theta = std::acos(dot);
//
//// ...thetaとsinを使って補間係数scale0,scale1を求める
//
////それぞれの補間係数を利用して補聞後のQuaternionを求める
//return scale0 * q0 + scale1 * q1;
//
//注意点
//q0およびq1は単位Quaternionであり、
//数学的には結果も必ず単位Quaternion
//である。
//が、数値計算による誤差があるので厳
//密に単位Quaternionとはならない。
//何度も補間を繰り返す場合などは、単
//位Quaternionとは呼べないほどに誤差
//が蓄積してしまうことがあるので定期
//的に正規化を行うこと。
//Slerpの中で行うのはコストが無駄に高
//いので避けること
//
//
//実行結果例
//
//Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
//Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.0f, 0.71f }, 3.141592f);
//
//Quaternion interpolate0 = Slerp(rotatione, rotation1, 0.0f);
//Quaternion interpolate1 = Slerp(rotatione, rotation1, 0.3f);
//Quaternion interpolate2 = Slerp(rotatione, rotation1, 0.5f);
//Quaternion interpolate3 = Slerp(rotatione, rotation1, 0.7f);
//Quaternion interpolate4 = Slerp(rotatione, rotation1, 1.0f);
//
//上記プログラムを実行して、結果の値を表示したものが↓となる
//0.11 0.40 0.00 0.99 : interpolate0, Slerp(q0, q1, 0.0f)
//0.40 0.09 0.31 0.86 : interpolate1, Slerp(q0, q1. 0.3f)
//0.56 0.07 0.48 0.67 : interpolate2, Slerp(q0, q1, 0.5f)
//0.66 0.05 0.62 0.43 : interpolate3, Slerp(q0, q1. 0.7f)
//0.71 0.00 0.71 0.00 : interpolate4, Slerp(q0, q1, 1.0f)