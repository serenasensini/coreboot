/** @file

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

  This file is automatically generated. Please do NOT modify !!!

**/

#ifndef __FSPUPDVPD_H__
#define __FSPUPDVPD_H__

#pragma pack(1)


/** TODO - Port to fit Quark SoC.
**/

#define MAX_CHANNELS_NUM       1
#define MAX_DIMMS_NUM          1

typedef struct {
	UINT8         DimmId;
	UINT32        SizeInMb;
	UINT16        MfgId;
	/** Module part number for DDR3 is 18 bytes however for
	 DRR4 20 bytes as per JEDEC Spec, so reserving 20 bytes
	 **/
	UINT8         ModulePartNum[20];
} DIMM_INFO;

typedef struct {
	UINT8         ChannelId;
	UINT8         DimmCount;
	DIMM_INFO     DimmInfo[MAX_DIMMS_NUM];
} CHANNEL_INFO;

typedef struct {
	UINT8         Revision;
	UINT16        DataWidth;
	/** As defined in SMBIOS 3.0 spec
	Section 7.18.2 and Table 75
	**/
	UINT8         MemoryType;
	UINT16        MemoryFrequencyInMHz;
	/** As defined in SMBIOS 3.0 spec
	Section 7.17.3 and Table 72
	**/
	UINT8         ErrorCorrectionType;
	UINT8         ChannelCount;
	CHANNEL_INFO  ChannelInfo[MAX_CHANNELS_NUM];
} FSP_SMBIOS_MEMORY_INFO;



typedef struct {
/** Offset 0x0018
**/
  UINT64                      Signature;
/** Offset 0x0020
**/
  UINT64                      Revision;
/** Offset 0x0028
**/
  UINT32                      RmuBaseAddress;
/** Offset 0x002C
**/
  UINT32                      RmuLength;
/** Offset 0x0030
**/
  UINT32                      SerialPortBaseAddress;
/** Offset 0x0034
**/
  UINT32                      tRAS;
/** Offset 0x0038
**/
  UINT32                      tWTR;
/** Offset 0x003C
**/
  UINT32                      tRRD;
/** Offset 0x0040
**/
  UINT32                      tFAW;
/** Offset 0x0044
**/
  UINT32                      Flags;
/** Offset 0x0048
**/
  UINT8                       DramWidth;
/** Offset 0x0049
**/
  UINT8                       DramSpeed;
/** Offset 0x004A
**/
  UINT8                       DramType;
/** Offset 0x004B
**/
  UINT8                       RankMask;
/** Offset 0x004C
**/
  UINT8                       ChanMask;
/** Offset 0x004D
**/
  UINT8                       ChanWidth;
/** Offset 0x004E
**/
  UINT8                       AddrMode;
/** Offset 0x004F
**/
  UINT8                       SrInt;
/** Offset 0x0050
**/
  UINT8                       SrTemp;
/** Offset 0x0051
**/
  UINT8                       DramRonVal;
/** Offset 0x0052
**/
  UINT8                       DramRttNomVal;
/** Offset 0x0053
**/
  UINT8                       DramRttWrVal;
/** Offset 0x0054
**/
  UINT8                       SocRdOdtVal;
/** Offset 0x0055
**/
  UINT8                       SocWrRonVal;
/** Offset 0x0056
**/
  UINT8                       SocWrSlewRate;
/** Offset 0x0057
**/
  UINT8                       DramDensity;
/** Offset 0x0058
**/
  UINT8                       tCL;
/** Offset 0x0059
**/
  UINT8                       EccScrubInterval;
/** Offset 0x005A
**/
  UINT8                       EccScrubBlkSize;
/** Offset 0x005B
**/
  UINT8                       SmmTsegSize;
/** Offset 0x005C
**/
  UINT32                      FspReservedMemoryLength;
/** Offset 0x0060
**/
  UINT32                      MrcDataPtr;
/** Offset 0x0064
**/
  UINT32                      MrcDataLength;
/** Offset 0x0068
**/
  UINT8                       ReservedMemoryInitUpd[8];
} MEMORY_INIT_UPD;

typedef struct {
/** Offset 0x0070
**/
  UINT64                      Signature;
/** Offset 0x0078
**/
  UINT64                      Revision;
/** Offset 0x0080
**/
  UINT16                      PcdRegionTerminator;
} SILICON_INIT_UPD;

#define FSP_UPD_SIGNATURE                0x244450554B525124        /* '$QRKUPD$' */
#define FSP_MEMORY_INIT_UPD_SIGNATURE    0x244450554D454D24        /* '$MEMUPD$' */
#define FSP_SILICON_INIT_UPD_SIGNATURE   0x244450555F495324        /* '$SI_UPD$' */

typedef struct _UPD_DATA_REGION {
/** Offset 0x0000
**/
  UINT64                      Signature;
/** Offset 0x0008
**/
  UINT64                      Revision;
/** Offset 0x0010
**/
  UINT32                      MemoryInitUpdOffset;
/** Offset 0x0014
**/
  UINT32                      SiliconInitUpdOffset;
/** Offset 0x0018
**/
  MEMORY_INIT_UPD             MemoryInitUpd;
/** Offset 0x0070
**/
  SILICON_INIT_UPD            SiliconInitUpd;
} UPD_DATA_REGION;

#define FSP_IMAGE_ID    0x305053462D4B5551        /* 'QUK-FSP0' */
#define FSP_IMAGE_REV   0x00000000

typedef struct _VPD_DATA_REGION {
/** Offset 0x0000
**/
  UINT64                      PcdVpdRegionSign;
/** Offset 0x0008
    PcdImageRevision
**/
  UINT32                      PcdImageRevision;
/** Offset 0x000C
**/
  UINT32                      PcdUpdRegionOffset;
} VPD_DATA_REGION;

#pragma pack()

#endif
