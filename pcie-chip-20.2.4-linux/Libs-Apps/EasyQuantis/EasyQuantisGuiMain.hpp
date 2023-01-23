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

#ifndef EASY_QUANTIS_GUI_MAIN_HPP
#define EASY_QUANTIS_GUI_MAIN_HPP

#include "GccWarningWConversionDisable.h"
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFileDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QStringListModel>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "GccWarningWConversionEnable.h"
#include <string>

#include <Quantis/Quantis.hpp>

#include "EasyQuantisGuiProgressDialog.hpp"
#include "Quantis2File.hpp"

namespace idQ
{
namespace EasyQuantis
{
class EasyQuantisGuiMain : public QMainWindow
{
  Q_OBJECT

public:
  EasyQuantisGuiMain(QWidget *parent = 0);

protected:
  void changeEvent(QEvent *e);
  void TranslateUi();

private slots:
  void on_actionFileExit_triggered();
  void on_actionHelpAbout_triggered();
  void on_doubleSpinBoxDataScalingMin_valueChanged(double value);
  void on_doubleSpinBoxDataScalingMax_valueChanged(double value);
  void on_doubleSpinBoxDataSize_valueChanged(double value);
  void on_pushButtonDataGenerate_clicked();
  void on_pushButtonDataOutputBrowseFilename_clicked();
  void on_pushButtonExtractionMatrixBrowseFilename_clicked();
  void on_pushButtonQuantisInfo_clicked();
  void on_pushButtonRefreshQuantisList_clicked();
  void on_radioButtonDataFormatBinary_toggled(bool checked);
  void on_radioButtonDataFormatFloats_toggled(bool checked);
  void on_radioButtonDataFormatIntegers_toggled(bool checked);
  void on_radioButtonDataOutputScreen_toggled(bool checked);
  void on_radioButtonDataOutputFile_toggled(bool checked);

  void on_pushButtonExtractionFileMatrixBrowseFilename_clicked();
  void on_pushButtonExtractionFromFileFromBrowseFilename_clicked();
  void on_pushButtonExtractionFromFileToBrowseFilename_clicked();
  void on_pushButtonExtractionFromFileProcess_clicked();

  void on_pushButtonExtractionMatrixElementaryOutputBrowseFilename_clicked();
  void on_pushButtonExtractionMatrixCreateElementary_clicked();

  void on_pushButtonExtractionMatrixInputAdd_clicked();
  void on_pushButtonExtractionMatrixInputRemove_clicked();
  void on_pushButtonExtractionMatrixOutputBrowseFilename_clicked();
  void on_pushButtonExtractionMatrixCreate_clicked();

private:
  void UpdateLabelDataSize2();
  void GetCurrentQuantisInfo(QuantisDeviceType *deviceType, unsigned int *deviceNumber);

private:
  // ********** Menu **********
  QMenuBar *menubar;
  QMenu *menuFile;
  QAction *actionFileExit;
  QMenu *menuHelp;
  QAction *actionHelpAbout;

  // ********** Status bar **********
  QStatusBar *statusbar;

  // ********** Toolbar **********
  QToolBar *toolBar;
  QComboBox *comboBoxQuantisList;
  QPushButton *pushButtonQuantisInfo;
  QPushButton *pushButtonRefreshQuantisList;

  // ********** Tabs **********

  QTabWidget *mTabWidget;

  // ********** Raw data acquisition page **********
  QWidget *pageDataAcquisition;
  QVBoxLayout *pageDataAcquisitionVerticalLayout;
  // ---------- Data format ----------
  QGroupBox *groupBoxDataFormat;
  QVBoxLayout *groupBoxDataFormatVerticalLayout;
  QRadioButton *radioButtonDataFormatBinary;
  QRadioButton *radioButtonDataFormatFloats;
  QRadioButton *radioButtonDataFormatIntegers;
  QComboBox *comboBoxDataFormat;
  // ---------- Data scaling ----------
  QGroupBox *groupBoxDataScaling;
  QHBoxLayout *groupBoxDataScalingHorizontalLayout;
  QLabel *labelDataScaling1;
  QDoubleSpinBox *doubleSpinBoxDataScalingMin;
  QLabel *labelDataScaling2;
  QDoubleSpinBox *doubleSpinBoxDataScalingMax;
  QSpacerItem *dataScalingHorizontalSpacer;

  // ---------- Extraction enable  ----------
  QGroupBox *groupBoxExtractionEnable;

  QVBoxLayout *groupBoxExtractionEnableVerticalLayout;
  QHBoxLayout *groupBoxExtractionEnableSizeHorizontalLayout;
  QLabel *labelExtractionSize;
  QComboBox *comboBoxExtractionSize;
  QSpacerItem *enableExtractionHorizontalSpacer;

  QHBoxLayout *groupBoxExtractionEnableMatrixFilenameHorizontalLayout;
  QLabel *labelExtractionMatrixFilename;
  QLineEdit *lineEditExtractionMatrixFilename;
  QPushButton *pushButtonExtractionMatrixBrowseFilename;

  // ---------- Output ----------
  QGroupBox *groupBoxDataOutput;
  QVBoxLayout *groupBoxDataOutputVerticalLayout;
  QRadioButton *radioButtonDataOutputScreen;
  QHBoxLayout *radioButtonOutputFileHorizontalLayout;
  QRadioButton *radioButtonDataOutputFile;
  QLineEdit *lineEditDataOutputFilename;
  QPushButton *pushButtonDataOutputBrowseFilename;
  // ---------- Data size ----------
  QGroupBox *groupBoxDataSize;
  QHBoxLayout *pageDataSizeHorizontalLayout;
  QLabel *labelDataSize1;
  QDoubleSpinBox *doubleSpinBoxDataSize;
  QLabel *labelDataSize2;
  QPushButton *pushButtonDataGenerate;

  // ********** File Extraction page **********
  QWidget *pageFileExtraction;
  QVBoxLayout *pageFileExtractionVerticalLayout;

  QGroupBox *groupBoxExtractionFile;
  QVBoxLayout *groupBoxExtractionFileVerticalLayout;
  QHBoxLayout *groupBoxExtractionFileSizeHorizontalLayout;
  QLabel *labelExtractionFileSize;
  QComboBox *comboBoxExtractionFileSize;
  QSpacerItem *fileExtractionHorizontalSpacer;

  QHBoxLayout *groupBoxExtractionFileMatrixFilenameHorizontalLayout;
  QLabel *labelExtractionFileMatrixFilename;
  QLineEdit *lineEditExtractionFileMatrixFilename;
  QPushButton *pushButtonExtractionFileMatrixBrowseFilename;

  // ---------- Extraction From File  ----------
  QGroupBox *groupBoxExtractionFromFile;

  QVBoxLayout *groupBoxExtractionFormFileVerticalLayout;
  QHBoxLayout *groupBoxExtractionFromFileFromHorizontalLayout;
  QLineEdit *lineEditExtractionFromFileFrom;
  QLabel *labelExtractionFromFileFrom;
  QPushButton *pushButtonExtractionFromFileFromBrowseFilename;

  QHBoxLayout *groupBoxExtractionFromFileToHorizontalLayout;
  QLineEdit *lineEditExtractionFromFileTo;
  QLabel *labelExtractionFromFileTo;
  QPushButton *pushButtonExtractionFromFileToBrowseFilename;

  QHBoxLayout *groupBoxExtractionFromFileProcess;
  QPushButton *pushButtonExtractionFromFileProcess;

  QSpacerItem *pageFileExtractionVerticalSpacer;

  // ********** Matrix Extraction page **********
  QWidget *pageExtractionMatrix;

  QVBoxLayout *pageMatrixExtractionVerticalLayout;

  //create elementary matrix
  QGroupBox *groupBoxExtractionElementaryMatrix;
  QVBoxLayout *groupBoxExtractionElementaryMatrixVerticalLayout;
  QHBoxLayout *groupBoxExtractionElementaryMatrixSizeHorizontalLayout;
  QLabel *labelExtractionMatrixElementarySize;
  QComboBox *comboBoxExtractionMatrixElementarySize;
  QSpacerItem *enableExtractionMatrixElementaryHorizontalSpacer;
  QLabel *labelExtractionMatrixElementaryPeriod;
  QSpinBox *spinBoxExtractionMatrixElementaryPeriod;
  QLabel *labelExtractionMatrixElementaryPeriodUnit;
  QSpacerItem *matrixElementaryExtractionHorizontalSpacer;

  QHBoxLayout *groupBoxExtractionMatrixElementaryOutputFilenameHorizontalLayout;
  QLabel *labelExtractionMatrixElementaryOutputFilename;
  QLineEdit *lineEditExtractionMatrixElementaryOutputFilename;
  QPushButton *pushButtonExtractionMatrixElementaryOutputBrowseFilename;

  QHBoxLayout *groupBoxExtractionMatrixCreateElementary;
  QPushButton *pushButtonExtractionMatrixCreateElementary;

  //create matrix
  QGroupBox *groupBoxExtractionMatrix;
  QVBoxLayout *vBoxExtractionMatrix;
  QHBoxLayout *hBoxExtractionMatrixInput;
  QLabel *labelExtractionMatrixInput;
  QPushButton *pushButtonExtractionMatrixInputAdd;
  QPushButton *pushButtonExtractionMatrixInputRemove;
  QHBoxLayout *hBoxExtractionMatrixInputFiles;
  QListView *listViewExtractionMatrixInputFiles;
  QStringListModel *stringListModelMatrixInputFiles;

  QHBoxLayout *hBoxExtractionMatrixOutputFilename;
  QLabel *labelExtractionMatrixOutputFilename;
  QLineEdit *lineEditExtractionMatrixOutputFilename;
  QPushButton *pushButtonExtractionMatrixOutputBrowseFilename;

  QHBoxLayout *hBoxExtractionMatrixCreate;
  QPushButton *pushButtonExtractionMatrixCreate;

  QSpacerItem *pageMatrixExtractionVerticalSpacer;
};
} // namespace EasyQuantis
} // namespace idQ

#endif // EASY_QUANTIS_GUI_MAIN_HPP
