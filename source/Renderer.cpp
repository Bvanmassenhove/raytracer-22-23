//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

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
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();
	float fov{ std::tan(camera.fovAngle / 2) };
	//loop over pixels
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			float x = ((2 * ((px + 0.5f) / m_Width)) -1) * (float(m_Width) / float(m_Height)) * fov;
			float y = (1 - 2 * ((py + 0.5f) / m_Height)) * fov;

			Vector3 rayDirection{ x,y,1};
			rayDirection.Normalize();
			rayDirection = camera.CalculateCameraToWorld().TransformVector(rayDirection);
			Ray viewRay{ camera.origin, rayDirection };


			ColorRGB finalColor{};

			HitRecord closestHit{};

			pScene->GetClosestHit(viewRay, closestHit);
			
			if (closestHit.didHit)
			{
				for(Light light : lights)
				{
					Vector3 LightVector = LightUtils::GetDirectionToLight(light, closestHit.origin);
					Ray ShadowRay{ closestHit.origin ,LightVector.Normalized() };
					ShadowRay.max = LightVector.Magnitude();
					if (pScene->DoesHit(ShadowRay))
					{
						finalColor = materials[closestHit.materialIndex]->Shade()*.5f;
					}
					else
					{
						finalColor = materials[closestHit.materialIndex]->Shade() ;
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
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
