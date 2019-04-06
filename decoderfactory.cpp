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

#include <QIODevice>
#include <QList>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QtPlugin>

#include <qmmp/qmmp.h>

#include "decoderfactory.h"
#include "decoder.h"
#include "metadatamodel.h"
#include "settingsdialog.h"
#include "xmpwrap.h"

bool XMPDecoderFactory::canDecode(QIODevice *) const
{
  return false;
}

DecoderProperties XMPDecoderFactory::properties() const
{
  DecoderProperties properties;

  properties.name = tr("XMP Plugin");
  properties.filters << "*.669" << "*.amf" << "*.dbm" << "*.digi" << "*.emod" << "*.far" << "*.fnk"
                     << "*.gdm" << "*.gmc" << "*.imf" << "*.ims" << "*.it" << "*.j2b" << "*.liq"
                     << "*.mdl" << "*.med" << "*.mgt" << "*.mod" << "*.mtm" << "*.ntp" << "*.oct"
                     << "*.okta" << "*.psm" << "*.ptm" << "*.rad" << "*.rtm" << "*.s3m" << "*.stm"
                     << "*.ult" << "*.umx" << "*.xm";
  properties.description = tr("XMP Module Files");
  properties.shortName = "cas-xmp";
  properties.hasAbout = true;
  properties.hasSettings = true;
  properties.noInput = true;
  properties.protocols << "file";

  return properties;
}

Decoder *XMPDecoderFactory::create(const QString &path, QIODevice *)
{
  return new XMPDecoder(path);
}

QList<TrackInfo *> XMPDecoderFactory::createPlayList(const QString &filename, TrackInfo::Parts parts, QStringList *)
{
  QList<TrackInfo *> list;

  if(parts & (TrackInfo::MetaData | TrackInfo::Properties))
  {
    try
    {
      XMPWrap xmp(filename.toUtf8().constData());
      TrackInfo *file_info = new TrackInfo(filename);

      if(parts & TrackInfo::Properties)
      {
        file_info->setValue(Qmmp::FORMAT_NAME, QString::fromStdString(xmp.format()));
        file_info->setDuration(xmp.duration());
      }

      if(parts & TrackInfo::MetaData)
      {
        if(settings.get_use_filename())
        {
          file_info->setValue(Qmmp::TITLE, filename.section('/', -1));
        }
        else if(!xmp.title().empty())
        {
          file_info->setValue(Qmmp::TITLE, QString::fromStdString(xmp.title()));
        }
      }

      list << file_info;
    }
    catch(const XMPWrap::InvalidFile &)
    {
    }
  }

  return list;
}

MetaDataModel *XMPDecoderFactory::createMetaDataModel(const QString &path, bool)
{
  return new XMPMetaDataModel(path);
}

void XMPDecoderFactory::showSettings(QWidget *parent)
{
  SettingsDialog *d = new SettingsDialog(parent);
  d->show();
}

void XMPDecoderFactory::showAbout(QWidget *parent)
{
  QString title = tr("About XMP Audio Plugin");
  QString text = tr("XMP Audio Plugin");
  text += "\n";
  text += tr("Written by: Chris Spiegel <cspiegel@gmail.com>");

  QMessageBox::about(parent, title, text);
}

QString XMPDecoderFactory::translation() const
{
  return QString(":/cas-xmp_plugin_");
}
