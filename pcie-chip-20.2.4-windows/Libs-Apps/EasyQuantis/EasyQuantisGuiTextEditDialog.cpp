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
#include <QtGui/QClipboard>
#else
#include <QtWidgets/QApplication>
#include <QClipboard>
#endif

#include "GccWarningWConversionEnable.h"

#include "EasyQuantisGuiTextEditDialog.hpp"


idQ::EasyQuantis::EasyQuantisGuiTextEditDialog::EasyQuantisGuiTextEditDialog(
    QWidget* parent,
    const QString& dialogTitle,
    const QString& text) :
    QDialog(parent)
{
  if (objectName().isEmpty())
  {
    setObjectName(QString::fromUtf8("EasyQuantisGuiTextEditDialog"));
  }
  resize(360, 420);
  setWindowTitle(dialogTitle);
  setWindowIcon(QIcon(QString::fromUtf8(":/icons/img/appicon.png")));
  setWindowModality(Qt::WindowModal);
  setModal(true);

  verticalLayout = new QVBoxLayout(this);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

  plainTextEdit = new QPlainTextEdit(this);
  plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
  verticalLayout->addWidget(plainTextEdit);

  horizontalLayout = new QHBoxLayout();
  horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
  verticalLayout->addLayout(horizontalLayout);

  pushButtonClipboard = new QPushButton(this);
  pushButtonClipboard->setObjectName(QString::fromUtf8("pushButtonClipboard"));

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  pushButtonClipboard->setText(QApplication::translate("EasyQuantisGuiTextEditDialog",
      "Copy to Clipboard",
      0,
      QApplication::UnicodeUTF8));
#else
  pushButtonClipboard->setText(QApplication::translate("EasyQuantisGuiTextEditDialog",
      "Copy to Clipboard",
      0));
#endif

  horizontalLayout->addWidget(pushButtonClipboard);

  buttonBox = new QDialogButtonBox(this);
  buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
  buttonBox->setStandardButtons(QDialogButtonBox::Close);
  horizontalLayout->addWidget(buttonBox);

  // connects matching signals objects in this main window to slots of object
  // that follow the following form:
  // void on_<widget name>_<signal name>(<signal parameters>);
  QMetaObject::connectSlotsByName(this);

  setPlainText(text);
}


void idQ::EasyQuantis::EasyQuantisGuiTextEditDialog::setPlainText(const QString& text)
{
  if (!plainTextEdit)
  {
    return;
  }

  if (!isVisible())
  {
    show();
  }
  plainTextEdit->setPlainText(text);
}


void idQ::EasyQuantis::EasyQuantisGuiTextEditDialog::on_buttonBox_clicked(QAbstractButton* /*button*/)
{
  close();
}


void idQ::EasyQuantis::EasyQuantisGuiTextEditDialog::on_pushButtonClipboard_clicked()
{
  QClipboard* clipboard = QApplication::clipboard();

  // Copy text into the clipboard
  clipboard->setText(plainTextEdit->toPlainText(), QClipboard::Clipboard);
}









