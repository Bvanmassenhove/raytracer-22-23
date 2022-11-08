//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Scene.h"
#include <future> //async
#include <ppl.h> //parallel_for

//#define ASYNC
#define PARALLEL_FOR
using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	
	Camera& camera = pScene->GetCamera();
	Matrix cameraToWorld = camera.CalculateCameraToWorld();

	const float fov{ std::tan(camera.fovAngle / 2) };

	const float aspectRatio = m_Width / static_cast<float>(m_Height);

	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const uint32_t numPixels = m_Width * m_Height;

#if defined(ASYNC)
	//async Logic

	const uint32_t numCores = std::thread::hardware_concurrency();
	std::vector<std::future<void>> async_futures{};
	const uint32_t numPixelsPerTask = numPixels / numCores;
	uint32_t numUnassignedPixels = numPixels % numCores;
	uint32_t currPixelIdex = 0;

	for (uint32_t coreId{0}; coreId < numCores; ++coreId)
	{
		uint32_t tasksize = numPixelsPerTask;
		if (numUnassignedPixels > 0)
		{
			++tasksize;
			--numUnassignedPixels;
		}

		async_futures.push_back(std::async(std::launch::async, [=, this]
			{
				//Render all pixels for this task (currentPixelIndex > currPixelIndex + taskSize)
				const uint32_t pixelIndexEnd = currPixelIdex + tasksize;
				for (uint32_t pixelIdex{currPixelIdex}; pixelIdex < pixelIndexEnd; ++pixelIdex)
				{
					RenderPixel(pScene, pixelIdex, fov, aspectRatio, camera, lights, materials);
				}
			}));

		currPixelIdex += tasksize;
	}

	for (const std::future<void>& f : async_futures)
	{
		f.wait();
	}

#elif defined(PARALLEL_FOR)
	//Parrallel for logic

	concurrency::parallel_for(0u, numPixels, [=, this](int i) {RenderPixel(pScene, i, fov, aspectRatio, camera, lights, materials); });
#else 
	//synchronous logic (no threading)
	for (uint32_t i{ 0 }; i < numPixels; ++i)
	{
		RenderPixel(pScene, i, fov, aspectRatio, camera, lights, materials);
	}
#endif 


	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);

	//loop over pixels

//#pragma omp parallel for
//	for (int px{}; px < m_Width; ++px)
//	{
//		for (int py{}; py < m_Height; ++py)
//		{
//
//			float x = ((2 * ((px + 0.5f) / m_Width)) - 1) * (float(m_Width) / float(m_Height)) * fov;
//			float y = (1 - 2 * ((py + 0.5f) / m_Height)) * fov;
//
//			Vector3 rayDirection{ x,y,1};
//			rayDirection.Normalize();
//			rayDirection = cameraToWorld.TransformVector(rayDirection);
//			Ray viewRay{ camera.origin, rayDirection };
//
//
//			ColorRGB finalColor{};
//
//			HitRecord closestHit{};
//
//			pScene->GetClosestHit(viewRay, closestHit);
//			
//			if (closestHit.didHit)
//			{
//				for(Light light : lights)
//				{
//					Vector3 LightVector = LightUtils::GetDirectionToLight(light, closestHit.origin);
//					float lightvectorLenght = LightVector.Normalize();
//					Ray ShadowRay{ closestHit.origin+ closestHit.normal*0.001f ,LightVector };
//					ShadowRay.max = lightvectorLenght;
//					float dot = Vector3::Dot(LightVector, closestHit.normal);
//					if (dot >= 0)
//					{
//						switch (m_CurrentLightingMode)
//						{
//						case LightingMode::observedArea:
//							
//							if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
//							{
//								finalColor += ColorRGB{ 1.f,1.f,1.f } * dot *.5f;
//							}
//							else
//							{
//								finalColor += ColorRGB{ 1.f,1.f,1.f } *dot;
//							}
//							break;
//						case LightingMode::Radience:
//							if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
//							{
//								finalColor += LightUtils::GetRadiance(light, closestHit.origin) * dot * .5f;
//							}
//							else
//							{
//								finalColor += LightUtils::GetRadiance(light, closestHit.origin) * dot;
//							}
//							break;
//						case LightingMode::BDRF:
//							if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
//							{
//								finalColor += materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction) * .5f;
//							}
//							else
//							{
//								finalColor +=materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction);
//							}
//							break;
//						case LightingMode::Combined:
//							if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
//							{
//								finalColor += LightUtils::GetRadiance(light, closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction) * dot * .5f;
//							}
//							else
//							{
//								finalColor += LightUtils::GetRadiance(light, closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction) * dot;
//							}
//							break;
//						}
//					}
//					
//				}
//			}
//			//Update Color in Buffer
//			finalColor.MaxToOne();
//
//			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//				static_cast<uint8_t>(finalColor.r * 255),
//				static_cast<uint8_t>(finalColor.g * 255),
//				static_cast<uint8_t>(finalColor.b * 255));
//		}
//	}

}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Camera& camera, const std::vector<Light>& lights, const std::vector<Material*>& materials) const
{
	const int px = pixelIndex % m_Width;
	const int py = pixelIndex / m_Width;

	float rx = px + 0.5f;
	float ry = py + 0.5f;

	float cx = (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov;
	float cy = (1 - (2 * (ry / float(m_Height)))) * fov;

	/*Vector3 rayDirection{ cx,cy,1 };
	rayDirection.Normalize();
	rayDirection = camera.cameraToWorld.TransformVector(rayDirection);
	Ray viewRay{ camera.origin, rayDirection };*/

	ColorRGB finalColor{};

	HitRecord closestHit{};

	//pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{
		for (Light light : lights)
		{
			Vector3 rayDirection{ cx,cy,1 };
			rayDirection.Normalize();
			rayDirection = camera.cameraToWorld.TransformVector(rayDirection);
			Ray viewRay{ camera.origin, rayDirection };
			pScene->GetClosestHit(viewRay, closestHit);

			Vector3 LightVector = LightUtils::GetDirectionToLight(light, closestHit.origin);
			float lightvectorLenght = LightVector.Normalize();
			Ray ShadowRay{ closestHit.origin + closestHit.normal * 0.001f ,LightVector };
			ShadowRay.max = lightvectorLenght;
			float dot = Vector3::Dot(LightVector, closestHit.normal);
			if (dot >= 0)
			{
				switch (m_CurrentLightingMode)
				{
				case LightingMode::observedArea:

					if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
					{
						finalColor += ColorRGB{ 1.f,1.f,1.f } *dot * .5f;
					}
					else
					{
						finalColor += ColorRGB{ 1.f,1.f,1.f } *dot;
					}
					break;
				case LightingMode::Radience:
					if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
					{
						finalColor += LightUtils::GetRadiance(light, closestHit.origin) * dot * .5f;
					}
					else
					{
						finalColor += LightUtils::GetRadiance(light, closestHit.origin) * dot;
					}
					break;
				case LightingMode::BDRF:
					if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
					{
						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction) * .5f;
					}
					else
					{
						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction);
					}
					break;
				case LightingMode::Combined:
					if (pScene->DoesHit(ShadowRay) && m_ShadowsEnabled)
					{
						finalColor += LightUtils::GetRadiance(light, closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction) * dot * .5f;
					}
					else
					{
						finalColor += LightUtils::GetRadiance(light, closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, LightVector, -viewRay.direction) * dot;
					}
					break;
				}
			}

		}
	}
	//Update Color in Buffer
	finalColor.MaxToOne();

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
