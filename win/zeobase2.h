// Copyright 2013 Peter Wallström
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

// while it is not required i like to request a few things
// 1. please do share any meaningfull/usefull changes/additions/fixes you make with me so that i could include it in any future version
// 2. likewise do share any ideas for improvements
// 3. If you make something comersiol or at least something you release publicly that relies on this code then i would like to know and maybe use in my CV
// 4. Please do include me in your credits

// glz basecode - lets just say you will need this
// visit http://www.flashbang.se or contact me at overlord@flashbang.se
// the entire toolkit should exist in it's entirety at github
// https://github.com/zeoverlord/glz.git

#pragma once

#include "resource.h"
#include <wchar.h>
#include "..\glz-core\state\baseState.h"


typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	LPCWSTR		className;						// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
	Application*		application;				// Application Structure
	LPCWSTR				title;						// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} GL_WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
//	Keys*				keys;						// Key Structure
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
	float				deltaTime;					// deltatime
	int					x,y;
} GL_Window;										// GL_Window


void preInitialize(void);

void TerminateApplication();		// Terminate The Application

void ToggleFullscreen();			// Toggle Fullscreen / Windowed Mode

//bool Initialize(int width, int height);	// Performs All Your Initialization

//void Deinitialize(void);							// Performs All Your DeInitialization

//void Update(float seconds);					// Perform Motion Updates

//void DisplayUpdate(int width, int height);

//void Draw(void);									// Perform All Your Scene Drawing

//bool pollMessageQuit();
//bool pollMessageFullscreen();