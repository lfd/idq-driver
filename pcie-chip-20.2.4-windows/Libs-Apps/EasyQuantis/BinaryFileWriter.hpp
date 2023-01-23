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

#ifndef BINARY_FILE_WRITER_HPP
#define BINARY_FILE_WRITER_HPP

#ifdef _WIN32
  // Visual C++ does not implement checked exceptions
  // Disables "C++ exception specification ignored except to indicate a function is not __declspec(nothrow)"
# pragma warning(disable: 4290) 
#endif

#include <boost/thread/recursive_mutex.hpp>
#include <fstream>
#include <stdexcept>
#include <string>

namespace idQ
{
  namespace EasyQuantis
  {
    /**
     * BinaryFileWriter class 
     */
    class BinaryFileWriter
    {
      public:
        /**
         *
         */
        BinaryFileWriter();

        /**
         * Construct a new BinaryFileWriter object.
         * @param filename The filename of the BinaryFileWriter.
         * @param discardContent When set to true any current content is
         * discarded.
         * @throw std::runtime_error 
         */
        BinaryFileWriter(const std::string& filename,
                         bool discardContent = false)
            throw (std::runtime_error);

        /**
         * BinaryFileWriter deconstructor.
         */
        ~BinaryFileWriter();

        /**
         * Closes the file.
         */
        void Close();

        std::string GetFilename() const;

        unsigned long long GetSize() const
            throw (std::runtime_error);

        bool IsOpen();

        void Open(const std::string& filename,
                  bool discardContent = false)
            throw (std::runtime_error);

        void Write(const std::string& content)
            throw (std::runtime_error);

      private:
        void UpdateFileSize()
            throw (std::runtime_error);

        std::ofstream outputFile;

        std::string filename;
        bool discardContent;

        unsigned long long fileSize;

        boost::recursive_mutex mutex;
    };
  }
}

#endif
