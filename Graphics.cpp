
#include "Graphics.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


//imgui
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl3.h"
using namespace glm;

Graphics::Graphics()
{
	// don't really want to do anything here but could move init here
}
Graphics::~Graphics()
{

}


void Graphics::esInitContext(Target_State* p_state)
{
	if (p_state != NULL)
	{
		memset(p_state, 0, sizeof(Target_State));
	}
}

bool Graphics::GetDisplay()
{
	/*
	 * X11 native display initialization
	 */

	x_display = XOpenDisplay(NULL);
	if (x_display == NULL)
	{
		printf("Sorry to say we can't open an XDisplay and this will fail\n");
		return false;
	}
	else
		printf("we got an XDisplay\n");

	root = DefaultRootWindow(x_display);
	return true;
}

/*
Set up the EGL and Set up OpenGL states needed
Also set up the X11 window for rendering on a standard X11 system (like Rpi4/5, not like Rpi4)
*/

void Graphics::init_ogl(Target_State* state, int width, int height, int FBResX, int FBResY)

{

	XSetWindowAttributes swa;
	XSetWindowAttributes  xattr;
	Atom wm_state;
	XWMHints hints;
	//XEvent xev;

#define ES_WINDOW_RGB           0
	state->width = width;
	state->height = height;

	EGLint numConfigs;
	EGLint majorVersion = 0;
	EGLint minorVersion = 0;

	root = DefaultRootWindow(x_display);

	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
	swa.background_pixmap = None;
	swa.background_pixel = 0;
	swa.border_pixel = 0;
	swa.override_redirect = false;

	win = XCreateWindow(
		x_display,
		root,
		0,		// puts it at the top left of the screen
		0,
		state->width,	//set size  
		state->height,
		0,
		CopyFromParent,
		InputOutput,
		CopyFromParent,
		CWEventMask,
		&swa);

	if (win == 0)
	{
		
		("Sorry to say we can't create a window and this will fail\n");
		exit(0);       // we need to trap this;
	}
	else
		printf("we got an (Native) XWindow\n");

	state->nativewindow = (EGLNativeWindowType)win;

	//	XSelectInput(x_display, win, KeyPressMask | KeyReleaseMask);
	XSelectInput(x_display, root, KeyPressMask);

	xattr.override_redirect = FALSE;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

	hints.input = TRUE;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);


	char* title = (char*)"x11 Demo";
	// make the window visible on the screen
	XMapWindow(x_display, win);
	XStoreName(x_display, win, title);

	// get identifiers for the provided atom name strings
	wm_state = XInternAtom(x_display, "_NET_WM_STATE", FALSE);

	/*memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = FALSE;
	XSendEvent(
		x_display,
		DefaultRootWindow(x_display),
		FALSE,
		SubstructureNotifyMask,
		&xev);*/
		// end of xdisplay

		// Get EGLDisplay	
	egldisplay = eglGetDisplay((EGLNativeDisplayType)x_display);       //eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (egldisplay == EGL_NO_DISPLAY)
	{
		printf("Sorry to say we have an GetDisplay error and this will fail");
		exit(0);          // we need to trap this;
	}
	else  	printf("we got an EGLDisplay\n");

	// Initialize EGL
	if (!eglInitialize(egldisplay, &majorVersion, &minorVersion))
	{
		printf("Sorry to say we have an EGLinit error and this will fail");
		EGLint err = eglGetError();    // should be getting error values that make sense now
		exit(err);            // we need to trap this;
	}
	else 	printf("we initialised EGL\n");


	// Get configs
	if (!eglGetConfigs(egldisplay, NULL, 0, &numConfigs))
	{
		printf("Sorry to say we have EGL config errors and this will fail");
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}
	else 	printf("we got %i Configs\n", numConfigs);


	// Choose config
	if (!eglChooseConfig(egldisplay, attribute_list, &config, 1, &numConfigs))
	{
		printf("Sorry to say we have config choice issues, and this will fail");
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}
	else	printf("we chose our config\n");




	// Create a GL context

#ifdef GLES3
	context = eglCreateContext(egldisplay, config, NULL, GiveMeGLES3);
#else
	context = eglCreateContext(egldisplay, config, EGL_NO_CONTEXT, GiveMeGLES2);
#endif
	if (context == EGL_NO_CONTEXT)
	{
		EGLint err = eglGetError();
		exit(err);             // we need to trap this;
	}
	else	printf("Created a context ok\n");

	// Create a surface
	surface = eglCreateWindowSurface(egldisplay, config, state->nativewindow, NULL);     // this fails with a segmentation error?

	if (surface == EGL_NO_SURFACE)
	{
		EGLint err = eglGetError();
		exit(err);               // we need to trap this;
	}
	else 	printf("we got a Surface\n");


	// Make the context current
	if (!eglMakeCurrent(egldisplay, surface, surface, context))
	{
		EGLint err = eglGetError();
		exit(err);            // we need to trap this;
	}

	state->display = egldisplay;
	state->surface = surface;
	state->context = context;

	eglSwapInterval(egldisplay, 0);        // 1 to lock speed to 60fps (assuming we are able to maintain it), 0 for immediate swap (may cause tearing) which will indicate actual frame rate
	// on xu4 this seems to have no effect

	// Some OpenGLES2.0 states that we might need
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(TRUE);
	glDepthRangef(0.0f, 1.0f);
	glClearDepthf(1.0f);


	//these are the options you can have for the depth, play with them?
	//#define GL_NEVER                          0x0200
	//#define GL_LESS                           0x0201
	//#define GL_EQUAL                          0x0202
	//#define GL_LEQUAL                         0x0203
	//#define GL_GREATER                        0x0204
	//#define GL_NOTEQUAL                       0x0205
	//#define GL_GEQUAL                         0x0206

	int t; // a dum,ing variable to extra some useful data
#ifdef DEBUG
	printf("This SBC supports version %i.%i of EGL\n", majorVersion, minorVersion);
	printf("This GPU supplied by  :%s\n", glGetString(GL_VENDOR));
	printf("This GPU supports     :%s\n", glGetString(GL_VERSION));
	printf("This GPU Renders with :%s\n", glGetString(GL_RENDERER));
	printf("This GPU supports     :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &t);
	printf("This GPU MaxTexSize is    :%i\n", t);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &t);
	printf("This GPU supports %i Texture units\n", t); // pi4 16


	printf("This GPU supports these extensions	:%s\n", glGetString(GL_EXTENSIONS));
#endif
	glViewport(0, 0, state->width, state->height);
	glClearColor(0.529, 0.8078, 0.9215f, 1.0f);
	//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glCullFace(GL_BACK);

	if (glGetError() == GL_NO_ERROR)	return;
	else
		printf("Oh bugger, Some part of the EGL/OGL graphic init failed\n");
}


/*
 Now we have be able to create a shader object, pass the shader source
 and them compile the shader. Take note... this is called load shader but it actually works from a memory buffer
 but... hint hint, nothing stopping you loading that memory buffer with a text file?
*/


///
// Create a simple width x height texture image with RGBA format
//
GLuint Graphics::CreateSimpleTexture2D(int width, int height, char* TheData)
{
	// Texture object handle
	GLuint textureId;

	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generate a texture object
	glGenTextures(1, &textureId);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Load the texture

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		TheData);
	//glGetError is a very primative way to check for GL errors, though in this case its quite spefific	
	if (glGetError() != GL_NO_ERROR) printf("Oh bugger");

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	if (glGetError() == GL_NO_ERROR)	return textureId;
	printf("Oh bugger");

	return textureId;
}


std::vector<int>* Graphics::UpdateKeyboardInput() {
	XEvent ev;
	KeySym key = 0;
	if (XPending(x_display) > 0) {
		XNextEvent(x_display, &ev);

		if (ev.type == KeyPress) {
			keys.push_back((int)XLookupKeysym(&ev.xkey, 0));
		}
		if (ev.type == KeyRelease) {
			for (int i = 0; i < keys.size(); i++) {
				if (keys[i] == (int)XLookupKeysym(&ev.xkey, 0)) {
					keys.erase(keys.begin() + i);
				}
			}
		}
	}
	keys.push_back((int)key);

	return &keys;
}
void Graphics::updateMousePos(float DT, float screenWidth, float screenHeight) {
	Window window_returned; // we don't really care too much about this as we should only have 1 window

	int root_x;
	int root_y;
	int win_x;
	int win_y;
	uint mask_return;
	XQueryPointer(
		x_display,
		win,
		&window_returned,
		&window_returned,

		&root_x,
		&root_y,
		&win_x,
		&win_y,
		&mask_return);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(screenWidth, screenHeight);
	io.DeltaTime = DT;
	// we should also pass on mouse and key info into the IO here

	io.MousePos = ImVec2(win_x, win_y);
	io.MouseDown[0] = mask_return & (1 << 8); // left
	io.MouseDown[1] = mask_return & (1 << 10); // right
}



