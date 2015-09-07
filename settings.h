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

#ifndef QMMP_XMP_SETTINGS_H
#define QMMP_XMP_SETTINGS_H

#include <QList>
#include <QPair>
#include <QSettings>
#include <QString>

#include <qmmp/qmmp.h>

#include "xmpwrap.h"

class XMPSettings
{
  public:
    XMPSettings() : settings(new QSettings(Qmmp::configFile(), QSettings::IniFormat))
    {
      for(const XMPWrap::Interpolator &interpolator : XMPWrap::get_interpolators())
      {
        interpolators.append(QPair<QString, int>(QString::fromStdString(interpolator.name), interpolator.value));
      }

      settings->beginGroup("cas-xmp-plugin");
    }

    ~XMPSettings()
    {
      settings->endGroup();
      delete settings;
    }

    const QList<QPair<QString, int>> get_interpolators()
    {
      return interpolators;
    }

    int get_interpolator()
    {
      int interpolator = settings->value("interpolator", XMPWrap::default_interpolator()).toInt();

      return XMPWrap::is_valid_interpolator(interpolator) ? interpolator : default_interpolator();
    }

    void set_interpolator(int value)
    {
      if(XMPWrap::is_valid_interpolator(value))
      {
        settings->setValue("interpolator", value);
      }
    }

    int default_interpolator()
    {
      return XMPWrap::default_interpolator();
    }

    int get_stereo_separation()
    {
      int separation = settings->value("stereo_separation", default_stereo_separation()).toInt();

      if(!XMPWrap::is_valid_stereo_separation(separation)) separation = default_stereo_separation();

      return separation;
    }

    void set_stereo_separation(int separation)
    {
      if(XMPWrap::is_valid_stereo_separation(separation))
      {
        settings->setValue("stereo_separation", separation);
      }
    }

    int default_stereo_separation()
    {
      return XMPWrap::default_stereo_separation();
    }

    int get_panning_amplitude()
    {
      int panning = settings->value("panning_amplitude", default_panning_amplitude()).toInt();

      if(!XMPWrap::is_valid_panning_amplitude(panning)) panning = default_panning_amplitude();

      return panning;
    }

    void set_panning_amplitude(int panning)
    {
      if(XMPWrap::is_valid_panning_amplitude(panning))
      {
        settings->setValue("panning_amplitude", panning);
      }
    }

    int default_panning_amplitude()
    {
      return XMPWrap::default_panning_amplitude();
    }

    bool get_use_filename()
    {
      return settings->value("use_filename", default_use_filename()).toBool();
    }

    void set_use_filename(bool use)
    {
      settings->setValue("use_filename", use);
    }

    bool default_use_filename()
    {
      return false;
    }

  private:
    XMPSettings(const XMPSettings &);
    XMPSettings &operator=(const XMPSettings &);

    QSettings *settings;
    QList<QPair<QString, int>> interpolators;
};

#endif
