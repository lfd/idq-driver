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

import com.idquantique.quantis.*;

/**
 * This class is used quantis java library can be used to access Quantis Module.
 * This library is OS independent and has been tested with Windows and Linux.
 */
public class QuantisExtractor {

    static {
        System.loadLibrary("Quantis_Extensions");
    }

    private Quantis.QuantisDeviceType _deviceType;
    private int _deviceNumber;

    public QuantisExtractor(Quantis.QuantisDeviceType deviceType, int deviceNumber) {
        _deviceType = deviceType;
        _deviceNumber = deviceNumber;
    }

    public static float GetLibVersion() {
        return JniGetLibVersion();
    }

    public void InitializeMatrix(String matrixFileName, short matrixSizeIn, short matrixSizeOut)
            throws QuantisExtractorException {
        JniInitializeMatrix(matrixFileName, matrixSizeIn, matrixSizeOut);
    }

    public void GetDataFromQuantis(int amountOfBytes, byte[] outputBuffer) throws QuantisExtractorException {
        JniGetDataFromQuantis(_deviceType.getType(), _deviceNumber, amountOfBytes, outputBuffer);
    }

    public void UninitializeMatrix() throws QuantisExtractorException {
        JniUninitializeMatrix();
    }

    // -------------------- Native methods declaration --------------------
    private static native float JniGetLibVersion();

    private native void JniInitializeMatrix(String matrixFileName, short matrixSizeIn, short matrixSizeOut)
            throws QuantisExtractorException;

    private native void JniGetDataFromQuantis(int deviceType, int deviceNumber, int amountOfBytes, byte[] outputBuffer)
            throws QuantisExtractorException;

    private native void JniUninitializeMatrix() throws QuantisExtractorException;
}