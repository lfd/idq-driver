/*
 * EasyQuantis application
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

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include "EasyQuantisConfig.h"
#endif

#ifndef DISABLE_EASYQUANTIS_GUI
#include "GccWarningWConversionDisable.h"
#include <QtGui/QApplication>
#include "GccWarningWConversionEnable.h"
#include "EasyQuantisGuiMain.hpp"
#endif

#include "EasyQuantisCmd.hpp"

using namespace idQ::EasyQuantis;

int main(int argc, char *argv[])
{
#ifndef DISABLE_EASYQUANTIS_GUI
  if (argc == 1)
  {
#ifdef _WIN32
    // On Windows the application is compiled with /SUBSYSTEM:CONSOLE,
    // thus a console window is always displayed. Here we hide it since
    // not needed
    HWND hwnd = GetConsoleWindow();
    if (hwnd)
    {
      ShowWindow(hwnd, SW_HIDE);
    }
#endif /* _WIN32 */

    // No command line arguments -> display GUI
    QApplication application(argc, argv);

    EasyQuantisGuiMain easyQuantisGuiMain;
    easyQuantisGuiMain.show();
    easyQuantisGuiMain.setFocus();

    return application.exec();
  }
  else
#endif /* DISABLE_EASYQUANTIS_GUI */
  {
    // Uses command line version
    EasyQuantisCmd easyQuantisCmd(argc, argv);
    return easyQuantisCmd.Exec();
  }
}
