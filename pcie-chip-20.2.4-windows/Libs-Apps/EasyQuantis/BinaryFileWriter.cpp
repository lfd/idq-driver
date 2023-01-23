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

#include <boost/filesystem.hpp>
#include <fstream>

#include "BinaryFileWriter.hpp"

using namespace std;


idQ::EasyQuantis::BinaryFileWriter::BinaryFileWriter() : fileSize(0u)
{
}


idQ::EasyQuantis::BinaryFileWriter::BinaryFileWriter(const std::string& filename,
                                                     bool discardContent)
    throw (std::runtime_error)
{
  Open(filename, discardContent);
}


idQ::EasyQuantis::BinaryFileWriter::~BinaryFileWriter()
{
  Close();
}


void idQ::EasyQuantis::BinaryFileWriter::Close()
{
  if (outputFile.is_open())
  {
    outputFile.close();
  }

  UpdateFileSize();

  filename.clear();
}


std::string idQ::EasyQuantis::BinaryFileWriter::GetFilename() const
{
  return filename;
}


unsigned long long idQ::EasyQuantis::BinaryFileWriter::GetSize() const
    throw (std::runtime_error)
{
  return fileSize;
}


bool idQ::EasyQuantis::BinaryFileWriter::IsOpen()
{
  // NOTE: IsOpen function cannot be declared const since is_open()
  // is NOT const on every platform (e.g Solaris)
  return outputFile.is_open();
}


void idQ::EasyQuantis::BinaryFileWriter::Open(const std::string& filename,
                                              bool discardContent)
    throw (std::runtime_error)
{
  Close();

  if (discardContent)
  {
    if (boost::filesystem::exists(filename))
    {
      boost::filesystem::remove(filename);
    }
  }

  // Set mode
  ios_base::openmode mode = ios::out | ios::binary | ios::app;

  // Open file
  outputFile.open(filename.c_str(), mode);
  if (!outputFile.is_open())
  {
    throw runtime_error("BinaryFileWriter: Unable to open file '" + filename + "'");
  }

  UpdateFileSize();

  this->filename = filename;
  this->discardContent = discardContent;
}


void idQ::EasyQuantis::BinaryFileWriter::Write(const std::string& content)
    throw (std::runtime_error)
{
  boost::recursive_mutex::scoped_lock lock(mutex);

  // Checks if file is opened
  if (!outputFile.is_open())
  {
    throw runtime_error("BinaryFileWriter: File is not opened");
  }

  // Write content
  outputFile << content;
  if (outputFile.fail())
  {
    throw runtime_error("BinaryFileWriter: Error while writing file '" + filename + "'");
  }

  UpdateFileSize();
}


void idQ::EasyQuantis::BinaryFileWriter::UpdateFileSize()
    throw (std::runtime_error)
{
  if (outputFile.is_open())
  {
    streampos pos = outputFile.tellp();
    if (pos < 0)
    {
      throw runtime_error("BinaryFileWriter: unable to get file size");
    }
    else
    {
      fileSize = static_cast<unsigned long long>(pos);
    }
  }
  else
  {
    fileSize = 0u;
  }
}

