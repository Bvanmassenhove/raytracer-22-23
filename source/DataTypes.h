#pragma once
#include <cassert>

#include "Math.h"
#include "vector"

namespace dae
{
#pragma region GEOMETRY
	struct Sphere
	{
		Vector3 origin{};
		float radius{};

		unsigned char materialIndex{ 0 };
	};

	struct Plane
	{
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };
	};

	enum class TriangleCullMode
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle
	{
		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal):
			v0{_v0}, v1{_v1}, v2{_v2}, normal{_normal.Normalized()}{}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
		}

		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};

		TriangleCullMode cullMode{};
		unsigned char materialIndex{};
	};

	struct TriangleMesh
	{
		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode):
		positions(_positions), indices(_indices), cullMode(_cullMode)
		{
			//Calculate Normals
			CalculateNormals();

			//Update Transforms
			UpdateTransforms();
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			UpdateTransforms();
		}

		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<int> indices{};
		unsigned char materialIndex{};

		TriangleCullMode cullMode{TriangleCullMode::BackFaceCulling};

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		Vector3 minAABB;
		Vector3 maxAABB;

		Vector3 transformedminAABB;
		Vector3 transformedMaxAABB;

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex = static_cast<int>(positions.size());

			positions.push_back(triangle.v0);
			positions.push_back(triangle.v1);
			positions.push_back(triangle.v2);

			indices.push_back(startIndex);
			indices.push_back(++startIndex);
			indices.push_back(++startIndex);

			normals.push_back(triangle.normal);

			//Not ideal, but making sure all vertices are updated
			if(!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CalculateNormals()
		{
			for (int i = 0; i < indices.size(); i+=3)
			{
				const Vector3 edgeV0V1 = positions[indices[i+1]] - positions[indices[i]];
				const Vector3 edgeV0V2 = positions[indices[i+2]] - positions[indices[i]];
				normals.push_back(Vector3::Cross(edgeV0V1, edgeV0V2).Normalized());
			}		
		}

		void UpdateTransforms()
		{
			//update AABB
			//UpdateTransformedAABB(finalTransform);
			transformedPositions = positions;
			transformedNormals = normals;


			/*Vector3 result{};
			for (int i = 0; i < positions.size(); ++i)
			{
				for (int i = 0; i < 4; ++i)
				{
					result[i] = positions[i][0] * translationTransform[0][i] + positions[i][1] * translationTransform[1][i] + positions[i][2] + translationTransform[2][i] + positions[i][3] * translationTransform[3][i];
				}
					
				result[0] = result[0] / result[3];
				result[1] = result[1] / result[3];
				result[2] = result[2] / result[3];
				transformedPositions.push_back(result);
				for (int i = 0; i < 4; ++i)
				{
					transformedPositions[i][i] = positions[i][0] * rotationTransform[0][i] + positions[i][1] * rotationTransform[1][i] + positions[i][2] + rotationTransform[2][i] + positions[i][3] * rotationTransform[3][i];
				}

				transformedPositions[i][0] = transformedPositions[i][0] / transformedPositions[i][3];
				transformedPositions[i][1] = transformedPositions[i][1] / transformedPositions[i][3];
				transformedPositions[i][2] = transformedPositions[i][2] / transformedPositions[i][3];
			}
			
			for (int i = 0; i < normals.size(); ++i)
			{
				for (int i = 0; i < 4; ++i)
				{
					result[i] = normals[i][0] * translationTransform[0][i] + normals[i][1] * translationTransform[1][i] + normals[i][2] + translationTransform[2][i] + normals[i][3] * translationTransform[3][i];
				}

				result[0] = result[0] / result[3];
				result[1] = result[1] / result[3];
				result[2] = result[2] / result[3];
				transformedNormals.push_back(result);
				for (int i = 0; i < 4; ++i)
				{
					transformedPositions[i][i] = normals[i][0] * rotationTransform[0][i] + normals[i][1] * rotationTransform[1][i] + normals[i][2] + rotationTransform[2][i] + normals[i][3] * rotationTransform[3][i];
				}

				transformedNormals[i][0] = transformedNormals[i][0] / transformedNormals[i][3];
				transformedNormals[i][1] = transformedNormals[i][1] / transformedNormals[i][3];
				transformedNormals[i][2] = transformedNormals[i][2] / transformedNormals[i][3];
			}
		*/

			//Calculate Final Transform 
			//const auto finalTransform = ...

			//Transform Positions (positions > transformedPositions)
			//...

			//Transform Normals (normals > transformedNormals)
			//...
		}

		void UpdateAABB()
		{
			if (positions.size() > 0)
			{
				minAABB = positions[0];
				maxAABB = positions[0];
				for (auto& p : positions)
				{
					minAABB = Vector3::Min(p, minAABB);
					maxAABB = Vector3::Max(p, maxAABB);
				}
			}
		}
		void UpdateTransformedAABB(const Matrix& finalTransform)
		{
			// AABB update: be careful -> transform the 8 vectices of the aabb
			// and calculate new min and max
			Vector3 tMinAABB = finalTransform.TransformPoint(minAABB);
			Vector3 tMaxAABB = tMinAABB;
			// (xmax,ymin,zmin)
			Vector3 tAABB = finalTransform.TransformPoint(maxAABB.x, minAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMinAABB);
			// (xmax,ymin,zmax)
			tAABB = finalTransform.TransformPoint(maxAABB.x, minAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMinAABB);
			// (xmin,ymin,zmax)
			tAABB = finalTransform.TransformPoint(minAABB.x, minAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMinAABB);
			// (xmin,ymax,zmin)
			tAABB = finalTransform.TransformPoint(minAABB.x, maxAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMinAABB);
			// (xmax,ymax,zmin)
			tAABB = finalTransform.TransformPoint(maxAABB.x, maxAABB.y, minAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMinAABB);
			// (xmax,ymax,zmax)
			tAABB = finalTransform.TransformPoint(maxAABB);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMinAABB);
			// (xmin,ymax,zmax)
			tAABB = finalTransform.TransformPoint(minAABB.x, maxAABB.y, maxAABB.z);
			tMinAABB = Vector3::Min(tAABB, tMinAABB);
			tMaxAABB = Vector3::Max(tAABB, tMinAABB);

			transformedminAABB = tMinAABB;
			transformedMaxAABB = tMaxAABB;
		}
	};
#pragma endregion
#pragma region LIGHT
	enum class LightType
	{
		Point,
		Directional
	};

	struct Light
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};
#pragma endregion
#pragma region MISC
	struct Ray
	{
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.001f };
		float max{ FLT_MAX };
	};

	struct HitRecord
	{
		Vector3 origin{};
		Vector3 normal{};
		float t = FLT_MAX;

		bool didHit{ false };
		unsigned char materialIndex{ 0 };
	};
#pragma endregion
}