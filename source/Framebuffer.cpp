#include "Framebuffer.hpp"

#include <3ds.h>
#include <string.h>

FFramebuffer * GFramebufferTopLeft;
FFramebuffer * GFramebufferBottom;

FFramebuffer::FFramebuffer (gfxScreen_t source)
{
	m_current = 0;
	switch (source)
	{
	case GFX_TOP:
		m_source = GFX_TOP;
		m_width  = 400;
		m_height = 240;
		m_buffer = gfxTopLeftFramebuffers[m_current];
		break;
	case GFX_BOTTOM:
		m_source = GFX_BOTTOM;
		m_width  = 320;
		m_height = 240;
		m_buffer = gfxBottomFramebuffers[m_current];
		break;
	default:
		// error?
		m_source = GFX_TOP;
		m_width  = 400;
		m_height = 240;
		m_buffer = gfxTopLeftFramebuffers[m_current];
		break;
	}
}

void FFramebuffer::Clear ()
{
	memset(GetBuffer(), 0, m_width*m_height*3);
}

void FFramebuffer::PutPixel (u16 x, u16 y, FColor color)
{
	if (x >= m_width)
	{
		return;
	}
	if (y >= m_height)
	{
		return;
	}
	if (y + x * m_height >= m_width*m_height)
	{
		x = 1;
		y = 1;
	}

	//y = (m_height - 1) - y; // flip
	m_buffer = GetBuffer ();
	m_buffer[3 * (y+x*m_height) + 0] = color.r;
	m_buffer[3 * (y+x*m_height) + 1] = color.g;
	m_buffer[3 * (y+x*m_height) + 2] = color.b;
}


u8 * FFramebuffer::GetPositionInBuffer (u16 x, u16 y)
{
	m_buffer = GetBuffer ();
	return &m_buffer[3 * (y + x*m_height)];
}

u8 * FFramebuffer::GetBuffer ()
{
	return gfxGetFramebuffer (m_source, GFX_LEFT, &m_height, &m_width);
}

void GFlushFramebuffers ()
{
	gfxFlushBuffers ();
}

void GSwapFramebuffers ()
{
	gfxSwapBuffers ();
}
