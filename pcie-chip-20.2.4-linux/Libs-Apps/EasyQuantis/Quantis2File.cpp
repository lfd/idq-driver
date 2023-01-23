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

#include <limits>

#include "Quantis/Conversion.h"

#include "Quantis2File.hpp"

using namespace std;

/* Set to a multiple of 64 to avoid any problem */
const size_t idQ::EasyQuantis::Quantis2File::CHUNK_SIZE = 8192u;

idQ::EasyQuantis::Quantis2File::Quantis2File() : remaining(0u)
{
}

idQ::EasyQuantis::Quantis2File::~Quantis2File()
{
}

void idQ::EasyQuantis::Quantis2File::CancelRead2File()
{
  canRead = false;
}

unsigned long long idQ::EasyQuantis::Quantis2File::GetRemainingSize() const
{
  return remaining;
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateBinaryFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long size) throw(std::runtime_error)
{
  idQ::Quantis quantis(deviceType, deviceNumber);
  BinaryFileWriter outputFile(filename, discardContent);

  std::string randomData;
  size_t chunkSize = CHUNK_SIZE;
  remaining = size;

  canRead = true;

  while ((remaining > 0u) && canRead)
  {
    // Chunk size
    if (remaining < chunkSize)
    {
      chunkSize = static_cast<size_t>(remaining);
    }

    // Read data
    randomData = quantis.Read(chunkSize);

    // Write data to file
    outputFile.Write(randomData);

    // Update info
    remaining -= chunkSize;
  }

  canRead = false;

  return outputFile.GetSize();
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateIntsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator) throw(std::runtime_error)
{
  return GenerateFormattedFile<int>(deviceType,
                                    deviceNumber,
                                    filename,
                                    discardContent,
                                    count,
                                    dataSeparator,
                                    &Quantis::ReadInt);
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateIntsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    int min,
    int max) throw(std::runtime_error)
{
  if ((min >= static_cast<int>(numeric_limits<short>::min())) &&
      (max <= static_cast<int>(numeric_limits<short>::max())))
  {
    return GenerateFormattedFile<short>(deviceType,
                                        deviceNumber,
                                        filename,
                                        discardContent,
                                        count,
                                        dataSeparator,
                                        NULL,
                                        &Quantis::ReadShort,
                                        static_cast<short>(min),
                                        static_cast<short>(max));
  }
  else
  {
    return GenerateFormattedFile<int>(deviceType,
                                      deviceNumber,
                                      filename,
                                      discardContent,
                                      count,
                                      dataSeparator,
                                      NULL,
                                      &Quantis::ReadInt,
                                      min,
                                      max);
  }
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateFloatsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator) throw(std::runtime_error)
{
  return GenerateFormattedFile<float>(deviceType,
                                      deviceNumber,
                                      filename,
                                      discardContent,
                                      count,
                                      dataSeparator,
                                      &Quantis::ReadFloat);
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateFloatsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    float min,
    float max) throw(std::runtime_error)
{
  return GenerateFormattedFile<float>(deviceType,
                                      deviceNumber,
                                      filename,
                                      discardContent,
                                      count,
                                      dataSeparator,
                                      NULL,
                                      &Quantis::ReadFloat,
                                      min,
                                      max);
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateExtractedBinaryFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long size,
    const int matrixSizeIn,
    const int matrixSizeOut,
    const std::string &matrixFilename) throw(std::runtime_error)
{
  idQ::QuantisExtractor quantisExt;

  BinaryFileWriter outputFile(filename, discardContent);

  std::string randomData;
  size_t chunkSize = CHUNK_SIZE;
  remaining = size;

  canRead = true;

  quantisExt.InitializeMatrix(matrixFilename, static_cast<uint16_t>(matrixSizeIn), static_cast<uint16_t>(matrixSizeOut));

  while ((remaining > 0u) && canRead)
  {
    // Chunk size
    if (remaining < chunkSize)
    {
      chunkSize = static_cast<size_t>(remaining);
    }

    // Read data
    randomData = quantisExt.GetDataFromQuantis(deviceType, deviceNumber, chunkSize);

    // Write data to file
    outputFile.Write(randomData);

    // Update info
    remaining -= chunkSize;
  }

  canRead = false;

  quantisExt.UninitializeMatrix();

  return outputFile.GetSize();
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateExtractedIntsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    const int matrixSizeIn,
    const int matrixSizeOut,
    const std::string &matrixFilename) throw(std::runtime_error)
{
  return GenerateExtractedFormattedFile<int>(deviceType,
                                             deviceNumber,
                                             filename,
                                             discardContent,
                                             count,
                                             dataSeparator,
                                             matrixSizeIn,
                                             matrixSizeOut,
                                             matrixFilename,
                                             /*&Quantis::ReadInt*/
                                             &QuantisExtractor::GetIntFromQuantis);
}

template <typename numberType>
unsigned long long GenerateExtractedFormattedFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    const int matrixSizeIn,
    const int matrixSizeOut,
    const std::string &matrixFilename,
    numberType (idQ::QuantisExtractor::*read)(QuantisDeviceType /*deviceType*/, unsigned int /*deviceNumber*/) const,
    numberType (idQ::QuantisExtractor::*readScaled)(QuantisDeviceType /*deviceType*/, unsigned int /*deviceNumber*/, numberType /*min*/, numberType /*max*/) const = NULL,
    numberType min = 0,
    numberType max = 0) throw(std::runtime_error);

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateExtractedIntsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    const int matrixSizeIn,
    const int matrixSizeOut,
    const std::string &matrixFilename,
    int min,
    int max) throw(std::runtime_error)
{
  if ((min >= static_cast<int>(numeric_limits<short>::min())) &&
      (max <= static_cast<int>(numeric_limits<short>::max())))
  {
    return GenerateExtractedFormattedFile<short>(deviceType,
                                                 deviceNumber,
                                                 filename,
                                                 discardContent,
                                                 count,
                                                 dataSeparator,
                                                 matrixSizeIn,
                                                 matrixSizeOut,
                                                 matrixFilename,
                                                 NULL,
                                                 &QuantisExtractor::GetShortFromQuantis,
                                                 static_cast<short>(min),
                                                 static_cast<short>(max));
  }
  else
  {
    return GenerateExtractedFormattedFile<int>(deviceType,
                                               deviceNumber,
                                               filename,
                                               discardContent,
                                               count,
                                               dataSeparator,
                                               matrixSizeIn,
                                               matrixSizeOut,
                                               matrixFilename,
                                               NULL,
                                               &QuantisExtractor::GetIntFromQuantis,
                                               min,
                                               max);
  }
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateExtractedFloatsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    const int matrixSizeIn,
    const int matrixSizeOut,
    const std::string &matrixFilename) throw(std::runtime_error)
{
  return GenerateExtractedFormattedFile<float>(deviceType,
                                               deviceNumber,
                                               filename,
                                               discardContent,
                                               count,
                                               dataSeparator,
                                               matrixSizeIn,
                                               matrixSizeOut,
                                               matrixFilename,
                                               &QuantisExtractor::GetFloatFromQuantis);
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateExtractedFloatsFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    const int matrixSizeIn,
    const int matrixSizeOut,
    const std::string &matrixFilename,
    float min,
    float max) throw(std::runtime_error)
{
  return GenerateExtractedFormattedFile<float>(deviceType,
                                               deviceNumber,
                                               filename,
                                               discardContent,
                                               count,
                                               dataSeparator,
                                               matrixSizeIn,
                                               matrixSizeOut,
                                               matrixFilename,
                                               NULL,
                                               &QuantisExtractor::GetFloatFromQuantis,
                                               min,
                                               max);
}

void idQ::EasyQuantis::Quantis2File::GenerateRandomFile(
    idQ::EasyQuantis::Quantis2File *quantis2File,
    const idQ::EasyQuantis::RandomDataGenerationInfo *randomDataGenerationInfo,
    const std::string &filename,
    std::string *errorMessage)
{
  try
  {
    if (!randomDataGenerationInfo->extractorEnabled)
    {
      switch (randomDataGenerationInfo->dataType)
      {
      // Binary data
      case RANDOM_DATA_TYPE_BINARY:
        quantis2File->GenerateBinaryFile(randomDataGenerationInfo->deviceType,
                                         randomDataGenerationInfo->deviceNumber,
                                         filename,
                                         true,
                                         randomDataGenerationInfo->count);
        break;

      // Integers
      case RANDOM_DATA_TYPE_INTEGERS:
        if (randomDataGenerationInfo->scaleData)
        {
          quantis2File->GenerateIntsFile(randomDataGenerationInfo->deviceType,
                                         randomDataGenerationInfo->deviceNumber,
                                         filename,
                                         true,
                                         randomDataGenerationInfo->count,
                                         randomDataGenerationInfo->dataSeparator,
                                         static_cast<int>(randomDataGenerationInfo->min),
                                         static_cast<int>(randomDataGenerationInfo->max));
        }
        else
        {
          quantis2File->GenerateIntsFile(randomDataGenerationInfo->deviceType,
                                         randomDataGenerationInfo->deviceNumber,
                                         filename,
                                         true,
                                         randomDataGenerationInfo->count,
                                         randomDataGenerationInfo->dataSeparator);
        }
        break;

      // Floats
      case RANDOM_DATA_TYPE_FLOATS:
        if (randomDataGenerationInfo->scaleData)
        {
          quantis2File->GenerateFloatsFile(randomDataGenerationInfo->deviceType,
                                           randomDataGenerationInfo->deviceNumber,
                                           filename,
                                           true,
                                           randomDataGenerationInfo->count,
                                           randomDataGenerationInfo->dataSeparator,
                                           static_cast<float>(randomDataGenerationInfo->min),
                                           static_cast<float>(randomDataGenerationInfo->max));
        }
        else
        {
          quantis2File->GenerateFloatsFile(randomDataGenerationInfo->deviceType,
                                           randomDataGenerationInfo->deviceNumber,
                                           filename,
                                           true,
                                           randomDataGenerationInfo->count,
                                           randomDataGenerationInfo->dataSeparator);
        }
        break;
      }
    }
    else
    {
      switch (randomDataGenerationInfo->dataType)
      {
      // Extracted Binary data
      case RANDOM_DATA_TYPE_BINARY:
        quantis2File->GenerateExtractedBinaryFile(randomDataGenerationInfo->deviceType,
                                                  randomDataGenerationInfo->deviceNumber,
                                                  filename,
                                                  true,
                                                  randomDataGenerationInfo->count,
                                                  randomDataGenerationInfo->extractorMatrixSizeIn,
                                                  randomDataGenerationInfo->extractorMatrixSizeOut,
                                                  randomDataGenerationInfo->extractorMatrixFilename);
        break;

      // Integers
      case RANDOM_DATA_TYPE_INTEGERS:
        if (randomDataGenerationInfo->scaleData)
        {
          quantis2File->GenerateExtractedIntsFile(randomDataGenerationInfo->deviceType,
                                                  randomDataGenerationInfo->deviceNumber,
                                                  filename,
                                                  true,
                                                  randomDataGenerationInfo->count,
                                                  randomDataGenerationInfo->dataSeparator,
                                                  randomDataGenerationInfo->extractorMatrixSizeIn,
                                                  randomDataGenerationInfo->extractorMatrixSizeOut,
                                                  randomDataGenerationInfo->extractorMatrixFilename,
                                                  static_cast<int>(randomDataGenerationInfo->min),
                                                  static_cast<int>(randomDataGenerationInfo->max));
        }
        else
        {
          quantis2File->GenerateExtractedIntsFile(randomDataGenerationInfo->deviceType,
                                                  randomDataGenerationInfo->deviceNumber,
                                                  filename,
                                                  true,
                                                  randomDataGenerationInfo->count,
                                                  randomDataGenerationInfo->dataSeparator,
                                                  randomDataGenerationInfo->extractorMatrixSizeIn,
                                                  randomDataGenerationInfo->extractorMatrixSizeOut,
                                                  randomDataGenerationInfo->extractorMatrixFilename);
        }
        break;

      // Floats
      case RANDOM_DATA_TYPE_FLOATS:
        if (randomDataGenerationInfo->scaleData)
        {
          quantis2File->GenerateExtractedFloatsFile(randomDataGenerationInfo->deviceType,
                                                    randomDataGenerationInfo->deviceNumber,
                                                    filename,
                                                    true,
                                                    randomDataGenerationInfo->count,
                                                    randomDataGenerationInfo->dataSeparator,
                                                    randomDataGenerationInfo->extractorMatrixSizeIn,
                                                    randomDataGenerationInfo->extractorMatrixSizeOut,
                                                    randomDataGenerationInfo->extractorMatrixFilename,
                                                    static_cast<float>(randomDataGenerationInfo->min),
                                                    static_cast<float>(randomDataGenerationInfo->max));
        }
        else
        {
          quantis2File->GenerateExtractedFloatsFile(randomDataGenerationInfo->deviceType,
                                                    randomDataGenerationInfo->deviceNumber,
                                                    filename,
                                                    true,
                                                    randomDataGenerationInfo->count,
                                                    randomDataGenerationInfo->dataSeparator,
                                                    randomDataGenerationInfo->extractorMatrixSizeIn,
                                                    randomDataGenerationInfo->extractorMatrixSizeOut,
                                                    randomDataGenerationInfo->extractorMatrixFilename);
        }
        break;
      }
    }
  }
  catch (runtime_error &ex)
  {
    errorMessage->assign(ex.what());
  }
}

void idQ::EasyQuantis::Quantis2File::ProcessExtractionFile(idQ::EasyQuantis::Quantis2File *quantis2File,
                                                           idQ::EasyQuantis::FileExtractionGenerationInfo *fileExtractionGenerationInfo,
                                                           std::string *errorMessage)
{
  try
  {
    quantis2File->ProcessExtraction(fileExtractionGenerationInfo);
  }

  catch (runtime_error &ex)
  {
    errorMessage->assign(ex.what());
  }
}

unsigned long long idQ::EasyQuantis::Quantis2File::ProcessExtraction(idQ::EasyQuantis::FileExtractionGenerationInfo *fileExtractionGenerationInfo) throw(std::runtime_error)
{
  idQ::QuantisExtractor quantisExtractor;

  std::ifstream inputFile;

  uint8_t *inputBuffer;
  uint8_t *outputBuffer;
  uint32_t inputBufferSize;
  uint32_t outputBufferSize;

  //remaining is used to display the progressbar.
  //remaining is computed after loading the input file.
  //on large file > 100 Mb the thread may the GUI application may read remaining value
  //before it is computed. this cause the progressbar not working.
  // make sure remaining is > 0;
  remaining = 1;

  canRead = true;

  //load buffer from file
  inputFile.open(fileExtractionGenerationInfo->inputFile.c_str(), ifstream::in | ios::binary);

  if (!inputFile.is_open())
  {
    throw runtime_error("Unable to open input file: '" + fileExtractionGenerationInfo->inputFile + "'");
  }

  // get length of file:
  inputFile.seekg(0, ios::end);
  inputBufferSize = static_cast<uint32_t>(inputFile.tellg());
  inputFile.seekg(0, ios::beg);

  //Initialize the extractor
  quantisExtractor.InitializeMatrix(fileExtractionGenerationInfo->extractorMatrixFilename,
                                    static_cast<unsigned short>(fileExtractionGenerationInfo->extractorMatrixSizeIn),
                                    static_cast<unsigned short>(fileExtractionGenerationInfo->extractorMatrixSizeOut));

  outputBufferSize = quantisExtractor.InitializeOutputBuffer(inputBufferSize, &outputBuffer);

  //Update remaining before loading file
  remaining = outputBufferSize;

  //Update count value to get the real number of output bytes
  fileExtractionGenerationInfo->count = remaining + 1;

  // allocate memory:
  inputBuffer = new uint8_t[inputBufferSize];

  // read data as a block:
  inputFile.read(reinterpret_cast<char *>(inputBuffer), inputBufferSize);

  inputFile.close();

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

    if (canRead == false)
    {
      break;
    }

    remaining = outputBufferSize - (outputBufferSize / numberOfBlocksToProcess * i);
  }

  remaining = 0;

  if (canRead == true)
  {
    //save output buffer to file
    std::ofstream outputFile;

    outputFile.open(fileExtractionGenerationInfo->outputFile.c_str(), ios::out | ios::binary);
    if (!outputFile.is_open())
    {
      throw runtime_error("Unable to open output file: '" + fileExtractionGenerationInfo->outputFile + "'");
    }

    outputFile.write(reinterpret_cast<char *>(outputBuffer), outputBufferSize);
    outputFile.close();
    //cout << "Output written in: " << fileExtractionGenerationInfo->outputFile<< endl;
  }

  //Free memory
  quantisExtractor.UninitializeOutputBuffer(outputBuffer);
  quantisExtractor.UninitializeMatrix();

  canRead = false;

  //cout << "nbr bytes after extraction: " << dec << outputBufferSize << endl;

  return static_cast<unsigned long long>(outputBufferSize);
}

void idQ::EasyQuantis::Quantis2File::GenerateExtractorElementaryMatrix(Quantis2File *quantis2File,
                                                                       const ElementaryMatrixExtractionGenerationInfo *elementaryMatrixExtractionGenerationInfo,
                                                                       std::string *errorMessage)
{
  try
  {
    quantis2File->GenerateElementaryMatrix(elementaryMatrixExtractionGenerationInfo);
  }

  catch (runtime_error &ex)
  {
    cerr << "Error: GenerateExtractorElementaryMatrix: " << ex.what() << endl;
    errorMessage->assign(ex.what());
  }
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateElementaryMatrix(const idQ::EasyQuantis::ElementaryMatrixExtractionGenerationInfo *elementaryMatrixExtractionGenerationInfo) throw(std::runtime_error)
{
  uint32_t processedBytes;                             // number of bytes which were output (after under sampling and Von Neumann processing) up to now
  uint32_t sizeOfElementaryMatrix;                     // size of the elementaryMatrixBuffer in bytes
  std::vector<uint8_t> *elementaryMatrixBuffer = NULL; // buffer containing the output elementary matrix

  uint32_t rawChunkSize = 32768;         // size in bytes of the chunk which is read with each call to QuantisRead
  int32_t sizeOfUnderSampledChunkBuffer; // size of underSampledChunkBuffer in bytes
  idQ::QuantisExtractor quantisExtractor;
  uint32_t sizeOfVonNeumannChunkBuffer; // size of vonNeumannChunkBuffer in bytesNum
  int32_t bytesToAppend;

  processedBytes = 0;
  sizeOfElementaryMatrix = elementaryMatrixExtractionGenerationInfo->extractorMatrixSizeIn * elementaryMatrixExtractionGenerationInfo->extractorMatrixSizeOut / 8;

  remaining = sizeOfElementaryMatrix - processedBytes;

  // allocate memory:
  elementaryMatrixBuffer = new vector<uint8_t>(sizeOfElementaryMatrix);

  while ((processedBytes < sizeOfElementaryMatrix) && (canRead))
  {
    //read and undersample
    std::vector<uint8_t> underSampledChunkBuffer(rawChunkSize / 8); //4096 bytes

    sizeOfUnderSampledChunkBuffer = quantisExtractor.ReadUnderSampled(elementaryMatrixExtractionGenerationInfo->deviceType,
                                                                      elementaryMatrixExtractionGenerationInfo->deviceNumber,
                                                                      rawChunkSize,
                                                                      static_cast<uint16_t>(elementaryMatrixExtractionGenerationInfo->numberOfBytesToSkip),
                                                                      underSampledChunkBuffer);
    underSampledChunkBuffer.resize(sizeOfUnderSampledChunkBuffer);

    std::vector<uint8_t> vonNeumannChunkBuffer(rawChunkSize / 16); // 2048 bytes

    sizeOfVonNeumannChunkBuffer = quantisExtractor.ProcessBufferVonNeumann(underSampledChunkBuffer,
                                                                           vonNeumannChunkBuffer);

    // append the processed bytes to vonNeumannBuffer
    if (processedBytes + sizeOfVonNeumannChunkBuffer > sizeOfElementaryMatrix)
    {
      bytesToAppend = sizeOfElementaryMatrix - processedBytes;
    }
    else
    {
      bytesToAppend = sizeOfVonNeumannChunkBuffer;
    }

    // copy the newly processed bytes into the overall buffer which contains the output of the Von Neumann algorithm
    copy(vonNeumannChunkBuffer.begin(), vonNeumannChunkBuffer.begin() + bytesToAppend, elementaryMatrixBuffer->begin() + processedBytes);

    // update the number of processed bytes
    processedBytes += (uint32_t)bytesToAppend;

    remaining = sizeOfElementaryMatrix - processedBytes;

  } // end while (processedBytes < sizeOfElementaryMatrix)

  //save output buffer to file
  std::ofstream outputFile;

  outputFile.open(elementaryMatrixExtractionGenerationInfo->outputElementaryMatrixFile.c_str(), ios::out | ios::binary);
  if (!outputFile.is_open())
  {
    throw runtime_error("Unable to open output file: '" + elementaryMatrixExtractionGenerationInfo->outputElementaryMatrixFile + "'");
  }

  copy(elementaryMatrixBuffer->begin(), elementaryMatrixBuffer->begin() + sizeOfElementaryMatrix, std::ostream_iterator<uint8_t>(outputFile));
  outputFile.close();

  delete elementaryMatrixBuffer;

  return static_cast<unsigned long long>(processedBytes);
}

void idQ::EasyQuantis::Quantis2File::GenerateExtractorExtractorMatrix(Quantis2File *quantis2File,
                                                                      const MatrixExtractionGenerationInfo *matrixExtractionGenerationInfo,
                                                                      std::string *errorMessage)
{
  try
  {
    quantis2File->GenerateMatrix(matrixExtractionGenerationInfo);
  }

  catch (runtime_error &ex)
  {
    errorMessage->assign(ex.what());
  }
}

unsigned long long idQ::EasyQuantis::Quantis2File::GenerateMatrix(const idQ::EasyQuantis::MatrixExtractionGenerationInfo *matrixExtractionGenerationInfo) throw(std::runtime_error)
{
  idQ::QuantisExtractor quantisExtractor;
  uint32_t matrixSize = (matrixExtractionGenerationInfo->extractorMatrixSizeIn * matrixExtractionGenerationInfo->extractorMatrixSizeOut) / 8;

  cout << "CreateElementaryMatrix" << endl;

  quantisExtractor.CreateMatrix(matrixSize,
                                matrixExtractionGenerationInfo->elementaryMatrixFiles,
                                matrixExtractionGenerationInfo->outputMatrixFile);

  return matrixSize;
}

/*
std::vector<std::string> idQ::EasyQuantis::GetHexaArray(const std::vector<unsigned char>& buffer,
                                      const int rowLength,
                                      const char replacementChar)
{
  int numberOfRow;
  int moduloRow;
  vector<string> output;
  stringstream newRow;
  int rowCpt;
  
  numberOfRow = static_cast<int>(buffer.size()) / rowLength;
  moduloRow = static_cast<int>(buffer.size()) % rowLength;

  //LOG4CXX_DEBUG(Log::idqLogger, "Utils: GetHexaArray: numberOfRow: " << numberOfRow);
  //LOG4CXX_DEBUG(Log::idqLogger, "Utils: GetHexaArray: moduloRow:   " << moduloRow);

  for(rowCpt = 0; rowCpt < numberOfRow; rowCpt++)
  {
    //Add the index range
    newRow << "["
           << hex << setw(4)<< setfill('0')
           << rowCpt * rowLength
           << ".."
           << hex << setw(4)<< setfill('0')
           << rowCpt * rowLength + rowLength - 1
           << "]";

    //Add the hexa values representation       
    for(int j = 0; j < rowLength; j++)
    {
      newRow << " ";      
      newRow << hex << setw(2)<< setfill('0');
      newRow << static_cast<int>(buffer[rowCpt * rowLength + j]);
    }
    newRow << "  |  ";

    //Add the character values representation
    for(int j=0; j < rowLength; j++)
    {
      //make sure the char is visible, otherwise display the replacement character.
      if((buffer[rowCpt * rowLength + j] >= ' ') && (buffer[rowCpt * rowLength + j] <= '~'))
      {
        newRow << static_cast<char>(buffer[rowCpt * rowLength + j]);
      }
      else
      {
        newRow << replacementChar;  
      }
    }    
    output.push_back(newRow.str());
    newRow.str("");
  }

  //Add the remaining values
  if(moduloRow)
  {
    //Add the index range
    newRow << "["
           << hex << setw(4)<< setfill('0')
           << rowCpt * rowLength
           << ".."
           << hex << setw(4)<< setfill('0')
           << rowCpt * rowLength + moduloRow
           << "]";

    //Add the hexa values representation
    for(int j = 0; j < moduloRow; j++)
    {
      newRow << " ";
      newRow << hex << setw(2)<< setfill('0');
      newRow << static_cast<int>(buffer[rowCpt * rowLength + j]);
    }
    for(int j = moduloRow; j < rowLength; j++)
    {
      newRow << "   ";
    }
    
    newRow << "  |  ";

    //Add the character values representation
    for(int j=0; j < moduloRow; j++)
    {
      //make sure the char is visible, otherwise display the replacement character.
      if((buffer[rowCpt * rowLength + j] >= ' ') && (buffer[rowCpt * rowLength + j] <= '~'))
      {
        newRow << static_cast<char>(buffer[rowCpt * rowLength + j]);
      }
      else
      {
        newRow << replacementChar;
      }
    }
    for(int j = moduloRow; j < rowLength; j++)
    {
      newRow << " ";
    }
    output.push_back(newRow.str());
    newRow.str("");
  }

  return(output);
}

*/
