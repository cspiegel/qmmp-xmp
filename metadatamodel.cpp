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

#include <QHash>
#include <QLatin1Char>
#include <QObject>
#include <QString>
#include <QtGui/qtextdocument.h>

#include <qmmp/metadatamodel.h>

#include "metadatamodel.h"
#include "xmpwrap.h"

XMPMetaDataModel::XMPMetaDataModel(const QString &path, QObject *parent) :
  MetaDataModel(parent)
{
  try
  {
    XMPWrap xmp(path.toUtf8().constData());
    fill_in_audio_properties(xmp);
    fill_in_descriptions(xmp);
  }
  catch(XMPWrap::InvalidFile)
  {
  }
}

void XMPMetaDataModel::fill_in_audio_properties(XMPWrap &xmp)
{
  QString text;

  for(const std::string &s : xmp.instruments())
  {
    text += QString::fromStdString(s) + "\n";
  }
  desc.insert(tr("Instruments"), text);

  text = "";
  for(const std::string &s : xmp.samples())
  {
    text += QString::fromStdString(s) + "\n";
  }
  desc.insert(tr("Samples"), text);

  if(!xmp.comment().empty())
  {
    desc.insert(tr("Comment"), QString::fromStdString(xmp.comment()));
  }
}

void XMPMetaDataModel::fill_in_descriptions(XMPWrap &xmp)
{
  ap.insert(tr("Title"), Qt::escape(QString::fromStdString(xmp.title())));
  ap.insert(tr("Format"), Qt::escape(QString::fromStdString(xmp.format())));
  ap.insert(tr("Patterns"), QString::number(xmp.pattern_count()));
  ap.insert(tr("Tracks"), QString::number(xmp.track_count()));
  ap.insert(tr("Instruments"), QString::number(xmp.instrument_count()));
  ap.insert(tr("Samples"), QString::number(xmp.sample_count()));
  ap.insert(tr("Initial speed"), QString::number(xmp.initial_speed()));
  ap.insert(tr("Initial BPM"), QString::number(xmp.initial_bpm()));
  ap.insert(tr("Length"), tr("%1 patterns").arg(xmp.length()));

  QString channels = QString::number(xmp.channel_count()) + " [ ";
  for(const char &pan : xmp.channel_pan())
  {
    channels += pan;
    channels += ' ';
  }
  channels += " ]";
  ap.insert(tr("Channels"), channels);

  int duration = xmp.duration() / 1000;
  if(duration < 60 * 60)
  {
    ap.insert(tr("Duration"), QString("%1:%2").
                                  arg(duration / 60).
                                  arg(duration % 60, 2, 10, QLatin1Char('0')));
  }
  else
  {
    ap.insert(tr("Duration"), QString("%1:%2:%3").
                                  arg(duration / 3600).
                                  arg((duration / 60) % 60, 2, 10, QLatin1Char('0')).
                                  arg(duration % 60, 2, 10, QLatin1Char('0')));
  }
}

XMPMetaDataModel::~XMPMetaDataModel()
{
}

QHash<QString, QString> XMPMetaDataModel::audioProperties()
{
  return ap;
}

QHash<QString, QString> XMPMetaDataModel::descriptions()
{
  return desc;
}
