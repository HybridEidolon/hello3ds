#include "bmploader.hpp"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <3ds.h>
#include <stdlib.h>

typedef struct __attribute__ ((__packed__))
{
	u16 type; // 2
	u32 size; // 4
	u16 reserved1; // 2
	u16 reserved2; // 2
	u32 offset; // 4
} BitmapHeader;

typedef struct __attribute__ ((__packed__))
{
	u32 headerSize;
	s32 width;
	s32 height;
	u16 planes;
	u16 bpp;
	u32 compressionMethod;
	u32 size;
	s32 resX;
	s32 resY;
	u32 paletteSize;
	u32 importantColors;
} DIBHeader;

typedef struct __attribute__ ((__packed__))
{
	u32 headerSize; //4
	u16 width; // 6
	u16 height; //8
	u16 planes;//10
	u16 bpp;//12
} BasicDIBHeader;

FPixelBuffer * LoadBMP (const char * path)
{
	FPixelBuffer * ret = nullptr;
	BitmapHeader header;
	DIBHeader info;
	BasicDIBHeader *basic = (BasicDIBHeader*) &info;
	FILE *file;
	u8 * rbuf;
	u8 padding = 0;

	int bytesRead = 0;

	file = fopen (path, "rb");
	if (!file)
	{
		goto finally;
	}
	bytesRead = fread (&header, 1, 14, file);
	if (bytesRead != 14)
	{
		goto finally;
	}
	//_SwapHeader (&header);
	if (header.type != (u16)0x4D42)
	{
		goto finally;
	}
	bytesRead = fread (&info, 1, 40, file);
	if (bytesRead != 40)
	{
		goto finally;
	}

	// do actual reading
	if (fseek (file, header.offset, SEEK_SET) != 0)
	{
		goto finally;
	}
	u16 usew, useh;
	if (info.headerSize == 40)
	{
		usew = info.width;
		useh = info.height;
	}
	else
	{
		usew = basic->width;
		useh = basic->height;
	}
	ret = new FPixelBuffer (usew, useh, EPixelBufferFormat::RGB888);

	if ((usew * 3) / 4 != 0)
	{
		padding = 4 - ((usew * 3) % 4);
		if (padding == 4) padding = 0;
	}
	rbuf = (u8*) malloc (usew * useh * 3 + (padding * useh));
	fread (rbuf, 1, usew * useh * 3 + (padding * useh), file);

	for (int y = 0; y < useh; y++)
	{
		for (int x = 0; x < usew; x++)
		{
			u8 * c = &rbuf[(x + y * usew) * 3 + (padding * y)];
			ret->Put (x, (useh - y), FColor (c[2], c[1], c[0]));
		}
	}
	free (rbuf);

	ret->Flip ();

	goto finally;

finally:
	fclose (file);
	return ret;
}
