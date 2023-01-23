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

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <limits>
#include <string>

#include "GccWarningWConversionDisable.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QMessageBox>
#include "GccWarningWConversionEnable.h"

#include "Quantis/Quantis.hpp"

#include "EasyQuantis.hpp"
#include "EasyQuantisGuiMain.hpp"
#include "EasyQuantisGuiProgressDialog.hpp"
#include "EasyQuantisGuiTextEditDialog.hpp"
#include "Utils.hpp"

using namespace std;
using namespace boost;
using namespace idQ::EasyQuantis::Utils;

// Macro for QApplication::translate
#define QTRANSLATE(key) QApplication::translate("EasyQuantisGuiMain", \
                                                key,                  \
                                                0,                    \
                                                QApplication::UnicodeUTF8)

// Indices of pages in centralStackedWidget
#define IDX_PAGE_MAIN__DATA_ACQUISITION 0

idQ::EasyQuantis::EasyQuantisGuiMain::EasyQuantisGuiMain(QWidget *parent) : QMainWindow(parent)
{
  // -------------------------------- Setup UI --------------------------------

  // ********** Main Window **********
  if (objectName().isEmpty())
  {
    setObjectName(QString::fromUtf8("EasyQuantisGuiMain"));
  }
  setMinimumSize(QSize(640, 600));
  resize(800, 600);
  setWindowIcon(QIcon(QString::fromUtf8(":/icons/img/appicon.png")));

  // ********** Menu **********
  menubar = new QMenuBar(this);
  menubar->setObjectName(QString::fromUtf8("menubar"));
  menubar->setGeometry(QRect(0, 0, 538, 23));
  setMenuBar(menubar);

  // File menu
  menuFile = new QMenu(menubar);
  menuFile->setObjectName(QString::fromUtf8("menuFile"));
  menubar->addAction(menuFile->menuAction());

  actionFileExit = new QAction(this);
  actionFileExit->setObjectName(QString::fromUtf8("actionFileExit"));
  menuFile->addAction(actionFileExit);

  // Help menu
  menuHelp = new QMenu(menubar);
  menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
  menubar->addAction(menuHelp->menuAction());

  actionHelpAbout = new QAction(this);
  actionHelpAbout->setObjectName(QString::fromUtf8("actionHelpAbout"));
  menuHelp->addAction(actionHelpAbout);

  // ********** Status bar **********
  statusbar = new QStatusBar(this);
  statusbar->setObjectName(QString::fromUtf8("statusbar"));
  setStatusBar(statusbar);

  // ********** Toolbar **********
  toolBar = new QToolBar(this);
  toolBar->setObjectName(QString::fromUtf8("toolBar"));
  toolBar->setFixedHeight(30);
  addToolBar(Qt::TopToolBarArea, toolBar);

  // Quantis list combo box
  comboBoxQuantisList = new QComboBox(toolBar);
  comboBoxQuantisList->setObjectName(QString::fromUtf8("comboBoxQuantisList"));
  comboBoxQuantisList->move(12, 3);
  comboBoxQuantisList->setFixedSize(256, 24);

  // Refresh button
  pushButtonRefreshQuantisList = new QPushButton(toolBar);
  pushButtonRefreshQuantisList->setObjectName(QString::fromUtf8("pushButtonRefreshQuantisList"));
  pushButtonRefreshQuantisList->move(comboBoxQuantisList->geometry().right() + 3, 3);
  pushButtonRefreshQuantisList->setFixedSize(24, 24);
  pushButtonRefreshQuantisList->setIcon(QIcon(QString::fromUtf8(":/icons/img/refresh.png")));

  // Info button
  pushButtonQuantisInfo = new QPushButton(toolBar);
  pushButtonQuantisInfo->setObjectName(QString::fromUtf8("pushButtonQuantisInfo"));
  pushButtonQuantisInfo->move(pushButtonRefreshQuantisList->geometry().right() + 3, 3);
  pushButtonQuantisInfo->setFixedSize(24, 24);
  pushButtonQuantisInfo->setIcon(QIcon(QString::fromUtf8(":/icons/img/info.png")));

  // ********** Create Tabs **********
  mTabWidget = new QTabWidget(this);

  pageDataAcquisition = new QWidget(mTabWidget);
  pageDataAcquisition->setObjectName(QString::fromUtf8("pageDataAcquisition"));
  mTabWidget->addTab(pageDataAcquisition, "&Acquisition");

  pageFileExtraction = new QWidget(mTabWidget);
  pageFileExtraction->setObjectName(QString::fromUtf8("pageExtraction"));
  mTabWidget->addTab(pageFileExtraction, "File &Extraction");

  pageExtractionMatrix = new QWidget(mTabWidget);
  pageExtractionMatrix->setObjectName(QString::fromUtf8("pageExtractionMatrix"));
  mTabWidget->addTab(pageExtractionMatrix, "Extraction &Matrix");

  setCentralWidget(mTabWidget);

  // ********** Random data acquisition page **********

  // Set central's widget background image
  QPalette palette;
  palette.setBrush(pageDataAcquisition->backgroundRole(),
                   QBrush(QImage(QString::fromUtf8(":/images/img/dices.png"))));
  pageDataAcquisition->setPalette(palette);
  pageDataAcquisition->setAutoFillBackground(true);

  pageDataAcquisitionVerticalLayout = new QVBoxLayout(pageDataAcquisition);
  pageDataAcquisitionVerticalLayout->setObjectName(QString::fromUtf8("pageDataAcquisitionVerticalLayout"));

  // ---------- Data format ----------
  groupBoxDataFormat = new QGroupBox(pageDataAcquisition);
  groupBoxDataFormat->setObjectName(QString::fromUtf8("groupBoxDataFormat"));
  pageDataAcquisitionVerticalLayout->addWidget(groupBoxDataFormat);

  groupBoxDataFormatVerticalLayout = new QVBoxLayout(groupBoxDataFormat);
  groupBoxDataFormatVerticalLayout->setObjectName(QString::fromUtf8("groupBoxDataFormatVerticalLayout"));

  // Binary data radion button
  radioButtonDataFormatBinary = new QRadioButton(groupBoxDataFormat);
  radioButtonDataFormatBinary->setObjectName(QString::fromUtf8("radioButtonDataFormatBinary"));
  groupBoxDataFormatVerticalLayout->addWidget(radioButtonDataFormatBinary);

  // Int data radio button
  radioButtonDataFormatIntegers = new QRadioButton(groupBoxDataFormat);
  radioButtonDataFormatIntegers->setObjectName(QString::fromUtf8("radioButtonDataFormatIntegers"));
  groupBoxDataFormatVerticalLayout->addWidget(radioButtonDataFormatIntegers);

  // Floats radio button
  radioButtonDataFormatFloats = new QRadioButton(groupBoxDataFormat);
  radioButtonDataFormatFloats->setObjectName(QString::fromUtf8("radioButtonDataFormatFloats"));
  groupBoxDataFormatVerticalLayout->addWidget(radioButtonDataFormatFloats);

  // Quantis list combo box
  comboBoxDataFormat = new QComboBox(toolBar);
  comboBoxDataFormat->setObjectName(QString::fromUtf8("comboBoxDataFormat"));
  comboBoxDataFormat->setFixedSize(256, 24);
  groupBoxDataFormatVerticalLayout->addWidget(comboBoxDataFormat);

  // ---------- Data scaling ----------
  groupBoxDataScaling = new QGroupBox(pageDataAcquisition);
  groupBoxDataScaling->setObjectName(QString::fromUtf8("groupBoxDataScaling"));
  groupBoxDataScaling->setCheckable(true);
  groupBoxDataScaling->setEnabled(true);
  groupBoxDataScaling->setChecked(false);
  pageDataAcquisitionVerticalLayout->addWidget(groupBoxDataScaling);

  groupBoxDataScalingHorizontalLayout = new QHBoxLayout(groupBoxDataScaling);
  groupBoxDataScalingHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxDataScalingHorizontalLayout"));

  labelDataScaling1 = new QLabel(groupBoxDataScaling);
  labelDataScaling1->setObjectName(QString::fromUtf8("labelDataScaling1"));
  groupBoxDataScalingHorizontalLayout->addWidget(labelDataScaling1);

  doubleSpinBoxDataScalingMin = new QDoubleSpinBox(groupBoxDataScaling);
  doubleSpinBoxDataScalingMin->setObjectName(QString::fromUtf8("doubleSpinBoxDataScalingMin"));
  doubleSpinBoxDataScalingMin->setFixedSize(62, 26);
  doubleSpinBoxDataScalingMin->setDecimals(0);
  doubleSpinBoxDataScalingMin->setValue(0);
  groupBoxDataScalingHorizontalLayout->addWidget(doubleSpinBoxDataScalingMin);

  labelDataScaling2 = new QLabel(groupBoxDataScaling);
  labelDataScaling2->setObjectName(QString::fromUtf8("labelDataScaling2"));
  groupBoxDataScalingHorizontalLayout->addWidget(labelDataScaling2);

  doubleSpinBoxDataScalingMax = new QDoubleSpinBox(groupBoxDataScaling);
  doubleSpinBoxDataScalingMax->setObjectName(QString::fromUtf8("doubleSpinBoxDataScalingMax"));
  doubleSpinBoxDataScalingMax->setFixedSize(62, 26);
  doubleSpinBoxDataScalingMax->setDecimals(0);
  doubleSpinBoxDataScalingMax->setValue(1);
  groupBoxDataScalingHorizontalLayout->addWidget(doubleSpinBoxDataScalingMax);

  dataScalingHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  groupBoxDataScalingHorizontalLayout->addItem(dataScalingHorizontalSpacer);

  // enable extraction group
  groupBoxExtractionEnable = new QGroupBox(pageDataAcquisition);
  groupBoxExtractionEnable->setObjectName(QString::fromUtf8("groupBoxExtractionEnable"));
  groupBoxExtractionEnable->setCheckable(true);
  groupBoxExtractionEnable->setEnabled(true);
  groupBoxExtractionEnable->setChecked(false);
  pageDataAcquisitionVerticalLayout->addWidget(groupBoxExtractionEnable);

  groupBoxExtractionEnableVerticalLayout = new QVBoxLayout(groupBoxExtractionEnable);
  groupBoxExtractionEnableVerticalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionEnableVerticalLayout"));

  groupBoxExtractionEnableSizeHorizontalLayout = new QHBoxLayout;
  groupBoxExtractionEnableSizeHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionEnableSizeHorizontalLayout"));

  groupBoxExtractionEnableVerticalLayout->addLayout(groupBoxExtractionEnableSizeHorizontalLayout);

  labelExtractionSize = new QLabel(groupBoxExtractionEnable);
  labelExtractionSize->setObjectName(QString::fromUtf8("labelExtractionSize"));
  groupBoxExtractionEnableSizeHorizontalLayout->addWidget(labelExtractionSize);

  comboBoxExtractionSize = new QComboBox(groupBoxExtractionEnable);
  comboBoxExtractionSize->setObjectName(QString::fromUtf8("comboBoxExtractionSize"));
  groupBoxExtractionEnableSizeHorizontalLayout->addWidget(comboBoxExtractionSize);

  enableExtractionHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  groupBoxExtractionEnableSizeHorizontalLayout->addItem(enableExtractionHorizontalSpacer);

  //Matrix filename
  groupBoxExtractionEnableMatrixFilenameHorizontalLayout = new QHBoxLayout();
  groupBoxExtractionEnableMatrixFilenameHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionEnableMatrixFilenameHorizontalLayout"));

  labelExtractionMatrixFilename = new QLabel(groupBoxExtractionEnable);
  labelExtractionMatrixFilename->setObjectName(QString::fromUtf8("labelExtractionMatrixFilename"));
  groupBoxExtractionEnableMatrixFilenameHorizontalLayout->addWidget(labelExtractionMatrixFilename);

  lineEditExtractionMatrixFilename = new QLineEdit(groupBoxExtractionEnable);
  lineEditExtractionMatrixFilename->setObjectName(QString::fromUtf8("lineEditExtractionMatrixFilename"));
  lineEditExtractionMatrixFilename->setFixedHeight(24);
  lineEditExtractionMatrixFilename->setMinimumWidth(350);
  lineEditExtractionMatrixFilename->setMaximumWidth(1000);
  lineEditExtractionMatrixFilename->setText("");
  groupBoxExtractionEnableMatrixFilenameHorizontalLayout->addWidget(lineEditExtractionMatrixFilename);

  pushButtonExtractionMatrixBrowseFilename = new QPushButton(groupBoxExtractionEnable);
  pushButtonExtractionMatrixBrowseFilename->setObjectName(QString::fromUtf8("pushButtonExtractionMatrixBrowseFilename"));
  pushButtonExtractionMatrixBrowseFilename->setFixedSize(24, 24);
  groupBoxExtractionEnableMatrixFilenameHorizontalLayout->addWidget(pushButtonExtractionMatrixBrowseFilename);

  groupBoxExtractionEnableVerticalLayout->addLayout(groupBoxExtractionEnableMatrixFilenameHorizontalLayout);

  // ---------- Output ----------
  groupBoxDataOutput = new QGroupBox(pageDataAcquisition);
  groupBoxDataOutput->setObjectName(QString::fromUtf8("groupBoxDataOutput"));
  pageDataAcquisitionVerticalLayout->addWidget(groupBoxDataOutput);

  groupBoxDataOutputVerticalLayout = new QVBoxLayout(groupBoxDataOutput);
  groupBoxDataOutputVerticalLayout->setObjectName(QString::fromUtf8("groupBoxDataOutputVerticalLayout"));

  radioButtonDataOutputScreen = new QRadioButton(groupBoxDataOutput);
  radioButtonDataOutputScreen->setObjectName(QString::fromUtf8("radioButtonDataOutputScreen"));
  groupBoxDataOutputVerticalLayout->addWidget(radioButtonDataOutputScreen);

  radioButtonOutputFileHorizontalLayout = new QHBoxLayout();
  radioButtonOutputFileHorizontalLayout->setObjectName(QString::fromUtf8("radioButtonOutputFileHorizontalLayout"));
  groupBoxDataOutputVerticalLayout->addLayout(radioButtonOutputFileHorizontalLayout);

  radioButtonDataOutputFile = new QRadioButton(groupBoxDataOutput);
  radioButtonDataOutputFile->setObjectName(QString::fromUtf8("radioButtonDataOutputFile"));
  radioButtonOutputFileHorizontalLayout->addWidget(radioButtonDataOutputFile);

  lineEditDataOutputFilename = new QLineEdit(groupBoxDataOutput);
  lineEditDataOutputFilename->setObjectName(QString::fromUtf8("lineEditDataOutputFilename"));
  lineEditDataOutputFilename->setFixedHeight(24);
  lineEditDataOutputFilename->setMinimumWidth(350);
  lineEditDataOutputFilename->setMaximumWidth(1000);
  lineEditDataOutputFilename->setText(QDir::toNativeSeparators(QDir::cleanPath(QDir::tempPath().append("/random.dat"))));

  radioButtonOutputFileHorizontalLayout->addWidget(lineEditDataOutputFilename);

  pushButtonDataOutputBrowseFilename = new QPushButton(groupBoxDataOutput);
  pushButtonDataOutputBrowseFilename->setObjectName(QString::fromUtf8("pushButtonDataOutputBrowseFilename"));
  pushButtonDataOutputBrowseFilename->setFixedSize(24, 24);
  radioButtonOutputFileHorizontalLayout->addWidget(pushButtonDataOutputBrowseFilename);

  // ---------- Data size ----------
  groupBoxDataSize = new QGroupBox(pageDataAcquisition);
  groupBoxDataSize->setObjectName(QString::fromUtf8("groupBoxDataSize"));
  pageDataAcquisitionVerticalLayout->addWidget(groupBoxDataSize);

  pageDataSizeHorizontalLayout = new QHBoxLayout(groupBoxDataSize);
  pageDataSizeHorizontalLayout->setObjectName(QString::fromUtf8("pageDataSizeHorizontalLayout"));

  labelDataSize1 = new QLabel(groupBoxDataSize);
  labelDataSize1->setObjectName(QString::fromUtf8("labelDataSize1"));
  labelDataSize1->setFixedHeight(24);
  pageDataSizeHorizontalLayout->addWidget(labelDataSize1);

  doubleSpinBoxDataSize = new QDoubleSpinBox(groupBoxDataSize);
  doubleSpinBoxDataSize->setObjectName(QString::fromUtf8("doubleSpinBoxDataSize"));
  doubleSpinBoxDataSize->setDecimals(0);
  doubleSpinBoxDataSize->setMinimum(1);
  doubleSpinBoxDataSize->setMaximum(static_cast<double>(numeric_limits<int>::max()));
  doubleSpinBoxDataSize->setFixedHeight(24);
  pageDataSizeHorizontalLayout->addWidget(doubleSpinBoxDataSize);

  labelDataSize2 = new QLabel(groupBoxDataSize);
  labelDataSize2->setObjectName(QString::fromUtf8("labelDataSize2"));
  labelDataSize2->setFixedHeight(24);
  pageDataSizeHorizontalLayout->addWidget(labelDataSize2);

  pushButtonDataGenerate = new QPushButton(groupBoxDataSize);
  pushButtonDataGenerate->setObjectName(QString::fromUtf8("pushButtonDataGenerate"));
  pageDataSizeHorizontalLayout->addWidget(pushButtonDataGenerate);

  //================================================================
  // ********** Extraction from file page **********

  pageFileExtractionVerticalLayout = new QVBoxLayout(pageFileExtraction);
  pageFileExtractionVerticalLayout->setObjectName(QString::fromUtf8("pageExtractionVerticalLayout"));

  // file extraction group
  groupBoxExtractionFile = new QGroupBox(pageFileExtraction);
  groupBoxExtractionFile->setObjectName(QString::fromUtf8("groupBoxExtractionFile"));
  groupBoxExtractionFile->setEnabled(true);
  pageFileExtractionVerticalLayout->addWidget(groupBoxExtractionFile);

  groupBoxExtractionFileVerticalLayout = new QVBoxLayout(groupBoxExtractionFile);
  groupBoxExtractionFileVerticalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionFileVerticalLayout"));

  // Size
  groupBoxExtractionFileSizeHorizontalLayout = new QHBoxLayout;
  groupBoxExtractionFileSizeHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionFileSizeHorizontalLayout"));

  groupBoxExtractionFileVerticalLayout->addLayout(groupBoxExtractionFileSizeHorizontalLayout);

  labelExtractionFileSize = new QLabel(groupBoxExtractionFile);
  labelExtractionFileSize->setObjectName(QString::fromUtf8("labelExtractionFileSize"));
  groupBoxExtractionFileSizeHorizontalLayout->addWidget(labelExtractionFileSize);

  comboBoxExtractionFileSize = new QComboBox(groupBoxExtractionFile);
  comboBoxExtractionFileSize->setObjectName(QString::fromUtf8("comboBoxExtractionFileSize"));
  groupBoxExtractionFileSizeHorizontalLayout->addWidget(comboBoxExtractionFileSize);

  fileExtractionHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  groupBoxExtractionFileSizeHorizontalLayout->addItem(fileExtractionHorizontalSpacer);

  //Matrix filename
  groupBoxExtractionFileMatrixFilenameHorizontalLayout = new QHBoxLayout();
  groupBoxExtractionFileMatrixFilenameHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionFileMatrixFilenameHorizontalLayout"));

  labelExtractionFileMatrixFilename = new QLabel(groupBoxExtractionFile);
  labelExtractionFileMatrixFilename->setObjectName(QString::fromUtf8("labelExtractionFileMatrixFilename"));
  groupBoxExtractionFileMatrixFilenameHorizontalLayout->addWidget(labelExtractionFileMatrixFilename);

  lineEditExtractionFileMatrixFilename = new QLineEdit(groupBoxExtractionFile);
  lineEditExtractionFileMatrixFilename->setObjectName(QString::fromUtf8("lineEditExtractionFileMatrixFilename"));
  lineEditExtractionFileMatrixFilename->setFixedHeight(24);
  lineEditExtractionFileMatrixFilename->setMinimumWidth(350);
  lineEditExtractionFileMatrixFilename->setMaximumWidth(1000);
  lineEditExtractionFileMatrixFilename->setText("");
  groupBoxExtractionFileMatrixFilenameHorizontalLayout->addWidget(lineEditExtractionFileMatrixFilename);

  pushButtonExtractionFileMatrixBrowseFilename = new QPushButton(groupBoxExtractionFile);
  pushButtonExtractionFileMatrixBrowseFilename->setObjectName(QString::fromUtf8("pushButtonExtractionFileMatrixBrowseFilename"));
  pushButtonExtractionFileMatrixBrowseFilename->setFixedSize(24, 24);
  groupBoxExtractionFileMatrixFilenameHorizontalLayout->addWidget(pushButtonExtractionFileMatrixBrowseFilename);

  groupBoxExtractionFileVerticalLayout->addLayout(groupBoxExtractionFileMatrixFilenameHorizontalLayout);

  //From file group

  groupBoxExtractionFromFile = new QGroupBox(pageFileExtraction);
  groupBoxExtractionFromFile->setObjectName(QString::fromUtf8("groupBoxExtractionFromFile"));
  groupBoxExtractionFromFile->setEnabled(true);
  pageFileExtractionVerticalLayout->addWidget(groupBoxExtractionFromFile);

  groupBoxExtractionFormFileVerticalLayout = new QVBoxLayout(groupBoxExtractionFromFile);
  groupBoxExtractionFormFileVerticalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionFormFileVerticalLayout"));

  // From file:
  groupBoxExtractionFromFileFromHorizontalLayout = new QHBoxLayout;
  groupBoxExtractionFromFileFromHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionFromFileFromHorizontalLayout"));

  groupBoxExtractionFormFileVerticalLayout->addLayout(groupBoxExtractionFromFileFromHorizontalLayout);

  labelExtractionFromFileFrom = new QLabel(groupBoxExtractionFromFile);
  labelExtractionFromFileFrom->setObjectName(QString::fromUtf8("labelExtractionFromFileFrom"));
  labelExtractionFromFileFrom->setFixedSize(100, 20);
  groupBoxExtractionFromFileFromHorizontalLayout->addWidget(labelExtractionFromFileFrom);

  lineEditExtractionFromFileFrom = new QLineEdit(groupBoxExtractionFromFile);
  lineEditExtractionFromFileFrom->setObjectName(QString::fromUtf8("lineEditExtractionFromFileFrom"));
  lineEditExtractionFromFileFrom->setFixedHeight(24);
  lineEditExtractionFromFileFrom->setMinimumWidth(350);
  lineEditExtractionFromFileFrom->setMaximumWidth(1000);
  lineEditExtractionFromFileFrom->setText("");
  groupBoxExtractionFromFileFromHorizontalLayout->addWidget(lineEditExtractionFromFileFrom);

  pushButtonExtractionFromFileFromBrowseFilename = new QPushButton(groupBoxExtractionFromFile);
  pushButtonExtractionFromFileFromBrowseFilename->setObjectName(QString::fromUtf8("pushButtonExtractionFromFileFromBrowseFilename"));
  pushButtonExtractionFromFileFromBrowseFilename->setFixedSize(24, 24);
  groupBoxExtractionFromFileFromHorizontalLayout->addWidget(pushButtonExtractionFromFileFromBrowseFilename);

  // To File
  groupBoxExtractionFromFileToHorizontalLayout = new QHBoxLayout;
  groupBoxExtractionFromFileToHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionFromFileToHorizontalLayout"));

  groupBoxExtractionFormFileVerticalLayout->addLayout(groupBoxExtractionFromFileToHorizontalLayout);

  labelExtractionFromFileTo = new QLabel(groupBoxExtractionFromFile);
  labelExtractionFromFileTo->setObjectName(QString::fromUtf8("labelExtractionFromFileTo"));
  labelExtractionFromFileTo->setFixedSize(100, 20);
  groupBoxExtractionFromFileToHorizontalLayout->addWidget(labelExtractionFromFileTo);

  lineEditExtractionFromFileTo = new QLineEdit(groupBoxExtractionFromFile);
  lineEditExtractionFromFileTo->setObjectName(QString::fromUtf8("lineEditExtractionFromFileTo"));
  lineEditExtractionFromFileTo->setFixedHeight(24);
  lineEditExtractionFromFileTo->setMinimumWidth(350);
  lineEditExtractionFromFileTo->setMaximumWidth(1000);
  lineEditExtractionFromFileTo->setText("");
  groupBoxExtractionFromFileToHorizontalLayout->addWidget(lineEditExtractionFromFileTo);

  pushButtonExtractionFromFileToBrowseFilename = new QPushButton(groupBoxExtractionFromFile);
  pushButtonExtractionFromFileToBrowseFilename->setObjectName(QString::fromUtf8("pushButtonExtractionFromFileToBrowseFilename"));
  pushButtonExtractionFromFileToBrowseFilename->setFixedSize(24, 24);
  groupBoxExtractionFromFileToHorizontalLayout->addWidget(pushButtonExtractionFromFileToBrowseFilename);

  //Process button

  groupBoxExtractionFromFileProcess = new QHBoxLayout;
  groupBoxExtractionFromFileProcess->setObjectName(QString::fromUtf8("groupBoxExtractionFromFileProcess"));

  groupBoxExtractionFormFileVerticalLayout->addLayout(groupBoxExtractionFromFileProcess);

  pushButtonExtractionFromFileProcess = new QPushButton(groupBoxExtractionFromFile);
  pushButtonExtractionFromFileProcess->setObjectName(QString::fromUtf8("pushButtonExtractionFromFileProcess"));
  groupBoxExtractionFromFileProcess->addWidget(pushButtonExtractionFromFileProcess);

  // ---------- Spacer ----------
  pageFileExtractionVerticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  pageFileExtractionVerticalLayout->addItem(pageFileExtractionVerticalSpacer);

  //================================================================
  // ********** Matrix Extraction page **********

  pageMatrixExtractionVerticalLayout = new QVBoxLayout(pageExtractionMatrix);
  pageMatrixExtractionVerticalLayout->setObjectName(QString::fromUtf8("pageMatrixExtractionVerticalLayout"));

  // elementary matrix extraction group
  groupBoxExtractionElementaryMatrix = new QGroupBox(pageExtractionMatrix);
  groupBoxExtractionElementaryMatrix->setObjectName(QString::fromUtf8("groupBoxExtractionElementaryMatrix"));
  groupBoxExtractionElementaryMatrix->setEnabled(true);
  pageMatrixExtractionVerticalLayout->addWidget(groupBoxExtractionElementaryMatrix);

  groupBoxExtractionElementaryMatrixVerticalLayout = new QVBoxLayout(groupBoxExtractionElementaryMatrix);
  groupBoxExtractionElementaryMatrixVerticalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionElementaryMatrixVerticalLayout"));

  // Size
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout = new QHBoxLayout;
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionElementaryMatrixSizeHorizontalLayout"));
  groupBoxExtractionElementaryMatrixVerticalLayout->addLayout(groupBoxExtractionElementaryMatrixSizeHorizontalLayout);

  labelExtractionMatrixElementarySize = new QLabel(groupBoxExtractionElementaryMatrix);
  labelExtractionMatrixElementarySize->setObjectName(QString::fromUtf8("labelExtractionMatrixElementarySize"));
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->addWidget(labelExtractionMatrixElementarySize);

  comboBoxExtractionMatrixElementarySize = new QComboBox(groupBoxExtractionElementaryMatrix);
  comboBoxExtractionMatrixElementarySize->setObjectName(QString::fromUtf8("comboBoxExtractionMatrixElementarySize"));
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->addWidget(comboBoxExtractionMatrixElementarySize);

  enableExtractionMatrixElementaryHorizontalSpacer = new QSpacerItem(140, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->addItem(enableExtractionMatrixElementaryHorizontalSpacer);

  labelExtractionMatrixElementaryPeriod = new QLabel(groupBoxExtractionElementaryMatrix);
  labelExtractionMatrixElementaryPeriod->setObjectName(QString::fromUtf8("labelExtractionMatrixElementaryPeriod"));
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->addWidget(labelExtractionMatrixElementaryPeriod);

  spinBoxExtractionMatrixElementaryPeriod = new QSpinBox(groupBoxExtractionElementaryMatrix);
  spinBoxExtractionMatrixElementaryPeriod->setObjectName(QString::fromUtf8("spinBoxExtractionMatrixElementaryPeriod"));
  spinBoxExtractionMatrixElementaryPeriod->setMinimum(13);
  spinBoxExtractionMatrixElementaryPeriod->setValue(13);
  spinBoxExtractionMatrixElementaryPeriod->setMaximum(200);
  spinBoxExtractionMatrixElementaryPeriod->setFixedHeight(24);
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->addWidget(spinBoxExtractionMatrixElementaryPeriod);

  labelExtractionMatrixElementaryPeriodUnit = new QLabel(groupBoxExtractionElementaryMatrix);
  labelExtractionMatrixElementaryPeriodUnit->setObjectName(QString::fromUtf8("labelExtractionMatrixElementaryPeriodUnit"));
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->addWidget(labelExtractionMatrixElementaryPeriodUnit);

  matrixElementaryExtractionHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  groupBoxExtractionElementaryMatrixSizeHorizontalLayout->addItem(matrixElementaryExtractionHorizontalSpacer);

  //Matrix filename
  groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout = new QHBoxLayout();
  groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout->setObjectName(QString::fromUtf8("groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout"));

  labelExtractionMatrixElementaryOutputFilename = new QLabel(groupBoxExtractionElementaryMatrix);
  labelExtractionMatrixElementaryOutputFilename->setObjectName(QString::fromUtf8("labelExtractionMatrixElementaryOutputFilename"));
  groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout->addWidget(labelExtractionMatrixElementaryOutputFilename);

  lineEditExtractionMatrixElementaryOutputFilename = new QLineEdit(groupBoxExtractionElementaryMatrix);
  lineEditExtractionMatrixElementaryOutputFilename->setObjectName(QString::fromUtf8("lineEditExtractionMatrixElementaryOutputFilename"));
  lineEditExtractionMatrixElementaryOutputFilename->setFixedHeight(24);
  lineEditExtractionMatrixElementaryOutputFilename->setMinimumWidth(350);
  lineEditExtractionMatrixElementaryOutputFilename->setMaximumWidth(1000);
  lineEditExtractionMatrixElementaryOutputFilename->setText("");
  groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout->addWidget(lineEditExtractionMatrixElementaryOutputFilename);

  pushButtonExtractionMatrixElementaryOutputBrowseFilename = new QPushButton(groupBoxExtractionElementaryMatrix);
  pushButtonExtractionMatrixElementaryOutputBrowseFilename->setObjectName(QString::fromUtf8("pushButtonExtractionMatrixElementaryOutputBrowseFilename"));
  pushButtonExtractionMatrixElementaryOutputBrowseFilename->setFixedSize(24, 24);
  groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout->addWidget(pushButtonExtractionMatrixElementaryOutputBrowseFilename);

  groupBoxExtractionElementaryMatrixVerticalLayout->addLayout(groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout);

  //Create button

  groupBoxExtractionMatrixCreateElementary = new QHBoxLayout;
  groupBoxExtractionMatrixCreateElementary->setObjectName(QString::fromUtf8("groupBoxExtractionMatrixCreateElementary"));

  groupBoxExtractionElementaryMatrixVerticalLayout->addLayout(groupBoxExtractionMatrixCreateElementary);

  pushButtonExtractionMatrixCreateElementary = new QPushButton(groupBoxExtractionElementaryMatrix);
  pushButtonExtractionMatrixCreateElementary->setObjectName(QString::fromUtf8("pushButtonExtractionMatrixCreateElementary"));
  groupBoxExtractionMatrixCreateElementary->addWidget(pushButtonExtractionMatrixCreateElementary);

  //matrix extraction group
  groupBoxExtractionMatrix = new QGroupBox(pageExtractionMatrix);
  groupBoxExtractionMatrix->setObjectName(QString::fromUtf8("groupBoxExtractionMatrix"));
  groupBoxExtractionMatrix->setEnabled(true);
  pageMatrixExtractionVerticalLayout->addWidget(groupBoxExtractionMatrix);

  vBoxExtractionMatrix = new QVBoxLayout(groupBoxExtractionMatrix);
  vBoxExtractionMatrix->setObjectName(QString::fromUtf8("vBoxExtractionMatrix"));

  hBoxExtractionMatrixInput = new QHBoxLayout;
  hBoxExtractionMatrixInput->setObjectName(QString::fromUtf8("hBoxExtractionMatrixInput"));
  vBoxExtractionMatrix->addLayout(hBoxExtractionMatrixInput);

  labelExtractionMatrixInput = new QLabel(groupBoxExtractionMatrix);
  labelExtractionMatrixInput->setObjectName(QString::fromUtf8("labelExtractionMatrixInput"));
  hBoxExtractionMatrixInput->addWidget(labelExtractionMatrixInput);

  pushButtonExtractionMatrixInputAdd = new QPushButton(groupBoxExtractionMatrix);
  pushButtonExtractionMatrixInputAdd->setObjectName(QString::fromUtf8("pushButtonExtractionMatrixInputAdd"));
  pushButtonExtractionMatrixInputAdd->setFixedSize(24, 24);
  hBoxExtractionMatrixInput->addWidget(pushButtonExtractionMatrixInputAdd);

  pushButtonExtractionMatrixInputRemove = new QPushButton(groupBoxExtractionMatrix);
  pushButtonExtractionMatrixInputRemove->setObjectName(QString::fromUtf8("pushButtonExtractionMatrixInputRemove"));
  pushButtonExtractionMatrixInputRemove->setFixedSize(24, 24);
  hBoxExtractionMatrixInput->addWidget(pushButtonExtractionMatrixInputRemove);

  hBoxExtractionMatrixInputFiles = new QHBoxLayout;
  hBoxExtractionMatrixInputFiles->setObjectName(QString::fromUtf8("hBoxExtractionMatrixInputFiles"));
  vBoxExtractionMatrix->addLayout(hBoxExtractionMatrixInputFiles);

  stringListModelMatrixInputFiles = new QStringListModel();

  listViewExtractionMatrixInputFiles = new QListView(groupBoxExtractionMatrix);
  listViewExtractionMatrixInputFiles->setObjectName(QString::fromUtf8("listViewExtractionMatrixInputFiles"));
  listViewExtractionMatrixInputFiles->setModel(stringListModelMatrixInputFiles);
  listViewExtractionMatrixInputFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

  hBoxExtractionMatrixInputFiles->addWidget(listViewExtractionMatrixInputFiles);

  hBoxExtractionMatrixOutputFilename = new QHBoxLayout;
  hBoxExtractionMatrixOutputFilename->setObjectName(QString::fromUtf8("hBoxExtractionMatrixOutputFilename"));
  vBoxExtractionMatrix->addLayout(hBoxExtractionMatrixOutputFilename);

  labelExtractionMatrixOutputFilename = new QLabel(groupBoxExtractionMatrix);
  labelExtractionMatrixOutputFilename->setObjectName(QString::fromUtf8("labelExtractionMatrixOutputFilename"));
  hBoxExtractionMatrixOutputFilename->addWidget(labelExtractionMatrixOutputFilename);

  lineEditExtractionMatrixOutputFilename = new QLineEdit(groupBoxExtractionMatrix);
  lineEditExtractionMatrixOutputFilename->setObjectName(QString::fromUtf8("lineEditExtractionMatrixOutputFilename"));
  lineEditExtractionMatrixOutputFilename->setFixedHeight(24);
  lineEditExtractionMatrixOutputFilename->setMinimumWidth(350);
  lineEditExtractionMatrixOutputFilename->setMaximumWidth(1000);
  lineEditExtractionMatrixOutputFilename->setText("");
  hBoxExtractionMatrixOutputFilename->addWidget(lineEditExtractionMatrixOutputFilename);

  pushButtonExtractionMatrixOutputBrowseFilename = new QPushButton(groupBoxExtractionMatrix);
  pushButtonExtractionMatrixOutputBrowseFilename->setObjectName(QString::fromUtf8("pushButtonExtractionMatrixOutputBrowseFilename"));
  pushButtonExtractionMatrixOutputBrowseFilename->setFixedSize(24, 24);
  hBoxExtractionMatrixOutputFilename->addWidget(pushButtonExtractionMatrixOutputBrowseFilename);

  hBoxExtractionMatrixCreate = new QHBoxLayout;
  hBoxExtractionMatrixCreate->setObjectName(QString::fromUtf8("hBoxExtractionMatrixCreate"));
  vBoxExtractionMatrix->addLayout(hBoxExtractionMatrixCreate);

  pushButtonExtractionMatrixCreate = new QPushButton(groupBoxExtractionMatrix);
  pushButtonExtractionMatrixCreate->setObjectName(QString::fromUtf8("pushButtonExtractionMatrixCreate"));
  hBoxExtractionMatrixCreate->addWidget(pushButtonExtractionMatrixCreate);

  // ---------- Spacer ----------
  pageMatrixExtractionVerticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  pageMatrixExtractionVerticalLayout->addItem(pageMatrixExtractionVerticalSpacer);

  // ----------------------------- End of Setup UI -----------------------------

  // Center window on screen
  QDesktopWidget *desktop = QApplication::desktop();
  int mainWindowX = (desktop->width() - width()) / 2;
  int mainWindowY = (desktop->height() - height()) / 2 - 50;
  move(mainWindowX, mainWindowY);

  // connects matching signals objects in this main window to slots of object
  // that follow the following form:
  // void on_<widget name>_<signal name>(<signal parameters>);
  QMetaObject::connectSlotsByName(this);

  // Set texts
  TranslateUi();

  // Fill Quantis list
  pushButtonRefreshQuantisList->click();

  // Select Data Acquisition page
  //centralStackedWidget->setCurrentIndex(IDX_PAGE_MAIN__DATA_ACQUISITION);

  // Select default binary data format
  radioButtonDataFormatBinary->setChecked(true);
  radioButtonDataOutputFile->setChecked(true);

  doubleSpinBoxDataSize->setMaximum(static_cast<double>(numeric_limits<int>::max()));
  doubleSpinBoxDataSize->setValue(1024); // TODO: user last value
}

void idQ::EasyQuantis::EasyQuantisGuiMain::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type())
  {
  case QEvent::LanguageChange:
    TranslateUi();
    break;

  default:
    break;
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::GetCurrentQuantisInfo(
    QuantisDeviceType *deviceType,
    unsigned int *deviceNumber)
{
  int idx = comboBoxQuantisList->currentIndex();
  QByteArray quantisData = comboBoxQuantisList->itemData(idx).toByteArray();
  *deviceType = static_cast<QuantisDeviceType>(quantisData.at(0));
  *deviceNumber = static_cast<unsigned int>(quantisData.at(1));
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_actionFileExit_triggered()
{
  setEnabled(false);
  close();
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_actionHelpAbout_triggered()
{
  setEnabled(false);

  // TODO: use QTRANSLATE
  QString msg = EASY_QUANTIS_APPNAME;
  msg.append(" version ");
  msg.append(EASY_QUANTIS_VERSION);
  msg.append("\nCopyright 2004-2020 ID Quantique");

  // TODO: create dialog box
  QMessageBox messageBox(QMessageBox::Information,
                         QString(EASY_QUANTIS_APPTITLE),
                         msg,
                         QMessageBox::Ok | QMessageBox::Default,
                         this);
  messageBox.exec();

  setEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_doubleSpinBoxDataScalingMin_valueChanged(double value)
{
  if (value >= doubleSpinBoxDataScalingMax->value())
  {
    doubleSpinBoxDataScalingMax->setValue(value + 1.0f);
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_doubleSpinBoxDataScalingMax_valueChanged(double value)
{
  if (value <= doubleSpinBoxDataScalingMin->value())
  {
    doubleSpinBoxDataScalingMin->setValue(value - 1.0f);
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_doubleSpinBoxDataSize_valueChanged(double /*value*/)
{
  UpdateLabelDataSize2();
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonDataGenerate_clicked()
{
  // Disable main window
  setEnabled(false);

  // Update status bar
  statusbar->showMessage(QTRANSLATE("Initializing..."));

  // Retrieve needed information
  RandomDataGenerationInfo randomDataGenerationInfo;

  randomDataGenerationInfo.count = static_cast<unsigned long long>(doubleSpinBoxDataSize->value());
  GetCurrentQuantisInfo(&randomDataGenerationInfo.deviceType,
                        &randomDataGenerationInfo.deviceNumber);
  randomDataGenerationInfo.scaleData = groupBoxDataScaling->isChecked();
  randomDataGenerationInfo.min = doubleSpinBoxDataScalingMin->value();
  randomDataGenerationInfo.max = doubleSpinBoxDataScalingMax->value();

  int dataSeparatorIdx = comboBoxDataFormat->currentIndex();
  QVariant qDataSeparator = comboBoxDataFormat->itemData(dataSeparatorIdx);
  randomDataGenerationInfo.dataSeparator = qDataSeparator.toString().toStdString();

  if (radioButtonDataFormatBinary->isChecked())
  {
    randomDataGenerationInfo.dataType = RANDOM_DATA_TYPE_BINARY;
  }
  else if (radioButtonDataFormatIntegers->isChecked())
  {
    randomDataGenerationInfo.dataType = RANDOM_DATA_TYPE_INTEGERS;
  }
  else if (radioButtonDataFormatFloats->isChecked())
  {
    randomDataGenerationInfo.dataType = RANDOM_DATA_TYPE_FLOATS;
  }

  // Output filename
  std::string filename;
  if (radioButtonDataOutputScreen->isChecked())
  {
    QTemporaryFile temporaryFile; // destroy file on destruction
    if (temporaryFile.open())
    {
      // Returns the unique file name
      filename = temporaryFile.fileName().toStdString();
    }
    temporaryFile.close();
  }
  else
  {
    filename = QDir::fromNativeSeparators(lineEditDataOutputFilename->text()).toStdString();
  }

  if (groupBoxExtractionEnable->isChecked())
  {
    QFile matrixFile(lineEditExtractionMatrixFilename->text());

    if (!matrixFile.exists())
    {
      QMessageBox messageBox(QMessageBox::Critical,
                             QString(EASY_QUANTIS_APPTITLE),
                             "The matrix file '" + matrixFile.fileName() + "' does not exist",
                             QMessageBox::Ok | QMessageBox::Default,
                             this);
      messageBox.exec();
      setEnabled(true);
      statusbar->showMessage(QTRANSLATE(""));
      return;
    }

    qint64 matrixFileSize;
    matrixFileSize = matrixFile.size();

    qint64 requiredMatrixSize;

    if (comboBoxExtractionSize->currentIndex() == 0)
    {
      requiredMatrixSize = 1024 * 768 / 8;
    }
    else if (comboBoxExtractionSize->currentIndex() == 1)
    {
      requiredMatrixSize = 2048 * 1792 / 8;
    }
    else
    {
      QMessageBox messageBox(QMessageBox::Critical,
                             QString(EASY_QUANTIS_APPTITLE),
                             "Unexpected matrix size",
                             QMessageBox::Ok | QMessageBox::Default,
                             this);
      messageBox.exec();
      setEnabled(true);
      return;
    }

    if (matrixFileSize < requiredMatrixSize)
    {
      QMessageBox messageBox(QMessageBox::Critical,
                             QString(EASY_QUANTIS_APPTITLE),
                             "The size of " + matrixFile.fileName() + " is too small for the required matrix size",
                             QMessageBox::Ok | QMessageBox::Default,
                             this);
      messageBox.exec();
      setEnabled(true);
      statusbar->showMessage(QTRANSLATE(""));
      return;
    }

    randomDataGenerationInfo.extractorEnabled = true;
    randomDataGenerationInfo.extractorMatrixFilename = matrixFile.fileName().toStdString();

    if (comboBoxExtractionSize->currentIndex() == 0)
    {
      randomDataGenerationInfo.extractorMatrixSizeIn = 1024;
      randomDataGenerationInfo.extractorMatrixSizeOut = 768;
    }
    else if (comboBoxExtractionSize->currentIndex() == 1)
    {
      randomDataGenerationInfo.extractorMatrixSizeIn = 2048;
      randomDataGenerationInfo.extractorMatrixSizeOut = 1792;
    }
  }
  else
  {
    randomDataGenerationInfo.extractorEnabled = false;
  }

  statusbar->showMessage(QTRANSLATE("Generating random data..."));

  // Creates the progress dialog
  EasyQuantisGuiProgressDialog progressDialog(this);
  progressDialog.setLabel1Text(QTRANSLATE("Generating random data..."));
  progressDialog.setValue(0);
  progressDialog.setLabel2Text(QTRANSLATE("Initializing..."));
  qApp->processEvents();

  // Creates Quantis2File
  Quantis2File quantis2File;

  // Get start time
  posix_time::ptime timeStart = posix_time::second_clock::local_time();

  // Error message returned on the thread
  string errorMessage;

  // Launch data acquisition
  boost::thread threadGenerate(boost::function<void()>(boost::bind(&Quantis2File::GenerateRandomFile,
                                                                   &quantis2File,
                                                                   &randomDataGenerationInfo,
                                                                   filename,
                                                                   &errorMessage)));

  // Wait some time to be sure the thread is started
  boost::this_thread::sleep(boost::posix_time::millisec(100));

  // Update progress dialog
  while (quantis2File.GetRemainingSize() > 0u)
  {
    int progressValue = 0;

    string details = Utils::BuildProgressInfoString(timeStart,
                                                    randomDataGenerationInfo,
                                                    quantis2File.GetRemainingSize(),
                                                    progressValue);
    if (progressDialog.wasCanceled())
    {
      quantis2File.CancelRead2File();
      break;
    }
    else if (!errorMessage.empty())
    {
      break;
    }
    else
    {
      progressDialog.setValue(progressValue);
      progressDialog.setLabel2Text(QString::fromStdString(details));
      qApp->processEvents();
    }

    // Wait some time: it is useless and just time-consuming to update too
    // frequently the dialog
    boost::this_thread::sleep(boost::posix_time::millisec(500));
  } // while

  // Waits until background thread terminates
  threadGenerate.join();

  // Hides the dialog
  progressDialog.hide();

  if (!errorMessage.empty())
  {
    // Status message
    statusbar->showMessage("Error while generating random data", 3000);

    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           QString::fromStdString(errorMessage),
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }
  else if (progressDialog.wasCanceled())
  {
    // Status message
    statusbar->showMessage("Random data generation aborted by user", 3000);
  }
  else
  {
    // Status message
    statusbar->showMessage("Random data successfully generated", 3000);

    if (radioButtonDataOutputScreen->isChecked())
    {
      QFile file(QString::fromStdString(filename));
      if (file.open(QIODevice::ReadOnly))
      {
        QString randomData;
        randomData.append(file.readAll());
        EasyQuantisGuiTextEditDialog randomDataDialog(this, EASY_QUANTIS_APPTITLE, randomData);
        randomDataDialog.exec();
        file.close();
      }
      else
      {
        // TODO: better message
        QMessageBox messageBox(QMessageBox::Critical,
                               QString(EASY_QUANTIS_APPTITLE),
                               "Unable to read from temporary file",
                               QMessageBox::Ok | QMessageBox::Default,
                               this);
        messageBox.exec();
      }
      try
      {
        if (boost::filesystem::exists(filename))
        {
          boost::filesystem::remove(filename);
        }
      }
      catch (boost::exception &ex)
      {
        // Silently ignore error
      }
    }
    else
    {
      // Display message
      // TODO: better message
      QMessageBox messageBox(QMessageBox::Information,
                             QString(EASY_QUANTIS_APPTITLE),
                             "Random data successfully generated",
                             QMessageBox::Ok | QMessageBox::Default,
                             this);
      messageBox.exec();
    }
  }

  // Re-enable main window
  setEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonDataOutputBrowseFilename_clicked()
{
  // Select a directory using file dialog
  QDir directory(lineEditDataOutputFilename->text());
  QString path = QFileDialog::getSaveFileName(this,
                                              QTRANSLATE("Output file"),
                                              directory.path());
  if (!path.isNull())
  {
    lineEditDataOutputFilename->setText(QDir::toNativeSeparators(path));
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionMatrixBrowseFilename_clicked()
{
  // Select a directory using file dialog
  QDir directory(lineEditExtractionMatrixFilename->text());
  QString path;

  path = QFileDialog::getOpenFileName(this,
                                      QTRANSLATE("Matrix file"),
                                      directory.path(),
                                      QTRANSLATE("Matrix Files (*.dat *.*)"));

  if (!path.isNull())
  {
    lineEditExtractionMatrixFilename->setText(QDir::toNativeSeparators(path));
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonQuantisInfo_clicked()
{
  setEnabled(false);

  // TODO: create QDialog
  try
  {
    QString msg;

    QuantisDeviceType deviceType;
    unsigned int deviceNumber;
    GetCurrentQuantisInfo(&deviceType, &deviceNumber);

    switch (deviceType)
    {
    case QUANTIS_DEVICE_PCI:
      msg = "Quantis PCI/PCIe\n";
      break;

    case QUANTIS_DEVICE_USB:
      msg = "Quantis USB\n";
      break;

    default:
      msg = "Unknown Quantis device\n";
      break;
    }

    idQ::Quantis quantis(deviceType, deviceNumber);

    // Board version
    msg.append("Board core version: 0x");
    msg.append(QString::number(quantis.GetBoardVersion(), 16));
    msg.append("\n");

    // Modules counts
    msg.append("Number of modules: ");
    msg.append(QString::number(quantis.GetModulesCount()));
    msg.append("\n");

    string deviceSerial = quantis.GetSerialNumber();
    if (deviceSerial != "S/N not available")
    {
      msg.append("Serial number: ");
      msg.append(QString::fromStdString(deviceSerial));
      msg.append("\n");
    }

    if (deviceType == QUANTIS_DEVICE_USB)
    {
      msg.append("Manufacturer: ");
      msg.append(QString::fromStdString(quantis.GetManufacturer()));
      msg.append("\n");
    }
    QMessageBox messageBox(QMessageBox::Information,
                           QString(EASY_QUANTIS_APPTITLE),
                           msg,
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }
  catch (runtime_error &ex)
  {
    QMessageBox messageBox(QMessageBox::Information,
                           QString(EASY_QUANTIS_APPTITLE),
                           QString::fromStdString(ex.what()),
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }

  setEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonRefreshQuantisList_clicked()
{
  setEnabled(false);

  statusbar->showMessage(QTRANSLATE("Scanning for Quantis devices..."));

  // Clear combo box
  comboBoxQuantisList->clear();

  unsigned int quantisPciCount = Quantis::Count(QUANTIS_DEVICE_PCI);
  unsigned int numSerials = 0;

  for (unsigned int i = 0; i < quantisPciCount; i++)
  {
    QByteArray quantisData;
    quantisData.append(QUANTIS_DEVICE_PCI);   // Device type
    quantisData.append(static_cast<char>(i)); // Device number

    Quantis quantis(QUANTIS_DEVICE_PCI, i);
    QString str = "Quantis PCI #";

    str.append(48 + i); // TODO
    
    string deviceSerial=quantis.GetSerialNumber();
    
    if (deviceSerial != "S/N not available")
    {
      str.append(" (S/N ");
      str.append(QString::fromStdString(deviceSerial));
      str.append(")");
    }

    comboBoxQuantisList->addItem(str, QVariant(quantisData));
  }

  unsigned int quantisUsbCount = Quantis::Count(QUANTIS_DEVICE_USB);
  for (unsigned int i = 0; i < quantisUsbCount; i++)
  {
    try
    {
      QByteArray quantisData;
      quantisData.append(QUANTIS_DEVICE_USB);   // Device type
      quantisData.append(static_cast<char>(i)); // Device number

      Quantis quantis(QUANTIS_DEVICE_USB, i);
      QString str = "Quantis USB #";
      str.append(48 + i); // TODO
      str.append(" (S/N ");
      str.append(QString::fromStdString(quantis.GetSerialNumber()));
      str.append(")");

      comboBoxQuantisList->addItem(str, QVariant(quantisData));
    }
    catch (runtime_error)
    {
      // Do nothing,
    }
  }

  //statusbar->showMessage(QTRANSLATE("Found Quantis devices...");
  QString msg = "Scan complete: found ";
  msg.append(QString::number(comboBoxQuantisList->count()));
  msg.append(" Quantis device(s)");
  statusbar->showMessage(msg, 3000);

  if (comboBoxQuantisList->count() > 0)
  {
    // Enable central widget
    pageDataAcquisition->setEnabled(true);
    pushButtonQuantisInfo->setEnabled(true);
    groupBoxExtractionElementaryMatrix->setEnabled(true);
  }
  else
  {
    // Disable central widget
    pageDataAcquisition->setEnabled(false);
    pushButtonQuantisInfo->setEnabled(false);
    groupBoxExtractionElementaryMatrix->setEnabled(false);

    QByteArray quantisData;
    quantisData.append(-1); // Device type
    quantisData.append(-1); // Device number

    comboBoxQuantisList->addItem(QTRANSLATE("No Quantis device found"), QVariant(quantisData));
  }

  setEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_radioButtonDataFormatBinary_toggled(bool checked)
{
  // Disables data output separator on binary format
  comboBoxDataFormat->setEnabled(!checked);

  // Disables scaling on binary format
  groupBoxDataScaling->setEnabled(!checked);

  // Disables display output on binary format;
  radioButtonDataOutputScreen->setEnabled(!checked);

  if (checked)
  {
    radioButtonDataOutputFile->click();

    UpdateLabelDataSize2();
  }

  TranslateUi();
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_radioButtonDataFormatFloats_toggled(bool checked)
{
  if (checked)
  {
    // Set scaling to handle floats
    doubleSpinBoxDataScalingMin->setMinimum(-numeric_limits<float>::max());
    doubleSpinBoxDataScalingMin->setMaximum(numeric_limits<float>::max());
    doubleSpinBoxDataScalingMin->setDecimals(4);
    doubleSpinBoxDataScalingMin->setFixedSize(128, 26);

    doubleSpinBoxDataScalingMax->setDecimals(4);
    doubleSpinBoxDataScalingMax->setFixedSize(128, 26);
    doubleSpinBoxDataScalingMax->setMinimum(-numeric_limits<float>::max());
    doubleSpinBoxDataScalingMax->setMaximum(numeric_limits<float>::max());

    UpdateLabelDataSize2();
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_radioButtonDataFormatIntegers_toggled(bool checked)
{
  if (checked)
  {
    // Set scaling to handle integers
    doubleSpinBoxDataScalingMin->setMinimum(numeric_limits<int>::min());
    doubleSpinBoxDataScalingMin->setMaximum(numeric_limits<int>::max());
    doubleSpinBoxDataScalingMin->setDecimals(0);
    doubleSpinBoxDataScalingMin->setFixedSize(100, 26);

    doubleSpinBoxDataScalingMax->setMinimum(numeric_limits<int>::min());
    doubleSpinBoxDataScalingMax->setMaximum(numeric_limits<int>::max());
    doubleSpinBoxDataScalingMax->setDecimals(0);
    doubleSpinBoxDataScalingMax->setFixedSize(100, 26);

    UpdateLabelDataSize2();
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_radioButtonDataOutputScreen_toggled(bool checked)
{
  if (checked)
  {
    // Set maximal value of number that can be generated on display
    doubleSpinBoxDataSize->setMaximum(10000);
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_radioButtonDataOutputFile_toggled(bool checked)
{
  if (checked)
  {
    // Set maximal value of number that can be generated to file
    doubleSpinBoxDataSize->setMaximum(static_cast<double>(numeric_limits<int>::max()));
  }

  lineEditDataOutputFilename->setEnabled(checked);
  pushButtonDataOutputBrowseFilename->setEnabled(checked);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionFileMatrixBrowseFilename_clicked()
{
  // Select a directory using file dialog
  QDir directory(lineEditExtractionFileMatrixFilename->text());
  QString path;

  path = QFileDialog::getOpenFileName(this,
                                      QTRANSLATE("Matrix file"),
                                      directory.path(),
                                      QTRANSLATE("Matrix Files (*.dat *.*)"));

  if (!path.isNull())
  {
    lineEditExtractionFileMatrixFilename->setText(QDir::toNativeSeparators(path));
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionFromFileFromBrowseFilename_clicked()
{
  // Select a directory using file dialog
  QDir directory(lineEditExtractionFromFileFrom->text());
  QString path;

  path = QFileDialog::getOpenFileName(this,
                                      QTRANSLATE("Input file"),
                                      directory.path(),
                                      QTRANSLATE("All Files (*.*)"));

  if (!path.isNull())
  {
    lineEditExtractionFromFileFrom->setText(QDir::toNativeSeparators(path));
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionFromFileToBrowseFilename_clicked()
{
  // Select a directory using file dialog
  QDir directory(lineEditExtractionFromFileTo->text());
  QString path;

  path = QFileDialog::getSaveFileName(this,
                                      QTRANSLATE("Output file"),
                                      directory.path());

  if (!path.isNull())
  {
    lineEditExtractionFromFileTo->setText(QDir::toNativeSeparators(path));
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionFromFileProcess_clicked()
{
  // Disable main window
  setEnabled(false);

  // Update status bar
  statusbar->showMessage(QTRANSLATE("Initializing..."));

  // Retrieve needed information
  FileExtractionGenerationInfo fileExtractionGenerationInfo;

  QFile matrixFile(lineEditExtractionFileMatrixFilename->text());

  if (!matrixFile.exists())
  {
    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           "The matrix file '" + matrixFile.fileName() + "' does not exists",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
    setEnabled(true);
    return;
  }

  qint64 matrixFileSize;
  matrixFileSize = matrixFile.size();

  qint64 requiredMatrixSize;

  if (comboBoxExtractionFileSize->currentIndex() == 0)
  {
    requiredMatrixSize = 1024 * 768 / 8;
  }
  else if (comboBoxExtractionFileSize->currentIndex() == 1)
  {
    requiredMatrixSize = 2048 * 1792 / 8;
  }
  else
  {
    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           "Unexpected matrix size",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
    setEnabled(true);
    return;
  }

  if (matrixFileSize < requiredMatrixSize)
  {
    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           "The size of " + matrixFile.fileName() + " is too small for the required matrix size",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
    setEnabled(true);
    return;
  }

  fileExtractionGenerationInfo.extractorMatrixFilename = matrixFile.fileName().toStdString();

  if (comboBoxExtractionFileSize->currentIndex() == 0)
  {
    fileExtractionGenerationInfo.extractorMatrixSizeIn = 1024;
    fileExtractionGenerationInfo.extractorMatrixSizeOut = 768;
  }
  else if (comboBoxExtractionFileSize->currentIndex() == 1)
  {
    fileExtractionGenerationInfo.extractorMatrixSizeIn = 2048;
    fileExtractionGenerationInfo.extractorMatrixSizeOut = 1792;
  }

  //Input file
  QFile inputFile(lineEditExtractionFromFileFrom->text());

  if (!inputFile.exists())
  {
    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           "The input file '" + inputFile.fileName() + "' does not exists",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
    setEnabled(true);
    return;
  }

  fileExtractionGenerationInfo.inputFile = inputFile.fileName().toStdString();
  fileExtractionGenerationInfo.count = inputFile.size();

  // Output filename
  std::string outputFile;
  outputFile = QDir::fromNativeSeparators(lineEditExtractionFromFileTo->text()).toStdString();

  fileExtractionGenerationInfo.outputFile = outputFile;

  statusbar->showMessage(QTRANSLATE("Processing file..."));

  // Creates the progress dialog
  EasyQuantisGuiProgressDialog progressDialog(this);
  progressDialog.setLabel1Text(QTRANSLATE("Extraction processing on file..."));
  progressDialog.setValue(0);
  progressDialog.setLabel2Text(QTRANSLATE("Initializing..."));
  qApp->processEvents();

  // Creates Quantis2File
  Quantis2File quantis2File;

  // Get start time
  posix_time::ptime timeStart = posix_time::second_clock::local_time();

  // Error message returned on the thread
  string errorMessage;

  // Launch data acquisition
  boost::thread threadGenerate(boost::function<void()>(boost::bind(&Quantis2File::ProcessExtractionFile,
                                                                   &quantis2File,
                                                                   &fileExtractionGenerationInfo,
                                                                   &errorMessage)));

  // Wait some time to be sure the thread is started
  boost::this_thread::sleep(boost::posix_time::millisec(100));

  // Update progress dialog
  while (quantis2File.GetRemainingSize() > 0u)
  {
    int progressValue = 0;
    string details;
    details = Utils::BuildProcessExtractionProgressInfoString(timeStart,
                                                              fileExtractionGenerationInfo,
                                                              quantis2File.GetRemainingSize(),
                                                              progressValue);
    if (progressDialog.wasCanceled())
    {
      quantis2File.CancelRead2File();
      break;
    }
    else if (!errorMessage.empty())
    {
      break;
    }
    else
    {
      progressDialog.setValue(progressValue);

      switch (progressValue)
      {
      case 0:
        progressDialog.setLabel2Text("Loading input file...");
        break;

      case 99:
        progressDialog.setLabel2Text("Saving output file...");
        break;

      default:
        progressDialog.setLabel2Text(QString::fromStdString(details));
        break;
      }
      qApp->processEvents();
    }

    // Wait some time: it is useless and just time-consuming to update too
    // frequently the dialog
    boost::this_thread::sleep(boost::posix_time::millisec(500));
  } // while

  // Waits until background thread terminates
  threadGenerate.join();

  // Hides the dialog
  progressDialog.hide();

  if (!errorMessage.empty())
  {
    // Status message
    statusbar->showMessage("Error while processing random data", 3000);

    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           QString::fromStdString(errorMessage),
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }
  else if (progressDialog.wasCanceled())
  {
    // Status message
    statusbar->showMessage("Extraction processing aborted by user", 3000);
  }
  else
  {
    // Status message
    statusbar->showMessage("Extraction processing completed", 3000);

    // Display message
    // TODO: better message
    QMessageBox messageBox(QMessageBox::Information,
                           QString(EASY_QUANTIS_APPTITLE),
                           "Extraction processing completed",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }

  // Re-enable main window
  setEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionMatrixElementaryOutputBrowseFilename_clicked()
{
  // Select a directory using file dialog
  QDir directory(lineEditExtractionMatrixElementaryOutputFilename->text());
  QString path;

  path = QFileDialog::getSaveFileName(this,
                                      QTRANSLATE("Elementary matrix file"),
                                      directory.path());

  if (!path.isNull())
  {
    lineEditExtractionMatrixElementaryOutputFilename->setText(QDir::toNativeSeparators(path));
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionMatrixCreateElementary_clicked()
{
  // Disable main window
  setEnabled(false);

  // Update status bar
  statusbar->showMessage(QTRANSLATE("Initializing..."));

  // Retrieve needed information
  ElementaryMatrixExtractionGenerationInfo elementaryMatrixExtractionGenerationInfo;

  GetCurrentQuantisInfo(&elementaryMatrixExtractionGenerationInfo.deviceType,
                        &elementaryMatrixExtractionGenerationInfo.deviceNumber);

  if (comboBoxExtractionMatrixElementarySize->currentIndex() == 0)
  {
    elementaryMatrixExtractionGenerationInfo.extractorMatrixSizeIn = 1024;
    elementaryMatrixExtractionGenerationInfo.extractorMatrixSizeOut = 768;
  }
  else if (comboBoxExtractionMatrixElementarySize->currentIndex() == 1)
  {
    elementaryMatrixExtractionGenerationInfo.extractorMatrixSizeIn = 2048;
    elementaryMatrixExtractionGenerationInfo.extractorMatrixSizeOut = 1792;
  }

  elementaryMatrixExtractionGenerationInfo.numberOfBytesToSkip = spinBoxExtractionMatrixElementaryPeriod->value();

  elementaryMatrixExtractionGenerationInfo.count = elementaryMatrixExtractionGenerationInfo.extractorMatrixSizeIn * elementaryMatrixExtractionGenerationInfo.extractorMatrixSizeOut / 8;

  // Output filename
  //elementaryMatrixExtractionGenerationInfo.outputElementaryMatrixFile = QDir::fromNativeSeparators(lineEditExtractionMatrixElementaryOutputFilename->text()).toStdString();
  elementaryMatrixExtractionGenerationInfo.outputElementaryMatrixFile = lineEditExtractionMatrixElementaryOutputFilename->text().toStdString();

  if (lineEditExtractionMatrixElementaryOutputFilename->text().size() == 0)
  {
    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           "Elementary filename is missing",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();

    setEnabled(true);
    return;
  }

  statusbar->showMessage(QTRANSLATE("Generating elementary matrix file..."));

  // Creates the progress dialog
  EasyQuantisGuiProgressDialog progressDialog(this);
  progressDialog.setLabel1Text(QTRANSLATE("Generating elementary matrix file..."));
  progressDialog.setValue(0);
  progressDialog.setLabel2Text(QTRANSLATE("Initializing..."));
  qApp->processEvents();

  // Creates Quantis2File
  Quantis2File quantis2File;

  // Get start time
  posix_time::ptime timeStart = posix_time::second_clock::local_time();

  // Error message returned on the thread
  string errorMessage;

  // Launch data acquisition
  boost::thread threadGenerate(boost::function<void()>(boost::bind(&Quantis2File::GenerateExtractorElementaryMatrix,
                                                                   &quantis2File,
                                                                   &elementaryMatrixExtractionGenerationInfo,
                                                                   &errorMessage)));

  // Wait some time to be sure the thread is started
  boost::this_thread::sleep(boost::posix_time::millisec(100));

  // Update progress dialog
  while (quantis2File.GetRemainingSize() > 0u)
  {
    int progressValue = 0;
    string details = Utils::BuildProgressExtractionElementaryMatrixInfoString(timeStart,
                                                                              elementaryMatrixExtractionGenerationInfo,
                                                                              quantis2File.GetRemainingSize(),
                                                                              progressValue);
    if (progressDialog.wasCanceled())
    {
      quantis2File.CancelRead2File();
      break;
    }
    else if (!errorMessage.empty())
    {
      break;
    }
    else
    {
      progressDialog.setValue(progressValue);
      progressDialog.setLabel2Text(QString::fromStdString(details));
      qApp->processEvents();
    }

    // Wait some time: it is useless and just time-consuming to update too
    // frequently the dialog
    boost::this_thread::sleep(boost::posix_time::millisec(500));
  } // while

  // Waits until background thread terminates
  threadGenerate.join();

  // Hides the dialog
  progressDialog.hide();

  if (!errorMessage.empty())
  {
    // Status message
    statusbar->showMessage("Error while generating elementary matrix", 3000);

    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           QString::fromStdString(errorMessage),
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }
  else if (progressDialog.wasCanceled())
  {
    // Status message
    statusbar->showMessage("Elementary matrix generation aborted by user", 3000);
  }
  else
  {
    // Status message
    statusbar->showMessage("Elementary matrix successfully generated", 3000);

    // Display message
    // TODO: better message
    QMessageBox messageBox(QMessageBox::Information,
                           QString(EASY_QUANTIS_APPTITLE),
                           "Elementary matrix successfully generated",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();

    //Add to the list for the final matrix
    QStringList currentStringList;
    currentStringList = stringListModelMatrixInputFiles->stringList();
    currentStringList.append(QDir::fromNativeSeparators(lineEditExtractionMatrixElementaryOutputFilename->text()));
    stringListModelMatrixInputFiles->setStringList(currentStringList);
  }

  // Re-enable main window
  setEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionMatrixInputAdd_clicked()
{
  // Select a directory using file dialog

  QDir directory(lineEditExtractionMatrixElementaryOutputFilename->text());
  QStringList paths;

  paths = QFileDialog::getOpenFileNames(this,
                                        QTRANSLATE("Elementry Matrix file"),
                                        directory.path(),
                                        QTRANSLATE("All Files (*.*)"));

  QStringList currentStringList;

  currentStringList = stringListModelMatrixInputFiles->stringList();

  //currentStringList << paths;

  QStringList::Iterator it = paths.begin();
  while (it != paths.end())
  {
    currentStringList << QDir::toNativeSeparators(*it);
    //qDebug() << "path: " << *it;
    ++it;
  }

  stringListModelMatrixInputFiles->setStringList(currentStringList);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionMatrixInputRemove_clicked()
{
  listViewExtractionMatrixInputFiles->setUpdatesEnabled(false);
  QModelIndexList indexes = listViewExtractionMatrixInputFiles->selectionModel()->selectedIndexes();

  qSort(indexes.begin(), indexes.end());

  for (int i = indexes.count() - 1; i > -1; --i)
  {
    stringListModelMatrixInputFiles->removeRow(indexes.at(i).row());
  }

  listViewExtractionMatrixInputFiles->setUpdatesEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionMatrixOutputBrowseFilename_clicked()
{
  // Select a directory using file dialog
  QDir directory(lineEditExtractionMatrixOutputFilename->text());
  QString path;

  path = QFileDialog::getSaveFileName(this,
                                      QTRANSLATE("Matrix file"),
                                      directory.path());

  if (!path.isNull())
  {
    lineEditExtractionMatrixOutputFilename->setText(QDir::toNativeSeparators(path));
  }
}

void idQ::EasyQuantis::EasyQuantisGuiMain::on_pushButtonExtractionMatrixCreate_clicked()
{
  // Disable main window
  setEnabled(false);

  // Update status bar
  statusbar->showMessage(QTRANSLATE("Initializing..."));

  // Retrieve needed information
  MatrixExtractionGenerationInfo matrixExtractionGenerationInfo;

  if (comboBoxExtractionMatrixElementarySize->currentIndex() == 0)
  {
    matrixExtractionGenerationInfo.extractorMatrixSizeIn = 1024;
    matrixExtractionGenerationInfo.extractorMatrixSizeOut = 768;
  }
  else if (comboBoxExtractionMatrixElementarySize->currentIndex() == 1)
  {
    matrixExtractionGenerationInfo.extractorMatrixSizeIn = 2048;
    matrixExtractionGenerationInfo.extractorMatrixSizeOut = 1792;
  }

  //elementary matrix
  QStringList elementaryMatrixStringList;
  elementaryMatrixStringList = stringListModelMatrixInputFiles->stringList();

  for (int i = 0; i < elementaryMatrixStringList.count(); i++)
  {
    matrixExtractionGenerationInfo.elementaryMatrixFiles.push_back(elementaryMatrixStringList[i].toStdString());
  }

  // Output filename
  matrixExtractionGenerationInfo.outputMatrixFile = QDir::fromNativeSeparators(lineEditExtractionMatrixOutputFilename->text()).toStdString();

  statusbar->showMessage(QTRANSLATE("Generating matrix file..."));

  // Creates Quantis2File
  Quantis2File quantis2File;

  // Get start time
  //posix_time::ptime timeStart = posix_time::second_clock::local_time();

  // Error message returned on the thread
  string errorMessage;

  // Launch data acquisition
  boost::thread threadGenerate(boost::function<void()>(boost::bind(&Quantis2File::GenerateExtractorExtractorMatrix,
                                                                   &quantis2File,
                                                                   &matrixExtractionGenerationInfo,
                                                                   &errorMessage)));

  // Wait some time to be sure the thread is started
  boost::this_thread::sleep(boost::posix_time::millisec(100));

  // Waits until background thread terminates
  threadGenerate.join();

  if (!errorMessage.empty())
  {
    // Status message
    statusbar->showMessage("Error while generating matrix", 3000);

    QMessageBox messageBox(QMessageBox::Critical,
                           QString(EASY_QUANTIS_APPTITLE),
                           QString::fromStdString(errorMessage),
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }
  else
  {
    // Status message
    statusbar->showMessage("Matrix file successfully generated", 3000);

    // Display message
    // TODO: better message
    QMessageBox messageBox(QMessageBox::Information,
                           QString(EASY_QUANTIS_APPTITLE),
                           "Matrix file successfully generated",
                           QMessageBox::Ok | QMessageBox::Default,
                           this);
    messageBox.exec();
  }

  // Re-enable main window
  setEnabled(true);
}

void idQ::EasyQuantis::EasyQuantisGuiMain::TranslateUi()
{
  // ***** Main window *****
  setWindowTitle(QTRANSLATE(EASY_QUANTIS_APPTITLE));

  // ***** Menus *****
  menuFile->setTitle(QTRANSLATE("&File"));
  actionFileExit->setText(QTRANSLATE("E&xit"));

  menuHelp->setTitle(QTRANSLATE("&Help"));
  actionHelpAbout->setText(QTRANSLATE("About"));

  // ***** Toolbar *****
#ifndef QT_NO_TOOLTIP
  comboBoxQuantisList->setToolTip(QTRANSLATE("Quantis device(s) available"));
  pushButtonRefreshQuantisList->setToolTip(QTRANSLATE("Refresh Quantis devices list"));
  pushButtonQuantisInfo->setToolTip(QTRANSLATE("Display info on selected Quantis device"));
#endif // QT_NO_TOOLTIP

  //   // ***** Data acquisition page *****
  groupBoxDataFormat->setTitle(QTRANSLATE("Data format"));
  radioButtonDataFormatBinary->setText(QTRANSLATE("Create binary data"));
  radioButtonDataFormatFloats->setText(QTRANSLATE("Create floating point numbers"));
  radioButtonDataFormatIntegers->setText(QTRANSLATE("Create integer numbers"));

  comboBoxDataFormat->clear();
  //   comboBoxDataFormat->addItem(QTRANSLATE("Comma-separated values (CSV)"), OUTPUT_FORMAT__CSV);
  //   comboBoxDataFormat->addItem(QTRANSLATE("One entry per line"), OUTPUT_FORMAT__ONE_ENTRY_PER_LINE);
  comboBoxDataFormat->addItem(QTRANSLATE("Comma-separated values (CSV)"), ",");
  comboBoxDataFormat->addItem(QTRANSLATE("One entry per line"), "\n");

  // Scaling
  groupBoxDataScaling->setTitle(QTRANSLATE("Scaling"));
  labelDataScaling1->setText(QTRANSLATE("Scale data between"));
  labelDataScaling1->adjustSize();
  labelDataScaling2->setText(QTRANSLATE("and"));
  labelDataScaling2->adjustSize();

  //randomness extraction enable
  groupBoxExtractionEnable->setTitle(QTRANSLATE("Enable randomness extraction"));
  labelExtractionSize->setText(QTRANSLATE("Size"));
  labelExtractionSize->adjustSize();

  comboBoxExtractionSize->clear();
  comboBoxExtractionSize->addItem(QTRANSLATE("1024 x 768 bits"));
  comboBoxExtractionSize->addItem(QTRANSLATE("2048 x 1792 bits"));
  comboBoxExtractionSize->adjustSize();

  labelExtractionMatrixFilename->setText(QTRANSLATE("Matrix filename"));
  pushButtonExtractionMatrixBrowseFilename->setText(QTRANSLATE("..."));
  pushButtonExtractionMatrixBrowseFilename->setToolTip(QTRANSLATE("Browse to select the matrix filename"));

  // Output
  radioButtonDataOutputScreen->setText(QTRANSLATE("Display"));
  radioButtonDataOutputFile->setText(QTRANSLATE("Save to file"));
  pushButtonDataOutputBrowseFilename->setText(QTRANSLATE("..."));
  pushButtonExtractionMatrixBrowseFilename->setToolTip(QTRANSLATE("Browse to define an output filename"));

  // Size
  QString dataSize1Text;
  if (radioButtonDataFormatBinary->isChecked())
  {
    dataSize1Text = QTRANSLATE("Amount of random data (in bytes) to read:");
  }
  else
  {
    dataSize1Text = QTRANSLATE("Quantity of random numbers to read:");
  }
  labelDataSize1->setText(dataSize1Text);
  labelDataSize1->adjustSize();

  pushButtonDataGenerate->setText(QTRANSLATE("Acquisition"));

  //   // ***** Extraction page *****

  groupBoxExtractionFile->setTitle(QTRANSLATE("Matrix"));
  labelExtractionFileSize->setText(QTRANSLATE("Size"));
  labelExtractionFileSize->adjustSize();

  comboBoxExtractionFileSize->clear();
  comboBoxExtractionFileSize->addItem(QTRANSLATE("1024 x 768 bits"));
  comboBoxExtractionFileSize->addItem(QTRANSLATE("2048 x 1792 bits"));
  comboBoxExtractionFileSize->adjustSize();

  labelExtractionFileMatrixFilename->setText(QTRANSLATE("Matrix filename"));
  pushButtonExtractionFileMatrixBrowseFilename->setText(QTRANSLATE("..."));
  pushButtonExtractionFileMatrixBrowseFilename->setToolTip(QTRANSLATE("Browse to select the matrix filename"));

  groupBoxExtractionFromFile->setTitle(QTRANSLATE("Process extraction from file"));

  labelExtractionFromFileFrom->setText(QTRANSLATE("Input file"));
  pushButtonExtractionFromFileFromBrowseFilename->setText(QTRANSLATE("..."));
  pushButtonExtractionFromFileFromBrowseFilename->setToolTip(QTRANSLATE("Browse to select an input filename to extract the randomness"));

  labelExtractionFromFileTo->setText(QTRANSLATE("Output file"));
  pushButtonExtractionFromFileToBrowseFilename->setText(QTRANSLATE("..."));
  pushButtonExtractionFromFileToBrowseFilename->setToolTip(QTRANSLATE("Browse to define an output filename"));

  pushButtonExtractionFromFileProcess->setText(QTRANSLATE("Process"));

  // **** Matrix page

  groupBoxExtractionElementaryMatrix->setTitle(QTRANSLATE("Elementary Matrix"));
  labelExtractionMatrixElementarySize->setText(QTRANSLATE("Size"));
  labelExtractionMatrixElementarySize->adjustSize();

  comboBoxExtractionMatrixElementarySize->clear();
  comboBoxExtractionMatrixElementarySize->addItem(QTRANSLATE("1024 x 768 bits"));
  comboBoxExtractionMatrixElementarySize->addItem(QTRANSLATE("2048 x 1792 bits"));
  comboBoxExtractionMatrixElementarySize->adjustSize();

  labelExtractionMatrixElementaryPeriod->setText(QTRANSLATE("Under sampling period"));
  labelExtractionMatrixElementaryPeriod->adjustSize();

  labelExtractionMatrixElementaryPeriodUnit->setText(QTRANSLATE("bytes"));
  labelExtractionMatrixElementaryPeriodUnit->adjustSize();

  labelExtractionMatrixElementaryOutputFilename->setText(QTRANSLATE("Output filename"));
  pushButtonExtractionMatrixElementaryOutputBrowseFilename->setText(QTRANSLATE("..."));
  pushButtonExtractionMatrixElementaryOutputBrowseFilename->setToolTip(QTRANSLATE("Browse to define the filename of the elementary matrix"));

  pushButtonExtractionMatrixCreateElementary->setText(QTRANSLATE("Create Elementary Matrix"));

  groupBoxExtractionMatrix->setTitle(QTRANSLATE("Matrix"));
  labelExtractionMatrixInput->setText(QTRANSLATE("Input Elementary matrix files"));
  labelExtractionMatrixInput->adjustSize();

  pushButtonExtractionMatrixInputAdd->setText(QTRANSLATE("+"));
  pushButtonExtractionMatrixInputAdd->setToolTip(QTRANSLATE("Browse to add elementary matrix to the list"));

  pushButtonExtractionMatrixInputRemove->setText(QTRANSLATE("-"));
  pushButtonExtractionMatrixInputRemove->setToolTip(QTRANSLATE("Remove the selected elementary matrix from the list"));

  labelExtractionMatrixOutputFilename->setText(QTRANSLATE("Output matrix file"));
  labelExtractionMatrixOutputFilename->adjustSize();

  pushButtonExtractionMatrixOutputBrowseFilename->setText(QTRANSLATE("..."));
  pushButtonExtractionMatrixOutputBrowseFilename->setToolTip(QTRANSLATE("Browse to define the filename of the matrix"));

  pushButtonExtractionMatrixCreate->setText(QTRANSLATE("Create Matrix"));
}

void idQ::EasyQuantis::EasyQuantisGuiMain::UpdateLabelDataSize2()
{
  double value = doubleSpinBoxDataSize->value();
  QString dataSize1Text;
  unsigned long long multiplier = 0;
  if (radioButtonDataFormatBinary->isChecked())
  {
    if (value > 1023.0f)
    {
      // 1KB or more
      multiplier = 1;
    }
  }
  else
  {
    if (radioButtonDataFormatIntegers->isChecked())
    {
      // An integer occupies on average 9 bytes
      // One byte is added for the data separation
      multiplier = 10;
    }
    else if (radioButtonDataFormatFloats->isChecked())
    {
      // A float occupies on average 13 bytes
      // One byte is added for the data separation
      multiplier = 14;
    }
  }

  QString formattedSize;
  if (multiplier != 1)
  {
    // Value is approximated
    formattedSize = "~";
  }

  if (multiplier > 0)
  {
    unsigned long long size = static_cast<unsigned long long>(value) * multiplier;
    formattedSize.append(QString::fromStdString(FormatSize(size)));
  }
  labelDataSize2->setText(formattedSize);
  labelDataSize2->adjustSize();
}
