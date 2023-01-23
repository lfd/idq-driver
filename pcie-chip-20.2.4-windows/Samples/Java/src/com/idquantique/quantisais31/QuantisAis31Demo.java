/*
 * QuantisAis31 Library for Java
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
 */

package com.idquantique.quantisais31;

import java.math.BigInteger;
import com.idquantique.quantis.*;

/**
 *
 * @author ID Quantique SA
 */
public class QuantisAis31Demo {

  /**
   * @param args the command line arguments
   * @throws QuantisException, QuantisAis31Exception
   */
  public static void main(String[] args) throws QuantisException, QuantisAis31Exception {

    System.out.println("QuantisAis31Demo for Java\n");

    System.out.println(
        "Searching QuantisAis31 library in following path:\n" + System.getProperty("java.library.path") + "\n");

    System.out.println("Using QuantisAis31 Library v" + Quantis.GetLibVersion() + "\n");

    int countPci = Quantis.Count(Quantis.QuantisDeviceType.QUANTIS_DEVICE_PCI);
    System.out.println("Found " + countPci + " Quantis PCI devices.");
    int countUsb = Quantis.Count(Quantis.QuantisDeviceType.QUANTIS_DEVICE_USB);
    System.out.println("Found " + countUsb + " Quantis USB devices.");

    Quantis quantis;
    if (countPci > 0) {
      System.out.println("Using first Quantis PCI device.");
      quantis = new Quantis(Quantis.QuantisDeviceType.QUANTIS_DEVICE_PCI, 0);
    } else if (countUsb > 0) {
      System.out.println("Using first Quantis USB device.");
      quantis = new Quantis(Quantis.QuantisDeviceType.QUANTIS_DEVICE_USB, 0);
    } else {
      System.out.println("No Quantis device installed.");
      return;
    }

    System.out.println("Board version: " + Integer.toHexString(quantis.GetBoardVersion()).toUpperCase());

    System.out.println("Serial number: " + quantis.GetSerialNumber());

    System.out.println("Manufacturer: " + quantis.GetManufacturer());

    QuantisAis31 quantisAis31 = new QuantisAis31();
    if (countPci > 0) {
      quantisAis31.QuantisAis31Open(Quantis.QuantisDeviceType.QUANTIS_DEVICE_PCI, 0);
    } else if (countUsb > 0) {
      quantisAis31.QuantisAis31Open(Quantis.QuantisDeviceType.QUANTIS_DEVICE_USB, 0);
    }

    byte[] data = quantisAis31.Read(4);
    System.out.println("Data: " + String.format("%X", new BigInteger(data)));
    System.out.println("Double: " + quantisAis31.ReadDouble());
    System.out.println("Float: " + quantisAis31.ReadFloat());
    System.out.println("Int: " + quantisAis31.ReadInt());
    System.out.println("Short: " + quantisAis31.ReadShort());

    quantisAis31.QuantisAis31Close();
  }

}
