/*****************************************************************************

Filename    :   main.cpp
Content     :   Simple minimal VR demo
Created     :   December 1, 2014
Author      :   Tom Heath
Copyright   :   Copyright 2012 Oculus, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

/*****************************************************************************/
/// This sample has not yet been fully assimiliated into the framework
/// and also the GL support is not quite fully there yet, hence the VR
/// is not that great!

//#include "Win32_GLAppUtil.h"
// Include the Oculus SDK
#include "OVR_CAPI_GL.h"
//#include "Win32_DirectXAppUtil.h"	//DirectX
//#include "Win32_BasicVR.h"	//BasicVR
#include "Scene.h"
#include "Texture.h"
#include <fmod.hpp>
#include <stdio.h>

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "fmod_vc.lib")
#pragma warning(disable:4996)
#endif

using namespace OVR;
using namespace FMOD;

struct OculusTextureBuffer
{
	ovrSession          Session;
	ovrTextureSwapChain ColorTextureChain;
	ovrTextureSwapChain DepthTextureChain;
	GLuint              fboId;
	Sizei               texSize;

	OculusTextureBuffer(ovrSession session, Sizei size, int sampleCount) :
		Session(session),
		ColorTextureChain(nullptr),
		DepthTextureChain(nullptr),
		fboId(0),
		texSize(0, 0)
	{
		assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

		texSize = size;

		// This texture isn't necessarily going to be a rendertarget, but it usually is.
		assert(session); // No HMD? A little odd.

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Width = size.w;
		desc.Height = size.h;
		desc.MipLevels = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleCount = sampleCount;
		desc.StaticImage = ovrFalse;

		{
			ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &ColorTextureChain);

			int length = 0;
			ovr_GetTextureSwapChainLength(session, ColorTextureChain, &length);

			if (OVR_SUCCESS(result))
			{
				for (int i = 0; i < length; ++i)
				{
					GLuint chainTexId;
					ovr_GetTextureSwapChainBufferGL(Session, ColorTextureChain, i, &chainTexId);
					glBindTexture(GL_TEXTURE_2D, chainTexId);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
			}
		}

		desc.Format = OVR_FORMAT_D32_FLOAT;

		{
			ovrResult result = ovr_CreateTextureSwapChainGL(Session, &desc, &DepthTextureChain);

			int length = 0;
			ovr_GetTextureSwapChainLength(session, DepthTextureChain, &length);

			if (OVR_SUCCESS(result))
			{
				for (int i = 0; i < length; ++i)
				{
					GLuint chainTexId;
					ovr_GetTextureSwapChainBufferGL(Session, DepthTextureChain, i, &chainTexId);
					glBindTexture(GL_TEXTURE_2D, chainTexId);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
			}
		}

		glGenFramebuffers(1, &fboId);
	}

	~OculusTextureBuffer()
	{
		if (ColorTextureChain)
		{
			ovr_DestroyTextureSwapChain(Session, ColorTextureChain);
			ColorTextureChain = nullptr;
		}
		if (DepthTextureChain)
		{
			ovr_DestroyTextureSwapChain(Session, DepthTextureChain);
			DepthTextureChain = nullptr;
		}
		if (fboId)
		{
			glDeleteFramebuffers(1, &fboId);
			fboId = 0;
		}
	}

	Sizei GetSize() const
	{
		return texSize;
	}

	void SetAndClearRenderSurface()
	{
		GLuint curColorTexId;
		GLuint curDepthTexId;
		{
			int curIndex;
			ovr_GetTextureSwapChainCurrentIndex(Session, ColorTextureChain, &curIndex);
			ovr_GetTextureSwapChainBufferGL(Session, ColorTextureChain, curIndex, &curColorTexId);
		}
		{
			int curIndex;
			ovr_GetTextureSwapChainCurrentIndex(Session, DepthTextureChain, &curIndex);
			ovr_GetTextureSwapChainBufferGL(Session, DepthTextureChain, curIndex, &curDepthTexId);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curColorTexId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, curDepthTexId, 0);

		glViewport(0, 0, texSize.w, texSize.h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	void UnsetRenderSurface()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	}

	void Commit()
	{
		ovr_CommitTextureSwapChain(Session, ColorTextureChain);
		ovr_CommitTextureSwapChain(Session, DepthTextureChain);
	}
};

static ovrGraphicsLuid GetDefaultAdapterLuid()
{
	ovrGraphicsLuid luid = ovrGraphicsLuid();

#if defined(_WIN32)
	IDXGIFactory* factory = nullptr;

	if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
	{
		IDXGIAdapter* adapter = nullptr;

		if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
		{
			DXGI_ADAPTER_DESC desc;

			adapter->GetDesc(&desc);
			memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
			adapter->Release();
		}

		factory->Release();
	}
#endif

	return luid;
}


static int Compare(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(ovrGraphicsLuid));
}


// return true to retry later (e.g. after display lost)
static bool MainLoop(bool retryCreate)
{
	OculusTextureBuffer * eyeRenderTexture[2] = { nullptr, nullptr };
	ovrMirrorTexture mirrorTexture = nullptr;
	GLuint          mirrorFBO = 0;
	Scene         * roomScene = nullptr;
	long long frameIndex = 0;

	ovrSession session;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&session, &luid);
	if (!OVR_SUCCESS(result))
		return retryCreate;

	//音频的设置
	FMOD::System	*system = NULL;
	FMOD::Sound		*sound1 = NULL;
	FMOD::Sound		*sound2 = NULL;
	//FMOD::Channel	*channel = 0;

	FMOD::System_Create(&system);
	system->init(32, FMOD_INIT_NORMAL, 0);
	system->createStream("../../../Music/BackgroundMusic.mp3", FMOD_INIT_NORMAL, 0, &sound1);
	system->createSound("../../../Music/ButtonClick.mp3", FMOD_INIT_NORMAL, 0, &sound2);
	sound1->setMode(FMOD_LOOP_NORMAL); // FMOD_LOOP_OFF 只播放一次
	sound2->setMode(FMOD_LOOP_OFF);
	system->playSound(sound1, NULL, false, NULL);

	///*VRLayer* Layer[16];
	//Layer[0] = new VRLayer(session);
	//Camera* MainCam;*/
	//Create a trivial model to represent the controller
	//TriangleSet cube;
	//cube.AddSolidColorBox(0.05f, -0.05f, 0.05f, -0.05f, 0.05f, -0.05f, 0xff404040);
	//Model * controllerL;
	////controllerL = new Model(&cube, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1), new Material(new Texture(false, 256, 256, Texture::AUTO_CEILING)));
	//Model * controllerR;
	////controllerR = new Model(&cube, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1), new Material(new Texture(false, 256, 256, Texture::generateTecture)));


	if (Compare(luid, GetDefaultAdapterLuid())) // If luid that the Rift is on is not the default adapter LUID...
	{
		VALIDATE(false, "OpenGL supports only the default graphics adapter.");
	}

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);

	// Setup Window and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
	ovrSizei windowSize = { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };
	if (!Platform.InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
		goto Done;

	// Make eye render buffers
	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(session, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
		eyeRenderTexture[eye] = new OculusTextureBuffer(session, idealTextureSize, 1);

		if (!eyeRenderTexture[eye]->ColorTextureChain || !eyeRenderTexture[eye]->DepthTextureChain)
		{
			if (retryCreate) goto Done;
			VALIDATE(false, "Failed to create texture.");
		}
	}

	ovrMirrorTextureDesc desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = windowSize.w;
	desc.Height = windowSize.h;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	result = ovr_CreateMirrorTextureWithOptionsGL(session, &desc, &mirrorTexture);
	if (!OVR_SUCCESS(result))
	{
		if (retryCreate) goto Done;
		VALIDATE(false, "Failed to create mirror texture.");
	}

	// Configure the mirror read buffer
	GLuint texId;
	ovr_GetMirrorTextureBufferGL(session, mirrorTexture, &texId);

	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Turn off vsync to let the compositor do its magic
	wglSwapIntervalEXT(0);

	// Make scene - can simplify further if needed
	roomScene = new Scene(false);

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

	// Main loop
	while (Platform.HandleMessages())
	{
		ovrSessionStatus sessionStatus;
		ovr_GetSessionStatus(session, &sessionStatus);

		//ActionFromInput(0.0f, false, true);
		//ovrTrackingState hmdState = Layer[0]->GetEyePoses();
		//ovrTrackerPose trackerPose = ovr_GetTrackerPose(session, 0);

		////Write position and orientation into controller models
		//controllerL->Pos = XMFLOAT3(XMVectorGetX(MainCam->Pos) + hmdState.HandPoses[ovrHand_Left].ThePose.Position.x,
		//	XMVectorGetY(MainCam->Pos) + hmdState.HandPoses[ovrHand_Left].ThePose.Position.y,
		//	XMVectorGetZ(MainCam->Pos) + hmdState.HandPoses[ovrHand_Left].ThePose.Position.z);
		//controllerL->Rot = XMFLOAT4(hmdState.HandPoses[ovrHand_Left].ThePose.Orientation.x,
		//	hmdState.HandPoses[ovrHand_Left].ThePose.Orientation.y,
		//	hmdState.HandPoses[ovrHand_Left].ThePose.Orientation.z,
		//	hmdState.HandPoses[ovrHand_Left].ThePose.Orientation.w);
		//controllerR->Pos = XMFLOAT3(XMVectorGetX(MainCam->Pos) + hmdState.HandPoses[ovrHand_Right].ThePose.Position.x,
		//	XMVectorGetY(MainCam->Pos) + hmdState.HandPoses[ovrHand_Right].ThePose.Position.y,
		//	XMVectorGetZ(MainCam->Pos) + hmdState.HandPoses[ovrHand_Right].ThePose.Position.z);
		//controllerR->Rot = XMFLOAT4(hmdState.HandPoses[ovrHand_Right].ThePose.Orientation.x,
		//	hmdState.HandPoses[ovrHand_Right].ThePose.Orientation.y,
		//	hmdState.HandPoses[ovrHand_Right].ThePose.Orientation.z,
		//	hmdState.HandPoses[ovrHand_Right].ThePose.Orientation.w);

		//Button presses are modifying the colour of the controller model below
		ovrInputState inputState;
		ovr_GetInputState(session, ovrControllerType_Touch, &inputState);

		if (sessionStatus.ShouldQuit)
		{
			// Because the application is requested to quit, should not request retry
			retryCreate = false;
			break;
		}
		if (sessionStatus.ShouldRecenter)
			ovr_RecenterTrackingOrigin(session);

		if (sessionStatus.IsVisible)
		{
			// Keyboard inputs to adjust player orientation
			static float Yaw(3.141592f);
			if (Platform.Key[VK_LEFT])  Yaw += 0.02f;
			if (Platform.Key[VK_RIGHT]) Yaw -= 0.02f;

			// Keyboard inputs to adjust player position
			static Vector3f Pos2(0.0f, 0.0f, 20.0f);
			if (Platform.Key['W'] || Platform.Key[VK_UP])     Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, -0.05f));
			if (Platform.Key['S'] || Platform.Key[VK_DOWN])   Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, +0.05f));
			if (Platform.Key['D'])                            Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.05f, 0, 0));
			if (Platform.Key['A'])                            Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.05f, 0, 0));

			//Here the left Touch controller thumbstick is updating our position, allowing us to move forward, backward, left and right.
			//And the right Touch controller thumbstick is updating our orientation, allowing us to rotate left and right.
			Vector2f leftStick = inputState.Thumbstick[ovrHand_Left];
			Vector2f rightStick = inputState.Thumbstick[ovrHand_Right];

			Pos2 += Matrix4f::RotationY(Yaw).Transform(
				Vector3f(leftStick.x * leftStick.x * (leftStick.x > 0 ? 0.1f : -0.1f), 0, leftStick.y * leftStick.y * (leftStick.y > 0 ? -0.1f : 0.1f)));

			//if (rightStick.x > 0) Yaw -= 0.01f;
			//else if (rightStick.x < 0) Yaw += 0.01f;
			static int timer = 0;
			timer++;
			bool setX = false, setY = false;
			// 在处理UI事件的时候设定timer使得它是离散的
			if (roomScene->menu.mainstatus != -1)
			{
				if (timer > 20)

				{
					system->playSound(sound2, NULL, false, NULL);
					if (Platform.Key['L'] || rightStick.x > 0.8)
					{
						if (!setX)
							roomScene->Event(1), setX = true, timer = 0;
					}
					else if (Platform.Key['J'] || rightStick.x < -0.8)
					{
						if (!setX)
							roomScene->Event(0), setX = true, timer = 0;
					}
					else setX = false;

					if (Platform.Key['I'] || rightStick.y > 0.8)
					{
						if (!setY)
							roomScene->Event(3), setY = true, timer = 0;
					}
					else if (Platform.Key['K'] || rightStick.y < -0.8)
					{
						if (!setY)
							roomScene->Event(2), setY = true, timer = 0;
					}
					else setY = false;


					// TODO add change Material
					if (Platform.Key['O'] || (inputState.Buttons&ovrTouch_A))
					{
						roomScene->Event(4);
						timer = 0;
					}
					// TODO add change Material
					if (Platform.Key['U'] || (inputState.Buttons&ovrTouch_B))
					{
						roomScene->Event(5);
						timer = 0;

					}
				}
			}
			else 
			{
				if (Platform.Key['L'] || rightStick.x > 0.8)
				{
						roomScene->Event(1),  timer = 0;
				}
				else if (Platform.Key['J'] || rightStick.x < -0.8)
				{
						roomScene->Event(0),  timer = 0;
				}

				if (Platform.Key['I'] || rightStick.y > 0.8)
				{
						roomScene->Event(3),  timer = 0;
				}
				else if (Platform.Key['K'] || rightStick.y < -0.8)
				{
						roomScene->Event(2),  timer = 0;
				}


				if(inputState.Buttons)
					int temp = inputState.Buttons;
				if (Platform.Key['U'] || (inputState.Buttons&ovrTouch_B)|| Platform.Key['O'] || (inputState.Buttons&ovrTouch_A)|| (inputState.Buttons&ovrTouch_RIndexTrigger))
				{
					roomScene->Event(4);
					timer = 0;
				}

			
			}
			// Animate the cube

			roomScene->Calculate();
			// Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyePose) may change at runtime.
			ovrEyeRenderDesc eyeRenderDesc[2];
			eyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
			eyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

			// Get eye poses, feeding in correct IPD offset
			ovrPosef EyeRenderPose[2];
			ovrPosef HmdToEyePose[2] = { eyeRenderDesc[0].HmdToEyePose,
										 eyeRenderDesc[1].HmdToEyePose };

			double sensorSampleTime;    // sensorSampleTime is fed into the layer later
			ovr_GetEyePoses(session, frameIndex, ovrTrue, HmdToEyePose, EyeRenderPose, &sensorSampleTime);

			ovrTimewarpProjectionDesc posTimewarpProjectionDesc = {};

			// Render Scene to Eye Buffers
			for (int eye = 0; eye < 2; ++eye)
			{
				// Switch to eye render target
				eyeRenderTexture[eye]->SetAndClearRenderSurface();

				// Get view and projection matrices
				Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
				Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(EyeRenderPose[eye].Orientation);
				Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
				Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
				Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[eye].Position);

				Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
				Matrix4f proj = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_None);
				posTimewarpProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(proj, ovrProjection_None);

				Matrix4f viewfollow = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + Vector3f(0, 0, -1), Vector3f(0, 1, 0));
				// Render world
				roomScene->Render(view, proj, Pos2, viewfollow);

				// Avoids an error when calling SetAndClearRenderSurface during next iteration.
				// Without this, during the next while loop iteration SetAndClearRenderSurface
				// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
				// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
				eyeRenderTexture[eye]->UnsetRenderSurface();

				// Commit changes to the textures so they get picked up frame
				eyeRenderTexture[eye]->Commit();
			}

			// Do distortion rendering, Present and flush/sync

			ovrLayerEyeFovDepth ld = {};
			ld.Header.Type = ovrLayerType_EyeFovDepth;
			ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
			ld.ProjectionDesc = posTimewarpProjectionDesc;

			for (int eye = 0; eye < 2; ++eye)
			{
				ld.ColorTexture[eye] = eyeRenderTexture[eye]->ColorTextureChain;
				ld.DepthTexture[eye] = eyeRenderTexture[eye]->DepthTextureChain;
				ld.Viewport[eye] = Recti(eyeRenderTexture[eye]->GetSize());
				ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
				ld.RenderPose[eye] = EyeRenderPose[eye];
				ld.SensorSampleTime = sensorSampleTime;
			}

			ovrLayerHeader* layers = &ld.Header;
			result = ovr_SubmitFrame(session, frameIndex, nullptr, &layers, 1);
			// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
			if (!OVR_SUCCESS(result))
				goto Done;

			frameIndex++;
		}

		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = windowSize.w;
		GLint h = windowSize.h;
		glBlitFramebuffer(0, h, w, 0,
			0, 0, w, h,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		SwapBuffers(Platform.hDC);
	}

Done:
	delete roomScene;
	if (mirrorFBO) glDeleteFramebuffers(1, &mirrorFBO);
	if (mirrorTexture) ovr_DestroyMirrorTexture(session, mirrorTexture);
	for (int eye = 0; eye < 2; ++eye)
	{
		delete eyeRenderTexture[eye];
	}
	Platform.ReleaseDevice();
	ovr_Destroy(session);

	//release sound
	sound1->release();
	sound2->release();
	system->close();

	// Retry on ovrError_DisplayLost
	return retryCreate || (result == ovrError_DisplayLost);
}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	Texture::generateTecture(400, 400, 0xAA1111, true);

	// Initializes LibOVR, and the Rift
	ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	VALIDATE(Platform.InitWindow(hinst, L"Demiurge"), "Failed to open window.");

	Platform.Run(MainLoop);

	ovr_Shutdown();

	return(0);
}
