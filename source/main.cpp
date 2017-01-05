#include <3ds.h>

#include "Framebuffer.hpp"
#include "PixelBuffer.hpp"
#include "bmploader.hpp"

static FPixelBuffer * _GeneratePixelBuffer ()
{
	FPixelBuffer * ret = new FPixelBuffer (32, 32, EPixelBufferFormat::RGB888);
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 32; x++)
		{
			ret->Put (x, y, FColor (x * 8, y * 8, 0));
		}
	}
	return ret;
}

int main (int argc, char ** argv)
{
	u16 offsetX = 0;
	u16 offsetY = 0;
	bool waitingForVblank = true;
	FPixelBuffer * buf;
	FPixelBuffer *buf2;
	// Initialize services
	srvInit();
	aptInit();
	gfxInit(
		gfxGetScreenFormat(GFX_TOP),
		gfxGetScreenFormat(GFX_BOTTOM),
	true);
	hidInit();
	fsInit();
	sdmcInit();
	//gfxSet3D(true); // uncomment if using stereoscopic 3D

	// Initialize global framebuffer references
	GFramebufferTopLeft = new FFramebuffer (GFX_TOP);
	GFramebufferBottom  = new FFramebuffer (GFX_BOTTOM);

	buf = _GeneratePixelBuffer ();
	buf2 = LoadBMP ("sdmc:/3ds/hello3ds/hi.bmp");

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Your code goes here

		u32 kDown = hidKeysDown ();
		u32 kHeld = hidKeysHeld ();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		if (kDown & KEY_A)
			waitingForVblank = !waitingForVblank;
		if (kHeld & KEY_DOWN)
			offsetY-=2;
		if (kHeld & KEY_UP)
			offsetY+=2;
		if (kHeld & KEY_LEFT)
			offsetX-=2;
		if (kHeld & KEY_RIGHT)
			offsetX+=2;

		GFramebufferTopLeft->Clear ();
		GFramebufferBottom->Clear ();

		if (buf2)
		{
			buf2->Blit (0, 0, GFramebufferTopLeft);
		}

		buf->Blit (offsetX+40, offsetY, GFramebufferTopLeft);


		// Flush and swap framebuffers
		GFlushFramebuffers ();
		GSwapFramebuffers ();
		if (waitingForVblank) { gspWaitForVBlank();}
	}
	GFlushFramebuffers ();
	GSwapFramebuffers ();
	if (waitingForVblank) { gspWaitForVBlank();}

	delete buf;
	if (buf2 != nullptr) delete buf2;

	// Exit services
	sdmcExit();
	fsExit();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
	return 0;
}
