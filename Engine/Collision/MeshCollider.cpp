#include "MeshCollider.h"
#include "Collision.h"

MeshCollider::MeshCollider(Object3D* object)
{
	// 形状タイプを球に設定
	shapeType_ = SHAPE_MESH;

	// オブジェクトを設定
	object_ = object;

	// 三角形の配列を構成する
	ConstructTriangles(object_->GetModel());
}

void MeshCollider::Update()
{
	// 衝突フラグを初期化
	isHit_ = false;

	// 衝突したときの情報を初期化
	inter_ = { 0.0f, 0.0f, 0.0f };
	reject_ = { 0.0f, 0.0f, 0.0f };
}

bool MeshCollider::CheckCollisionSphere(const Sphere& sphere, Vector3* inter, Vector3* reject)
{
	// オブジェクトのローカル座標系での球を得る(半径はXスケールを参照)
	Sphere localSphere;
	localSphere.center = sphere.center - object_->GetPosition();
	localSphere.radius = sphere.radius;

	// ローカル座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();

	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		// 球と三角形の当たり判定
		if (Collision::CheckSphere2Triangle(localSphere, triangle, inter, reject)) {
			if (inter) {
				const Matrix4& matWorld = object_->GetMatWorld();

				// ワールド座標系での交点を得る
				*inter = Vector3Transform(*inter, matWorld);
			}

			return true;
		}
	}

	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray& ray, float* distance, Vector3* inter)
{
	// オブジェクトのローカル座標系でのレイを得る
	Ray localRay;
	localRay.start = ray.start - object_->GetPosition();
	localRay.dir = ray.dir;

	// ローカル座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangles_.cbegin();

	for (; it != triangles_.cend(); ++it) {
		const Triangle& triangle = *it;

		Vector3 tempInter;

		// レイと三角形の当たり判定
		if (Collision::CheckRay2Triangle(localRay, triangle, nullptr, &tempInter)) {
			//const Matrix4& matWorld = object_->GetMatWorld();

			// ワールド座標系での交点を得る
			//tempInter = Vector3Transform(tempInter, matWorld);

			if (distance) {
				// 交点とレイ始点の距離を計算
				Vector3 sub = tempInter - localRay.start;
				*distance = Vector3Dot(sub, localRay.dir);
			}

			if (inter) {
				*inter = tempInter;
			}

			return true;
		}
	}

	return false;
}

void MeshCollider::ConstructTriangles(Model* model)
{
	// 三角形リストをクリア
	triangles_.clear();

	// モデルの持つメッシュリストを取得
	const std::vector<Mesh>& meshes = model->GetMesh();

	// オブジェクトに設定されているスケールを設定
	Vector3 scale = object_->GetScale();

	// 現在のメッシュの開始三角形番号を入れておく変数(0で初期化)
	uint16_t start = 0;

	// 全メッシュについて順に処理する
	std::vector<Mesh>::const_iterator it = meshes.cbegin();

	for (; it != meshes.cend(); ++it) {
		// 頂点データ
		const std::vector<Mesh::Vertex>& vertices = it._Ptr->GetVertex();

		// インデックス
		const std::vector<uint16_t>& indexes = it._Ptr->GetIndex();

		// インデックスは、三角形の数×3個あるので、そこからメッシュ内の三角形の数を逆算する
		size_t triangleNum = indexes.size() / 3;

		// 現在のメッシュの三角形の数だけ、三角形リストにスペースを追加する
		triangles_.resize(triangles_.size() + triangleNum);

		// 全三角形について順に処理する
		for (size_t i = 0; i < triangleNum; i++) {
			// 今から計算する三角形の参照
			Triangle& tri = triangles_[start + i];
			uint16_t idx0 = indexes[i * 3 + 0];
			uint16_t idx1 = indexes[i * 3 + 1];
			uint16_t idx2 = indexes[i * 3 + 2];

			// 三角形の3頂点の座標を代入
			tri.p0 = { vertices[idx0].pos.x * scale.x, vertices[idx0].pos.y * scale.y, vertices[idx0].pos.z * scale.z };
			tri.p1 = { vertices[idx1].pos.x * scale.x, vertices[idx1].pos.y * scale.y, vertices[idx1].pos.z * scale.z };
			tri.p2 = { vertices[idx2].pos.x * scale.x, vertices[idx2].pos.y * scale.y, vertices[idx2].pos.z * scale.z };

			// 3頂点からの法線を計算
			tri.ComputeNormal();
		}

		// 次のメッシュは、今までの三角形番号の次から使う
		start += static_cast<uint16_t>(triangleNum);
	}
}
