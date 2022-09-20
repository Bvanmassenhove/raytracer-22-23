#pragma once
#include <vector>
#include "Ray.h"
#include "Object.h"
class SceneGraph
{
	public:
		SceneGraph() = default;;
		~SceneGraph();
	
		bool Hit(const Ray & ray, HitRecord & hitRecord);
		bool Hit(const Ray & ray);
		void AddObject(Object * pObject);
		void Update(const float elapsedSec);

	private:

		std::vector<Object*> m_pObjects;
};

