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


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "Utils.hpp"

using namespace boost;
using namespace std;

static const unsigned long long BYTE     = 1u;
static const unsigned long long KILOBYTE = (BYTE      * 1024u);
static const unsigned long long MEGABYTE = (KILOBYTE  * 1024u);
static const unsigned long long GIGABYTE = (MEGABYTE  * 1024u);
static const unsigned long long TERABYTE = (GIGABYTE  * 1024u);
static const unsigned long long PETABYTE = (TERABYTE  * 1024u);
static const unsigned long long EXABYTE  = (PETABYTE  * 1024u);

static const unsigned int SECOND  = 1u;
static const unsigned int MINUTE  = SECOND * 60u;
static const unsigned int HOUR    = MINUTE * 60u;
static const unsigned int DAY     = HOUR * 24u;
//static const unsigned int WEEK    = DAY * 7u;
static const unsigned int MONTH   = DAY * 30u;
static const unsigned int YEAR    = DAY * 365u;


std::string idQ::EasyQuantis::Utils::BuildProgressInfoString(
    const boost::posix_time::ptime& timeStart,
    const idQ::EasyQuantis::RandomDataGenerationInfo& randomDataGenerationInfo,
    const unsigned long long& remainingSize,    
    int& progressValue)
{
  std::string infoString;
  unsigned long long speed;
    
  // Read data:
  unsigned long long readSize = randomDataGenerationInfo.count - remainingSize;
  if (readSize > 0u)
  {
    // Compute %
    double dCount = static_cast<double>(randomDataGenerationInfo.count);
    double dReadSize = static_cast<double>(readSize);
    double dProgressValue = dReadSize / dCount * 100.0f;
    progressValue = static_cast<int>(dProgressValue);

    // Build info string
    if (randomDataGenerationInfo.dataType == RANDOM_DATA_TYPE_BINARY)
    {
      // Creating random bytes
      infoString.append(Utils::FormatSize(readSize));
      infoString.append(" of ");
      infoString.append(Utils::FormatSize(randomDataGenerationInfo.count));
    }
    else
    {
      // Read numbers
      infoString.append(lexical_cast<string>(readSize));
      infoString.append(" of ");
      infoString.append(lexical_cast<string>(randomDataGenerationInfo.count));
    }

    // Duration
    boost::posix_time::ptime timeNow = boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration duration = timeNow - timeStart;
    double dDuration = static_cast<double>(duration.total_seconds());
    if (dDuration >= 1.0f)
    {
      unsigned long long remainingDuration =
        static_cast<unsigned long long>((dCount * dDuration / dReadSize) - dDuration);
      if (remainingDuration >= 1u)
      {
        // Adds time left
        infoString.append(" - ");
        infoString.append(Utils::FormatTime(remainingDuration));
        infoString.append(" left");

        // Append reading speed
        if (randomDataGenerationInfo.dataType == RANDOM_DATA_TYPE_BINARY)
        {
          speed = static_cast<unsigned long long>(dReadSize / dDuration);
          infoString.append(" (");
          infoString.append(Utils::FormatSize(speed, true, true));
          infoString.append("/sec)");
        }
      }
      else
      {
        infoString.append(" - less than 1 second left");
      }
    }
  }
  else
  {
    progressValue = 100;
  }

  return infoString;
}


std::string idQ::EasyQuantis::Utils::FormatSize(unsigned long long size, bool addByteString, bool bitUnit)
{
  string unitStr;
  unsigned long long d;

  
  if(bitUnit)
  {
    size = size * 8;
  }
  
  
  if (size < KILOBYTE)
  {
    // bytes
    unitStr = " ";
    d = BYTE;
  }
  else if (size < MEGABYTE)
  {
    // kilobytes
    unitStr = " K";
    d = KILOBYTE;
  }
  else if (size < GIGABYTE)
  {
    // megabytes
    unitStr = " M";
    d = MEGABYTE;
  }
  else if (size < TERABYTE)
  {
    // gigabytes
    unitStr = " G";
    d = GIGABYTE;
  }
  else if (size < PETABYTE)
  {
    // terabytes
    unitStr = " T";
    d = TERABYTE;
  }
  else if (size < EXABYTE)
  {
    // petabytes
    unitStr = " P";
    d = PETABYTE;
  }
  else
  {
    // Unknown, just return value as is
    unitStr = " ";
    d = BYTE;
  }

  float value = ceil(static_cast<float>(size) / static_cast<float>(d) * 10.0f) / 10.0f;
  stringstream ss;
  ss << fixed << setprecision(1) << value;

  string s = ss.str().append(unitStr);
  if (addByteString)
  {
    if(bitUnit)
    {
      s.append("bit");
    }
    else
    {
      s.append("byte");
    }
    
    if (static_cast<int>(value * 10.0f) != 10)
    {
      s.append("s");
    }
  }
  return s;
}


std::string FormatTimeUnit(unsigned int value,
                           const std::string& strUnit)
{
  if (value == 0)
  {
    return "";
  }

  string s;
  s.append(lexical_cast<string>(value));
  s.append(" ");
  s.append(strUnit);
  if (value > 1)
  {
    s.append("s");
  }
  return s;
}


std::string FormatTimeUnit(unsigned int value1,
                           const std::string& strUnit1,
                           unsigned int value2,
                           const std::string& strUnit2)
{
  string s1 = FormatTimeUnit(value1, strUnit1);
  string s2 = FormatTimeUnit(value2, strUnit2);

  if (s2.empty())
  {
    return "approximately " + s1;
  }
  else if(s1.empty())
  {
    return s2;
  }
  else
  {
    return s1 + " " + s2;
  }
}


std::string idQ::EasyQuantis::Utils::FormatTime(unsigned long long seconds)
{

  unsigned int years = static_cast<unsigned int>(floor(static_cast<float>(seconds) / YEAR));
  seconds -= years * YEAR;

  unsigned int months = static_cast<unsigned int>(floor(static_cast<float>(seconds) / MONTH));
  seconds -= months * MONTH;

  unsigned int days = static_cast<unsigned int>(floor(static_cast<float>(seconds) / DAY));
  seconds -= days * DAY;

  unsigned int hours = static_cast<unsigned int>(floor(static_cast<float>(seconds) / HOUR));
  seconds -= hours * HOUR;

  unsigned int min = static_cast<unsigned int>(floor(static_cast<float>(seconds) / MINUTE));
  seconds -= min * MINUTE;

  // Remaining seconds (< 60)
  unsigned int sec = static_cast<unsigned int>(seconds);

  string time;
  if (years > 0)
  {
    return FormatTimeUnit(years, "year", months, "month");
  }
  else if (months > 0)
  {
    return FormatTimeUnit(months, "month", days, "day");
  }
  else if (days > 0)
  {
    return FormatTimeUnit(days, "day", hours, "hour");
  }
  else if (hours > 0)
  {
    return FormatTimeUnit(hours, "hour", min, "minute");
  }
  else if (min > 0)
  {
    return FormatTimeUnit(min, "minute", sec, "second");
  }
  else
  {
    return FormatTimeUnit(sec, "second");
  }
}


std::string idQ::EasyQuantis::Utils::BuildProcessExtractionProgressInfoString(
    const boost::posix_time::ptime& timeStart,
    const idQ::EasyQuantis::FileExtractionGenerationInfo& fileExtractionGenerationInfo,
    const unsigned long long& remainingSize,    
    int& progressValue)
{
  std::string infoString;
  unsigned long long speed;

  // Read data:
  unsigned long long readSize = fileExtractionGenerationInfo.count - remainingSize;
  if (readSize > 0u)
  {
    // Compute %
    double dCount = static_cast<double>(fileExtractionGenerationInfo.count);
    double dReadSize = static_cast<double>(readSize);
    double dProgressValue = dReadSize / dCount * 100.0f;
    progressValue = static_cast<int>(dProgressValue);

    // Creating random bytes
    infoString.append(Utils::FormatSize(readSize));
    infoString.append(" of ");
    infoString.append(Utils::FormatSize(fileExtractionGenerationInfo.count));

    
    // Duration
    boost::posix_time::ptime timeNow = boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration duration = timeNow - timeStart;
    double dDuration = static_cast<double>(duration.total_seconds());
    if (dDuration >= 1.0f)
    {
      unsigned long long remainingDuration =
        static_cast<unsigned long long>((dCount * dDuration / dReadSize) - dDuration);
      if (remainingDuration >= 1u)
      {
        // Adds time left
        infoString.append(" - ");
        infoString.append(Utils::FormatTime(remainingDuration));
        infoString.append(" left");

        speed = static_cast<unsigned long long>(dReadSize / dDuration);
        infoString.append(" (");
        infoString.append(Utils::FormatSize(speed));
        infoString.append("/sec)");
      }
      else
      {
        infoString.append(" - less than 1 second left");
      }
    }
  }
  else
  {
    progressValue = 100;
  }

  return infoString;
}


std::string idQ::EasyQuantis::Utils::BuildProgressExtractionElementaryMatrixInfoString(
          const boost::posix_time::ptime& timeStart,
          const ElementaryMatrixExtractionGenerationInfo& elementaryMatrixExtractionGenerationInfo,
          const unsigned long long& remainingSize,
          int& progressValue) 
{
  std::string infoString;
  unsigned long long speed;

  // Read data:
  unsigned long long readSize = elementaryMatrixExtractionGenerationInfo.count - remainingSize;
  if (readSize > 0u)
  {
    // Compute %
    double dCount = static_cast<double>(elementaryMatrixExtractionGenerationInfo.count);
    double dReadSize = static_cast<double>(readSize);
    double dProgressValue = dReadSize / dCount * 100.0f;
    progressValue = static_cast<int>(dProgressValue);

    // Build info string
      infoString.append(Utils::FormatSize(readSize));
      infoString.append(" of ");
      infoString.append(Utils::FormatSize(elementaryMatrixExtractionGenerationInfo.count));

    // Duration
    boost::posix_time::ptime timeNow = boost::posix_time::second_clock::local_time();
    boost::posix_time::time_duration duration = timeNow - timeStart;
    double dDuration = static_cast<double>(duration.total_seconds());
    if (dDuration >= 1.0f)
    {
      unsigned long long remainingDuration =
        static_cast<unsigned long long>((dCount * dDuration / dReadSize) - dDuration);
      if (remainingDuration >= 1u)
      {
        // Adds time left
        infoString.append(" - ");
        infoString.append(Utils::FormatTime(remainingDuration));
        infoString.append(" left");


        speed = static_cast<unsigned long long>(dReadSize / dDuration);
        infoString.append(" (");
        infoString.append(Utils::FormatSize(speed));
        infoString.append("/sec)");

      }
      else
      {
        infoString.append(" - less than 1 second left");
      }
    }
  }
  else
  {
    progressValue = 100;
  }

  return infoString;
}

