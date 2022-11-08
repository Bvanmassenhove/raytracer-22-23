#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}
		
		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{

		HitRecord tempHit{};
		
		for (const Sphere& sphere : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray, tempHit))
			{
				if (tempHit.t < closestHit.t)
				{
					closestHit = tempHit;
				}
			}
		}
		for (const Plane& plane : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(plane, ray, tempHit))
			{
				if (tempHit.t < closestHit.t)
				{
					closestHit = tempHit;
				}
			}
		}
		for (const Triangle& triangle : m_triangles)
		{
			if (GeometryUtils::HitTest_Triangle(triangle, ray, tempHit))
			{
				if (tempHit.t < closestHit.t)
				{
					closestHit = tempHit;
				}
			}
		}
		for (const TriangleMesh& triangleMesh : m_TriangleMeshGeometries)
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray, tempHit))
			{
				if (tempHit.t < closestHit.t)
				{
					closestHit = tempHit;
				}
			}
		}
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		//todo W3
		for (const Sphere& sphere : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray))
			{
				return true;
			}
		}
		for (const Plane& plane : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(plane, ray))
			{
				return true;
			}
		}
		for (const Triangle& triangle : m_triangles)
		{
			if (GeometryUtils::HitTest_Triangle(triangle, ray))
			{
				return true;
			}
		}
		for (const TriangleMesh& triangleMesh : m_TriangleMeshGeometries)
		{
			if (GeometryUtils::HitTest_TriangleMesh(triangleMesh, ray))
			{
				return true;
			}
		}
		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
				//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });

		//Spheres
		AddSphere({-25.f, 0.f, 100.f}, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Plane
		AddPlane({-75.f, 0.f, 0.f}, {1.f, 0.f,0.f}, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion
#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f,3.f,-9.f };
		m_Camera.fovAngle = 45.f * TO_RADIANS;
		//default: Material id0 >> SolidColor Material (RED)
		constexpr unsigned char matId_Solid_Red = 0;
		const unsigned char matId_Solid_Blue = AddMaterial(new Material_SolidColor{ colors::Blue });

		const unsigned char matId_Solid_Yellow = AddMaterial(new Material_SolidColor{ colors::Yellow });
		const unsigned char matId_Solid_Green = AddMaterial(new Material_SolidColor{ colors::Green });
		const unsigned char matId_Solid_Magenta = AddMaterial(new Material_SolidColor{ colors::Magenta });


		//Plane
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

		//light
		AddPointLight({ 0.f,5.f,-5.f }, 70.f, colors::White);
		
	}
#pragma endregion

#pragma region SCENE W3
	void Scene_W3::Initialize()
	{
		m_Camera.origin = { 0.f,3.f,-9.f };
		m_Camera.fovAngle = 45.f * TO_RADIANS;

		const auto mathCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, 1.f));
		const auto mathCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .6f));
		const auto mathCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .1f));
		const auto mathCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, 1.f));
		const auto mathCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .6f));
		const auto mathCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .1f));

		const auto mathLambert_GrayBlue = AddMaterial(new Material_Lambert({.49f,.57f,.57f},1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, mathLambert_GrayBlue);//BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, mathLambert_GrayBlue);//BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, mathLambert_GrayBlue);//TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, mathLambert_GrayBlue);//RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, mathLambert_GrayBlue);//LEFT

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f,mathCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, .75f,mathCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f,mathCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f,mathCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, .75f,mathCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f,mathCT_GraySmoothPlastic);

		//light
		AddPointLight({ 0.f,5.f,5.f }, 50.f, ColorRGB{1.f,.61f,.45f}); //backlight
		AddPointLight({ -2.5f,5.f,-5.f }, 70.f, ColorRGB{1.f,.8f,.45f}); //front lights
		AddPointLight({ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{.34f,.47f,.68f});

	}
#pragma endregion


#pragma region SCENE W4
	void Scene_W4::Initialize()
	{
		m_Camera.origin = { 0.f,1.f,-5.f };
		m_Camera.totalYaw = {0};
		m_Camera.fovAngle = 45.f * TO_RADIANS;

		const auto mathLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f,.57f,.57f }, 1.f));
		const auto mathLambert_GrayWhite = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, mathLambert_GrayBlue);//BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, mathLambert_GrayBlue);//BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, mathLambert_GrayBlue);//TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, mathLambert_GrayBlue);//RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, mathLambert_GrayBlue);//LEFT

		////Triangle (temp)

		//auto triangle = Triangle{ {-.75f,.5f,.0f},{-.75f,2.f,.0f},{.75f,.5f,0.f} };
		//triangle.cullMode = TriangleCullMode::FrontFaceCulling;
		//triangle.materialIndex = mathLambert_GrayWhite;

		//m_triangles.emplace_back(triangle);

		// triangle mesh

		const auto triangleMesh = AddTriangleMesh(TriangleCullMode::NoCulling, mathLambert_GrayWhite);
		triangleMesh->positions = { {-.75f,-1.f,.0f},{-.75f,1.f,.0f},{.75f,1.f,1.f},{.75f,-1.f,0.f} };
		triangleMesh->indices = {
			0,1,2,
			0,2,3,
		};
		triangleMesh->CalculateNormals();
		/*triangleMesh->Translate({ 0.f,1.5f,0.f });
		triangleMesh->RotateY(45);*/
		triangleMesh->UpdateTransforms();
		//light
		AddPointLight({ 0.f,5.f,5.f }, 50.f, ColorRGB{ 1.f,.61f,.45f }); //backlight
		AddPointLight({ -2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //front lights
		AddPointLight({ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f });

	}
#pragma endregion
#pragma region SCENE W4
	void Scene_W4_ReferenceScene::Initialize()
	{
		sceneName = "Reference Scene";
		m_Camera.origin = { 0.f,3.f,-9.f };
		//m_Camera.totalYaw = { 0 };
		m_Camera.fovAngle = 45.f * TO_RADIANS;

		const auto mathCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, 1.f));
		const auto mathCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .6f));
		const auto mathCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f,.960f,.915f }, 1.f, .1f));
		const auto mathCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, 1.f));
		const auto mathCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .6f));
		const auto mathCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f,.75f,.75f }, 0.f, .1f));

		const auto mathLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f,.57f,.57f }, 1.f));
		const auto mathLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

		//Plane
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, mathLambert_GrayBlue);//BACK
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, mathLambert_GrayBlue);//BOTTOM
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, mathLambert_GrayBlue);//TOP
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, mathLambert_GrayBlue);//RIGHT
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, mathLambert_GrayBlue);//LEFT

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, mathCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, mathCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, mathCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, mathCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, mathCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, mathCT_GraySmoothPlastic);

		//CW Winding order
		const Triangle baseTriangle = { Vector3(-.75f,1.5f,0.f),Vector3(.75f,0.f,0.f),Vector3(-.75f,0.f,0.f) };

		m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, mathLambert_White);
		m_Meshes[0]->AppendTriangle(baseTriangle, true);
		m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
		m_Meshes[0]->UpdateTransforms();

		m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, mathLambert_White);
		m_Meshes[1]->AppendTriangle(baseTriangle, true);
		m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
		m_Meshes[1]->UpdateTransforms();

		m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, mathLambert_White);
		m_Meshes[2]->AppendTriangle(baseTriangle, true);
		m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
		m_Meshes[2]->UpdateTransforms();
		
		AddPointLight({ 0.f,5.f,5.f }, 50.f, ColorRGB{ 1.f,.61f,.45f }); //backlight
		AddPointLight({ -2.5f,5.f,-5.f }, 70.f, ColorRGB{ 1.f,.8f,.45f }); //front lights
		AddPointLight({ 2.5f,2.5f,-5.f }, 50.f, ColorRGB{ .34f,.47f,.68f });
	}
	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		const auto yawAngle = (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2;
		for (const auto m :m_Meshes)
		{
			m->RotateY(yawAngle);
			m->UpdateTransforms();
		}
	}
#pragma endregion
}

