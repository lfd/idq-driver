/*
 * This file is part of the EasyQuantis application
 *
 * Copyright (C) 2004-2020 ID Quantique SA, Carouge/Geneva, Switzerland
 * All rights reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY.
 *
 * ----------------------------------------------------------------------------
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * ----------------------------------------------------------------------------
 *
 * For history of changes, see ChangeLog.txt
 */

#include "GccWarningWConversionDisable.h"

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#else
#include <QtWidgets/QApplication>
#include <QIcon>
#endif

#include "GccWarningWConversionEnable.h"

// #include "Quantis/Scaling.h"

#include "EasyQuantisGuiProgressDialog.hpp"


idQ::EasyQuantis::EasyQuantisGuiProgressDialog::EasyQuantisGuiProgressDialog(
    QWidget* parent,
    const QString& dialogTitle) :
    QDialog(parent),
    cancellationFlag(false)
{
  if (objectName().isEmpty())
  {
    setObjectName(QString::fromUtf8("EasyQuantisGuiProgressDialog"));
  }
  resize(480, 160);
  setWindowTitle(dialogTitle);
  setWindowIcon(QIcon(QString::fromUtf8(":/icons/img/appicon.png")));

  verticalLayout = new QVBoxLayout(this);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

  label1 = new QLabel(this);
  label1->setObjectName(QString::fromUtf8("label1"));
  verticalLayout->addWidget(label1);

  progressBar = new QProgressBar(this);
  progressBar->setObjectName(QString::fromUtf8("progressBar"));
  progressBar->setValue(0);
  verticalLayout->addWidget(progressBar);

  label2 = new QLabel(this);
  label2->setObjectName(QString::fromUtf8("label2"));
  verticalLayout->addWidget(label2);

  buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
  verticalLayout->addWidget(buttonBox);

  // connects matching signals objects in this main window to slots of object
  // that follow the following form:
  // void on_<widget name>_<signal name>(<signal parameters>);
  QMetaObject::connectSlotsByName(this);
}


void idQ::EasyQuantis::EasyQuantisGuiProgressDialog::reject()
{
  cancellationFlag = true;
  QDialog::reject();
}


void idQ::EasyQuantis::EasyQuantisGuiProgressDialog::on_buttonBox_rejected()
{
  reject();
}


void idQ::EasyQuantis::EasyQuantisGuiProgressDialog::setLabel1Text(const QString& message)
{
  if (label1)
  {
    label1->setText(message);
  }
}


void idQ::EasyQuantis::EasyQuantisGuiProgressDialog::setLabel2Text(const QString& message)
{
  if (label2)
  {
    label2->setText(message);
  }
}


void idQ::EasyQuantis::EasyQuantisGuiProgressDialog::setRange(int minimum, int maximun)
{
  if (progressBar)
  {
    progressBar->setRange(minimum, maximun);
  }
}

void idQ::EasyQuantis::EasyQuantisGuiProgressDialog::setValue(int value)
{
  if (!progressBar)
  {
    // Progress bar do not exists...
    return;
  }

  // Checks if dialog is visible
  if (!isVisible())
  {
    show();
    qApp->processEvents();
  }

  progressBar->setValue(value);

  if (value >= progressBar->maximum())
  {
    accept();
  }
}


bool idQ::EasyQuantis::EasyQuantisGuiProgressDialog::wasCanceled()
{
  return cancellationFlag;
}










