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

import com.idquantique.quantis.*;

/**
 * The quantis java library can be used to access Quantis Module. This library
 * is OS independent and has been tested with Windows and Linux.
 */
public class QuantisAis31 {

    // Load the Library containing the native code implementation
    // NOTE: You might need to configure your environment so the loadLibrary
    // method can find the native code library.
    static {
        System.loadLibrary("Quantis_Ais31");
    }

    private long ais31DeviceHandle;

    /**
     * Create Quantis class.
     */
    public QuantisAis31() {
        // Do nothing
    }

    /**
     * Open a Quantis, must always be used before using the Quantis.
     * 
     * @param deviceType   specify the type of Quantis device.
     * @param deviceNumber the number of the Quantis device.
     */
    public void QuantisAis31Open(Quantis.QuantisDeviceType deviceType, int deviceNumber) throws QuantisAis31Exception {
        ais31DeviceHandle = QuantisAis31Open(deviceType.getType(), deviceNumber);
    }

    /**
     * Close a Quantis, must always be used after finishing using the Quantis.
     */
    public void QuantisAis31Close() throws QuantisAis31Exception {
        QuantisAis31Close(ais31DeviceHandle);
    }

    /**
     * Reads random data from the Quantis device.
     * 
     * @param size the number of bytes to read (not larger than
     *             QUANTIS_MAX_READ_SIZE).
     * @return The number of read bytes on success.
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public byte[] Read(int size) throws QuantisAis31Exception {
        return QuantisAis31Read(ais31DeviceHandle, size);
    }

    /**
     * Reads a random double floating precision value between 0.0 (inclusive) and
     * 1.0 (exclusive) from the Quantis device.
     * 
     * @return a random double floating precision value between 0.0 (inclusive) and
     *         1.0 (exclusive).
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public double ReadDouble() throws QuantisAis31Exception {
        return QuantisAis31ReadDouble01(ais31DeviceHandle);
    }

    /**
     * Reads a random number from the Quantis device and scale it to be between min
     * (inclusive) and max (exclusive).
     * 
     * @param min the minimal value the random number can take.
     * @param max the maximal value the random number can take.
     * @return a random double floating precision value between 0.0 (inclusive) and
     *         1.0 (exclusive).
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public double ReadDouble(double min, double max) throws QuantisAis31Exception {
        return QuantisAis31ReadScaledDouble(ais31DeviceHandle, min, max);
    }

    /**
     * Reads a random single floating precision value between 0.0 (inclusive) and
     * 1.0 (exclusive) from the Quantis device.
     * 
     * @return a random single floating precision value between 0.0 (inclusive) and
     *         1.0 (exclusive).
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public float ReadFloat() throws QuantisAis31Exception {
        return QuantisAis31ReadFloat01(ais31DeviceHandle);
    }

    /**
     * Reads a random number from the Quantis device and scale it to be between min
     * (inclusive) and max (exclusive).
     * 
     * @param min the minimal value the random number can take.
     * @param max the maximal value the random number can take.
     * @return a random single floating precision value between 0.0 (inclusive) and
     *         1.0 (exclusive).
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public float ReadFloat(float min, float max) throws QuantisAis31Exception {
        return QuantisAis31ReadScaledFloat(ais31DeviceHandle, min, max);

    }

    /**
     * Reads a random number from the Quantis device.
     * 
     * @return a random number.
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public int ReadInt() throws QuantisAis31Exception {
        return QuantisAis31ReadInt(ais31DeviceHandle);
    }

    /**
     * Reads a random number from the Quantis device and scale it to be between min
     * and max (inclusive).
     * 
     * @param min the minimal value the random number can take.
     * @param max the maximal value the random number can take.
     * @return a random number between min and max (inclusive).
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public int ReadInt(int min, int max) throws QuantisAis31Exception {
        return QuantisAis31ReadScaledInt(ais31DeviceHandle, min, max);
    }

    /**
     * Reads a random number from the Quantis device.
     * 
     * @return a random number.
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public short ReadShort() throws QuantisAis31Exception {
        return QuantisAis31ReadShort(ais31DeviceHandle);
    }

    /**
     * Reads a random number from the Quantis device and scale it to be between min
     * and max (inclusive).
     * 
     * @param min the minimal value the random number can take.
     * @param max the maximal value the random number can take.
     * @return a random number between min and max (inclusive).
     * @throws QuantisAis31Exception when unable to perform the operation.
     */
    public short ReadShort(short min, short max) throws QuantisAis31Exception {
        return QuantisAis31ReadScaledShort(ais31DeviceHandle, min, max);
    }

    // -------------------- Native methods declaration --------------------
    private static native long QuantisAis31Open(int deviceType, int deviceNumber) throws QuantisAis31Exception;

    private static native void QuantisAis31Close(long ais31DeviceHandle) throws QuantisAis31Exception;

    private static native byte[] QuantisAis31Read(long ais31DeviceHandle, int size) throws QuantisAis31Exception;

    private static native double QuantisAis31ReadDouble01(long ais31DeviceHandle) throws QuantisAis31Exception;

    private static native float QuantisAis31ReadFloat01(long ais31DeviceHandle) throws QuantisAis31Exception;

    private static native int QuantisAis31ReadInt(long ais31DeviceHandle) throws QuantisAis31Exception;

    private static native short QuantisAis31ReadShort(long ais31DeviceHandle) throws QuantisAis31Exception;

    private static native double QuantisAis31ReadScaledDouble(long ais31DeviceHandle, double min, double max)
            throws QuantisAis31Exception;

    private static native float QuantisAis31ReadScaledFloat(long ais31DeviceHandle, float min, float max)
            throws QuantisAis31Exception;

    private static native int QuantisAis31ReadScaledInt(long ais31DeviceHandle, int min, int max)
            throws QuantisAis31Exception;

    private static native short QuantisAis31ReadScaledShort(long ais31DeviceHandle, short min, short max)
            throws QuantisAis31Exception;
}
