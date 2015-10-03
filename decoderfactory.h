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

#ifndef QMMP_XMP_DECODERFACTORY_H
#define QMMP_XMP_DECODERFACTORY_H

#include <QList>
#include <QIODevice>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QWidget>

#include <qmmp/decoder.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/qmmp.h>

#include "settings.h"

class XMPDecoderFactory : public QObject, DecoderFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qmmp.qmmp.DecoderFactoryInterface.1.0")
  Q_INTERFACES(DecoderFactory)

  public:
    bool supports(const QString &) const;
    bool canDecode(QIODevice *) const;
    const DecoderProperties properties() const;
    Decoder *create(const QString &, QIODevice *);
    QList<FileInfo *> createPlayList(const QString &, bool, QStringList *);
    MetaDataModel *createMetaDataModel(const QString &, QObject * = 0);
    void showSettings(QWidget *);
    void showAbout(QWidget *);
    QTranslator *createTranslator(QObject *);

  private:
    XMPSettings settings;
};

#endif
