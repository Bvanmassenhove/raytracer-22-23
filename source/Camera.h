#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Vector3 camVelocity{};
		float camSpeed{ 10.f };
		Matrix finalRotation{};
		float camRotation{ 0.1f };

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			Vector3 worldUp{ 0,1,0 };
			right = Vector3::Cross(worldUp, forward);
			right.Normalize();
			up = Vector3::Cross(forward, right);
			up.Normalize();

			Matrix ONB{};
			ONB[0] = { right,0 };
			ONB[1] = { up,0 };
			ONB[2] = { forward,0 };
			ONB[3] = { origin , 1 };
			return ONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			bool hasMove{ false };

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				camVelocity = forward * camSpeed;
				hasMove = true;
			}
			else if (pKeyboardState[SDL_SCANCODE_S])
			{
				camVelocity = forward * (-camSpeed);
				hasMove = true;
			}

			if (pKeyboardState[SDL_SCANCODE_A])
			{
				camVelocity = right * (-camSpeed);
				hasMove = true;
			}
			else if (pKeyboardState[SDL_SCANCODE_D])
			{
				camVelocity = right * camSpeed;
				hasMove = true;
			}

			if (!hasMove)
			{
				camVelocity *= 0.f;
			}

			origin += camVelocity * deltaTime;

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (mouseState == (SDL_BUTTON(SDL_BUTTON_LEFT) | SDL_BUTTON(SDL_BUTTON_RIGHT)))
			{
				origin.y += mouseY;
			}
			else if (mouseState == SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				if (mouseY != 0)
				{
					origin += forward * -float(mouseY);
				}

				totalYaw += camRotation * -mouseX * deltaTime;
				finalRotation = Matrix::CreateRotation(totalYaw, 0.f, 0.f);
				
			}
			else if (mouseState == SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				totalYaw += camRotation * -mouseX * deltaTime;
				totalPitch += camRotation * -mouseY * deltaTime;

				finalRotation = Matrix::CreateRotation(totalYaw, totalPitch, 0.f);
				
			}
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();
			
			//todo: W2
			//assert(false && "Not Implemented Yet");
		}
	};
}
