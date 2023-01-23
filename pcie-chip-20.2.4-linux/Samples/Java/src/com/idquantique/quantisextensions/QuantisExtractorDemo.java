/*
 * Quantis Extensions Java Library
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
package com.idquantique.quantisextensions;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;
import java.lang.RuntimeException;
import com.idquantique.quantis.*;

/**
 * Demo-class for the QuantisExtractor.
 * 
 * @author ID Quantique SA
 */
public class QuantisExtractorDemo {
  private final Quantis.QuantisDeviceType _devType = Quantis.QuantisDeviceType.QUANTIS_DEVICE_PCI;
  private final Short _devNum = 0;
  private final QuantisExtractor _quex = new QuantisExtractor(_devType, _devNum);
  private AtomicBoolean _isStartedUp = new AtomicBoolean(); // Initialized to false

  public QuantisExtractorDemo() {
  }

  public void startup() {
    if (!_isStartedUp.get()) {
      System.out.println("QuantisExtractorDemo");
      System.out.println("--------------------");
      System.out.println("QuantisExtractor version: " + _quex.GetLibVersion());

      final String matrixFilename = "default_idq_matrix.dat";
      final Short sizeIn = 1024;
      final Short sizeOut = 768;
      try {
        _quex.InitializeMatrix(matrixFilename, sizeIn, sizeOut);
        _isStartedUp.set(Boolean.TRUE);
      } catch (QuantisExtractorException qe_exc) {
        System.out.println("EXTRACTOR'S MATRIX INITIALIZATION EXCEPTION: " + qe_exc.toString() + ".");
      }
    }
  }

  public void generateTrngFile(String outputFileName) throws IOException, QuantisExtractorException {
    if (!_isStartedUp.get()) {
      throw new RuntimeException("Demo not initialized.");
    }

    // TRNG generation process
    final FileOutputStream outstream = new FileOutputStream(new File(outputFileName));
    final int amountOfBytes = 8192;
    byte[] rngData = new byte[amountOfBytes];
    System.out.print("Generating data in " + outputFileName + ": ");
    for (long i = 0; i < 100L; i++) {
      _quex.GetDataFromQuantis(amountOfBytes, rngData);
      outstream.write(rngData);
      if (i % 10 == 0)
        System.out.print(".");
    }
    outstream.close();
    System.out.println("done.");
    System.out.println("FILE GENERATION COMPLETE.");
  }

  public void shutdown() {
    if (_isStartedUp.get()) {
      try {
        _quex.UninitializeMatrix();
        _isStartedUp.set(Boolean.FALSE);
      } catch (QuantisExtractorException qe_exc) {
        System.out.println("EXTRACTOR'S MATRIX UNINITIALIZATION EXCEPTION: " + qe_exc.toString() + ".");
        return;
      }
    }
  }

  /**
   * Main demo-program for the QuantisExtractor.
   */
  public static void main(String[] args) {
    QuantisExtractorDemo demo = new QuantisExtractorDemo();
    demo.startup();
    demo.startup();

    try {
      for (int i = 1; i <= 3; i++) {
        String fileName = "QuantisExtractorDemo" + i + ".bin";
        demo.generateTrngFile(fileName);
      }
    } catch (Exception exc) {
      System.out.println("EXCEPTION: " + exc.toString() + ".");
    } finally {
      demo.shutdown();
      demo.shutdown();
    }

  }
}// class
