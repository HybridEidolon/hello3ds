#pragma once

#include <3ds.h>

#include "Color.hpp"

class FFramebuffer
{
public:
	FFramebuffer (u8 * buffer, u16 w, u16 h) : m_buffer(buffer), m_width(w), m_height(h) {};
	FFramebuffer (gfxScreen_t source);
	virtual ~FFramebuffer () {};

	gfxScreen_t GetSource () const { return m_source; }
	u16 GetWidth () const { return m_width; }
	u16 GetHeight () const { return m_height; }
	u8 * GetBuffer ();
	u8 * GetPositionInBuffer (u16 x, u16 y);

	void PutPixel (u16 x, u16 y, FColor color);

	void Clear ();
	void Swap ();

private:
	u8 * m_buffer;
	u16 m_width;
	u16 m_height;
	gfxScreen_t m_source;
	u8 m_current = 0;
};

// Grab framebuffers in pre-main
extern FFramebuffer * GFramebufferTopLeft;
extern FFramebuffer * GFramebufferBottom;

void GFlushFramebuffers ();
void GSwapFramebuffers ();
