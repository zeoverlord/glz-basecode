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


// if i did this right you shouldn't have to change anything in this file, instead use the zeobase2.h file to set things up.

#include "stdafx.h"
#include "zeobase2.h"
#include <gl/gl.h>														// Header File For The OpenGL32 Library
#include <gl/glu.h>														// Header File For The GLu32 Library
#include <gl/glext.h>
#include <gl/wglext.h>
#include <fstream>
#include <windowsx.h>
#include "..\..\glz-core\app\appbase.h"
#include "..\..\glz-core\input\input.h"
#include "..\..\glz-core\state\stateManager.h"

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking

const int MAX_LOADSTRING = 100;
const int WM_TOGGLEFULLSCREEN = WM_USER + 1;									// Application Define Message For Toggling

using namespace std;

static BOOL g_isProgramLooping;											// Window Creation Loop, For FullScreen/Windowed Toggle			// Between Fullscreen / Windowed Mode
static BOOL g_createFullScreen;											// If TRUE, Then Create Fullscreen
static float dtTemp;													// temp value to store the deltatime in

DWORD windowStyle = WS_POPUP;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

GL_Window window;

GLZ::glzStateManager stateManager;

void TerminateApplication()							// Terminate The Application
{
	PostMessage (window.hWnd, WM_QUIT, 0, 0);							// Send A WM_QUIT Message
	g_isProgramLooping = FALSE;											// Stop Looping Of The Program
}

void ToggleFullscreen()								// Toggle Fullscreen/Windowed
{
	GLZ::glzAppinitialization app;
	if (app.data.ALLOW_FULLSCREENSWITCH)
		PostMessage (window.hWnd, WM_TOGGLEFULLSCREEN, 0, 0);				// Send A WM_TOGGLEFULLSCREEN Message
}


void ReshapeGL (int width, int height)									// Reshape The Window When It's Moved Or Resized
{
	stateManager.DisplayUpdate(width, height);
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				// Reset The Current Viewport
}

BOOL ChangeScreenResolution (int width, int height, int bitsPerPixel)	// Change The Screen Resolution
{
	DEVMODE dmScreenSettings;											// Device Mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));					// Make Sure Memory Is Cleared
	dmScreenSettings.dmSize				= sizeof (DEVMODE);				// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth		= width;						// Select Screen Width
	dmScreenSettings.dmPelsHeight		= height;						// Select Screen Height
	dmScreenSettings.dmBitsPerPel		= bitsPerPixel;					// Select Bits Per Pixel
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// Display Change Failed, Return False
	}
	return TRUE;														// Display Change Was Successful, Return True
}

BOOL CreateWindowGL ()									// This Code Creates Our OpenGL Window
{
	
  GLZ::glzAppinitialization app;
  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1; // Version Number
  pfd.dwFlags = PFD_DRAW_TO_WINDOW |  // Draws to a window
                PFD_SUPPORT_OPENGL |  // The format must support OpenGL
                PFD_DOUBLEBUFFER;     // Support for double buffering
  pfd.iPixelType = PFD_TYPE_RGBA;     // Uses an RGBA pixel format
  pfd.cColorBits = 32;                // 32 bits colors


	RECT windowRect = {0, 0, window.init.width, window.init.height};	// Define Our Window Coordinates

	int x=0,y=0;
	GLuint PixelFormat;													// Will Hold The Selected Pixel Format

	if (app.data.SHOW_FRAME)
	{
		if (app.data.ALLOW_RESIZE) windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
		else windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

		if (app.data.ALLOW_MAAXIMIZE)
			windowStyle=windowStyle | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	}
		
		DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// Define The Window's Extended Style
	



	if (window.init.isFullScreen == TRUE)								// Fullscreen Requested, Try Changing Video Modes
	{
		if (ChangeScreenResolution (window.init.width, window.init.height, window.init.bitsPerPixel) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			if (app.data.DISPLAY_ERRORS) MessageBox(HWND_DESKTOP, L"Mode Switch Failed.\nRunning In Windowed Mode.", L"Error", MB_OK | MB_ICONEXCLAMATION);
			window.init.isFullScreen = FALSE;							// Set isFullscreen To False (Windowed Mode)
			x=window.x;									
			y=window.y;
		}
		else															// Otherwise (If Fullscreen Mode Was Successful)
		{
			ShowCursor (FALSE);											// Turn Off The Cursor
			windowStyle = WS_POPUP ;										// Set The WindowStyle To WS_POPUP (Popup Window)
			windowExtendedStyle |= WS_EX_TOPMOST;						// Set The Extended Window Style To WS_EX_TOPMOST
			x=0;									
			y=0;													// (Top Window Covering Everything Else)
		}



	}
	else																// If Fullscreen Was Not Selected
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create The OpenGL Window
	window.hWnd = CreateWindowEx (windowExtendedStyle,					// Extended Style
								   window.init.application->className,	// Class Name
								   window.init.title,					// Window Title
								   WS_CLIPSIBLINGS | WS_CLIPCHILDREN | windowStyle,							// Window Style
								   window.x, window.y,								// Window X,Y Position
								   windowRect.right - windowRect.left,	// Window Width
								   windowRect.bottom - windowRect.top,	// Window Height
								   HWND_DESKTOP,						// Desktop Is Window's Parent
								   0,									// No Menu
								   window.init.application->hInstance, // Pass The Window Instance
								   &window);

	if (window.hWnd == 0)												// Was Window Creation A Success?
	{
		return FALSE;													// If Not Return False
	}

	window.hDC = GetDC (window.hWnd);									// Grab A Device Context For This Window
	if (window.hDC == 0)												// Did We Get A Device Context?
	{
		// Failed
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	PixelFormat = ChoosePixelFormat (window.hDC, &pfd);				// Find A Compatible Pixel Format
	if (PixelFormat == 0)												// Did We Find A Compatible Format?
	{
		// Failed
		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	if (SetPixelFormat (window.hDC, PixelFormat, &pfd) == FALSE)		// Try To Set The Pixel Format
	{
		// Failed
		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	
	int attribList[8] =
	{
	WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	 WGL_CONTEXT_MINOR_VERSION_ARB, 0,
	 WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
	  0, 0
	};

	HGLRC tempContext = wglCreateContext(window.hDC);
	wglMakeCurrent(window.hDC,tempContext);

	const char* version = (const char*)glGetString(GL_VERSION);

	if (version[0] == '2')
	{
		app.data.legacyMode = true;
		app.data.FORCE_OPENGL_VERSION = false;
	}
		
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

	if ((wglCreateContextAttribsARB == NULL) || (!app.data.FORCE_OPENGL_VERSION)) //OpenGL 3.0 is not supported or not wanted
	{
		if ((app.data.DISPLAY_ERRORS) && (app.data.FORCE_OPENGL_VERSION)) MessageBox(NULL, L"Cannot get Proc Adress for CreateContextAttribs.\n\nThe cause for this error is usually when your graphics drivers does not support at least openGL 3.0 \nIf they do then, try rebooting, that might fix it\n\nDefaulting to a regular decive context even though this couls cause problems", L"ERROR", MB_OK);

		if (!(window.hRC = wglCreateContext(window.hDC)))												// Did We Get A normal Rendering Context?
		{
			// Failed
			wglDeleteContext(tempContext);

			ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
			window.hDC = 0;												// Zero The Device Context
			DestroyWindow (window.hWnd);									// Destroy The Window
			window.hWnd = 0;												// Zero The Window Handle
			return FALSE;													// Return False
		}
	
		wglDeleteContext(tempContext);
		
	}
	
	else if (!(window.hRC=wglCreateContextAttribsARB(window.hDC,0, attribList)))												// Did We Get A openGL 3.0+ Rendering Context?
	{
		// Failed
		wglDeleteContext(tempContext);

		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	wglDeleteContext(tempContext);



	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent (window.hDC, window.hRC) == FALSE)
	{
		// Failed
		wglDeleteContext (window.hRC);									// Delete The Rendering Context
		window.hRC = 0;												// Zero The Rendering Context
		ReleaseDC (window.hWnd, window.hDC);							// Release Our Device Context
		window.hDC = 0;												// Zero The Device Context
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	ShowWindow (window.hWnd, SW_SHOW);								// Make The Window Visible
	window.isVisible = TRUE;											// Set isVisible To True

	ReshapeGL (window.init.width, window.init.height);				// Reshape Our GL Window

	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");

	if (wglSwapIntervalEXT != NULL)
	{
		if (app.data.ENABLE_VSYNC) wglSwapIntervalEXT(1);
		else wglSwapIntervalEXT(0);
	}
	
	
	window.lastTickCount = GetTickCount ();							// Get Tick Count

	app.data.hWnd = window.hWnd;
	app.data.hDC = window.hDC;
	app.data.hRC = window.hRC;

	return TRUE;														// Window Creating Was A Success
																		// Initialization Will Be Done In WM_CREATE
}

BOOL DestroyWindowGL ()								// Destroy The OpenGL Window & Release Resources
{
	if (window.hWnd != 0)												// Does The Window Have A Handle?
	{	
		if (window.hDC != 0)											// Does The Window Have A Device Context?
		{
			wglMakeCurrent (window.hDC, 0);							// Set The Current Active Rendering Context To Zero
			if (window.hRC != 0)										// Does The Window Have A Rendering Context?
			{
				wglDeleteContext (window.hRC);							// Release The Rendering Context
				window.hRC = 0;										// Zero The Rendering Context

			}
			ReleaseDC (window.hWnd, window.hDC);						// Release The Device Context
			window.hDC = 0;											// Zero The Device Context
		}
		DestroyWindow (window.hWnd);									// Destroy The Window
		window.hWnd = 0;												// Zero The Window Handle
	}

	if (window.init.isFullScreen)										// Is Window In Fullscreen Mode
	{
		ChangeDisplaySettings (NULL,0);									// Switch Back To Desktop Resolution
		ShowCursor (TRUE);												// Show The Cursor
	}	
	return TRUE;														// Return True
}

// Process Window Message Callbacks
LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GLZ::glzAppinitialization app;
	GLZ::glzInput input;
	
	// Get The Window Context
	//GL_Window* window = (GL_Window*)(GetWindowLong (hWnd, GWL_USERDATA));

	switch (uMsg)														// Evaluate Window Message
	{
		case WM_SYSCOMMAND:												// Intercept System Commands
		{
			switch (wParam)												// Check System Calls
			{
				case SC_SCREENSAVE:										// Screensaver Trying To Start?
				case SC_MONITORPOWER:									// Monitor Trying To Enter Powersave?
				return 0;												// Prevent From Happening
			}
			break;														// Exit
		}
		return 0;														// Return

		case WM_CREATE:													// Window Creation
		{
			        // Get the creation parameters.
		//	CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// Store Window Structure Pointer
		//	window = (GL_Window*)(creation->lpCreateParams);
		//	SetWindowLong (hWnd, GWL_USERDATA, (LONG)(window));
		}
		return 0;														// Return

		case WM_CLOSE:													// Closing The Window
			TerminateApplication();								// Terminate The Application
		return 0;														// Return

		case WM_SIZE:													// Size Action Has Taken Place
			switch (wParam)												// Evaluate Size Action
			{
				case SIZE_MINIMIZED:									// Was Window Minimized?
					window.isVisible = FALSE;							// Set isVisible To False
				return 0;												// Return

				case SIZE_MAXIMIZED:									// Was Window Maximized?
					window.isVisible = TRUE;							// Set isVisible To True
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;												// Return

				case SIZE_RESTORED:										// Was Window Restored?
					window.isVisible = TRUE;							// Set isVisible To True
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;												// Return
			}
		break;															// Break

		case WM_KEYDOWN:												// Update Keyboard Buffers For Keys Pressed
			if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
			{
				input.addKeyEvent(wParam,true);
				return 0;												// Return
			}
		break;															// Break

		case WM_KEYUP:													// Update Keyboard Buffers For Keys Released
			if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
			{
				input.addKeyEvent(wParam, false);
				return 0;												// Return
			}
		break;															// Break

		case WM_LBUTTONDOWN:
			input.SetMouseL(true);
			break;

		case WM_LBUTTONUP:
			input.SetMouseL(false);
			break;

		case WM_RBUTTONDOWN:
			input.SetMouseR(true);
			break;

		case WM_RBUTTONUP:
			input.SetMouseR(false);

		case WM_MBUTTONDOWN:
			input.SetMouseM(true);
			break;

		case WM_MBUTTONUP:
			input.SetMouseM(false);
			break;

		case WM_MOUSEMOVE:
			input.setMouseX(GET_X_LPARAM(lParam));
			input.setMouseY(GET_Y_LPARAM(lParam));

			break;

		case WM_MOUSEWHEEL:
			input.addMouseWeel(GET_WHEEL_DELTA_WPARAM(wParam));
			//window.keys->Mactive = true;
			break;

		case WM_CAPTURECHANGED:
			//	window.keys->Mactive=false;
			break;


		case WM_TOGGLEFULLSCREEN:										// Toggle FullScreen Mode On/Off
			g_createFullScreen = (g_createFullScreen == TRUE) ? FALSE : TRUE;

			if (g_createFullScreen== TRUE)							
			{
				window.init.width = app.data.FULLSCREEN_WIDTH;						// Window Width
				window.init.height = app.data.FULLSCREEN_HEIGHT;					// Window Height

				ShowCursor (FALSE);										// Turn Off The Cursor
				
				SetWindowLong(hWnd,GWL_EXSTYLE,WS_EX_TOPMOST);			// places the window so it is allways on top
				ChangeScreenResolution (window.init.width, window.init.height, window.init.bitsPerPixel);	// changes the screen resolution to set values
				SetWindowPos(hWnd,HWND_TOPMOST,0,0,window.init.width,window.init.height,SWP_NOZORDER);
				

			}														
			else
			{	
				
				window.init.width = app.data.WINDOW_WIDTH;						// Window Width
				window.init.height = app.data.WINDOW_HEIGHT;					// Window Height
				ShowCursor (TRUE);										// Turn On The Cursor

				SetWindowLong(hWnd,GWL_EXSTYLE,WS_EX_APPWINDOW);		// returns the window back to normalcy
				ChangeDisplaySettings (NULL,0);							// returns to the default screen resolution
				SetWindowPos(hWnd,HWND_TOPMOST,window.x,window.y,window.init.width,window.init.height,SWP_NOZORDER);

				stateManager.Draw();
			}



			
		//	PostMessage (hWnd, WM_QUIT, 0, 0);
		break;															// Break
	}

	return DefWindowProc (hWnd, uMsg, wParam, lParam);					// Pass Unhandled Messages To DefWindowProc
}

BOOL RegisterWindowClass (Application* application)						// Register A Window Class For This Application.
{																		// TRUE If Successful
	// Register A Window Class
	GLZ::glzAppinitialization app;

	WNDCLASSEX windowClass;												// Window Class
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						// Make Sure Memory Is Cleared
	windowClass.cbSize			= sizeof (WNDCLASSEX);					// Size Of The windowClass Structure
	windowClass.style			= CS_HREDRAW | CS_VREDRAW ;//| CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);				// WindowProc Handles Messages
	windowClass.hInstance		= application->hInstance;				// Set The Instance
	windowClass.hbrBackground	= 0;			// Class Background Brush Color
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	windowClass.lpszClassName	= application->className;				// Sets The Applications Classname
	if (RegisterClassEx (&windowClass) == 0)							// Did Registering The Class Fail?
	{
		// NOTE: Failure, Should Never Happen
		if (app.data.DISPLAY_ERRORS) MessageBox(HWND_DESKTOP, L"RegisterClassEx Failed!", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													// Return False (Failure)
	}
	return TRUE;														// Return True (Success)
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

	GLZ::glzAppinitialization app;
	GLZ::glzInput input;
	
	preInitialize();
	//	app.pull();
	//since the app data structure has allready been created, we need to do a pull operation to update the data in the structure to have up to date data in it
	app.pull();	

	Application			application;									// Application Structure
	//GL_Window			window;											// Window Structure
	BOOL				isMessagePumpActive;							// Message Pump Active?
	MSG					msg;											// Window Message Structure
	DWORD				tickCount;										// Used For The Tick Counter

	dtTemp=0;

	// Fill Out Application Data
	application.className = L"OpenGL";									// Application Class Name
	application.hInstance = hInstance;									// Application Instance


	// Fill Out Window
	ZeroMemory (&window, sizeof (GL_Window));							// Make Sure Memory Is Zeroed
//	window.keys					= &keys;								// Window Key Structure
	window.init.application		= &application;							// Window Application
	window.init.title = app.data.WINDOW_TITLE;							// Window Title
	window.init.width = app.data.FULLSCREEN_WIDTH;						// Window Width
	window.init.height = app.data.FULLSCREEN_HEIGHT;					// Window Height
	window.init.bitsPerPixel = 32;									// Bits Per Pixel
	window.init.isFullScreen = true;								// Fullscreen? (Set To TRUE)
	window.x = 0;									// x position of window
	window.y = 0;									// y position of window

		
	DEVMODE devMode = {0};
	devMode.dmSize = sizeof(devMode);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);			// this bit of code gets the current display settings

	if (app.data.NATIVE_FULLSCREEN) // normally better than to force a fixed resolution, but do make sure your app likes running att different resolutions
	{
		window.init.width = (int)devMode.dmPelsWidth;					// Window Width
		window.init.height = (int)devMode.dmPelsHeight;					// Window Height

	}

	if (app.data.START_WINDOWED) // runs if i want to start in windowed mode
	{
		window.init.isFullScreen = false;
		window.x = app.data.WINDOW_X;												// x position of window
		window.y = app.data.WINDOW_Y;												// y position of window	
		window.init.width = app.data.WINDOW_WIDTH;						// Window Width
		window.init.height = app.data.WINDOW_HEIGHT;					// Window Height

		if (app.data.START_CENTERED)  // this is a pretty good choice for windowed games
		{	

			window.x = (devMode.dmPelsWidth / 2) - (app.data.WINDOW_WIDTH / 2);									// x position of window
			window.y = (devMode.dmPelsHeight / 2) - (app.data.WINDOW_HEIGHT / 2);									// y position of window

		}		
	}


	// Register A Class For Our Window To Use
	if (RegisterWindowClass (&application) == FALSE)					// Did Registering A Class Fail?
	{
		// Failure
		if (app.data.DISPLAY_ERRORS) MessageBox(HWND_DESKTOP, L"Error Registering Window Class!", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														// Terminate Application
	}

	g_isProgramLooping = TRUE;											// Program Looping Is Set To TRUE
	g_createFullScreen = window.init.isFullScreen;						// g_createFullScreen Is Set To User Default
	while (g_isProgramLooping)											// Loop Until WM_QUIT Is Received
	{
		// Create A Window
		window.init.isFullScreen = g_createFullScreen;					// Set Init Param Of Window Creation To Fullscreen?
		if (CreateWindowGL() == TRUE)							// Was Window Creation Successful?
		{
			// At This Point We Should Have A Window That Is Setup To Render OpenGL
			if(!stateManager.Initialize(window.init.width, window.init.height))					// Call User Intialization
			{
				// Failure
				TerminateApplication();							// Close Window, This Will Handle The Shutdown
			}
			else														// Otherwise (Start The Message Pump)
			{	// Initialize was a success
				stateManager.DisplayUpdate(window.init.width, window.init.height);
				isMessagePumpActive = TRUE;								// Set isMessagePumpActive To TRUE
				while (isMessagePumpActive == TRUE)						// While The Message Pump Is Active
				{
					// Success Creating Window.  Check For Window Messages
					if(PeekMessage(&msg, window.hWnd, 0, 0, PM_REMOVE) != 0)
					{
						// Check For WM_QUIT Message
						if (msg.message != WM_QUIT)						// Is The Message A WM_QUIT Message?
						{
							DispatchMessage (&msg);						// If Not, Dispatch The Message
						}
						else											// Otherwise (If Message Is WM_QUIT)
						{
							isMessagePumpActive = FALSE;				// Terminate The Message Pump
						}
					}
					else												// If There Are No Messages
					{
						if(window.isVisible == FALSE)					// If Window Is Not Visible
						{
							WaitMessage();								// Application Is Minimized Wait For A Message
						}
						else											// If Window Is Visible
						{
							// Process Application Loop		
							// all of this needs restructuring to alow for a fast input rate

							// timing functions
							tickCount = GetTickCount ();				// Get The Tick Count
							window.deltaTime = ((float)(tickCount - window.lastTickCount)) / 1000;	// Update The _Delta time
							window.lastTickCount = tickCount;			// Set Last Count To Current Count

							input.updateKeys(window.deltaTime);
						
							stateManager.Update(window.deltaTime);			// Update The Counter

							if(stateManager.pollMessageQuit())
								TerminateApplication();

							if(stateManager.pollMessageFullscreen())
								ToggleFullscreen();

							stateManager.Draw();							// Draw Our Scene
							SwapBuffers(window.hDC);			// Swap Buffers (Double Buffering)
								
								

						}
					}
				}														// Loop While isMessagePumpActive == TRUE
			}															// If (Initialize (...

			// Application Is Finished
			stateManager.Deinitialize();											// User Defined DeInitialization
			
			DestroyWindowGL();									// Destroy The Active Window
		}
		else															// If Window Creation Failed
		{
			// Error Creating Window
			if (app.data.DISPLAY_ERRORS) MessageBox(HWND_DESKTOP, L"Error Creating OpenGL Window, try rebboring or fixing your drivers", L"Error", MB_OK | MB_ICONEXCLAMATION);
			g_isProgramLooping = FALSE;									// Terminate The Loop
		}
	}																	// While (isProgramLooping)

	UnregisterClass (application.className, application.hInstance);		// UnRegister Window Class
	return 0;
}																		// End Of WinMain()
