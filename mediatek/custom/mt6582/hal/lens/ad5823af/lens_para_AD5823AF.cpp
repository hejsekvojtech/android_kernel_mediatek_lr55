/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_lens.h"

const NVRAM_LENS_PARA_STRUCT AD5823AF_LENS_PARA_DEFAULT_VALUE =
{
    //Version
    NVRAM_CAMERA_LENS_FILE_VERSION,

    // Focus Range NVRAM
    {0, 1023},

    // AF NVRAM
    {
        // -------- AF ------------
        {50,  // i4Offset
         13,  // i4NormalNum
         13,  // i4MacroNum
         0,  // i4InfIdxOffset
         0,  // i4MacroIdxOffset
        {
                       0,  15,  30,  50,  95, 140, 184, 224, 262, 300,
                     350, 420, 510,   0,   0,   0,   0,   0,   0,   0,
                       0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        },
        15,  // i4THRES_MAIN
        10,  // i4THRES_SUB
        4,  // i4INIT_WAIT
        {0, 500, 500, 500, 500},  // i4FRAME_WAIT
        0,  // i4DONE_WAIT
        0,  // i4FAIL_POS
        33,  // i4FRAME_TIME
        5,  // i4FIRST_FV_WAIT
        40,  // i4FV_CHANGE_THRES
        10000,  // i4FV_CHANGE_OFFSET
        10,  // i4FV_CHANGE_CNT
        0,  // i4GS_CHANGE_THRES
        15,  // i4GS_CHANGE_OFFSET
        12,  // i4GS_CHANGE_CNT
        5,  // i4FV_STABLE_THRES
        10000,  // i4FV_STABLE_OFFSET
        6,  // i4FV_STABLE_NUM
        6,  // i4FV_STABLE_CNT
        5,  // i4FV_1ST_STABLE_THRES
        10000,  // i4FV_1ST_STABLE_OFFSET
        6,  // i4FV_1ST_STABLE_NUM
        6  // i4FV_1ST_STABLE_CNT
        },

        //-------- ZSD AF ------------
        {50,  // i4Offset
         13,  // i4NormalNum
         13,  // i4MacroNum
         0,  // i4InfIdxOffset
         0,  // i4MacroIdxOffset
        {
                       0,  15,  30,  50,  95, 140, 184, 224, 262, 300,
                     350, 420, 510,   0,   0,   0,   0,   0,   0,   0,
                       0,   0,   0,   0,   0,   0,   0,   0,   0,   0
        },
        15,  // i4THRES_MAIN
        10,  // i4THRES_SUB
        4,  // i4INIT_WAIT
        {0, 500, 500, 500, 500}, // i4FRAME_WAIT
        0,  // i4DONE_WAIT
        0,  // i4FAIL_POS
        33,  // i4FRAME_TIME
        5,  // i4FIRST_FV_WAIT
        40,  // i4FV_CHANGE_THRES
        1000,  // i4FV_CHANGE_OFFSET
        12,  // i4FV_CHANGE_CNT
        0,  // i4GS_CHANGE_THRES
        20,  // i4GS_CHANGE_OFFSET
        12,  // i4GS_CHANGE_CNT
        5,  // i4FV_STABLE_THRES
        10000,  // i4FV_STABLE_OFFSET
        6,  // i4FV_STABLE_NUM
        6,  // i4FV_STABLE_CNT
        5,  // i4FV_1ST_STABLE_THRES
        10000,  // i4FV_1ST_STABLE_OFFSET
        6,  // i4FV_1ST_STABLE_NUM
        6  // i4FV_1ST_STABLE_CNT
        },

        //-------- VAFC ------------
        {50,  // i4Offset
         13,  // i4NormalNum
         13,  // i4MacroNum
         0,  // i4InfIdxOffset
         0,  // i4MacroIdxOffset
        {
                       0,  15,  30,  50,  95, 140, 184, 224, 262, 300,
                     350, 420, 510,   0,   0,   0,   0,   0,   0,   0,
                       0,   0,   0,   0,   0,   0,   0,   0,   0,   0
        },
        15,  // i4THRES_MAIN
        10,  // i4THRES_SUB
        4,  // i4INIT_WAIT
        {0, 500, 500, 500, 500},  // i4FRAME_WAIT
        0,  // i4DONE_WAIT
        0,  // i4FAIL_POS
        33,  // i4FRAME_TIME
        5,  // i4FIRST_FV_WAIT
        40,  // i4FV_CHANGE_THRES
        20000,  // i4FV_CHANGE_OFFSET
        12,  // i4FV_CHANGE_CNT
        0,  // i4GS_CHANGE_THRES
        20,  // i4GS_CHANGE_OFFSET
        12,  // i4GS_CHANGE_CNT
        10,  // i4FV_STABLE_THRES
        10000,  // i4FV_STABLE_OFFSET
        8,  // i4FV_STABLE_NUM
        7,  // i4FV_STABLE_CNT
        20,  // i4FV_1ST_STABLE_THRES
        15000,  // i4FV_1ST_STABLE_OFFSET
        12,  // i4FV_1ST_STABLE_NUM
        10  // i4FV_1ST_STABLE_CNT
        },

        //-------- sAF_TH ------------
        {
        8,  // i4ISONum;
        {100,150,200,300,400,600,800,1600},    // i4ISO[ISO_MAX_NUM];
        6,  // i4GMeanNum;
        {20,55,105,150,180,205},    // i4GMean[GMEAN_MAX_NUM];
        {
                89,89,89,88,87,87,87,86,
                127,127,127,126,126,126,125,125,
                180,180,180,180,180,180,180,179
            }, // i4GMR[3][ISO_MAX_NUM];
        {
                10000,12000,14000,16000,20000,24000,30000,40000,
                10000,12000,14000,16000,20000,24000,30000,40000,
                10000,12000,14000,16000,20000,24000,30000,40000,
                10000,12000,14000,16000,20000,24000,30000,40000,
                10000,12000,14000,16000,20000,24000,30000,40000,
                10000,12000,14000,16000,20000,24000,30000,40000
            }, // i4FV_DC[GMEAN_MAX_NUM][ISO_MAX_NUM];
            {
                7000,7000,7000,7000,7000,7000,7000,7000,
                7000,7000,7000,7000,7000,7000,7000,7000,
                7000,7000,7000,7000,7000,7000,7000,7000,
                7000,7000,7000,7000,7000,7000,7000,7000,
                7000,7000,7000,7000,7000,7000,7000,7000,
            7000,7000,7000,7000,7000,7000,7000,7000
            }, // i4MIN_TH[GMEAN_MAX_NUM][ISO_MAX_NUM];  
            {
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10
            },// i4HW_TH[GMEAN_MAX_NUM][ISO_MAX_NUM]; 
            {
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0
            }, // i4FV_DC2[GMEAN_MAX_NUM][ISO_MAX_NUM];
            {
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0
            }, // i4MIN_TH2[GMEAN_MAX_NUM][ISO_MAX_NUM];
            {
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10,
            4,4,4,4,5,6,8,10
            }
        }, // i4HW_TH2[GMEAN_MAX_NUM][ISO_MAX_NUM]; 
        //-------- sZSDAF_TH ------------
        {
            8,  // i4ISONum;
            {100,150,200,300,400,600,800,1600},    // i4ISO[ISO_MAX_NUM];
            6,  // i4GMeanNum;
            {20,55,105,150,180,205},    // i4GMean[GMEAN_MAX_NUM];
            {
            87,87,87,86,85,85,84,83,
            126,126,126,124,123,123,123,122,
            180,180,180,179,178,178,178,178
            }, // i4GMR[3][ISO_MAX_NUM];
            {
            10000,12000,14000,16000,20000,24000,30000,40000,
            10000,12000,14000,16000,20000,24000,30000,40000,
            10000,12000,14000,16000,20000,24000,30000,40000,
            10000,12000,14000,16000,20000,24000,30000,40000,
            10000,12000,14000,16000,20000,24000,30000,40000,
            10000,12000,14000,16000,20000,24000,30000,40000
            }, // i4FV_DC[GMEAN_MAX_NUM][ISO_MAX_NUM];
            {
            10000,10000,10000,10000,10000,9000,9000,9000,
            10000,10000,10000,10000,10000,9000,9000,9000,
            10000,10000,10000,10000,10000,9000,9000,9000,
            10000,10000,10000,10000,10000,9000,9000,9000,
            10000,10000,10000,10000,10000,9000,9000,9000,
            10000,10000,10000,10000,10000,9000,9000,9000
            }, // i4MIN_TH[GMEAN_MAX_NUM][ISO_MAX_NUM];  
            {
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16
            }, // i4HW_TH[GMEAN_MAX_NUM][ISO_MAX_NUM]; 
            {
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0
            }, // i4FV_DC2[GMEAN_MAX_NUM][ISO_MAX_NUM];
            {
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0
            }, // i4MIN_TH2[GMEAN_MAX_NUM][ISO_MAX_NUM];
            {
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16,
            5,6,9,9,11,12,14,16
            }
        }, // i4HW_TH2[GMEAN_MAX_NUM][ISO_MAX_NUM]; 
        1,  // i4VAFC_FAIL_CNT
        0,  // i4CHANGE_CNT_DELTA
        0,  // i4LV_THRES
        18,  // i4WIN_PERCENT_W
        24,  // i4WIN_PERCENT_H
        50,  // i4InfPos
        20,  // i4AFC_STEP_SIZE
        {
            {50,100,150,200,250},  // back to bestpos step
            {0,0,0,0,0},  // hysteresis compensate step
        },
        {0,100,200,350,500},    // back jump
        300,  //i4BackJumpPos;
        80,  //i4FDWinPercent;
        40,  //i4FDSizeDiff;
        3,  //i4StatGain;
        {
            0,0,0,0,0,0,0,0,0,0,        
            0,0,0,0,0,0,0,0,0,0,        
        } // i4Coef[20];
	},
    {0}
};

UINT32 AD5823AF_getDefaultData(VOID *pDataBuf, UINT32 size)
{
    UINT32 dataSize = sizeof(NVRAM_LENS_PARA_STRUCT);

    if ((pDataBuf == NULL) || (size < dataSize))
    {
        return 1;
    }

    // copy from Buff to global struct
    memcpy(pDataBuf, &AD5823AF_LENS_PARA_DEFAULT_VALUE, dataSize);

    return 0;
}

PFUNC_GETLENSDEFAULT pAD5823AF_getDefaultData = AD5823AF_getDefaultData;

