#include "PixelBuffer.hpp"

#include <cstring>

FPixelBuffer::FPixelBuffer (u16 w, u16 h, EPixelBufferFormat format)
		: m_width(w), m_height(h), m_fmt(format)
{
	m_buffer = static_cast<u8 *> (malloc (m_width * m_height * 3));
}

FPixelBuffer::~FPixelBuffer ()
{
	free (m_buffer);
}

void FPixelBuffer::Put (u16 x, u16 y, FColor color)
{
	if (x >= m_width || y >= m_height)
	{
		return;
	}

	// we're going to store the buffer rotated
	// so it's easier to copy into the framebuffer later
	switch (m_fmt)
	{
	case EPixelBufferFormat::RGB888:
		m_buffer[3 * (y+x*m_height) + 0] = color.r;
		m_buffer[3 * (y+x*m_height) + 1] = color.g;
		m_buffer[3 * (y+x*m_height) + 2] = color.b;
	case EPixelBufferFormat::BGR888:
		m_buffer[3 * (y+x*m_height) + 2] = color.r;
		m_buffer[3 * (y+x*m_height) + 1] = color.g;
		m_buffer[3 * (y+x*m_height) + 0] = color.b;
	default:
		break;
	}
}

FColor FPixelBuffer::Get (u16 x, u16 y) const
{
	if (x > m_width || y > m_height)
	{
		return FColor(0, 0, 0);
	}
	switch (m_fmt)
	{
	case EPixelBufferFormat::RGB888:
		return FColor (m_buffer[3 * (y+x*m_height) + 0],
			m_buffer[3 * (y+x*m_height) + 1],
			m_buffer[3 * (y+x*m_height) + 2]);
	case EPixelBufferFormat::BGR888:
		return FColor (m_buffer[3 * (y+x*m_height) + 2],
			m_buffer[3 * (y+x*m_height) + 1],
			m_buffer[3 * (y+x*m_height) + 0]);
	default:
		return FColor (0, 0, 0);
	}
}

u8 * FPixelBuffer::GetPositionInBuffer (u16 x, u16 y)
{
	return &m_buffer[3 * (y+x*m_height)];
}

void FPixelBuffer::Flip ()
{
	u8 * nb = (u8 *) malloc (m_width * m_height * 3);
	u8 * ob = m_buffer;

	for (int ix = 0; ix < m_width; ix++)
	{
		for (int iy = 0; iy < m_height; iy++)
		{
			nb[3 * (iy + ix * m_height) + 0] = ob[3 * ((m_height - iy) + ix * m_height) + 0];
			nb[3 * (iy + ix * m_height) + 1] = ob[3 * ((m_height - iy) + ix * m_height) + 1];
			nb[3 * (iy + ix * m_height) + 2] = ob[3 * ((m_height - iy) + ix * m_height) + 2];
		}
	}

	m_buffer = nb;
	free (ob);
}

void FPixelBuffer::Blit (s16 x, s16 y, FFramebuffer * fb)
{
	//u8 * buf = fb->GetBuffer ();
	u16 bw = fb->GetWidth ();
	u16 bh = fb->GetHeight ();

	if (x > bw || y > bh || x < -(s16)m_width || y < -(s16)m_height)
	{
		return;
	}

	if (m_fmt == EPixelBufferFormat::BGR888)
	{
		// SLOW!!!
		for (s16 xi = 0; xi < m_width; xi++)
		{
			if (x + xi > bw) break;
			if (x + xi < 0)  continue;
			for (s16 yi = 0; yi < m_height; yi++)
			{
				if (y + yi > bw) break;
				if (y + yi < 0) continue;
				fb->PutPixel (x + xi, y + yi, Get (xi, yi));
			}
		}
	}
	else if (m_fmt == EPixelBufferFormat::RGB888)
	{
		// Much faster implementation
		for (s16 xi = 0; xi < m_width; xi++)
		{
			u16 realy = y;
			u16 ymin = 0;
			u16 ymax = m_height;
			if (y + m_height > bh) ymax = bh - y;
			if (y < 0)
			{
				ymin = -y;
				realy = 0;
			}
			if (x + xi > bw) break;
			if (x + xi < 0) continue;

			// copy entire columns/"rows"
			memcpy (fb->GetPositionInBuffer(x + xi, realy), GetPositionInBuffer (xi, ymin), (ymax - ymin) * 3);
		}
	}
}
