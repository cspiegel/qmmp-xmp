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

#include <QDialog>
#include <QPair>
#include <QString>
#include <QVariant>
#include <QWidget>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
  ui.setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

  for(const auto &interpolator : settings.get_interpolators())
  {
    ui.interpolate_combo->addItem(interpolator.first, interpolator.second);
  }

  set_interpolator(settings.get_interpolator());

  ui.stereo_separation->setSliderPosition(settings.get_stereo_separation());
  ui.panning_amplitude->setSliderPosition(settings.get_panning_amplitude());

  ui.use_filename->setChecked(settings.get_use_filename());
}

void SettingsDialog::accept()
{
  settings.set_interpolator(ui.interpolate_combo->itemData(ui.interpolate_combo->currentIndex()).toInt());
  settings.set_stereo_separation(ui.stereo_separation->value());
  settings.set_panning_amplitude(ui.panning_amplitude->value());
  settings.set_use_filename(ui.use_filename->isChecked());

  QDialog::accept();
}

void SettingsDialog::restore_defaults()
{
  set_interpolator(settings.default_interpolator());
  ui.stereo_separation->setSliderPosition(settings.default_stereo_separation());
  ui.panning_amplitude->setSliderPosition(settings.default_panning_amplitude());
  ui.use_filename->setChecked(settings.default_use_filename());
}

void SettingsDialog::set_interpolator(int interpolator)
{
  int i = ui.interpolate_combo->findData(interpolator);
  if(i != -1)
  {
    ui.interpolate_combo->setCurrentIndex(i);
  }
}
