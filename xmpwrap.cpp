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

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <xmp.h>

#include <QObject>

#include "xmpwrap.h"

XMPWrap::XMPWrap(std::string filename, int panning_amplitude) : ctx(xmp_create_context())
{
  struct xmp_module_info module_info;

  if(is_valid_panning_amplitude(panning_amplitude))
  {
    xmp_set_player(ctx, XMP_PLAYER_DEFPAN, panning_amplitude);
  }

  if(xmp_load_module(ctx, const_cast<char *>(filename.c_str())) != 0)
  {
    xmp_free_context(ctx);
    throw InvalidFile();
  }

  if(xmp_start_player(ctx, rate(), 0) != 0)
  {
    xmp_release_module(ctx);
    xmp_free_context(ctx);
    throw InvalidFile();
  };

  xmp_get_module_info(ctx, &module_info);
  struct xmp_module *mod = module_info.mod;

  duration_ = module_info.seq_data[0].duration;
  title_ = mod->name;
  format_ = mod->type;
  pattern_count_ = mod->pat;
  track_count_ = mod->trk;
  channel_count_ = mod->chn;
  instrument_count_ = mod->ins;
  sample_count_ = mod->smp;
  initial_speed_ = mod->spd;
  initial_bpm_ = mod->bpm;
  length_ = mod->len;

  for(int i = 0; i < mod->chn; i++)
  {
    struct xmp_channel c = mod->xxc[i];
    if(c.flg != 0)
    {
      if(c.flg & XMP_CHANNEL_SYNTH)
      {
        channel_pan_.push_back('S');
      }
      else if(c.flg & XMP_CHANNEL_MUTE)
      {
        channel_pan_.push_back('-');
      }
      else
      {
        channel_pan_.push_back('?');
      }
    }
    else
    {
      const char hexdigit[] = "0123456789abcdef";
      /* Channel pan is documented as "0x80 is center", but XMP reports
       * only the top 4 bits, so follow its lead.
       */
      channel_pan_.push_back(hexdigit[(c.pan >> 4) & 0x0f]);
    }
  }

  for(int i = 0; i < mod->ins; i++)
  {
    instruments_.push_back(mod->xxi[i].name);
  }

  for(int i = 0; i < mod->smp; i++)
  {
    samples_.push_back(mod->xxs[i].name);
  }

  if(module_info.comment != 0)
  {
    comment_ = module_info.comment;
  }
}

XMPWrap::~XMPWrap()
{
  xmp_end_player(ctx);
  xmp_release_module(ctx);
  xmp_free_context(ctx);
}

bool XMPWrap::can_play(std::string filename)
{
  return xmp_test_module(const_cast<char *>(filename.c_str()), NULL) == 0;
}

std::vector<XMPWrap::Interpolator> XMPWrap::get_interpolators()
{
  std::vector<Interpolator> interpolators = {
    Interpolator(QObject::tr("Nearest Neighbor").toUtf8().constData(), interp_nearest),
    Interpolator(QObject::tr("Linear").toUtf8().constData(), interp_linear),
    Interpolator(QObject::tr("Spline").toUtf8().constData(), interp_spline),
  };

  return interpolators;
}

bool XMPWrap::is_valid_interpolator(int interpolator_value)
{
  std::vector<Interpolator> interpolators = get_interpolators();

  return std::any_of(interpolators.begin(), interpolators.end(),
                     [&interpolator_value]
                     (const Interpolator &interpolator) { return interpolator.value == interpolator_value; });
}

int XMPWrap::default_interpolator()
{
  return interp_spline;
}

void XMPWrap::set_interpolator(int interpolator_value)
{
  if(is_valid_interpolator(interpolator_value))
  {
    xmp_set_player(ctx, XMP_PLAYER_INTERP, interpolator_value);
  }
}

bool XMPWrap::is_valid_stereo_separation(int separation)
{
  return separation >= 0 && separation <= 100;
}

int XMPWrap::default_stereo_separation()
{
  return 70;
}

void XMPWrap::set_stereo_separation(int separation)
{
  if(is_valid_stereo_separation(separation))
  {
    xmp_set_player(ctx, XMP_PLAYER_MIX, separation);
  }
}

bool XMPWrap::is_valid_panning_amplitude(int amplitude)
{
  return amplitude >= 0 && amplitude <= 100;
}

int XMPWrap::default_panning_amplitude()
{
  return 50;
}

XMPWrap::Frame XMPWrap::play_frame()
{
  struct xmp_frame_info fi;

  if(xmp_play_frame(ctx) != 0)
  {
    return Frame(0, 0);
  }
  xmp_get_frame_info(ctx, &fi);
  if(fi.loop_count > 0)
  {
    return Frame(0, 0);
  }

  return Frame(fi.buffer_size, fi.buffer);
}

void XMPWrap::seek(int pos)
{
  struct xmp_frame_info fi[2];

  xmp_get_frame_info(ctx, &fi[0]);
  xmp_seek_time(ctx, pos);
  xmp_get_frame_info(ctx, &fi[1]);

  /* XMP seeks on a pattern-by-pattern basis, approximating the
   * requested seek time.  If the pattern is so long that the seek
   * time would stay on the same pattern, jump to the next pattern.
   * Make an exception, though, if the desired seek time is zero.
   */
  if(pos > 0 && fi[0].pos == fi[1].pos) xmp_set_position(ctx, fi[1].pos + 1);
}
