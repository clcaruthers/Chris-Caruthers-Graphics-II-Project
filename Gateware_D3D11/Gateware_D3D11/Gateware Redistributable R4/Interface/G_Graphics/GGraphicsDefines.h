#ifndef GGRAPHICSDEFINES_H
#define GGRAPHICSDEFINES_H

/*!
File: GGraphicsDefines.h
Purpose: A Gateware interface contains useful resources for GGraphics libraries.
Author: Andre Reid
Contributors: N/A
Last Modified: 11/17/2017
Interface Status: Beta
Copyright: 7thGate Software LLC.
License: MIT
*/

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all Gateware Graphics library interfaces must belong.
	namespace GRAPHICS
	{
		//! GGraphicsInitOptions holds the special options that can be requested when initializing a GGraphics surface.
		enum GGraphicsInitOptions
		{
			COLOR_10_BIT			= 0x01,
			DEPTH_BUFFER_SUPPORT	= 0x02,
			DEPTH_STENCIL_SUPPORT	= 0X04,
			OPENGL_ES_SUPPORT		= 0x08
		};
	}
}

#endif