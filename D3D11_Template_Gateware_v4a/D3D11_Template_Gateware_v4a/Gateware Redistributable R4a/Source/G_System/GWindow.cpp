// Override export symbols for DLL builds (must be included before interface code).
#include "../../Source/DLL_Export_Symbols.h"
#include "../../Interface/G_System/GWindow.h"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include "unistd.h"
#elif __APPLE__

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#endif
#endif


#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <string.h>
#include "GUtility.h"


using namespace GW;
using namespace CORE;
using namespace SYSTEM;

//namespace
//{
// GWindow global variables.

//! Map of Listeners to send event information to.
//std::map<GListener *, unsigned long long> listeners;
//int testInt;
//}

#include "../../Source/G_System/GWindow_Callback.hpp"

class AppWindow : public GWindow
{
private:

#ifdef _WIN32
	//std::atomic<HWND> wndHandle;
	HWND wndHandle;
#elif __linux__
	Display * display;
	Window window;

	std::thread* linuxLoop = nullptr;
	Atom prop_type;
	Atom prop_hidden;
	Atom prop_hMax;
	Atom prop_vMax;
	Atom prop_remove;
	Atom prop_hints;
	Atom prop_iconic;
	Atom prop_active;

	typedef struct
	{
		unsigned long flags;
		unsigned long functions;
		unsigned long decorations;
		long inputMode;
		unsigned long status;

	} Hints;

	Hints hint;

#elif __APPLE__
	NSWindow * window;
	NSAutoreleasePool* pool;
	NSThread* nsMacLoop;
#endif

	std::atomic<unsigned int> refCount;

	std::atomic<int> xPos;
	std::atomic<int> yPos;
	std::atomic<int> width;
	std::atomic<int> height;

	GWindowStyle style;

	std::mutex refMutex;

public:

	AppWindow();
	virtual ~AppWindow();

	GReturn OpenWindow();

	GReturn ProcessWindowEvents();

	GReturn ReconfigureWindow(int _x, int _y, int _width, int _height, GWindowStyle _style);

	GReturn InitWindow(int _x, int _y, int _width, int _height, GWindowStyle _style);

	GReturn MoveWindow(int _x, int _y);

	GReturn ResizeWindow(int _width, int _height);

	GReturn Maximize();

	GReturn Minimize();

	GReturn ChangeWindowStyle(GWindowStyle _style);

	GReturn GetCount(unsigned int& _outCount);

	GReturn IncrementCount();

	GReturn DecrementCount();

	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);

	GReturn RegisterListener(GListener* _addListener, unsigned long long _eventMask);

	GReturn DeregisterListener(GListener* _removeListener);

	GReturn	GetWidth(unsigned int& _outWidth);

	GReturn	GetHeight(unsigned int& _outHeight);

	GReturn	GetClientWidth(unsigned int& _outClientWidth);

	GReturn	GetClientHeight(unsigned int& _outClientHeight);

	GReturn	GetX(unsigned int& _outX);

	GReturn	GetY(unsigned int& _outY);


	GReturn GetClientTopLeft(unsigned int &_outX, unsigned int &_outY);

	GReturn GetWindowHandle(unsigned int _handleSize, void** _outWindowHandle);

	GReturn IsFullscreen(bool& _outIsFullscreen);
};

AppWindow::AppWindow() : refCount(1), xPos(0), yPos(0), width(0), height(0), style(FULLSCREENBORDERED)
{
#ifdef _WIN32
	ZeroMemory(&wndHandle, sizeof(HWND));
#elif __linux__
	display = nullptr;
	window = { 0 };
#elif __APPLE__
	window = nil;
#endif // __WIN32
}

AppWindow::~AppWindow()
{

}

GReturn AppWindow::OpenWindow()
{
#ifdef _WIN32
	if (wndHandle)
		return REDUNDANT_OPERATION;

	WNDCLASSEX winClass;
	ZeroMemory(&winClass, sizeof(WNDCLASSEX));

	LPCWSTR appName = L"GWindow Application";

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.hCursor = LoadCursorW(NULL, IDC_CROSS);
	winClass.hIcon = LoadIconW(0, IDI_EXCLAMATION);
	winClass.lpfnWndProc = GWindowProc;
	winClass.lpszClassName = appName;
	winClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClass.hInstance = GetModuleHandleW(0);

	if (!RegisterClassExW(&winClass))
	{
		printf("RegisterClassExW Error : %d \n", GetLastError());
	}

	DWORD windowsStyle;

	if (style == WINDOWEDBORDERED || style == FULLSCREENBORDERED)
	{
		windowsStyle = WS_OVERLAPPEDWINDOW;
	}

	else if (style == WINDOWEDBORDERLESS || style == FULLSCREENBORDERLESS)
	{
		windowsStyle = WS_POPUP;
	}

	else if (style == MINIMIZED)
	{
		windowsStyle = WS_MINIMIZE;
	}
	//Create the window
	wndHandle = CreateWindowW(appName, L"Win32Window", windowsStyle, xPos, yPos,
		width, height, NULL, NULL, GetModuleHandleW(0), 0);

	if (wndHandle && style != MINIMIZED)
	{
		if (ShowWindow(wndHandle, SW_SHOWDEFAULT) != 0)
		{
			printf("ShowWindow Error Message : %d \n", GetLastError());
			return FAILURE;
		}
		else
		{
			return SUCCESS;
		}

	}

	else if (wndHandle && style == MINIMIZED)
	{
		ShowWindow(wndHandle, SW_MINIMIZE);
		return SUCCESS;
	}
	else
		return FAILURE;

#elif __linux__
	XInitThreads();
	if (window)
		return REDUNDANT_OPERATION;

	XSetWindowAttributes attributes;
	unsigned long valueMask = 0;
	XSizeHints rect;
	display = XOpenDisplay(NULL);
	int screen = DefaultScreen(display);
	int depth = DefaultDepth(display, screen);
	int x, y;
	unsigned int wid, heig, borderHeight, depthRet;

	attributes.background_pixel = XWhitePixel(display, 0);
	attributes.border_pixel = XBlackPixel(display, 0);
	attributes.event_mask = SubstructureNotifyMask | PropertyChangeMask | ExposureMask;

	//XVisualInfo vinfo;
	//XMatchVisualInfo(display, screen, depth, InputOutput, &vinfo);
	//Colormap cmap = XCreateColormap(display, RootWindow(display, screen), CopyFromParent, AllocNone);

	//attributes.colormap = cmap;
	valueMask |= CWBackPixel;
	valueMask |= CWEventMask;
	//valueMask |= CWColormap;

	// set rect hints
	rect.flags = PSize | PPosition;

	x = xPos;
	y = yPos;
	wid = width;
	heig = height;

	window = XCreateWindow(display, XRootWindow(display, screen), x, y, wid, heig, 5,
		depth, InputOutput, CopyFromParent, valueMask, &attributes);

	if (!window)
		return FAILURE;

	XStoreName(display, window, "BasicWindowApp");


	if (style == WINDOWEDBORDERLESS || style == FULLSCREENBORDERLESS)
	{
		hint.flags = 2;
		hint.decorations = 0;

		XChangeProperty(display, window, prop_hints, prop_hints, 32, PropModeReplace, (unsigned char *)&hint, 5);
	}

	XSetWMNormalHints(display, window, &rect);

	if (XMapWindow(display, window))
	{
		XSync(display, 0);
		prop_type = XInternAtom(display, "_NET_WM_STATE", False);
		prop_hidden = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
		prop_hMax = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		prop_vMax = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
		prop_remove = XInternAtom(display, "_NET_WM_STATE_REMOVE", False);
		prop_hints = XInternAtom(display, "_MOTIF_WM_HINTS", False);
		prop_iconic = XInternAtom(display, "_NET_WM_STATE_HIDDEN", False);
		prop_active = XInternAtom(display, "_NET_ACTIVE_WINDOW", True);

		linuxLoop = new std::thread(LinuxWndProc, display, window);

		linuxLoop->detach();

		return SUCCESS;
	}

	else
		return FAILURE;

#elif __APPLE__

	if (window)
		return REDUNDANT_OPERATION;

	dispatch_sync(dispatch_get_main_queue(), ^{
		pool = [[NSAutoreleasePool alloc] init];

	[NSApplication sharedApplication];

	[NSThread detachNewThreadSelector : @selector(doNothing : ) toTarget:[GWDelegate class] withObject : nil];

	[NSApp setActivationPolicy : NSApplicationActivationPolicyRegular];

	[NSApp setDelegate : appDel];

	NSUInteger windowStyleMask = 0;

	if (style == FULLSCREENBORDERED || style == WINDOWEDBORDERED || style == MINIMIZED)
		windowStyleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;

	NSRect windowRect = NSMakeRect(xPos, yPos, width, height);

	window = [[NSWindow alloc] initWithContentRect:windowRect
		styleMask : windowStyleMask
		backing : NSBackingStoreBuffered
		defer : NO];

	[window setTitle : @"SampleCocoaWindow"];

	//[window autorelease];

	[window setCollectionBehavior : NSWindowCollectionBehaviorFullScreenPrimary];

	[responder setNextResponder : window.nextResponder];
	[window setNextResponder : responder];
	[window makeFirstResponder : window.contentView];
	[window.contentView setNextResponder : responder];

	[window setDelegate : delegate];

	//[window canBecomeMainWindow:YES];

	//[window makeMainWindow];

	[window makeKeyAndOrderFront : nil];

	FlushMacEventLoop();


	if (style == FULLSCREENBORDERED || style == FULLSCREENBORDERLESS)
	{
		[window toggleFullScreen : nil];
		FlushMacEventLoop();
	}
	} );

	if ([window isVisible])
		return SUCCESS;
	else
		return FAILURE;

#endif

}

GReturn AppWindow::ProcessWindowEvents()
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;
#elif __linux__
	if (!display)
		return FAILURE;
#elif __APPLE__
	if (!window)
		return FAILURE;
#endif

#ifdef _WIN32
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message)
			{
				//translate messages
				TranslateMessage(&msg);
				//Send to WindowProc
				DispatchMessage(&msg);
			}
		}
		else
			return SUCCESS;
	}
	return SUCCESS;


#elif __linux__
	XFlush(display);
	XSync(display, 0);

#elif __APPLE__
	dispatch_sync(dispatch_get_main_queue(), ^{

		FlushMacEventLoop();
	});
#endif
}
GReturn AppWindow::ReconfigureWindow(int _x, int _y, int _width, int _height, GWindowStyle _style)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;
#elif __linux__
	if (!display)
		return FAILURE;
#elif __APPLE__
	if (!window)
		return FAILURE;
#endif

	GWindowStyle previousStyle = style;

	GReturn Gret = InitWindow(_x, _y, _width, _height, _style);
	if (Gret != SUCCESS)
		return Gret;

	switch (style)
	{
	case WINDOWEDBORDERED:
	{
#ifdef _WIN32
		SetWindowLongPtr(wndHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		BOOL winRet = SetWindowPos(wndHandle, nullptr, xPos, yPos, width, height, SWP_SHOWWINDOW);
		if (winRet == 0)
		{
			printf("SetWindowPos Error : %d \n", GetLastError());
			return FAILURE;
		}

		if (previousStyle == MINIMIZED)
			ShowWindow(wndHandle, SW_RESTORE);
		else
			ShowWindow(wndHandle, SW_SHOW);

		return SUCCESS;
#elif __linux__
		int x = xPos, y = yPos;
		unsigned int w = width, h = height;

		XClientMessageEvent ev;
		memset(&ev, 0, sizeof ev);
		ev.type = ClientMessage;
		ev.window = window;
		ev.message_type = prop_active;
		ev.format = 32;
		ev.data.l[0] = 1;
		ev.data.l[1] = CurrentTime;
		ev.data.l[2] = ev.data.l[3] = ev.data.l[4] = 0;
		int stat = XSendEvent(display, RootWindow(display, XDefaultScreen(display)), False,
			SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&ev);
		sleep(1);
		XFlush(display);

		if (!stat)
			return FAILURE;

		XSync(display, 0);

		hint.flags = 2;
		hint.decorations = 5;

		if (!XMoveResizeWindow(display, window, x, y, w, h))
			return FAILURE;

		if (!XChangeProperty(display, window, prop_hints, prop_hints, 32, PropModeReplace, (unsigned char *)&hint, 5))
			return FAILURE;

		XEvent unMaxEvent;
		unMaxEvent.type = ClientMessage;
		unMaxEvent.xclient.window = window;
		unMaxEvent.xclient.message_type = prop_type;
		unMaxEvent.xclient.format = 32;
		unMaxEvent.xclient.data.l[0] = 0;
		unMaxEvent.xclient.data.l[1] = prop_hMax;
		unMaxEvent.xclient.data.l[2] = prop_vMax;
		unMaxEvent.xclient.data.l[3] = 0;
		unMaxEvent.xclient.data.l[4] = 0;

		stat = XSendEvent(display, DefaultRootWindow(display), False,
			SubstructureNotifyMask | SubstructureRedirectMask, &unMaxEvent);
		if (!stat)
			return FAILURE;

		sleep(1);
		XSync(display, 0);

		return SUCCESS;
#elif __APPLE__
		if ([window isMiniaturized])
			[window deminiaturize : nil];

		NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;
		NSRect rect = NSMakeRect(xPos, yPos, width, height);

		[window setStyleMask : styleMask];
		[window setFrame : rect display : YES];

		dispatch_sync(dispatch_get_main_queue(), ^{

			FlushMacEventLoop();
		});

		if (window)
		{
			return SUCCESS;
		}

		return FAILURE;
#endif // __linux__
	}
	break;

	case WINDOWEDBORDERLESS:
	{
#ifdef _WIN32
		SetWindowLongPtr(wndHandle, GWL_STYLE, WS_POPUP);
		BOOL winRet = SetWindowPos(wndHandle, nullptr, xPos, yPos, width, height, SWP_SHOWWINDOW);
		if (winRet == 0)
		{
			printf("SetWindowPos Error : %d \n", GetLastError());
			return FAILURE;
		}


		if (previousStyle == MINIMIZED)
			ShowWindow(wndHandle, SW_RESTORE);
		else
			ShowWindow(wndHandle, SW_SHOW);

		return SUCCESS;

#elif __linux__
		int x = xPos, y = yPos;
		unsigned int w = width, h = height;

		XClientMessageEvent ev;
		memset(&ev, 0, sizeof ev);
		ev.type = ClientMessage;
		ev.window = window;
		ev.message_type = prop_active;
		ev.format = 32;
		ev.data.l[0] = 1;
		ev.data.l[1] = CurrentTime;
		ev.data.l[2] = ev.data.l[3] = ev.data.l[4] = 0;
		int stat = XSendEvent(display, RootWindow(display, XDefaultScreen(display)), False,
			SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&ev);
		sleep(1);
		XFlush(display);

		if (!stat)
			return FAILURE;

		hint.flags = 2;
		hint.decorations = 0;

		if (!XChangeProperty(display, window, prop_hints, prop_hints, 32, PropModeReplace, (unsigned char *)&hint, 5))
			return FAILURE;

		if (!XMoveResizeWindow(display, window, x, y, w, h))
			return FAILURE;

		XEvent unMaxEvent;
		unMaxEvent.type = ClientMessage;
		unMaxEvent.xclient.window = window;
		unMaxEvent.xclient.message_type = prop_type;
		unMaxEvent.xclient.format = 32;
		unMaxEvent.xclient.data.l[0] = 0;
		unMaxEvent.xclient.data.l[1] = prop_hMax;
		unMaxEvent.xclient.data.l[2] = prop_vMax;
		unMaxEvent.xclient.data.l[3] = 0;
		unMaxEvent.xclient.data.l[4] = 0;

		stat = XSendEvent(display, window, False,
			SubstructureNotifyMask | SubstructureRedirectMask, &unMaxEvent);
		if (!stat)
			return FAILURE;

		sleep(1);
		XSync(display, 0);

		return SUCCESS;

#elif __APPLE__
		if ([window isMiniaturized])
			[window deminiaturize : nil];

		NSUInteger styleMask = NSWindowStyleMaskBorderless;
		NSRect rect = NSMakeRect(xPos, yPos, width, height);

		[window setStyleMask : styleMask];
		[window setFrame : rect display : YES];

		[window setStyleMask : styleMask];

		dispatch_sync(dispatch_get_main_queue(), ^{
			FlushMacEventLoop();
		});

		if (window)
		{
			return SUCCESS;
		}

		return FAILURE;
#endif // __linux__

	}
	break;

	case FULLSCREENBORDERED:
	{
#ifdef _WIN32
		RECT windowRect;
		GetWindowRect(wndHandle, &windowRect);

		SetWindowLongPtr(wndHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		BOOL winRet = SetWindowPos(wndHandle, nullptr, windowRect.left, windowRect.top, windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top, SWP_NOREDRAW);
		if (winRet == 0)
			return FAILURE;


		if (previousStyle == MINIMIZED)
			ShowWindow(wndHandle, SW_RESTORE);
		else
			ShowWindow(wndHandle, SW_MAXIMIZE);

		return SUCCESS;

#elif __linux__

		XClientMessageEvent ev;
		memset(&ev, 0, sizeof ev);
		ev.type = ClientMessage;
		ev.window = window;
		ev.message_type = prop_active;
		ev.format = 32;
		ev.data.l[0] = 1;
		ev.data.l[1] = CurrentTime;
		ev.data.l[2] = ev.data.l[3] = ev.data.l[4] = 0;
		int stat = XSendEvent(display, RootWindow(display, XDefaultScreen(display)), False,
			SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&ev);
		sleep(1);
		XFlush(display);

		if (!stat)
			return FAILURE;

		hint.flags = 2;
		hint.decorations = 5;

		XChangeProperty(display, window, prop_hints, prop_hints, 32, PropModeReplace, (unsigned char *)&hint, 5);

		if (!XMoveResizeWindow(display, window, 0, 0, 1920, 1080))
			return FAILURE;

		XSync(display, 0);

		return SUCCESS;

#elif __APPLE__
		if ([window isMiniaturized])
			[window deminiaturize : nil];

		NSUInteger styleMask = NSWindowStyleMaskFullScreen;
		NSRect rect = NSMakeRect(xPos, yPos, width, height);

		[window setStyleMask : styleMask];
		//[window setFrame:rect display:YES];

		bool fullscreen;
		IsFullscreen(fullscreen);
		if (fullscreen == false)
			[window toggleFullScreen : nil];

		dispatch_sync(dispatch_get_main_queue(), ^{
			FlushMacEventLoop();
		});

		if (window)
		{
			return SUCCESS;
		}

		return FAILURE;
#endif // __linux__
	}
	break;

	case FULLSCREENBORDERLESS:
	{
#ifdef _WIN32
		RECT windowRect;
		GetWindowRect(wndHandle, &windowRect);

		SetWindowLongPtr(wndHandle, GWL_STYLE, WS_POPUP);
		BOOL winRet = SetWindowPos(wndHandle, nullptr, windowRect.left, windowRect.top, windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top, SWP_NOREDRAW);
		if (winRet == 0)
			return FAILURE;

		if (previousStyle == MINIMIZED)
			ShowWindow(wndHandle, SW_RESTORE);
		else
			ShowWindow(wndHandle, SW_MAXIMIZE);

		return SUCCESS;
#elif __linux__

		XClientMessageEvent ev;
		memset(&ev, 0, sizeof ev);
		ev.type = ClientMessage;
		ev.window = window;
		ev.message_type = prop_active;
		ev.format = 32;
		ev.data.l[0] = 1;
		ev.data.l[1] = CurrentTime;
		ev.data.l[2] = ev.data.l[3] = ev.data.l[4] = 0;
		int stat = XSendEvent(display, RootWindow(display, XDefaultScreen(display)), False,
			SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&ev);
		sleep(1);
		XFlush(display);

		if (!stat)
			return FAILURE;

		hint.flags = 2;
		hint.decorations = 0;

		XChangeProperty(display, window, prop_hints, prop_hints, 32, PropModeReplace, (unsigned char *)&hint, 5);

		if (!XMoveResizeWindow(display, window, 0, 0, 1920, 1080))
			return FAILURE;

		XSync(display, 0);
		return SUCCESS;

#elif __APPLE__
		if ([window isMiniaturized])
			[window deminiaturize : nil];

		NSUInteger styleMask = NSWindowStyleMaskFullScreen;
		NSRect rect = NSMakeRect(xPos, yPos, width, height);

		[window setStyleMask : styleMask];
		[window setFrame : rect display : YES];

		bool fullscreen;
		IsFullscreen(fullscreen);
		if (fullscreen == false)
			[window toggleFullScreen : nil];

		dispatch_sync(dispatch_get_main_queue(), ^{
			FlushMacEventLoop();
		});

		if (window)
		{
			return SUCCESS;
		}

		return FAILURE;

#endif // __linux__
	}
	break;

	case MINIMIZED:
	{
#ifdef _WIN32
		ShowWindow(wndHandle, SW_MINIMIZE);

		RECT windowRect = { xPos, yPos, width, height };
		GetWindowRect(wndHandle, &windowRect);

		return SUCCESS;
#elif __linux__

		if (!XIconifyWindow(display, window, DefaultScreen(display)))
			return FAILURE;

		else
		{
			XFlush(display);
			return SUCCESS;
		}

#elif __APPLE__
		bool fullscreen;
		IsFullscreen(fullscreen);
		if (fullscreen == true)
		{
			ReconfigureWindow(_x, _y, _width, _height, WINDOWEDBORDERED);
		}
		dispatch_sync(dispatch_get_main_queue(), ^{
			FlushMacEventLoop();
		});

		if (![window isMiniaturized])
			[window miniaturize : nil];

		dispatch_sync(dispatch_get_main_queue(), ^{
			FlushMacEventLoop();
		});

		if ([window isMiniaturized])
		{
			return SUCCESS;
		}

		return FAILURE;
#endif

	}
	break;
	}

	return SUCCESS;
}

GReturn AppWindow::InitWindow(int _x, int _y, int _width, int _height, GWindowStyle _style)
{
	if (_style < 0 || _style > 4)
		return INVALID_ARGUMENT;

	else
	{
#ifdef __APPLE__

#endif
		width = _width;
		height = _height;
		xPos = _x;
		yPos = _y;
		style = _style;
	}

	//display = nullptr;
	return SUCCESS;
}

GReturn AppWindow::MoveWindow(int _x, int _y)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;
#elif __linux__
	if (!display)
		return FAILURE;
#elif __APPLE__
	if (!window)
		return FAILURE;
#endif

	GReturn Gret = InitWindow(_x, _y, width, height, style);
	if (Gret != SUCCESS)
		return Gret;

#ifdef _WIN32
	BOOL Winret = SetWindowPos(wndHandle, nullptr, xPos, yPos, width, height, SWP_SHOWWINDOW);
	if (Winret == 0)
		return FAILURE;
	else
		return SUCCESS;
#elif __linux__
	if (XMoveWindow(display, window, xPos, yPos))
	{
		XSync(display, 0);
		return SUCCESS;
	}
	else
		return FAILURE;

#elif __APPLE__
	NSRect rect = window.frame;
	CGPoint newPos;
	newPos.y = yPos - height;
	newPos.x = xPos;

	dispatch_sync(dispatch_get_main_queue(), ^{
		[window setFrame : rect display : YES animate : YES];
	});

	return SUCCESS;
#endif

	return FAILURE;
}

GReturn AppWindow::ResizeWindow(int _width, int _height)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;
#elif __linux__
	if (!display)
		return FAILURE;
#elif __APPLE__
	if (!window)
		return FAILURE;
#endif

	GReturn Gret = InitWindow(xPos, yPos, _width, _height, style);
	if (Gret != SUCCESS)
		return Gret;

#ifdef _WIN32
	BOOL Winret = SetWindowPos(wndHandle, nullptr, xPos, yPos, width, height, SWP_SHOWWINDOW);
	if (Winret == 0)
		return FAILURE;
	else
		return SUCCESS;
#elif __linux__
	if (XResizeWindow(display, window, width, height))
	{
		XSync(display, 0);
		return SUCCESS;
	}
	else
		return FAILURE;

#elif __APPLE__
	//NSRect rect = window.frame;
    NSRect rect = NSMakeRect(xPos, yPos, _width, _height);
	CGSize newSize;
	newSize.height = height;
	newSize.width = width;

	dispatch_sync(dispatch_get_main_queue(), ^{
		[window setFrame : rect display : YES animate : YES];
	});

	return SUCCESS;
#endif
	return FAILURE;
}

GReturn AppWindow::Maximize()
{
	if (style == WINDOWEDBORDERED || style == MINIMIZED)
	{
		return ChangeWindowStyle(FULLSCREENBORDERED);
	}

	else if (style == WINDOWEDBORDERLESS)
		return ChangeWindowStyle(FULLSCREENBORDERLESS);
	else
		return REDUNDANT_OPERATION;
}

GReturn AppWindow::Minimize()
{
	return ChangeWindowStyle(MINIMIZED);
}

GReturn AppWindow::ChangeWindowStyle(GWindowStyle _style)
{
	return ReconfigureWindow(xPos, yPos, width, height, _style);
}

GReturn AppWindow::GetCount(unsigned int& _outCount)
{
	_outCount = refCount;
	return SUCCESS;
}

GReturn AppWindow::IncrementCount()
{
	//Check for possible overflow.
	if (refCount == G_UINT_MAX)
		return FAILURE;

	++refCount;

	return SUCCESS;
}

GReturn AppWindow::DecrementCount()
{
	//Check to make sure underflow will not occur.
	if (refCount == 0)
	{
		return FAILURE;
	}

	--refCount;

	if (refCount == 0)
	{
		//Release handles to any listeners that remain (releases handles)
		refMutex.lock();
		std::map<GListener*, unsigned long long>::iterator iter = listeners.begin();
		for (; iter != listeners.end(); ++iter)
			iter->first->DecrementCount(); // free handle, don't call Deregister as that would be bad
		listeners.clear(); // dump all invalid pointers
		refMutex.unlock();

#ifdef __linux__
		delete linuxLoop;
#endif // __linux__
		delete this;
	}

	return SUCCESS;
}

GReturn AppWindow::RequestInterface(const GUUIID& _interfaceID, void** _outputInterface)
{
	if (_outputInterface == nullptr)
		return INVALID_ARGUMENT;

	if (_interfaceID == GWindowUUIID)
	{
		GWindow* convert = reinterpret_cast<GWindow*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GBroadcastingUUIID)
	{
		GBroadcasting* convert = reinterpret_cast<GBroadcasting*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GMultiThreadedUUIID)
	{
		GMultiThreaded* convert = reinterpret_cast<GMultiThreaded*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GInterfaceUUIID)
	{
		GInterface* convert = reinterpret_cast<GInterface*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else
		return INTERFACE_UNSUPPORTED;

	return SUCCESS;
}

#ifdef __APPLE__
void FlushMacEventLoop()
{
	while (TRUE)
	{
		if ([NSThread isMainThread])
		{
			NSEvent * event = [NSApp nextEventMatchingMask : NSEventMaskAny
				untilDate : [NSDate distantPast]
				inMode : NSDefaultRunLoopMode
				dequeue : YES];

			if (event == nil)
				break;

			[NSApp sendEvent : event];
			[NSApp updateWindows];
		}
	}
}
#endif

GReturn AppWindow::RegisterListener(GListener* _addListener, unsigned long long _eventMask)
{
	if (_addListener == nullptr) {
		return INVALID_ARGUMENT;
	}

	refMutex.lock();

	std::map<GListener*, unsigned long long>::const_iterator iter = listeners.find(_addListener);
	if (iter != listeners.end()) {
		return REDUNDANT_OPERATION;
	}

	listeners[_addListener] = _eventMask;
	_addListener->IncrementCount();

	refMutex.unlock();

	return SUCCESS;
}

GReturn AppWindow::DeregisterListener(GListener* _removeListener)
{
	if (_removeListener == nullptr) {
		return INVALID_ARGUMENT;
	}

	refMutex.lock();

	std::map<GListener*, unsigned long long>::const_iterator iter = listeners.find(_removeListener);
	if (iter != listeners.end()) {
		iter->first->DecrementCount();
		listeners.erase(iter);
		refMutex.unlock();
	}
	else {
		refMutex.unlock();
		return FAILURE;
	}

	return SUCCESS;
}

GReturn AppWindow::GetWidth(unsigned int& _outWidth)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;

	RECT windowRect;
	GetWindowRect(wndHandle, &windowRect);

	_outWidth = windowRect.right - windowRect.left;
	if (_outWidth == 0)
	{
		return INVALID_ARGUMENT;
	}
#elif __linux__
	if (!display)
		return FAILURE;
	Window root;
	int x, y;
	unsigned int w, h, bord, depth;

	XSync(display, 0);

	if (!XGetGeometry(display, window, &root, &x, &y, &w, &h, &bord, &depth))
		return FAILURE;

	_outWidth = w;

#elif __APPLE__
	if (!window)
		return FAILURE;
	NSRect rect = window.frame;
	_outWidth = rect.size.width;
#endif
	return SUCCESS;
}

GReturn AppWindow::GetHeight(unsigned int& _outHeight)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;

	RECT windowRect;
	GetWindowRect(wndHandle, &windowRect);

	_outHeight = windowRect.bottom - windowRect.top;
#elif __linux__
	if (!display)
		return FAILURE;
	Window root;
	int x, y;
	unsigned int w, h, bord, depth;

	XSync(display, 0);

	if (!XGetGeometry(display, window, &root, &x, &y, &w, &h, &bord, &depth))
		return FAILURE;

	_outHeight = h + bord;

#elif __APPLE__
	if (!window)
		return FAILURE;
	NSRect rect = window.frame;
	_outHeight = rect.size.height;


#endif
	return SUCCESS;
}

GReturn AppWindow::GetClientWidth(unsigned int& _outClientWidth)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;
	RECT width;
	GetClientRect(wndHandle, &width);
	_outClientWidth = width.right - width.left;
	return SUCCESS;

#elif __linux__
	if (!display)
		return FAILURE;

	Window root;
	int x, y;
	unsigned int w, h, bord, depth;

	XSync(display, 0);

	if (XGetGeometry(display, window, &root, &x, &y, &w, &h, &bord, &depth))
	{
		_outClientWidth = w;
		return SUCCESS;
	}
	else
		return FAILURE;

#elif __APPLE__
	if (!window)
		return FAILURE;

	NSRect rect = window.frame;
	NSRect contentRect = [window contentRectForFrameRect : rect];
	_outClientWidth = contentRect.size.width;
	return SUCCESS;

#endif
}

GReturn AppWindow::GetClientHeight(unsigned int& _outClientHeight)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;
	RECT height;
	GetClientRect(wndHandle, &height);
	_outClientHeight = height.bottom - height.top;

	return SUCCESS;

#elif __linux__
	if (!display)
		return FAILURE;

	Window root;
	int x, y;
	unsigned int w, h, bord, depth;

	XSync(display, 0);

	if (XGetGeometry(display, window, &root, &x, &y, &w, &h, &bord, &depth))
	{
		_outClientHeight = h;
		return SUCCESS;
	}
	else
		return FAILURE;

#elif __APPLE__
	if (!window)
		return FAILURE;

	NSRect rect = window.frame;
	NSRect contentRect = [window contentRectForFrameRect : rect];
	_outClientHeight = contentRect.size.height;
	return SUCCESS;

#endif
}

GReturn AppWindow::GetX(unsigned int& _outX)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;

	RECT windowRect;
	GetWindowRect(wndHandle, &windowRect);

	_outX = windowRect.left;
#elif __linux__
	if (!display)
		return FAILURE;

	Window root;
	int x, y;
	unsigned int w, h, bord, depth;

	XSync(display, 0);

	if (!XGetGeometry(display, window, &root, &x, &y, &w, &h, &bord, &depth))
		return FAILURE;

	_outX = x;

#elif __APPLE__
	if (!window)
		return FAILURE;
	NSRect rect = window.frame;
	_outX = rect.origin.x;

#endif
	return SUCCESS;
}

GReturn AppWindow::GetY(unsigned int& _outY)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;

	RECT windowRect;
	GetWindowRect(wndHandle, &windowRect);

	_outY = windowRect.top;
#elif __linux__
	if (!display)
		return FAILURE;

	Window root;
	int x, y;
	unsigned int w, h, bord, depth;

	XSync(display, 0);

	if (!XGetGeometry(display, window, &root, &x, &y, &w, &h, &bord, &depth))
		return FAILURE;

	_outY = y;

#elif __APPLE__
	if (!window)
		return FAILURE;
	NSRect rect = window.frame;
	_outY = rect.origin.y + rect.size.height;

#endif
	return SUCCESS;
}



GReturn AppWindow::GetClientTopLeft(unsigned int &_outX, unsigned int &_outY)
{
#ifdef _WIN32
	if (!wndHandle)
		return FAILURE;

	RECT topLeft;
	GetClientRect(wndHandle, &topLeft);
	_outX = topLeft.left;
	_outY = topLeft.top;

#elif __linux__
	if (!display)
		return FAILURE;

	Window root;
	int x, y;
	unsigned int w, h, bord, depth;

	XSync(display, 0);

	if (!XGetGeometry(display, window, &root, &x, &y, &w, &h, &bord, &depth))
		return FAILURE;

	_outX = x;
	_outY = y + bord;

#elif __APPLE__
	if (!window)
		return FAILURE;

    NSRect rect = window.frame;
    NSRect contentRect = [window contentRectForFrameRect:rect];

    _outX = contentRect.origin.x;
    _outY = contentRect.origin.y;

#endif
	return SUCCESS;
}

GReturn AppWindow::GetWindowHandle(unsigned int _handleSize, void** _outWindowHandle)
{
#ifdef _WIN32
	if (!wndHandle)
	{
		return FAILURE;
	}
	if (_outWindowHandle == nullptr || _handleSize != sizeof(HWND))
	{
		return INVALID_ARGUMENT;
	}
	memcpy_s(_outWindowHandle, _handleSize, &wndHandle, _handleSize);

	return SUCCESS;
#elif __linux__
	if (!display)
	{
		return FAILURE;
	}
	if (_outWindowHandle == nullptr || _handleSize != sizeof(LINUX_WINDOW))
	{
		return INVALID_ARGUMENT;
	}
	LINUX_WINDOW linuxWnd;
	linuxWnd.window = (void*)&window;
	linuxWnd.display = (void*)display;

	memcpy(_outWindowHandle, &linuxWnd, _handleSize);
	return SUCCESS;
#elif __APPLE__
	if (!(__bridge void*)window)
	{
		return FAILURE;
	}
	if (_outWindowHandle == nullptr || _handleSize != sizeof(NSWindow*))
	{
		return INVALID_ARGUMENT;
	}

	*_outWindowHandle = window;

	return SUCCESS;
#endif
}

GReturn AppWindow::IsFullscreen(bool& _outIsFullscreen)
{
	unsigned int height = 0;
	unsigned int width = 0;
	unsigned int xMax = 0;
	unsigned int yMax = 0;
	int borderHeight = 0;
	int resizeBarHeight = 0;
#ifdef _WIN32
	xMax = GetSystemMetrics(SM_CXSCREEN);
	yMax = GetSystemMetrics(SM_CYSCREEN);
	borderHeight = GetSystemMetrics(SM_CYCAPTION);
	resizeBarHeight = GetSystemMetrics(SM_CYBORDER);
	GetWidth(width);
	GetHeight(height);
#elif __linux__
	Screen* scr = DefaultScreenOfDisplay(display);
	xMax = scr->width;
	yMax = scr->height;
	//borderHeight = scr->mheight;
	Atom actual_type_return, actual_type_return2;
	int actual_format_return;
	unsigned long nitems_return;
	unsigned long   bytes_after_return;
	unsigned char * prop_return = NULL;

	int result = XGetWindowProperty(display, window, prop_type, 0L,
		sizeof(Atom),
		False,
		AnyPropertyType,
		&actual_type_return,
		&actual_format_return,
		&nitems_return, &bytes_after_return, &prop_return);

	Atom vProp = ((Atom *)prop_return)[0];
	Atom hProp = ((Atom *)prop_return)[1];

    if(result != Success)
    {
        char temp[1024];
        XGetErrorText(display, result, temp, 1024);
        return FAILURE;
    }
	if (hProp == prop_hMax && vProp == prop_vMax)
		_outIsFullscreen = true;
	else
		_outIsFullscreen = false;

	return SUCCESS;

#elif __APPLE__
	if (([window styleMask] & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen)
		_outIsFullscreen = TRUE;
	else
		_outIsFullscreen = FALSE;

	return SUCCESS;

#endif

	if (width >= xMax && (height + borderHeight + resizeBarHeight) >= yMax)
		_outIsFullscreen = true;
	else
		_outIsFullscreen = false;

	return SUCCESS;
}


GATEWARE_EXPORT_EXPLICIT GReturn CreateGWindow(int _x, int _y, int _width, int _height, GWindowStyle _style, GWindow** _outWindow)
{
	return GW::SYSTEM::CreateGWindow(_x, _y, _width, _height, _style, _outWindow);
}

GReturn GW::SYSTEM::CreateGWindow(int _x, int _y, int _width, int _height, GWindowStyle _style, GWindow** _outWindow)
{
	if (_outWindow == nullptr)
		return INVALID_ARGUMENT;

	AppWindow* Window = new AppWindow();

	if (Window == nullptr)
		return FAILURE;

	GReturn Gret = Window->InitWindow(_x, _y, _width, _height, _style);

	if (Gret == INVALID_ARGUMENT)
		return Gret;

	*_outWindow = Window;

	return SUCCESS;
}
