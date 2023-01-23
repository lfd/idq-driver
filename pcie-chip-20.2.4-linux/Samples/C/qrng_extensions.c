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
#include "QuantisExtensions/QuantisExtractor.h"

#include <math.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#pragma comment(lib, "Quantis.lib")
//# pragma comment(lib, "Quantis-NoHw.lib")
#endif

#define BYTES_DEFAULT 10
#define BYTES_MAX 8000000

#define DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD 13

#define ACTION_UNDEFINED 0
#define ACTION_EXTRACTION_FROM_QUANTIS 1
#define ACTION_EXTRACTION_FROM_FILE 2
#define ACTION_CREATE_ELEMENTARY_MATRIX_FILE 3
#define ACTION_CREATE_MATRIX_FILE 4

#ifdef _WIN32
static const char MATRIX_FILENAME[] = "..\\..\\..\\..\\Libs-Apps\\QuantisExtensions\\default_idq_matrix.dat";
#else
static const char MATRIX_FILENAME[] = "../../Libs-Apps/QuantisExtensions/default_idq_matrix.dat";
#endif

static void printUsage()
{
  printf("Usage: qrng-extensions [-i|-p cardNumber|-u cardNumber| -f] [-n bytes]\n\n");
  printf("Options\n");
  printf("  -h : display help screen\n");
  printf("  -f : apply extraction from a file\n");
  printf("  -i : display information on all cards found\n");
  printf("  -n : set the number of bytes to read (default %d, max %d)\n", BYTES_DEFAULT, BYTES_MAX);
  printf("  -p : set the PCI card number\n");
  printf("  -u : set the USB card number\n");
  printf("  -b : enable storage buffer (disabled if not set)\n");

  printf("  -e : create elementary matrix\n");
  printf("  -s : set elementary matrix reading subperiod (default %d)\n", DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD);
  printf("  -m : create matrix\n");

  printf("\n");
  printf("Examples:\n");
  printf("  Get 100 bytes from Quantis USB 0:  qrng-extensions -u 0 - n 100\n");
  printf("  Apply extractor from 'input.dat' to 'output.dat':  qrng-extensions -f\n");

  printf("  Create a 1024x768 elementary matrix with 13 bytes\n");
  printf("                                  of undersampling:        qrng-extensions -u 0 -e -s 13\n");
  printf("  Create a 1024x768 matrix from 'elementaryMatrix0.dat'\n");
  printf("                                'elementaryMatrix1.dat'\n");
  printf("                                'elementaryMatrix2.dat':   qrng-extensions -m\n");
  printf("\n");
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

int GetExtractedDataFromQuantis(QuantisDeviceType deviceType,
                                int cardNumber,
                                int bytesNum,
                                uint16_t extractorSizeIn,
                                uint16_t extractorSizeOut,
                                char storageBufferEnable,
                                char *buffer)
{
  int32_t result;            // auxiliary variable used to check the correctness of the output of a function
  int readBytes = 0;         // initialize number of read bytes
  uint64_t *extractorMatrix; // pointer to the buffer where to store the extractor matrix

  //read the extractor matrix file and allocate memory
  result = QuantisExtractorInitializeMatrix((char *)MATRIX_FILENAME,
                                            &extractorMatrix,
                                            extractorSizeIn,
                                            extractorSizeOut);
  if (result != 0)
  {
    fprintf(stderr, "ERROR: printRandomExtractedData: init %s.\n", QuantisExtractorStrError(result));
    return result;
  }

  if (storageBufferEnable == 1)
  {
    QuantisExtractorStorageBufferEnable();
  }

  // Read random data from Quantis and apply the extraction post processing
  readBytes = QuantisExtractorGetDataFromQuantis(deviceType,
                                                 cardNumber,
                                                 buffer,
                                                 bytesNum,
                                                 extractorMatrix);

  if (storageBufferEnable == 1)
  {
    QuantisExtractorStorageBufferDisable();
  }

  // free the buffer which was allocated for the extractor matrix
  QuantisExtractorUninitializeMatrix(&extractorMatrix);

  return readBytes;
}

int GetExtractedDataFromFile(char *inputFilePath,
                             char *outputFilePath,
                             uint16_t extractorSizeIn,
                             uint16_t extractorSizeOut)
{
  int32_t result;            // auxiliary variable used to check the correctness of the output of a function
  int readBytes = 0;         // initialize number of read bytes
  uint64_t *extractorMatrix; // pointer to the buffer where to store the extractor matrix
  uint32_t numberOfBytesAfterExtraction;

  //read the extractor matrix file and allocate memory
  result = QuantisExtractorInitializeMatrix((char *)MATRIX_FILENAME,
                                            &extractorMatrix,
                                            extractorSizeIn,
                                            extractorSizeOut);
  if (result != 0)
  {
    fprintf(stderr, "ERROR: QuantisExtractorInitializeMatrix: init %s.\n", QuantisExtractorStrError(result));
    return result;
  }

  // Read random data from file and apply the extraction post processing
  result = QuantisExtractorGetDataFromFile(inputFilePath,
                                           outputFilePath,
                                           extractorMatrix);

  if (result < 0)
  {
    fprintf(stderr, "ERROR: processing file %s.\n", QuantisExtractorStrError(result));
    return result;
  }
  else
  {
    numberOfBytesAfterExtraction = (uint32_t)result;
  }

  printf("result: %d bytes after extraction\n", numberOfBytesAfterExtraction);

  // free the buffer which was allocated for the extractor matrix
  QuantisExtractorUninitializeMatrix(&extractorMatrix);

  return readBytes;
}

static void printBuffer(char *buffer, int size)
{
  int i;

  /* Display buffer in HEX format */
  printf("\nDisplaying %d extracted random bytes in HEX format:\n", size);

  for (i = 0; i < size; i++)
  {
    printf("%02x ", (unsigned char)buffer[i]);
  }
  printf("\n\n");
}

int main(int argc, char *argv[])
{
  int parameter = 0;
  int printInfo = 0;
  QuantisDeviceType deviceType = QUANTIS_DEVICE_PCI;
  int cardNumber = -1;
  int bytesNum = BYTES_DEFAULT;
  uint16_t extractorSizeIn = 1024;
  uint16_t extractorSizeOut = 768;
  uint8_t storageBufferEnable = 0;
  char inputFilePath[256];
  char outputFilePath[256];

  char elementaryMatrixFilename[256];
  char matrixFilename[256];

  int action = ACTION_UNDEFINED;
  uint16_t elementaryMatrixReadingSubperiod = DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD;

  /* Initialization */
  printf("\n*** Quantis-Extensions C sample ***\n");

  /* Check if user provided at least on command line argument */
  if (argc == 1)
  {
    printUsage();
    exit(EXIT_FAILURE);
  }

  /* Parse command line arguments */
  while ((parameter = getopt(argc, argv, "hbiemfn:p:u:s:")) >= 0)
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
        fprintf(stderr, "number of bytes can not be negative or null! Using n=10\n");
        bytesNum = 10;
      }
      else if (bytesNum > BYTES_MAX)
      {
        fprintf(stderr, "number of bytes can not be higher than %d! Using n=%d\n", BYTES_MAX, BYTES_MAX);
        bytesNum = BYTES_MAX;
      }
      break;
    }

    case 'p':
    {
      deviceType = QUANTIS_DEVICE_PCI;
      cardNumber = atoi(optarg);
      action = ACTION_EXTRACTION_FROM_QUANTIS;
      break;
    }

    case 'u':
    {
      deviceType = QUANTIS_DEVICE_USB;
      cardNumber = atoi(optarg);
      action = ACTION_EXTRACTION_FROM_QUANTIS;
      break;
    }

    case 'f':
    {
      sprintf(inputFilePath, "input.dat");
      ;
      sprintf(outputFilePath, "output.dat");
      action = ACTION_EXTRACTION_FROM_FILE;
      break;
    }

    case 'b':
    {
      storageBufferEnable = 1;
      break;
    }

    case 's':
    {
      elementaryMatrixReadingSubperiod = atoi(optarg);
      if (elementaryMatrixReadingSubperiod <= 0)
      {
        fprintf(stderr, "Reading subperiod must > 0. using default subperiod: %d\n", DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD);
        elementaryMatrixReadingSubperiod = DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD;
      }
      break;
    }

    case 'e':
    {
      sprintf(elementaryMatrixFilename, "%s", "elementaryMatrix.dat");
      action = ACTION_CREATE_ELEMENTARY_MATRIX_FILE;
      break;
    }

    case 'm':
    {
      sprintf(matrixFilename, "%s", "matrix.dat");
      action = ACTION_CREATE_MATRIX_FILE;
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
    exit(EXIT_SUCCESS);
  }

  switch (action)
  {
  case ACTION_EXTRACTION_FROM_QUANTIS:
  {
    printf(" Extract data from a Quantis:\n");
    printf(" Matrix size:    %u x %u [bits]\n", extractorSizeIn, extractorSizeOut);
    printf(" Matrix file:    %s\n", MATRIX_FILENAME);

    if (storageBufferEnable)
    {
      printf(" Storage buffer: Enabled (higher throughput)\n");
    }
    else
    {
      printf(" Storage buffer: Disabled (lower memory required)\n");
    }

    /* Read random data */
    if (cardNumber < 0)
    {
      printf(" Card number must >= 0\n");
      exit(EXIT_SUCCESS);
    }
    else
    {
      int readBytes;
      uint8_t *buffer;

      buffer = malloc(sizeof(uint8_t) * bytesNum);

      printf(" Read size:      %u bytes\n", bytesNum);

      readBytes = GetExtractedDataFromQuantis(deviceType,
                                              cardNumber,
                                              bytesNum,
                                              extractorSizeIn,
                                              extractorSizeOut,
                                              storageBufferEnable,
                                              buffer);

      if (readBytes < 0)
      {
        fprintf(stderr, "ERROR: main %s.\n", QuantisExtractorStrError(readBytes));
        exit(EXIT_FAILURE);
      }
      else if (readBytes != bytesNum)
      {
        fprintf(stderr, "ERROR: main: Asked to read %d bytes but %d bytes has been returned\n", bytesNum, readBytes);
        exit(EXIT_FAILURE);
      }

      printBuffer(buffer, readBytes);

      free(buffer);
    }
    break;
  }

  case ACTION_EXTRACTION_FROM_FILE:
  {
    printf(" Extract data from a file:\n");
    printf(" input file:    %s\n", inputFilePath);
    printf(" output file:   %s\n", outputFilePath);

    GetExtractedDataFromFile(inputFilePath,
                             outputFilePath,
                             extractorSizeIn,
                             extractorSizeOut);
    break;
  }

  case ACTION_CREATE_ELEMENTARY_MATRIX_FILE:
  {
    printf(" Create elementary matrix\n");
    printf(" Matrix size:    %u x %u [bits]\n", extractorSizeIn, extractorSizeOut);
    printf(" Reading subperiod: %d bytes\n", elementaryMatrixReadingSubperiod);
    printf(" elementary matrix file: %s \n", elementaryMatrixFilename);

    QuantisExtractorMatrixCreateElementary(deviceType,
                                           cardNumber,
                                           extractorSizeIn,
                                           extractorSizeOut,
                                           elementaryMatrixReadingSubperiod,
                                           elementaryMatrixFilename);
    printf("Done.\n");

    break;
  }

  case ACTION_CREATE_MATRIX_FILE:
  {
    int result;
    uint32_t bytesToXor;

    char *elementaryMatricesToXor[3];

    elementaryMatricesToXor[0] = malloc(sizeof(char) * 256);
    strcpy(elementaryMatricesToXor[0], "elementaryMatrix0.dat");

    elementaryMatricesToXor[1] = malloc(sizeof(char) * 256);
    strcpy(elementaryMatricesToXor[1], "elementaryMatrix1.dat");

    elementaryMatricesToXor[2] = malloc(sizeof(char) * 256);
    strcpy(elementaryMatricesToXor[2], "elementaryMatrix2.dat");

    bytesToXor = (extractorSizeIn * extractorSizeOut) / 8;

    printf("Create matrix in %s\n", matrixFilename);

    result = QuantisExtractorMatrixCreate(3,
                                          bytesToXor,
                                          elementaryMatricesToXor,
                                          matrixFilename);

    free(elementaryMatricesToXor[0]);
    free(elementaryMatricesToXor[1]);
    free(elementaryMatricesToXor[2]);

    if (result != QUANTIS_SUCCESS)
    {
      printf("Failed to create the extractor matrix.\n");
      exit(EXIT_FAILURE);
    }
    else
    {
      printf("Done.\n");
    }
    break;
  }

  case ACTION_UNDEFINED:
  default:
  {
    fprintf(stderr, " No action defined. use -u/-p, -f -e or -m argument\n");
    break;
  }
  }

  exit(EXIT_SUCCESS);
}
