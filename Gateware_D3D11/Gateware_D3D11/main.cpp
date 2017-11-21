// Sample of Gateware Middleware utilizing D3D11, designed as a potential template project for Project & Portfolio IV students.
// Gateware is a very powerful set of multi-platfrom Middleware libraries, however we are Still in BETA so expect a few bugs here & there.
// Author: Lari H Norri, Full Sail University [Template Version 0.1 Alpha 11/20/2017]
// FYI, I wrote this thing in roughly an hour & a half.(including fixing some bugs) Gateware is pretty slick once you learn how to use it.
// Special Thanks: Andre Reid, Shou-Yi Chang, Daniel Lima and the rest of the Gateware team!

#include <iostream>
// Main Gateware Interface for window creation, check out the included docs on how to use Gateware to the fullest!
#include "Gateware Redistributable R4/Interface/G_System/GWindow.h"
// Quick & Dirty library link, better to use project properties to support debug & release + 64 & 32 bit builds.
#pragma comment(lib, "Gateware Redistributable R4/Archive/Win32/Gateware_i386/Debug/GWindow.lib")
// Gateware also has full implicit/explicit DLL support(recommended) & Direct Source code support if desired. 

// Include Our DX11 Class for drawing
#include "LetsDrawSomeStuff.h" // 90% of Gateware is cross-platform, the stuff in this file is not just FYI. (see GOpenGLSurface instead)

// Gateware namespaces here
using namespace GW;
using namespace CORE;
using namespace SYSTEM;

// used to end the program
bool RunApplication = true;

// forward declare event responder
class MyGatewareEventProcessor;
// Used only to keep main() at the top for easy reading
MyGatewareEventProcessor* AllocListener();
void FreeListener(MyGatewareEventProcessor** freeMe);

// Main entry point
int main()
{
	// Handle to our new window, be sure to DecrementCount to free memory
	GWindow* myWindow = nullptr;
	// Used to listen to our new window
	MyGatewareEventProcessor *myListener = AllocListener();
	// Our class that contains all D3D11 related Code
	LetsDrawSomeStuff* myDrawSystem = nullptr;
	// Create new window
	if (G_SUCCESS(CreateGWindow(100, 100, 1024, 768, GWindowStyle::WINDOWEDBORDERED, &myWindow)))
	{
		// Register our custom event lister to find out what is going on with the window
		myWindow->RegisterListener(reinterpret_cast<GListener*>(myListener), 0); // listen for any event
		// Show the window
		if (G_SUCCESS(myWindow->OpenWindow()))
		{
			// Create our Draw system now that we have a window that is open.
			// KNOWN ISSUE: The GDirectX11Surface cannot be attatched to a closed window.
			myDrawSystem = new LetsDrawSomeStuff(myWindow);
			// main loop
			while (RunApplication)
			{
				// KNOWN ISSUE: The GDirectX11Surface must not be accessed after a DESTROY window event is processed.
				// Before events are processed lets do some drawing
				myDrawSystem->Render();

				// process platform specific window events (for now do this last to be safe)
				if (G_FAIL(myWindow->ProcessWindowEvents()))
					break; // nope...
			}
		}
		// Free drawing system and all internal elements
		delete myDrawSystem; 
		myDrawSystem = nullptr;
		// Unregister our custom event listner
		myWindow->DeregisterListener(reinterpret_cast<GListener*>(myListener));
		// free window
		myWindow->DecrementCount();
		// delete listener (never do this for true gateware interfaces, use "DecrementCount" instead)
		FreeListener(&myListener);
	}
    return 0;
}

// Used to process any Gateware Interface events
class MyGatewareEventProcessor : public GListener
{
public:
	// Override required GInterface functions & ignore since this class is not meant to be used by anyone else
	GReturn GetCount(unsigned int &outCount) { return FAILURE;  }
	GReturn IncrementCount() { return FAILURE;  }
	GReturn DecrementCount() { return FAILURE;  }
	GReturn RequestInterface(const GUUIID &ID, void** outInterface) { return FAILURE; }

	// Override The function that actaully matters for listening
	GReturn OnEvent(const GUUIID& senderID, unsigned int eventID, void* eventData, unsigned int eventDataSize)
	{
		// process any GWindow Events
		if (senderID == GWindowUUIID)
		{
			// play it safe
			if (eventDataSize == sizeof(GWINDOW_EVENT_DATA))
			{
				// Copy event
				GWINDOW_EVENT_DATA eventInfo = *reinterpret_cast<GWINDOW_EVENT_DATA*>(eventData);
				// Process specific event
				switch (eventID)
				{
					// stop the main loop
				case GWindowInputEvents::DESTROY:
					RunApplication = false; 
					break;
					// TODO: React to other GWindow events here
				}
			}
		};
		// TODO: Listen for other Gateware Broadcasting Intefaces here

		return SUCCESS; // hopefully we did what we needed to
	}
};

// Delay creation until we know what the above class is (not truly needed just keeps main up top)
MyGatewareEventProcessor* AllocListener() { return new MyGatewareEventProcessor(); }
void FreeListener(MyGatewareEventProcessor** freeMe) { delete *freeMe; *freeMe = nullptr; }