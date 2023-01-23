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

#ifndef QUANTIS_2_FILE_HPP
#define QUANTIS_2_FILE_HPP

#ifdef _WIN32
// Visual C++ does not implement checked exceptions
// Disables "C++ exception specification ignored except to indicate a function is not __declspec(nothrow)"
#pragma warning(disable : 4290)
#endif

#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <string>

#include "Quantis/Quantis.hpp"
#include "QuantisExtensions/QuantisExtractor.hpp"

#include "BinaryFileWriter.hpp"
#include "Utils.hpp"

namespace idQ
{
namespace EasyQuantis
{
/**
     *
     */
class Quantis2File
{
public:
  /**
         * Creates a new Quantis2File instance.
         */
  Quantis2File();

  /**
         * Destroy this instance
         */
  ~Quantis2File();

  /**
        * Cancel any Read2File operation.
        * @warning The
        */
  void CancelRead2File();

  /**
         * Returns how many bytes still have to be retrieved from the Quantis
         * device before completing current Read2File operation. If last
         * Read2File has been canceled, it returns how many bytes 
         * been 
         */
  unsigned long long GetRemainingSize() const;

  /**
         * Read random data from a Quantis and write it to a binary file.
         * @param size The size in bytes to read from the Quantis and write
         * to the output file.
         */
  unsigned long long GenerateBinaryFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long size) throw(std::runtime_error);

  unsigned long long GenerateIntsFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long count,
      const std::string &dataSeparator) throw(std::runtime_error);

  unsigned long long GenerateIntsFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long count,
      const std::string &dataSeparator,
      int min,
      int max) throw(std::runtime_error);

  unsigned long long GenerateFloatsFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long count,
      const std::string &dataSeparator) throw(std::runtime_error);

  unsigned long long GenerateFloatsFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long count,
      const std::string &dataSeparator,
      float min,
      float max) throw(std::runtime_error);

  unsigned long long GenerateExtractedBinaryFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long size,
      const int matrixSizeIn,
      const int matrixSizeOut,
      const std::string &matrixFilename) throw(std::runtime_error);

  unsigned long long GenerateExtractedIntsFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long count,
      const std::string &dataSeparator,
      const int matrixSizeIn,
      const int matrixSizeOut,
      const std::string &matrixFilename) throw(std::runtime_error);

  unsigned long long GenerateExtractedIntsFile(
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
      int max) throw(std::runtime_error);

  unsigned long long GenerateExtractedFloatsFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long count,
      const std::string &dataSeparator,
      const int matrixSizeIn,
      const int matrixSizeOut,
      const std::string &matrixFilename) throw(std::runtime_error);

  unsigned long long GenerateExtractedFloatsFile(
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
      float max) throw(std::runtime_error);

  /**
         *
         */
  static void GenerateRandomFile(Quantis2File *quantis2File,
                                 const RandomDataGenerationInfo *randomDataGenerationInfo,
                                 const std::string &filename,
                                 std::string *errorMessage);

  static void ProcessExtractionFile(idQ::EasyQuantis::Quantis2File *quantis2File,
                                    idQ::EasyQuantis::FileExtractionGenerationInfo *fileExtractionGenerationInfo,
                                    std::string *errorMessage);

  static void GenerateExtractorElementaryMatrix(Quantis2File *quantis2File,
                                                const ElementaryMatrixExtractionGenerationInfo *elementaryMatrixExtractionGenerationInfo,
                                                std::string *errorMessage);

  static void GenerateExtractorExtractorMatrix(Quantis2File *quantis2File,
                                               const MatrixExtractionGenerationInfo *matrixExtractionGenerationInfo,
                                               std::string *errorMessage);

private:
  unsigned long long remaining;
  bool canRead;

  /** Size of the chunk that will be requested */
  static const size_t CHUNK_SIZE;

  unsigned long long ProcessExtraction(idQ::EasyQuantis::FileExtractionGenerationInfo *fileExtractionGenerationInfo) throw(std::runtime_error);

  unsigned long long GenerateElementaryMatrix(const idQ::EasyQuantis::ElementaryMatrixExtractionGenerationInfo *elementaryMatrixExtractionGenerationInfo) throw(std::runtime_error);

  unsigned long long GenerateMatrix(const idQ::EasyQuantis::MatrixExtractionGenerationInfo *matrixExtractionGenerationInfo) throw(std::runtime_error);

  template <typename numberType>
  unsigned long long GenerateFormattedFile(
      QuantisDeviceType deviceType,
      unsigned int deviceNumber,
      const std::string &filename,
      bool discardContent,
      unsigned long long count,
      const std::string &dataSeparator,
      numberType (idQ::Quantis::*read)() const,
      numberType (idQ::Quantis::*readScaled)(numberType /*min*/, numberType /*max*/) const = NULL,
      numberType min = 0,
      numberType max = 0) throw(std::runtime_error);

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
};
} // namespace EasyQuantis
} // namespace idQ

// *************************** Templates definitions *************************

#include <cmath>
#include <cstdio>

template <typename numberType>
unsigned long long idQ::EasyQuantis::Quantis2File::GenerateFormattedFile(
    QuantisDeviceType deviceType,
    unsigned int deviceNumber,
    const std::string &filename,
    bool discardContent,
    unsigned long long count,
    const std::string &dataSeparator,
    numberType (idQ::Quantis::*read)() const,
    numberType (idQ::Quantis::*readScaled)(numberType /*min*/, numberType /*max*/) const,
    numberType min,
    numberType max) throw(std::runtime_error)
{
  using namespace std;

  idQ::Quantis quantis(deviceType, deviceNumber);
  BinaryFileWriter outputFile(filename, discardContent);
  char randomData[CHUNK_SIZE];
  string outputData;
  numberType value;
  size_t valueSize = sizeof(value);
  remaining = count;
  size_t chunkSize = CHUNK_SIZE;

  if (chunkSize % valueSize != 0)
  {
    // The chunk size is not a multiple of the value's size
    throw runtime_error("Quantis2File: invalid chunk size");
  }

  canRead = true;

  while ((remaining > 0u) && canRead)
  {
    // Chunk size
    if (remaining * valueSize < chunkSize)
    {
      chunkSize = static_cast<size_t>(remaining * valueSize);
    }

    // Read data
    quantis.Read(randomData, chunkSize);

    // Convert (& scale) data
    outputData.clear();
    size_t i = 0;
    while (i <= (chunkSize - valueSize))
    {
      if (readScaled)
      {
        value = (quantis.*readScaled)(min, max);
      }
      else if (read)
      {
        value = (quantis.*read)();
      }
      else
      {
        throw runtime_error("Quantis2File: no valid read function");
      }

      outputData.append(boost::lexical_cast<std::string>(value));
      outputData.append(dataSeparator);

      i += valueSize;
    }

    // Write data to file
    outputFile.Write(outputData);

    // Update info
    remaining -= chunkSize / valueSize;
  }

  canRead = false;

  return outputFile.GetSize();
}

template <typename numberType>
unsigned long long idQ::EasyQuantis::Quantis2File::GenerateExtractedFormattedFile(
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
    numberType (idQ::QuantisExtractor::*readScaled)(QuantisDeviceType /*deviceType*/, unsigned int /*deviceNumber*/, numberType /*min*/, numberType /*max*/) const,
    numberType min,
    numberType max) throw(std::runtime_error)
{
  using namespace std;

  idQ::QuantisExtractor quantisExtractor;
  BinaryFileWriter outputFile(filename, discardContent);
  char randomData[CHUNK_SIZE];
  string outputData;
  numberType value;
  size_t valueSize = sizeof(value);
  remaining = count;
  size_t chunkSize = CHUNK_SIZE;

  quantisExtractor.InitializeMatrix(matrixFilename, static_cast<uint16_t>(matrixSizeIn), static_cast<uint16_t>(matrixSizeOut));

  quantisExtractor.EnableStorageBuffer();

  if (chunkSize % valueSize != 0)
  {
    // The chunk size is not a multiple of the value's size
    throw runtime_error("Quantis2File: invalid chunk size");
  }

  canRead = true;

  while ((remaining > 0u) && canRead)
  {
    // Chunk size
    if (remaining * valueSize < chunkSize)
    {
      chunkSize = static_cast<size_t>(remaining * valueSize);
    }

    // Read data
    quantisExtractor.GetDataFromQuantis(deviceType, deviceNumber, randomData, chunkSize);

    // Convert (& scale) data
    outputData.clear();
    size_t i = 0;
    while (i <= (chunkSize - valueSize))
    {
      if (readScaled)
      {
        value = (quantisExtractor.*readScaled)(deviceType, deviceNumber, min, max);
      }
      else if (read)
      {
        value = (quantisExtractor.*read)(deviceType, deviceNumber);
      }
      else
      {
        throw runtime_error("Quantis2File: no valid read function");
      }

      outputData.append(boost::lexical_cast<std::string>(value));
      outputData.append(dataSeparator);

      i += valueSize;
    }

    // Write data to file
    outputFile.Write(outputData);

    // Update info
    remaining -= chunkSize / valueSize;
  }

  canRead = false;

  quantisExtractor.DisableStorageBuffer();

  quantisExtractor.UninitializeMatrix();

  return outputFile.GetSize();
}

#endif
