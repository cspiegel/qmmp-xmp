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

#ifndef QMMP_XMP_XMPWRAP_H
#define QMMP_XMP_XMPWRAP_H

#include <exception>
#include <string>
#include <vector>

#include <xmp.h>

class XMPWrap
{
  public:
    struct Interpolator
    {
      Interpolator(std::string name, int value) : name(name), value(value) { }
      std::string name;
      int value;
    };

    struct Frame
    {
      Frame(int n, void *buf) : n(n), buf(buf) { }
      int n;
      void *buf;
    };

    class InvalidFile : public std::exception
    {
      public:
        InvalidFile() : std::exception() { }
    };

    static const int interp_nearest = XMP_INTERP_NEAREST;
    static const int interp_linear = XMP_INTERP_LINEAR;
    static const int interp_spline = XMP_INTERP_SPLINE;

    explicit XMPWrap(std::string, int = -1);
    XMPWrap(const XMPWrap &) = delete;
    XMPWrap &operator=(const XMPWrap &) = delete;
    ~XMPWrap();

    static bool can_play(std::string);

    static std::vector<Interpolator> get_interpolators();
    static bool is_valid_interpolator(int);
    static int default_interpolator();
    void set_interpolator(int);

    static bool is_valid_stereo_separation(int);
    static int default_stereo_separation();
    void set_stereo_separation(int);

    static bool is_valid_panning_amplitude(int);
    static int default_panning_amplitude();

    Frame play_frame();
    void seek(int pos);

    int rate() { return 44100; }
    int channels() { return 2; }
    int depth() { return 16; }
    int duration() { return duration_; }
    std::string title() { return title_; }
    std::string format() { return format_; }
    int pattern_count() { return pattern_count_; }
    int track_count() { return track_count_; }
    int channel_count() { return channel_count_; }
    std::vector<char> channel_pan() { return channel_pan_; }
    int instrument_count() { return instrument_count_; }
    int sample_count() { return sample_count_; }
    int initial_speed() { return initial_speed_; }
    int initial_bpm() { return initial_bpm_; }
    int length() { return length_; }
    std::vector<std::string> instruments() { return instruments_; }
    std::vector<std::string> samples() { return samples_; }
    std::string comment() { return comment_; }

  private:
    xmp_context ctx;
    int duration_;
    std::string title_;
    std::string format_;
    int pattern_count_;
    int track_count_;
    int channel_count_;
    std::vector<char> channel_pan_;
    int instrument_count_;
    int sample_count_;
    int initial_speed_;
    int initial_bpm_;
    int length_;
    std::vector<std::string> instruments_;
    std::vector<std::string> samples_;
    std::string comment_;
};

#endif
