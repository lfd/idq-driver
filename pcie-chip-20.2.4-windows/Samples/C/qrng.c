/*
 * Quantis sample for C
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

#include <float.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#else
#include "getopt.h"
#endif

#include "Quantis/Quantis.h"

#ifdef _WIN32
#pragma comment(lib, "Quantis.lib")
//# pragma comment(lib, "Quantis-NoHw.lib")
#endif

#define BYTES_DEFAULT 10
#define BYTES_MAX 100000

static void printUsage()
{
  printf("Usage: qrng [-i|-p cardNumber|-u cardNumber] [-n bytes]\n\n");
  printf("Options\n");
  printf("  -h : display help screen\n");
  printf("  -i : display information on all cards found\n");
  printf("  -n : set the number of bytes to read (default %d, max %d)\n",
         BYTES_DEFAULT,
         BYTES_MAX);
  printf("  -p : set the PCI card number\n");
  printf("  -u : set the USB card number\n");
}

static void _printCardsInfo(QuantisDeviceType deviceType)
{
  float driverVersion = 0;
  int devicesCount = 0;
  int i = 0;
  int j = 0;

  /* Driver version */
  driverVersion = QuantisGetDriverVersion(deviceType);
  if (driverVersion < 0.0f)
  {
    fprintf(stderr,
            "Error while getting driver version: %s\n",
            QuantisStrError((int)driverVersion));
    return;
  }
  printf("  Using driver version %f\n",
         driverVersion);

  /* Devices count */
  devicesCount = QuantisCount(deviceType);
  printf("  Found %d card(s)\n", devicesCount);

  /* Devices details */
  for (i = 0; i < devicesCount; i++)
  {
    int boardVersion;
    int power;

    /* Display device's info */
    printf("  - Details for device #%d:\n", i);

    /* Board version */
    boardVersion = QuantisGetBoardVersion(deviceType, i);
    if (boardVersion < 0)
    {
      printf("      core version: error (%s)\n", QuantisStrError(boardVersion));
    }
    else
    {
      printf("      core version: 0x%08X\n", boardVersion);
    }

    /* Serial number */
    printf("      serial number: %s\n", QuantisGetSerialNumber(deviceType, i));

    /* Manufacturer */
    printf("      manufacturer: %s\n", QuantisGetManufacturer(deviceType, i));

    /* Modules power */
    printf("      module(s) powered: ");
    power = QuantisGetModulesPower(deviceType, i);
    if (power == 0)
    {
      printf("no\n");
    }
    else if (power > 0)
    {
      printf("yes\n");
    }
    else
    {
      printf("error (%s)\n", QuantisStrError(power));
    }

    /* Display device's modules info */
    for (j = 0; j < 4; j++)
    {
      int result;
      char *strMask = NULL;
      char *strStatus = NULL;

      result = QuantisGetModulesMask(deviceType, i);
      if (result < 0)
      {
        strMask = "error while retrieving mask";
        strStatus = "";
      }
      else if (result & (1 << j))
      {
        strMask = "found";
        result = QuantisGetModulesStatus(deviceType, i);
        if (result < 0)
        {
          strStatus = "(error while retrieving status)";
        }
        else if (result & (1 << j))
        {
          strStatus = "(enabled)";
        }
        else
        {
          strStatus = "(disabled)";
        }
      }
      else
      {
        strMask = "not found";
        strStatus = "";
      }
      printf("      module %d: %s %s\n", j, strMask, strStatus);
    }
  }
}

static void printCardsInfo()
{
  printf("Displaying cards info:\n");

  printf("\n* Searching for PCI devices...\n");
  _printCardsInfo(QUANTIS_DEVICE_PCI);

  printf("\n* Searching for USB devices...\n");
  _printCardsInfo(QUANTIS_DEVICE_USB);
}

static void printRandomData(QuantisDeviceType deviceType,
                            int cardNumber,
                            int bytesNum)
{
  unsigned char *buffer = NULL;
  int readBytes = 0;
  int i = 0;

  /*
   * NOTE: following example is meant to read small amount of random data
   * (no more than 5-10Mb of data at once). Please consult the user manual
   * on how to handle large amount or random data.
   */

  /* Allocate memory */
  buffer = (unsigned char *)malloc(bytesNum);
  if (!buffer)
  {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(EXIT_FAILURE);
  }

  /* Read random data */
  readBytes = QuantisRead(deviceType, cardNumber, buffer, bytesNum);
  if (readBytes < 0)
  {
    fprintf(stderr, "An error occured when reading random bytes: %s\n", QuantisStrError(readBytes));
    goto cleanup;
  }
  else if (readBytes != bytesNum)
  {
    fprintf(stderr, "Asked to read %d byts but %d bytes has been returned\n", bytesNum, readBytes);
    goto cleanup;
  }

  /* Display buffer in HEX format */
  printf("\nDisplaying %d random bytes in HEX format:\n", readBytes);
  for (i = 0; i < readBytes; i++)
  {
    printf("%02x ", (unsigned char)buffer[i]);
  }
  printf("\n");

/* Cleaup */
cleanup:

  if (buffer)
  {
    free(buffer);
  }
}

int main(int argc, char *argv[])
{

  int parameter = 0;
  int printInfo = 0;
  QuantisDeviceType deviceType = QUANTIS_DEVICE_PCI;
  int cardNumber = -1;
  int bytesNum = BYTES_DEFAULT;

  /* Initialization */
  printf("*** Quantis C sample ***\n");

  /* Check if user provided at least on command line argument */
  if (argc == 1)
  {
    printUsage();
    exit(EXIT_FAILURE);
  }

  /* Parse command line arguments */
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
        fprintf(stderr, "number of bytes cannot be negative or null! Using n=10\n");
        bytesNum = 10;
      }
      else if (bytesNum > BYTES_MAX)
      {
        fprintf(stderr, "number of bytes cannot be higher than %d! Using n=%d\n", BYTES_MAX, BYTES_MAX);
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
    } /* switch */
  }   /* while */

  /* Display cards information */
  if (printInfo != 0)
  {
    printCardsInfo();
  }

  /* Read random data */
  if (cardNumber >= 0)
  {
    /* print random data */
    printRandomData(deviceType, cardNumber, bytesNum);
    {
      double d;
      float f;
      int i;
      short s;
      int result;
      result = QuantisReadDouble_01(deviceType, cardNumber, &d);
      printf("\nReadDouble = %e, result = %d\n", d, result);

      result = QuantisReadScaledDouble(deviceType, cardNumber, &d, DBL_MIN, DBL_MAX);
      printf("\nReadDouble2 = %e, result = %d\n", d, result);

      result = QuantisReadFloat_01(deviceType, cardNumber, &f);
      printf("\nReadFloat = %f (%e), result = %d\n", f, f, result);

      result = QuantisReadInt(deviceType, cardNumber, &i);
      printf("\nReadInt = %d, result = %d\n", i, result);

      result = QuantisReadShort(deviceType, cardNumber, &s);
      printf("\nReadShort = %d, result = %d\n", s, result);
    }

    {
      int t, i;
      short s;
      printf("\nReading scaled numbers (ints/shorts between 1 and 9):\n\n");
      for (t = 0; t < 10; t++)
      {
        QuantisReadScaledShort(deviceType, cardNumber, &s, 1, 9);
        printf("%d ", s);
        QuantisReadScaledInt(deviceType, cardNumber, &i, 1, 9);
        printf("%d ", i);
      }
      printf("\n\n");
    }
  }

  return QUANTIS_SUCCESS;

  exit(EXIT_SUCCESS);
}
