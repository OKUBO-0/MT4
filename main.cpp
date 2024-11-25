#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "LE2C_04_オオクボ_タク";

struct Vector3 {
	float x, y, z;
};

struct Quaternion {
	float x, y, z, w;
};

struct Matrix4x4 {
	float m[4][4];
};

// ベクトルの正規化
Vector3 Normalize(const Vector3& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length != 0.0f) {
		return { v.x / length, v.y / length, v.z / length };
	}
	return { 0.0f, 0.0f, 0.0f };
}

// ベクトルを行列で変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	return {
		vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0],
		vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1],
		vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2]
	};
}

// 回転軸と角度からクォータニオンを作成
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Vector3 normalizedAxis = Normalize(axis);
	float halfAngle = angle / 2.0f;
	float sinHalfAngle = std::sin(halfAngle);
	float cosHalfAngle = std::cos(halfAngle);

	return {
		normalizedAxis.x * sinHalfAngle,  // x
		normalizedAxis.y * sinHalfAngle,  // y
		normalizedAxis.z * sinHalfAngle,  // z
		cosHalfAngle                      // w
	};
}

// ベクトルをクォータニオンで回転
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	// クォータニオンの共役
	Quaternion conjugate = { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };

	// ベクトルをクォータニオンに拡張
	Quaternion vectorQuat = { vector.x, vector.y, vector.z, 0.0f };

	// q * v
	Quaternion intermediate = {
		quaternion.w * vectorQuat.x + quaternion.x * vectorQuat.w + quaternion.y * vectorQuat.z - quaternion.z * vectorQuat.y,
		quaternion.w * vectorQuat.y - quaternion.x * vectorQuat.z + quaternion.y * vectorQuat.w + quaternion.z * vectorQuat.x,
		quaternion.w * vectorQuat.z + quaternion.x * vectorQuat.y - quaternion.y * vectorQuat.x + quaternion.z * vectorQuat.w,
		quaternion.w * vectorQuat.w - quaternion.x * vectorQuat.x - quaternion.y * vectorQuat.y - quaternion.z * vectorQuat.z
	};

	// (q * v) * q^*
	Quaternion rotated = {
		intermediate.w * conjugate.x + intermediate.x * conjugate.w + intermediate.y * conjugate.z - intermediate.z * conjugate.y,
		intermediate.w * conjugate.y - intermediate.x * conjugate.z + intermediate.y * conjugate.w + intermediate.z * conjugate.x,
		intermediate.w * conjugate.z + intermediate.x * conjugate.y - intermediate.y * conjugate.x + intermediate.z * conjugate.w,
		intermediate.w * conjugate.w - intermediate.x * conjugate.x - intermediate.y * conjugate.y - intermediate.z * conjugate.z
	};

	// 回転後のベクトル部分を返す
	return { rotated.x, rotated.y, rotated.z };
}

// クォータニオンから回転行列を作成
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;
	float xy = quaternion.x * quaternion.y;
	float xz = quaternion.x * quaternion.z;
	float yz = quaternion.y * quaternion.z;
	float wx = quaternion.w * quaternion.x;
	float wy = quaternion.w * quaternion.y;
	float wz = quaternion.w * quaternion.z;

	return {
		{
			{ 1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz),       2.0f * (xz - wy),       0.0f },
			{ 2.0f * (xy - wz),       1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx),       0.0f },
			{ 2.0f * (xz + wy),       2.0f * (yz - wx),       1.0f - 2.0f * (xx + yy), 0.0f },
			{ 0.0f,                   0.0f,                   0.0f,                   1.0f }
		}
	};
}

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(
				x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.03f",
				matrix.m[row][column]);
		}
	}
}

void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

void QuaternionScreenPrintf(int x, int y, Quaternion quaternion, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", quaternion.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", quaternion.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", quaternion.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%.02f", quaternion.w);
	Novice::ScreenPrintf(x + kColumnWidth * 4, y, label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 v1{ 1.0f, 3.0f, -5.0f };
	Vector3 v2{ 4.0f, -1.0f, 2.0f };
	//float k = { 4.0f };

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

		Quaternion rotation = MakeRotateAxisAngleQuaternion(
		Normalize(Vector3{ 1.0f, 0.4f, -0.2f }), 0.45f);
		Vector3 pointY = { 2.1f, -0.9f, 1.3f };
		Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
		Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
		Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);

		QuaternionScreenPrintf(0, kRowHeight * 0, rotation, "   : rotation");
		MatrixScreenPrintf(0, kRowHeight * 1, rotateMatrix, "rotateMatrix");
		VectorScreenPrintf(0, kRowHeight * 6, rotateByQuaternion, "   : rotateByQuaternion");
		VectorScreenPrintf(0, kRowHeight * 7, rotateByMatrix, "   : rotateByMatrix");

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