/*-
 * Copyright (c) 2015 Chris Spiegel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <cstring>
#include <memory>

#include <QString>
#include <QtGlobal>

#include <qmmp/decoder.h>

#include "decoder.h"

XMPDecoder::XMPDecoder(const QString &path)
        : Decoder(),
          path(path)
{
}

bool XMPDecoder::initialize()
{
  try
  {
    xmp = std::unique_ptr<XMPWrap>(new XMPWrap(path.toUtf8().constData(), settings.get_panning_amplitude()));
  }
  catch(XMPWrap::InvalidFile)
  {
    return false;
  }

  configure(xmp->rate(), xmp->channels(), Qmmp::PCM_S16LE);

  return true;
}

qint64 XMPDecoder::totalTime()
{
  return xmp->duration();
}

int XMPDecoder::bitrate()
{
  return xmp->channel_count();
}

qint64 XMPDecoder::read(unsigned char *audio, qint64 max_size)
{
  qint64 copied;

  xmp->set_interpolator(settings.get_interpolator());
  xmp->set_stereo_separation(settings.get_stereo_separation());

  copied = copy(audio, max_size);
  audio += copied;
  max_size -= copied;

  if(buf_filled == 0)
  {
    XMPWrap::Frame frame = xmp->play_frame();
    if(frame.n == 0)
    {
      return copied;
    }

    bufptr = reinterpret_cast<unsigned char *>(frame.buf);
    buf_filled += frame.n;
  }

  return copied + copy(audio, max_size);
}

qint64 XMPDecoder::copy(unsigned char *audio, qint64 max_size)
{
  qint64 to_copy;

  to_copy = qMin(buf_filled, max_size);

  /* It's safe to copy 0 bytes, but both pointers must still have valid
   * values, and bufptr will be invalid on the first call.
   */
  if(to_copy != 0)
  {
    std::memcpy(audio, bufptr, to_copy);
  }

  bufptr += to_copy;
  buf_filled -= to_copy;

  return to_copy;
}

void XMPDecoder::seek(qint64 pos)
{
  xmp->seek(pos);
}
