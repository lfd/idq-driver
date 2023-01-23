/*
 * Quantis sample for the C++11 TRNG interface
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

#include "Quantis_random_device.hpp"
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#else
#include "getopt.h"
#endif

#ifdef _WIN32
#pragma comment(lib, "Quantis.lib")
#endif

const int NUMS_DEFAULT = 10;
const int NUMS_MAX = 100000;

using namespace std;
using namespace idQ;

static void printUsage()
{
  cout << "Usage: qrng [-i|-p cardNumber|-u cardNumber] [-n bytes]" << endl
       << endl;
  cout << "Options" << endl;
  cout << "  -h : display help screen" << endl;
  cout << "  -i : display information on all cards found" << endl;
  cout << "  -n : set the number of integers to read (default "
       << NUMS_DEFAULT << ", max " << NUMS_MAX << ")" << endl;
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
      int driverVersion = quantis.GetDriverVersion();
      cout << "      driver version: " << static_cast<int>(driverVersion / 10)
           << "." << driverVersion % 10 << endl;
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

int main(int argc, char *argv[])
{

  string params = "";
  unsigned int numints = NUMS_DEFAULT;
  string moduleType;
  string moduleNum;

  stringstream temp;
  int parameter = 0;
  int printInfo = 0;
  int cardNumber = -1;
  int bytesNum = NUMS_DEFAULT;

  // Initialization
  cout << "*** Quantis C++11 Usage Example ***" << endl;

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
    case 'u':
    {
      temp << 'u';
      if (temp >> moduleType)
      {
        params.append(moduleType);
      }
      else
      {
        throw std::runtime_error("Unable to convert the module type");
      }
      params.append(string(optarg));
      break;
    }
    case 'p':
    {
      temp << 'p';
      if (temp >> moduleType)
      {
        params.append(moduleType);
      }
      else
      {
        throw std::runtime_error("Unable to convert the module type");
      }
      params.append(string(optarg));
      break;
    }
    case 'n':
    {
      numints = atoi(optarg);
      if (numints <= 0)
      {
        cerr << "Number of bytes cannot be negative or null! Using n=" << NUMS_DEFAULT << endl;
        numints = NUMS_DEFAULT;
      }
      else if (numints > NUMS_MAX)
      {
        cerr << "Number of bytes cannot be higher than " << NUMS_MAX << "."
                                                                        "Using n="
             << NUMS_DEFAULT << endl;
        numints = NUMS_DEFAULT;
      }
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

  if (printInfo != 0)
  {
    printCardsInfo();
  }

  random_device rd(params);

  cout << "\nMinimal possible output value: " << random_device::min() << dec << endl;
  cout << "Maximal possible output value: " << random_device::max() << dec << endl;

  std::cout << "\nExample random numbers:\n\n"
            << std::endl;
  for (unsigned int i = 0; i < numints; ++i)
  {
    std::cout << "  " << rd() << dec << std::endl;
  }
  rd.~random_device();

  exit(EXIT_SUCCESS);
}
