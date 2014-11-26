#pragma once

#include <3ds.h>

#include "Color.hpp"
#include "Framebuffer.hpp"

enum class EPixelBufferFormat : u8
{
	RGB888 = 0,
	// Native framebuffer format, for fast blit
	BGR888 = 1
};

class FPixelBuffer
{
public:
	FPixelBuffer (u16 w, u16 h, EPixelBufferFormat format);
	virtual ~FPixelBuffer ();

	void Put (u16 x, u16 y, FColor color);
	
	FColor Get (u16 x, u16 y) const;

	u8 * GetBuffer () { return m_buffer; }
	u8 * GetPositionInBuffer (u16 x, u16 y);

	u16 GetWidth () const { return m_width; }
	u16 GetHeight () const { return m_height; }

	void Blit (s16 x, s16 y, FFramebuffer * fb);

	void Flip ();
private:
	u16 m_width;
	u16 m_height;
	EPixelBufferFormat m_fmt;
	u8 * m_buffer;
};
