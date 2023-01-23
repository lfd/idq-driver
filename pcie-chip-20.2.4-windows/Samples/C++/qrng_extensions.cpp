/*
 * Quantis-Extensions sample for C++
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

#include <algorithm>
#include <iterator>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#else
#include "getopt.h"
#endif

#include "Quantis/Quantis.hpp"
#include "QuantisExtensions/QuantisExtractor.hpp"

#ifdef _WIN32
#pragma comment(lib, "Quantis.lib")
#endif

const int BYTES_DEFAULT = 10;
const int BYTES_MAX = 100000;

const uint16_t DEFAULT_EXTRACTOR_SIZE_IN = 1024;
const uint16_t DEFAULT_EXTRACTOR_SIZE_OUT = 768;

const uint16_t DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD = 13;

const int ACTION_UNDEFINED = 0;
const int ACTION_EXTRACTION_FROM_QUANTIS = 1;
const int ACTION_EXTRACTION_FROM_FILE = 2;
const int ACTION_CREATE_ELEMENTARY_MATRIX_FILE = 3;
const int ACTION_CREATE_MATRIX_FILE = 4;

using namespace std;
using namespace idQ;

#ifdef _WIN32
static const char MATRIX_FILENAME[] = "..\\..\\..\\..\\Libs-Apps\\QuantisExtensions\\default_idq_matrix.dat";
#else
static const char MATRIX_FILENAME[] = "../../Libs-Apps/QuantisExtensions/default_idq_matrix.dat";
#endif

static void printUsage()
{
  cout << "Usage: qrng-extensions [-i|-p cardNumber|-u cardNumber| -f] [-n bytes]" << endl
       << endl;
  cout << "Options" << endl;
  cout << "  -h : display help screen" << endl;
  cout << "  -i : display information on all cards found" << endl;
  cout << "  -n : set the number of bytes to read (default "
       << BYTES_DEFAULT << ", max " << BYTES_MAX << ")" << endl;
  cout << "  -p : set the PCI card number" << endl;
  cout << "  -u : set the USB card number" << endl;
  cout << "  -f : apply extraction from a file" << endl;
  cout << "  -e : create elementary matrix" << endl;
  cout << "  -s : set elementary matrix reading subperiod (default " << DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD << ")" << endl;
  cout << "  -m : create matrix" << endl;
  cout << endl;
  cout << "Examples:" << endl;
  cout << "  Get 100 extracted bytes from Quantis USB 0:              qrng-extensions -u 0 -n 100" << endl;
  cout << "  Apply extractor from 'input.dat' to 'output.dat':        qrng-extensions -f" << endl;
  cout << "  Create a 1024x768 elementary matrix with 13 bytes" << endl;
  cout << "                                  of undersampling:        qrng-extensions -u 0 -e -s 13" << endl;
  cout << "  Create a 1024x768 matrix from 'elementaryMatrix0.dat'" << endl;
  cout << "                                'elementaryMatrix1.dat'" << endl;
  cout << "                                'elementaryMatrix2.dat':   qrng-extensions -m" << endl;
  cout << endl;
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

static std::string GetExtractedDataFromQuantis(QuantisDeviceType deviceType,
                                               int cardNumber,
                                               int bytesNum,
                                               uint16_t extractorSizeIn,
                                               uint16_t extractorSizeOut,
                                               bool storageBufferEnable)
{
  string outputBuffer;
  idQ::QuantisExtractor quantisExtractor;

  try
  {
    quantisExtractor.InitializeMatrix(MATRIX_FILENAME, extractorSizeIn, extractorSizeOut);

    if (storageBufferEnable)
    {
      quantisExtractor.EnableStorageBuffer();
    }

    outputBuffer = quantisExtractor.GetDataFromQuantis(deviceType, cardNumber, bytesNum);

    if (storageBufferEnable)
    {
      quantisExtractor.DisableStorageBuffer();
    }

    quantisExtractor.UninitializeMatrix();
  }
  catch (runtime_error &ex)
  {
    cerr << "Error: GetExtractedDataFromFile: " << ex.what() << endl;
  }

  return outputBuffer;
}

static void GetExtractedDataFromFile(const std::string &inputFilePath,
                                     const std::string &outputFilePath,
                                     uint16_t extractorSizeIn,
                                     uint16_t extractorSizeOut)
{
  idQ::QuantisExtractor quantisExtractor;

  try
  {
    uint32_t nbrBytesAfterExtraction;

    quantisExtractor.InitializeMatrix(MATRIX_FILENAME, extractorSizeIn, extractorSizeOut);

    nbrBytesAfterExtraction = quantisExtractor.GetDataFromFile(inputFilePath, outputFilePath);

    quantisExtractor.UninitializeMatrix();

    cout << "nbr bytes after extraction : " << nbrBytesAfterExtraction << endl;
  }
  catch (runtime_error &ex)
  {
    cerr << "Error: GetExtractedDataFromFile: " << ex.what() << endl;
  }
}

static void GetExtractedDataFromBuffer(const std::string &inputFilePath,
                                       const std::string &outputFilePath,
                                       uint16_t extractorSizeIn,
                                       uint16_t extractorSizeOut)
{
  idQ::QuantisExtractor quantisExtractor;

  std::ifstream inputFile;

  uint8_t *inputBuffer;
  uint8_t *outputBuffer;
  uint32_t inputBufferSize;
  uint32_t outputBufferSize;

  try
  {
    //load buffer from file
    inputFile.open(inputFilePath.c_str(), ifstream::in | ios::binary);

    if (!inputFile.is_open())
    {
      throw runtime_error("Unable to open input file: '" + inputFilePath + "'");
    }
    inputBufferSize = inputFile.gcount();

    // get length of file:
    inputFile.seekg(0, ios::end);
    inputBufferSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    // allocate memory:
    inputBuffer = new uint8_t[inputBufferSize];

    // read data as a block:
    inputFile.read(reinterpret_cast<char *>(inputBuffer), inputBufferSize);
    inputFile.close();

    //Initialize the extractor
    quantisExtractor.InitializeMatrix(MATRIX_FILENAME, extractorSizeIn, extractorSizeOut);

    outputBufferSize = quantisExtractor.InitializeOutputBuffer(inputBufferSize, &outputBuffer);

    uint32_t elementsExtractorInput = quantisExtractor.GetMatrixSizeIn() / 64;
    uint32_t elementsExtractorOutput = quantisExtractor.GetMatrixSizeOut() / 64;

    uint64_t *inputBuffer64 = reinterpret_cast<uint64_t *>(inputBuffer);

    uint64_t *outputBuffer64 = reinterpret_cast<uint64_t *>(outputBuffer);

    uint32_t numberOfBlocksToProcess = outputBufferSize / (quantisExtractor.GetMatrixSizeOut() / 8);

    //Process the extraction
    for (uint32_t i = 0; i < numberOfBlocksToProcess; i++)
    {
      // perform the extraction on the current chunk of the input buffer and store the result in the output buffer
      quantisExtractor.ProcessBlock(&inputBuffer64[i * elementsExtractorInput],
                                    &outputBuffer64[i * elementsExtractorOutput]);
    }

    //save output buffer to file
    cout << "Writing " << outputBufferSize << " bytes..." << endl;
    std::ofstream outputFile;

    outputFile.open(outputFilePath.c_str(), ios::out | ios::binary);
    if (!outputFile.is_open())
    {
      throw runtime_error("Unable to open output file: '" + outputFilePath + "'");
    }

    outputFile.write(reinterpret_cast<char *>(outputBuffer), outputBufferSize);
    outputFile.close();

    //Free memory
    quantisExtractor.UninitializeOutputBuffer(outputBuffer);
    quantisExtractor.UninitializeMatrix();

    cout << "nbr bytes after extraction : " << dec << outputBufferSize << endl;
  }
  catch (runtime_error &ex)
  {
    cerr << "Error: GetExtractedDataFromBuffer: " << ex.what() << endl;
  }

  free(inputBuffer);
  //delete inputBuffer;
}

static void CreateElementaryMatrix(QuantisDeviceType deviceType,
                                   int deviceNumber,
                                   uint16_t extractorSizeIn,
                                   uint16_t extractorSizeOut,
                                   uint16_t elementaryMatrixReadingSubperiod,
                                   std::string &elementaryMatrixFilename)
{
  uint32_t processedBytes;                             // number of bytes which were output (after under sampling and Von Neumann processing) up to now
  uint32_t sizeOfElementaryMatrix;                     // size of the elementaryMatrixBuffer in bytes
  std::vector<uint8_t> *elementaryMatrixBuffer = NULL; // buffer containing the output elementary matrix

  uint32_t rawChunkSize = 32768;         // size in bytes of the chunk which is read with each call to QuantisRead
  int32_t sizeOfUnderSampledChunkBuffer; // size of underSampledChunkBuffer in bytes
  idQ::QuantisExtractor quantisExtractor;
  uint32_t sizeOfVonNeumannChunkBuffer; // size of vonNeumannChunkBuffer in bytesNum
  int32_t bytesToAppend;

  try
  {
    processedBytes = 0;
    sizeOfElementaryMatrix = extractorSizeIn * extractorSizeOut / 8;

    // allocate memory:
    elementaryMatrixBuffer = new vector<uint8_t>(sizeOfElementaryMatrix);

    while (processedBytes < sizeOfElementaryMatrix)
    {
      cout << processedBytes << " bytes processed" << endl;

      //read and undersample
      std::vector<uint8_t> underSampledChunkBuffer(rawChunkSize / 8); //4096 bytes

      sizeOfUnderSampledChunkBuffer = quantisExtractor.ReadUnderSampled(deviceType, deviceNumber, rawChunkSize, elementaryMatrixReadingSubperiod, underSampledChunkBuffer);
      underSampledChunkBuffer.resize(sizeOfUnderSampledChunkBuffer);

      std::vector<uint8_t> vonNeumannChunkBuffer(rawChunkSize / 16); // 2048 bytes
      sizeOfVonNeumannChunkBuffer = quantisExtractor.ProcessBufferVonNeumann(underSampledChunkBuffer, vonNeumannChunkBuffer);

      // append the processed bytes to vonNeumannBuffer
      if (processedBytes + sizeOfVonNeumannChunkBuffer > sizeOfElementaryMatrix)
      {
        bytesToAppend = sizeOfElementaryMatrix - processedBytes;
      }
      else
      {
        bytesToAppend = sizeOfVonNeumannChunkBuffer;
      }

      vector<uint8_t>::iterator iteratorvonNeumannChunkBufferEnd;
      iteratorvonNeumannChunkBufferEnd = vonNeumannChunkBuffer.begin() + bytesToAppend;

      vector<uint8_t>::iterator iteratorElementaryMatrixBuffer;
      iteratorElementaryMatrixBuffer = elementaryMatrixBuffer->begin() + processedBytes;

      // copy the newly processed bytes into the overall buffer which contains the output of the Von Neumann algorithm
      copy(vonNeumannChunkBuffer.begin(), iteratorvonNeumannChunkBufferEnd, iteratorElementaryMatrixBuffer);

      // update the number of processed bytes
      processedBytes += (uint32_t)bytesToAppend;

    } // end while (processedBytes < sizeOfElementaryMatrix)

    //save output buffer to file
    cout << "Writing " << sizeOfElementaryMatrix << " bytes in " << elementaryMatrixFilename << endl;
    std::ofstream outputFile;

    outputFile.open(elementaryMatrixFilename.c_str(), ios::out | ios::binary);
    if (!outputFile.is_open())
    {
      throw runtime_error("Unable to open output file: '" + elementaryMatrixFilename + "'");
    }

    outputFile.write(reinterpret_cast<const char *>(elementaryMatrixBuffer->data()), sizeOfElementaryMatrix);
    outputFile.close();
  }
  catch (runtime_error &ex)
  {
    cerr << "Error: CreateElementaryMatrix: " << ex.what() << endl;
  }

  delete elementaryMatrixBuffer;
}

static void CreateMatrix(std::vector<std::string> &elementaryMatrixFilenames, std::string &matrixFilename)
{
  idQ::QuantisExtractor quantisExtractor;
  uint32_t matrixSize = (DEFAULT_EXTRACTOR_SIZE_IN * DEFAULT_EXTRACTOR_SIZE_OUT) / 8;

  try
  {
    quantisExtractor.CreateMatrix(matrixSize,
                                  elementaryMatrixFilenames,
                                  matrixFilename);
  }
  catch (runtime_error &ex)
  {
    cerr << "Error: CreateMatrix: " << ex.what() << endl;
  }
}

int main(int argc, char *argv[])
{
  int parameter = 0;
  int printInfo = 0;
  QuantisDeviceType deviceType = QUANTIS_DEVICE_PCI;
  int cardNumber = -1;
  int bytesNum = BYTES_DEFAULT;
  uint16_t extractorSizeIn = DEFAULT_EXTRACTOR_SIZE_IN;
  uint16_t extractorSizeOut = DEFAULT_EXTRACTOR_SIZE_OUT;
  bool storageBufferEnable = false;

  string inputFilename;
  string outputFilename;
  int action = ACTION_UNDEFINED;

  string elementaryMatrixFilename;
  string matrixFilename;
  vector<string> elementaryMatrixFilenames;

  uint16_t elementaryMatrixReadingSubperiod = DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD;

  // Initialization
  cout << "*** Quantis-Extensions C++ sample ***" << endl;

  // Check if user provided at least on command line argument
  if (argc == 1)
  {
    printUsage();
    exit(EXIT_FAILURE);
  }

  // Parse command line arguments
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
      inputFilename = "input.dat";
      outputFilename = "output.dat";
      action = ACTION_EXTRACTION_FROM_FILE;
      break;
    }

    case 'b':
    {
      storageBufferEnable = true;
      break;
    }

    case 's':
    {
      elementaryMatrixReadingSubperiod = atoi(optarg);
      if (elementaryMatrixReadingSubperiod <= 0)
      {
        cerr << "Reading subperiod must > 0. using default subperiod: " << DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD << endl;
        elementaryMatrixReadingSubperiod = DEFAULT_ELEMENTARY_MATRIX_READING_SUBPERIOD;
      }
      break;
    }

    case 'e':
    {
      elementaryMatrixFilename = "elementaryMatrix.dat";
      action = ACTION_CREATE_ELEMENTARY_MATRIX_FILE;
      break;
    }

    case 'm':
    {
      elementaryMatrixFilenames.push_back("elementaryMatrix0.dat");
      elementaryMatrixFilenames.push_back("elementaryMatrix1.dat");
      elementaryMatrixFilenames.push_back("elementaryMatrix2.dat");
      matrixFilename = "matrix.dat";
      action = ACTION_CREATE_MATRIX_FILE;
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
    exit(EXIT_SUCCESS);
  }

  switch (action)
  {
  case ACTION_EXTRACTION_FROM_QUANTIS:
  {
    cout << "Extract data from a Quantis:" << endl;
    cout << " Matrix size:    " << extractorSizeIn << " x " << extractorSizeOut << "[bits]" << endl;
    cout << " Matrix file:    " << MATRIX_FILENAME << endl;

    if (storageBufferEnable)
    {
      cout << " Storage buffer: Enabled (higher throughput)" << endl;
    }
    else
    {
      cout << " Storage buffer: Disabled (lower memory required)" << endl;
    }

    /* Read random data */
    if (cardNumber < 0)
    {
      cerr << "Error Card number must >= 0" << endl;
      exit(EXIT_FAILURE);
    }
    else
    {
      string buffer;
      cout << " Read size:      " << bytesNum << " bytes" << endl;

      buffer = GetExtractedDataFromQuantis(deviceType,
                                           cardNumber,
                                           bytesNum,
                                           extractorSizeIn,
                                           extractorSizeOut,
                                           storageBufferEnable);

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
    break;
  }
  case ACTION_EXTRACTION_FROM_FILE:
  {
    cout << " Extract data from a file:" << endl;
    cout << " Matrix size:    " << extractorSizeIn << " x " << extractorSizeOut << "[bits]" << endl;
    cout << " Matrix file:    " << MATRIX_FILENAME << endl;
    cout << " input file:  " << inputFilename << endl;
    cout << " output file: " << outputFilename << endl;

    /*  GetExtractedDataFromFile(inputFilename,
                              outputFilename,
                              extractorSizeIn,
                              extractorSizeOut);
      */

    GetExtractedDataFromBuffer(inputFilename,
                               outputFilename,
                               extractorSizeIn,
                               extractorSizeOut);
    break;
  }
  case ACTION_CREATE_ELEMENTARY_MATRIX_FILE:
  {
    cout << " Create elementary matrix" << endl;
    cout << " Matrix size:    " << extractorSizeIn << " x " << extractorSizeOut << "[bits]" << endl;
    cout << " Reading subperiod:  " << elementaryMatrixReadingSubperiod << " bytes" << endl;
    cout << " elementary matrix file: " << elementaryMatrixFilename << endl;

    CreateElementaryMatrix(deviceType,
                           cardNumber,
                           extractorSizeIn,
                           extractorSizeOut,
                           elementaryMatrixReadingSubperiod,
                           elementaryMatrixFilename);
    break;
  }
  case ACTION_CREATE_MATRIX_FILE:
  {
    cout << "Create matrix in " << matrixFilename << endl;

    CreateMatrix(elementaryMatrixFilenames, matrixFilename);
    break;
  }

  case ACTION_UNDEFINED:
  default:
  {
    cout << " No action defined. use -u/-p, -f, -e, or -m argument" << endl;
    break;
  }
  }
  exit(EXIT_SUCCESS);
}
