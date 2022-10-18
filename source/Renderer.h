#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;
		void SwitchShadows()
		{
			m_ShadowsEnabled = !m_ShadowsEnabled;
		}
		void scycleLightMode()
		{
			switch (m_CurrentLightingMode)
			{
			case LightingMode::observedArea:
				m_CurrentLightingMode = LightingMode::Radience;
				break;
			case LightingMode::Radience:
				m_CurrentLightingMode = LightingMode::BDRF;
				break;
			case LightingMode::BDRF:
				m_CurrentLightingMode = LightingMode::Combined;
				break;
			case LightingMode::Combined:
				m_CurrentLightingMode = LightingMode::observedArea;
				break;
			}
		}

	private:

		enum class LightingMode
		{
			observedArea, //lambert Law
			Radience, //Incident Radiance
			BDRF, //Scattering of the Light
			Combined // observedArea*Radiance*BDRF
		};
		LightingMode m_CurrentLightingMode{ LightingMode::Combined };
		bool m_ShadowsEnabled{ true };

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
		
	};
}
