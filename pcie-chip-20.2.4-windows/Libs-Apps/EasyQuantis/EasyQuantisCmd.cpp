/*
 * EasyQuantis application
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


#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <string>
#include <vector>

#include <QtCore/QFile>

#include "EasyQuantis.hpp"
#include "EasyQuantisCmd.hpp"
#include "Quantis2File.hpp"



using namespace std;

namespace pa = boost::program_options;
namespace fs = boost::filesystem;


idQ::EasyQuantis::EasyQuantisCmd::EasyQuantisCmd(int argc, char** argv) :
    argc(argc),
    argv(argv)
{
}


idQ::EasyQuantis::EasyQuantisCmd::~EasyQuantisCmd()
{
}


int idQ::EasyQuantis::EasyQuantisCmd::Exec()
{
  // Configure and parse command line arguments
  pa::options_description generic("Generic options");
  generic.add_options()
    ("help,h", "Display this help message")
    ("version,v", "Display the version of EasyQuantis")
    ;

  pa::options_description quantis("Quantis options");
  quantis.add_options()
    ("list,l", "List all Quantis devices")
    ("pci,p",
     pa::value<unsigned int>(),
     "Set Quantis PCI device number")
    ("usb,u",
     pa::value<unsigned int>(),
     "Set Quantis USB device number")
  ;

  pa::options_description acquisition("Acquisition options");
  acquisition.add_options()
    ("size,n",
     pa::value<unsigned long long>()->default_value(1024u),
     "Number of bytes/numbers to read")
    ("binary,b",
     pa::value<string>(),
     "Create a binary file")
    ("integers,i",
     pa::value<string>(),
     "Create a file with integers numbers")
    ("floats,f",
     pa::value<string>(),
     "Create a file with floats numbers")
    ("separator,s",
     pa::value<string>()->default_value("\n"),
     "Defines the separator for non-binary files")
    ("min",
     pa::value<double>(),
     "Specify the minimal value of the number")
    ("max",
     pa::value<double>(),
     "Specify the maximal value of the number")
  ;

  
  pa::options_description extraction("Extraction options");
  extraction.add_options()
    ("matrix-file,m", pa::value<string>()->default_value(""), "The path of the matrix file. If not defined, extraction processing is disabled")  
    ("matrix-size-in,I", pa::value<int>()->default_value(1024), "The matrix input size in bits (default 1024)")
    ("matrix-size-out,O", pa::value<int>()->default_value(768), "The matrix output size in bits (default 768)")
    ("extraction-from-file", "If defined perform extraction processing from 'extraction-input-file' and save to 'extraction-output-file'")
    ("extraction-input-file", pa::value<string>(), "The path of the binary input file")  
    ("extraction-output-file", pa::value<string>(), "The path of the binary output file")
  ;
   
  
  
  pa::options_description desc;  
  desc.add(generic).add(quantis).add(acquisition).add(extraction);

  pa::variables_map vm;
  pa::store(pa::parse_command_line(argc, argv, desc), vm);
  pa::notify(vm);

  // Parse general options
  if (vm.count("help"))
  {
    PrintUsage(argv[0], desc);
    return 0;
  }

  
  RandomDataGenerationInfo randomDataGenerationInfo;
  FileExtractionGenerationInfo fileExtractionGenerationInfo;
  string filename;
  ActionType action = ACTION_ACQUISITION;
  
  // Parse quantis options
  if (vm.count("list"))
  {
    PrintDevicesList();
    return 0;
  }
  
  else if (vm.count("version"))
  {
    cout << EASY_QUANTIS_NAME_VERSION << endl;
    return 0;
  }  
  
  else if (vm.count("pci") && vm.count("usb"))
  {
    cerr << "--pci and --usb cannot be specified simultaniously!" << endl;
    return -1;
  }
  else if (vm.count("pci"))
  {
    randomDataGenerationInfo.deviceType = QUANTIS_DEVICE_PCI;
    randomDataGenerationInfo.deviceNumber = vm["pci"].as<unsigned int>();
    action = ACTION_ACQUISITION;
  }
  else if (vm.count("usb"))
  {
    randomDataGenerationInfo.deviceType = QUANTIS_DEVICE_USB;
    randomDataGenerationInfo.deviceNumber = vm["usb"].as<unsigned int>();
    action = ACTION_ACQUISITION;
  }
  else if (vm.count("extraction-from-file"))
  {
    action = ACTION_FILE_EXTRACTION;
  }  
  else
  {
    PrintUsage(argv[0], desc);
    return -1;
  }

  
  if(action == ACTION_ACQUISITION)
  {
    if (static_cast<int>(randomDataGenerationInfo.deviceNumber) >
      Quantis::Count(randomDataGenerationInfo.deviceType))
    {
      cerr << "Specified device do not exists!" << endl;
      return -1;
    }

    // Parse acquisition options
    if (vm.count("size"))
    {
      randomDataGenerationInfo.count = vm["size"].as<unsigned long long>();
    }

    if ((vm.count("min") && !vm.count("max")) ||
        (!vm.count("min") && vm.count("max")))
    {
      cerr << "You must specify both min and max values!" << endl;
      return -1;
    }

    if (vm.count("min"))
    {
      randomDataGenerationInfo.min =  vm["min"].as<double>();
      randomDataGenerationInfo.max =  vm["max"].as<double>();

      if (randomDataGenerationInfo.min >= randomDataGenerationInfo.max)
      {
        cerr << "min must be lower than max!" << endl;
        return -1;
      }

      randomDataGenerationInfo.scaleData = true;
    }
    else
    {
      randomDataGenerationInfo.scaleData = false;
    }

    randomDataGenerationInfo.dataSeparator = vm["separator"].as<string>();


    if (vm.count("binary"))
    {
      randomDataGenerationInfo.dataType = RANDOM_DATA_TYPE_BINARY;
      filename = vm["binary"].as<string>();
    }
    else if (vm.count("integers"))
    {
      randomDataGenerationInfo.dataType = RANDOM_DATA_TYPE_INTEGERS;
      filename = vm["integers"].as<string>();
    }
    else if (vm.count("floats"))
    {
      randomDataGenerationInfo.dataType = RANDOM_DATA_TYPE_FLOATS;
      filename = vm["floats"].as<string>();
    }
    else
    {
      cerr << "No output format provided! " << endl;
      return -1;
    }
  }
  
  //Parse extraction options
  if (vm.count("matrix-size-in"))
  {
    randomDataGenerationInfo.extractorMatrixSizeIn = vm["matrix-size-in"].as<int>();
    fileExtractionGenerationInfo.extractorMatrixSizeIn = vm["matrix-size-in"].as<int>();
  }  
  if (vm.count("matrix-size-out"))
  {
    randomDataGenerationInfo.extractorMatrixSizeOut = vm["matrix-size-out"].as<int>();
    fileExtractionGenerationInfo.extractorMatrixSizeOut = vm["matrix-size-out"].as<int>();
  }  
  if (vm.count("matrix-file"))
  {
    randomDataGenerationInfo.extractorMatrixFilename = vm["matrix-file"].as<string>();
    fileExtractionGenerationInfo.extractorMatrixFilename = vm["matrix-file"].as<string>();    
  }  

  if(randomDataGenerationInfo.extractorMatrixFilename.size() == 0)
  {
    randomDataGenerationInfo.extractorEnabled = false;  
  }
  else
  {
    randomDataGenerationInfo.extractorEnabled = true;
  }
  
  


  
  if (vm.count("extraction-input-file"))
  {        
    QFile inputFile(QString::fromStdString(vm["extraction-input-file"].as<string>()));
  
    if(!inputFile.exists())
    {
      cerr << "The input file '" << inputFile.fileName().toStdString() << "' does not exists" << endl;
      return -1;
    }
  
    fileExtractionGenerationInfo.inputFile = inputFile.fileName().toStdString();
    fileExtractionGenerationInfo.count = inputFile.size();     
  }    

  
  if (vm.count("extraction-output-file"))
  {
    fileExtractionGenerationInfo.outputFile = vm["extraction-output-file"].as<string>();
  }    
  
  
  if(action == ACTION_ACQUISITION)
  {
    return Acquisition(randomDataGenerationInfo, filename);
  }
  else if(action == ACTION_FILE_EXTRACTION)
  {
    return ExtractionFromFile(fileExtractionGenerationInfo);
  }
  else
  {
    cerr << "unconsistent command" << endl;
    return -1;
  }
}


int idQ::EasyQuantis::EasyQuantisCmd::Acquisition(const RandomDataGenerationInfo& randomDataGenerationInfo, const std::string& filename)
{  
  
  // Get start time
  boost::posix_time::ptime timeStart = boost::posix_time::second_clock::local_time();

  
  Quantis2File quantis2File;
  
  // Error message returned on the thread
  string errorMessage;

  // Launch data acquisition
  boost::thread threadGenerate(boost::bind(&Quantis2File::GenerateRandomFile,
                                          &quantis2File,
                                          &randomDataGenerationInfo,
                                          filename,
                                          &errorMessage));

  // Wait some time to be sure the thread is started
  boost::this_thread::sleep(boost::posix_time::millisec(100));

  // Update progress
  string details;
  while (quantis2File.GetRemainingSize() > 0u)
  {
    // Clear line
    cout << '\r' << string(details.length(), ' ') << '\r' << flush;
    details.clear();

    // Build info 
    int progressValue = 0;

    details = Utils::BuildProgressInfoString(timeStart,
                                            randomDataGenerationInfo,
                                            quantis2File.GetRemainingSize(),
                                            progressValue);

    if (!details.empty())
    {
      details.append(" [");
      details.append(boost::lexical_cast<string>(progressValue));
      details.append("%]");
    }

    if (!errorMessage.empty())
    {
      break;
    }

    cout << details << flush;

    // Wait some time: it is useless and just time-consuming to update too
    // frequently the screen
    boost::this_thread::sleep(boost::posix_time::millisec(250));
  } // while

  // Clear line
  cout << '\r' << string(details.length(), ' ') << '\r' << flush;

  // Waits until background thread terminates
  threadGenerate.join();
  
  if (!errorMessage.empty())
  {
    cerr << errorMessage << endl;
    return -1;
  }
  else
  {
    cout << "Done." << endl;
    return 0;
  }   
}


int idQ::EasyQuantis::EasyQuantisCmd::ExtractionFromFile(FileExtractionGenerationInfo& fileExtractionGenerationInfo)
{
  if(fileExtractionGenerationInfo.inputFile.size() == 0)
  {
    cerr << "The input file must be defined, --extraction-input-file argument is missing" << endl;
    return -1;
  }
  
  if(fileExtractionGenerationInfo.extractorMatrixFilename.size() == 0)
  {
    cerr << "The matrix file must be defined, --matrix-file argument is missing" << endl;
    return -1;
  }
 
  // Creates Quantis2File
  Quantis2File quantis2File;
  
  // Get start time
  boost::posix_time::ptime timeStart = boost::posix_time::second_clock::local_time();

  // Error message returned on the thread
  string errorMessage;

  // Launch data acquisition
  boost::thread threadGenerate(boost::function<void()>(boost::bind(&Quantis2File::ProcessExtractionFile,
                                                                   &quantis2File,
                                                                   &fileExtractionGenerationInfo,
                                                                   &errorMessage)));

  // Wait some time to be sure the thread is started
  boost::this_thread::sleep(boost::posix_time::millisec(100));

  string details;
  
  // Update progression
  while (quantis2File.GetRemainingSize() > 0u)
  {


    if (!errorMessage.empty())
    {
      break;
    }
    else
    { 
      // Clear line
      cout << '\r' << string(details.length(), ' ') << '\r' << flush;
      details.clear();

      // Build info 
      int progressValue = 0;

      details = Utils::BuildProcessExtractionProgressInfoString(timeStart,
                                                                fileExtractionGenerationInfo,
                                                                quantis2File.GetRemainingSize(),
                                                                progressValue);      

      if (!details.empty())
      {
        details.append(" [");
        details.append(boost::lexical_cast<string>(progressValue));
        details.append("%]");
      }

      if (!errorMessage.empty())
      {
        break;
      }

      cout << details << flush;      

    }

    // Wait some time: it is useless and just time-consuming to update too
    // frequently the dialog
    boost::this_thread::sleep(boost::posix_time::millisec(500));
  } // while

  
  cout << '\r' << string(details.length(), ' ') << '\r' << flush;
  
  // Waits until background thread terminates
  threadGenerate.join();

  
  
  if (!errorMessage.empty())
  {
    cerr << errorMessage << endl;
    return -1;
  }
  else
  {
    // Status message
    cout << "Done." << endl;
    return 0;
  }
 
}


void idQ::EasyQuantis::EasyQuantisCmd::PrintUsage(
    char* programName, 
    boost::program_options::options_description& desc)
{
  fs::path programPath (programName);
  cout << EASY_QUANTIS_APPTITLE << endl;
  cout << endl;
  cout << "Usage:" << endl;
  cout << "  " << programPath.filename() << " [options]" << endl;
  cout << desc << endl;
  cout << "Examples:" << endl;
  cout << "  The following display all Quantis (USB, PCI or PCIe) devices found" << endl;
  cout << "    " << programPath.filename() << " -l" << endl;
  cout << endl;  
  cout << "  The following generates a binary file of 1Gbyte named random.dat using" << endl;
  cout << "  first Quantis PCI device:" << endl;
  cout << "    " << programPath.filename() << " -p 0 -b random.dat -n 1073741824" << endl;
  cout << endl;
  cout << "  The following generates the file integers.dat with 10 numbers (one number " << endl;
  cout << "  per line) whose values are between 1 and 6 with first Quantis USB device:" << endl;
  cout << "    " << programPath.filename() << " -u 0 -i integers.dat -n 10 --min 1 --max 6" << endl;
  cout << endl;
  cout << "  The following generates the file extracted.dat with 10 numbers (one number " << endl;
  cout << "  per line) with the first Quantis USB device using extraction processing " << endl;
  cout << "  with a 2048x1972 bits matrix size:" << endl;  
  cout << "    " << programPath.filename() << " -u 0 -i integers.dat -n 10 " << endl;
  cout << "     --matrix-file default_idq_matrix.dat --matrix-size-in 2048 --matrix-size-out 1792" << endl;
  cout << endl;
  cout << "  The following Reads an binary input file input.dat apply extraction processing" << endl;
  cout << "  with default_idq_matrix.dat matrix file and save result in output.dat" << endl;
  cout << "    " << programPath.filename() << " -m default_idq_matrix.dat --extraction-from-file " << endl; 
  cout << "     --extraction-input-file input.dat --extraction-output-file output.dat" << endl << endl;
  
  
  
}


void idQ::EasyQuantis::EasyQuantisCmd::PrintDevicesList()
{
  PrintDevicesList(QUANTIS_DEVICE_PCI);
  PrintDevicesList(QUANTIS_DEVICE_USB);
}


void idQ::EasyQuantis::EasyQuantisCmd::PrintDevicesList(QuantisDeviceType deviceType)
{
  string deviceTypeStr;
  switch(deviceType)
  {
    case QUANTIS_DEVICE_PCI:
      deviceTypeStr = "PCI";
      break;

    case QUANTIS_DEVICE_USB:
      deviceTypeStr = "USB";
      break;

    default:
      deviceTypeStr = "Unknown";
      break;
  }

  int devicesCount = Quantis::Count(deviceType);
  if (devicesCount <= 0)
  {
    cout << "No Quantis " << deviceTypeStr << " device found." << endl;
    return;
  }

  for (int i = 0; i < devicesCount; i++)
  {
    try
    {
      // Creates a quantis object
      Quantis quantis(deviceType, i);

      // Display device info
      cout << "* Quantis " << deviceTypeStr << " device #" << i << endl;
      cout << "    core version: " << hex << quantis.GetBoardVersion() << endl;
      cout << "    serial number: " << hex << quantis.GetSerialNumber() << endl;
      cout << "    manufacturer: " << hex << quantis.GetManufacturer() << endl;

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
    } // try
    catch (runtime_error &ex)
    {
      cerr << "Error while getting devices information: " << ex.what() << endl;
    } // catch
  } // for
}

