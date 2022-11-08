#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"
#include <iostream>

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			hitRecord.didHit = false;
			//todo W1
			
			/*Vector3 rayToSphere{ sphere.origin - ray.origin };
			float rayToPerpendicular{ Vector3::Dot(rayToSphere, ray.direction) };

			float originToPerpendicularSquared{ rayToSphere.Magnitude() * 2 - rayToPerpendicular * 2};
			if (sqrt(originToPerpendicularSquared) < sphere.radius)
			{
				float intercectPoint{ sqrt((sphere.radius * 2) - (originToPerpendicularSquared * 2)) };
				float rayToIntersect{ rayToPerpendicular - intercectPoint };
				Vector3 intersectVector{ ray.origin + ray.direction * rayToIntersect };
				if (intersectVector.Magnitude() > ray.min)
				{
					hitRecord.didHit = true;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.normal = intersectVector;
					hitRecord.normal.Normalize();
					hitRecord.origin = intersectVector;
					hitRecord.t = -intersectVector.Magnitude();

					return true;
				}
			}
			return false;		*/

			const float a{ Vector3::Dot(ray.direction, ray.direction) };
			const float b{ Vector3::Dot(2 * ray.direction, ray.origin - sphere.origin) };
			const float c{ Vector3::Dot(ray.origin - sphere.origin,ray.origin - sphere.origin) - sphere.radius * sphere.radius };
			const float dis{ b * b - 4 * (a * c) };


			if (dis < 0)
			{
				return false;
			}
			else
			{
				float t{ (-b - std::sqrt(dis)) / (2 * a) };
				if (t < ray.min)
				{
					t = { (-b + std::sqrt(dis)) / (2 * a) };
				}
				if (t >= ray.min && t <= ray.max)
				{
					hitRecord.didHit = true;
					hitRecord.materialIndex = sphere.materialIndex;
					hitRecord.origin = (ray.origin + t * ray.direction);
					hitRecord.normal = hitRecord.origin - sphere.origin;
					hitRecord.normal.Normalize();
					hitRecord.t = t;
					return true;
				}
				
			}
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			float rayToIntersect = Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal);
			if (rayToIntersect > ray.min && rayToIntersect < ray.max)
			{
				hitRecord.didHit = true;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal = plane.normal;
				hitRecord.origin = ray.origin + rayToIntersect * ray.direction;
				hitRecord.t = rayToIntersect;
				return true;
				
			}
			else
			{
				return false;
			}
		
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			switch (triangle.cullMode)
			{
			case TriangleCullMode::BackFaceCulling:
				if (Vector3::Dot(triangle.normal, ray.direction) > 0)
				{
					return false;
				}
				break;
			case  TriangleCullMode::FrontFaceCulling:
				if (Vector3::Dot(triangle.normal, ray.direction) < 0)
				{
					return false;
				}
				break;
			}
			if (Vector3::Dot(triangle.normal, ray.direction) == 0)
			{
				return false;
			}

			Vector3 center = (triangle.v0 + triangle.v1 + triangle.v2) / 3;
			Vector3 L = center - ray.origin;
			float rayToIntersect = Vector3::Dot(L, triangle.normal) / Vector3::Dot(ray.direction, triangle.normal);
			if (rayToIntersect < ray.min && rayToIntersect > ray.max)
			{
				return false;
			}

			Vector3 P = ray.origin + rayToIntersect * ray.direction;
			Vector3 edgeA = triangle.v1 - triangle.v0;
			Vector3 edgeB = triangle.v2 - triangle.v0;
			Vector3 edgeC = triangle.v2 - triangle.v1;
			Vector3 pointToSideA = P - triangle.v0;
			Vector3 pointToSideB = P - triangle.v1;

			if (Vector3::Dot(triangle.normal,Vector3::Cross(edgeA,pointToSideA)) < 0)
			{
				return false;
			}
			if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeB, pointToSideA)) > 0)
			{
				return false;
			}
			if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeC, pointToSideB)) < 0)
			{
				return false;
			}
			

			hitRecord.didHit = true;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.normal = triangle.normal;
			hitRecord.origin = ray.origin + rayToIntersect * ray.direction;
			hitRecord.t = rayToIntersect;
			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//slabtest
			/*if (!SlabTest_TriangleMesh(mesh, ray))
			{
				return false;
			}*/
			//todo W5

			for (int i = 0; i < mesh.normals.size(); ++i)
			{

				switch (mesh.cullMode)
				{
				case TriangleCullMode::BackFaceCulling:
					if (Vector3::Dot(mesh.normals[i], ray.direction) > 0)
					{
						continue;
					}
					break;
				case  TriangleCullMode::FrontFaceCulling:
					if (Vector3::Dot(mesh.normals[i], ray.direction) < 0)
					{
						continue;
					}
					break;
				}
				if (Vector3::Dot(mesh.normals[i], ray.direction) == 0)
				{
					continue;
				}

				Vector3 center = (mesh.positions[mesh.indices[0 + 3 * i]] + mesh.positions[mesh.indices[1 + 3 * i]] + mesh.positions[mesh.indices[2 + 3 * i]]) / 3;
				Vector3 L = center - ray.origin;
				float rayToIntersect = Vector3::Dot(L, mesh.normals[i]) / Vector3::Dot(ray.direction, mesh.normals[i]);
				if (rayToIntersect < ray.min && rayToIntersect > ray.max)
				{
					continue;
				}

				Vector3 P = ray.origin + rayToIntersect * ray.direction;
				Vector3 edgeA = mesh.positions[mesh.indices[1 + 3 * i]] - mesh.positions[mesh.indices[0 + 3 * i]];
				Vector3 edgeB = mesh.positions[mesh.indices[2 + 3 * i]] - mesh.positions[mesh.indices[0 + 3 * i]];
				Vector3 edgeC = mesh.positions[mesh.indices[2 + 3 * i]] - mesh.positions[mesh.indices[1 + 3 * i]];
				Vector3 pointToSideA = P - mesh.positions[mesh.indices[0 + 3 * i]];
				Vector3 pointToSideB = P - mesh.positions[mesh.indices[1 + 3 * i]];

				if (Vector3::Dot(mesh.normals[i], Vector3::Cross(edgeA, pointToSideA)) < 0)
				{
					continue;
				}
				if (Vector3::Dot(mesh.normals[i], Vector3::Cross(edgeB, pointToSideA)) > 0)
				{
					continue;
				}
				if (Vector3::Dot(mesh.normals[i], Vector3::Cross(edgeC, pointToSideB)) < 0)
				{
					continue;
				}


				hitRecord.didHit = true;
				hitRecord.materialIndex = mesh.materialIndex;
				hitRecord.normal = mesh.normals[i];
				hitRecord.origin = ray.origin + rayToIntersect * ray.direction;
				hitRecord.t = rayToIntersect;
				return true;
			}

			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}

		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedminAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedminAABB.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedminAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedminAABB.y - ray.origin.y) / ray.direction.y;

			tmin = std::min(tmin, std::min(ty1, ty2));
			tmax = std::max(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedminAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedminAABB.z - ray.origin.z) / ray.direction.z;

			tmin = std::min(tmin, std::min(tz1, tz2));
			tmax = std::max(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			
			return {light.origin - origin};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			if (light.type == LightType::Point)
			{
				Vector3 lightToHit = light.origin - target;
				return { light.color * (light.intensity / Vector3::Dot(lightToHit, lightToHit)) };
			}
			else
			{
				return { light.color * light.intensity };

			}
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}