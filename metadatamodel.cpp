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

XMPMetaDataModel::XMPMetaDataModel(const QString &path) :
  MetaDataModel(true)
{
  try
  {
    XMPWrap xmp(path.toUtf8().constData());
    fill_in_extra_properties(xmp);
    fill_in_descriptions(xmp);
  }
  catch(const XMPWrap::InvalidFile &)
  {
  }
}

void XMPMetaDataModel::fill_in_extra_properties(XMPWrap &xmp)
{
  QString text;
  auto is_empty_string = [](const std::string &s) { return s == ""; };

  if(!std::all_of(xmp.instruments().begin(), xmp.instruments().end(), is_empty_string))
  {
    for(const std::string &s : xmp.instruments())
    {
      text += QString::fromStdString(s) + "\n";
    }
    desc << MetaDataItem(tr("Instruments"), text);
  }

  if(!std::all_of(xmp.samples().begin(), xmp.samples().end(), is_empty_string))
  {
    text = "";
    for(const std::string &s : xmp.samples())
    {
      text += QString::fromStdString(s) + "\n";
    }
    desc << MetaDataItem(tr("Samples"), text);
  }

  if(!xmp.comment().empty())
  {
    desc << MetaDataItem(tr("Comment"), QString::fromStdString(xmp.comment()));
  }
}

void XMPMetaDataModel::fill_in_descriptions(XMPWrap &xmp)
{
  ap << MetaDataItem(tr("Patterns"), QString::number(xmp.pattern_count()));
  ap << MetaDataItem(tr("Tracks"), QString::number(xmp.track_count()));
  ap << MetaDataItem(tr("Instruments"), QString::number(xmp.instrument_count()));
  ap << MetaDataItem(tr("Samples"), QString::number(xmp.sample_count()));
  ap << MetaDataItem(tr("Initial speed"), QString::number(xmp.initial_speed()));
  ap << MetaDataItem(tr("Initial BPM"), QString::number(xmp.initial_bpm()));
  ap << MetaDataItem(tr("Length"), tr("%1 patterns").arg(xmp.length()));

  QString channels = QString::number(xmp.channel_count()) + " [ ";
  for(const char &pan : xmp.channel_pan())
  {
    channels += pan;
    channels += ' ';
  }
  channels += " ]";
  ap << MetaDataItem(tr("Channels"), channels);
}

XMPMetaDataModel::~XMPMetaDataModel()
{
}

QList<MetaDataItem> XMPMetaDataModel::extraProperties() const
{
  return ap;
}

QList<MetaDataItem> XMPMetaDataModel::descriptions() const
{
  return desc;
}
