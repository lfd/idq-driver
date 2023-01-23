/*
 * QuantisAis31 sample for C++
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
 * terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
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
 */

#include <cstdlib>
#include <iomanip>
#include <iostream>

// #include <ctype.h>
// #include <cstdio>
// #include <cstdlib>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#else
#include "getopt.h"
#endif

#include "Quantis/Quantis.hpp"
#include "QuantisAis31/QuantisAis31.hpp"

#ifdef _WIN32
#pragma comment(lib, "Quantis.lib")
#endif

const int BYTES_DEFAULT = 10;
const int BYTES_MAX = 100000;

using namespace std;
using namespace idQ;

static void printUsage()
{
  cout << "Usage: qrng [-i|-p cardNumber|-u cardNumber] [-n bytes]" << endl
       << endl;
  cout << "Options" << endl;
  cout << "  -h : display help screen" << endl;
  cout << "  -i : display information on all cards found" << endl;
  cout << "  -n : set the number of bytes to read (default "
       << BYTES_DEFAULT << ", max " << BYTES_MAX << ")" << endl;
  cout << "  -p : set the PCI card number" << endl;
  cout << "  -u : set the USB card number" << endl;
}

static void _printCardsInfo(QuantisDeviceType deviceType)
{

  try
  {
    // Devices count
    int devicesCount = Quantis::Count(deviceType);
    cout << "  Found " << devicesCount << " card(s)" << endl;

    // Device details
    for (int i = 0; i < devicesCount; i++)
    {
      // Creates a quantis object
      Quantis quantis(deviceType, i);

      // Display device info
      cout << "  - Details for device #" << i << endl;
      cout << "      driver version: " << fixed << setprecision(2) << quantis.GetDriverVersion() << endl;
      cout << "      core version: " << hex << quantis.GetBoardVersion() << endl;
      cout << "      serial number: " << hex << quantis.GetSerialNumber() << endl;
      cout << "      manufacturer: " << hex << quantis.GetManufacturer() << endl;

      // Display device's modules info
      for (int j = 0; j < 4; j++)
      {
        string strMask = "not found";
        string strStatus = "";
        if (quantis.GetModulesMask() & (1 << j))
        {
          strMask = "found";
          if (quantis.GetModulesStatus() & (1 << j))
          {
            strStatus = "(enabled)";
          }
          else
          {
            strStatus = "(disabled)";
          }
        }
        cout << "      module " << j << ": " << strMask << " " << strStatus << endl;
      }
    }
  }
  catch (runtime_error &ex)
  {
    cerr << "Error while getting cards information: " << ex.what() << endl;
  }
}

static void printCardsInfo()
{
  cout << "Displaying cards info:" << endl;

  cout << endl
       << "* Searching for PCI devices..." << endl;
  _printCardsInfo(QUANTIS_DEVICE_PCI);

  cout << endl
       << "* Searching for USB devices..." << endl;
  _printCardsInfo(QUANTIS_DEVICE_USB);
}

static void printRandomData(QuantisDeviceType deviceType,
                            int cardNumber,
                            int bytesNum)
{
  try
  {
    // Creates a quantis object
    QuantisAis31 quantis(deviceType, cardNumber);

    // Read random data
    // NOTE: following example is meant to read small amount of random data
    //       (no more than 5-10Mb of data at once). Please consult the user
    //       manual on how to handle large amount or random data.
    string buffer = quantis.Read(bytesNum);

    // Display buffer in HEX format
    cout << endl
         << "Displaying " << bytesNum << " random bytes in HEX format:" << endl;
    string::iterator it = buffer.begin();

    while (it != buffer.end())
    {
      cout << setw(2) << setfill('0') << hex << static_cast<int>(static_cast<unsigned char>(*it++)) << " ";
    }
    cout << endl;
  }
  catch (runtime_error &ex)
  {
    cerr << "Error while getting random data: " << ex.what() << endl;
  }
}

int main(int argc, char *argv[])
{

  int parameter = 0;
  int printInfo = 0;
  QuantisDeviceType deviceType = QUANTIS_DEVICE_PCI;
  int cardNumber = -1;
  int bytesNum = BYTES_DEFAULT;

  // Initialization
  cout << "*** QuantisAis31 C++ sample ***" << endl;

  // Check if user provided at least on command line argument
  if (argc == 1)
  {
    printUsage();
    exit(EXIT_FAILURE);
  }

  // Parse command line arguments
  while ((parameter = getopt(argc, argv, "hin:p:u:")) >= 0)
  {
    switch (parameter)
    {
    case 'i':
    {
      printInfo = 1;
      break;
    }

    case 'n':
    {
      bytesNum = atoi(optarg);
      if (bytesNum <= 0)
      {
        cerr << "Number of bytes cannot be negative or null! Using n=" << BYTES_DEFAULT << endl;
        bytesNum = BYTES_DEFAULT;
      }
      else if (bytesNum > BYTES_MAX)
      {
        cerr << "Number of bytes cannot be higher than " << BYTES_MAX << "."
                                                                         "Using n="
             << BYTES_MAX << endl;
        bytesNum = BYTES_MAX;
      }
      break;
    }

    case 'p':
    {
      deviceType = QUANTIS_DEVICE_PCI;
      cardNumber = atoi(optarg);
      break;
    }

    case 'u':
    {
      deviceType = QUANTIS_DEVICE_USB;
      cardNumber = atoi(optarg);
      break;
    }

    case 'h':
    default:
    {
      printUsage();
      return -1;
    }
    } // switch
  }   // while

  // Display cards information
  if (printInfo != 0)
  {
    printCardsInfo();
  }

  // Read random data
  if (cardNumber >= 0)
  {
    // print random data
    printRandomData(deviceType, cardNumber, bytesNum);
  }

  exit(EXIT_SUCCESS);
}
