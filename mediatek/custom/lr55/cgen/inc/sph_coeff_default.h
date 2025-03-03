#ifndef SPEECH_COEFF_DEFAULT_H
#define SPEECH_COEFF_DEFAULT_H

#ifndef FALSE
#define FALSE 0
#endif

//speech parameter depen on BT_CHIP cersion
#if defined(MTK_MT6611)

#define BT_COMP_FILTER (1 << 15)
#define BT_SYNC_DELAY  86

#elif defined(MTK_MT6612)

#define BT_COMP_FILTER (1 << 15)
#define BT_SYNC_DELAY  86

#elif defined(MTK_MT6616) || defined(MTK_MT6620) || defined(MTK_MT6622) || defined(MTK_MT6626) || defined(MTK_MT6628)

#define BT_COMP_FILTER (1 << 15)
#define BT_SYNC_DELAY  86

#else // MTK_MT6620

#define BT_COMP_FILTER (0 << 15)
#define BT_SYNC_DELAY  86

#endif

#ifdef MTK_DUAL_MIC_SUPPORT
#define SPEECH_MODE_PARA13 (371)
#define SPEECH_MODE_PARA14 (23)
#else
#define SPEECH_MODE_PARA13 (0)
#define SPEECH_MODE_PARA14 (0)
#endif

#define DEFAULT_SPEECH_NORMAL_MODE_PARA \
    96,   253, 16388,    29, 57351,   799,   400,    64,\
   400,  4325,   611,     0, 20488,   371,    23,  8192

#define DEFAULT_SPEECH_EARPHONE_MODE_PARA \
     0,   189, 10756,    31, 57351,    31,   400,    64,\
    80,  4325,   611,     0, 20488,     0,     0,     0

#define DEFAULT_SPEECH_BT_EARPHONE_MODE_PARA \
     0,   253, 10756,    31, 53255,    31,   400,     0,\
    80,  4325,   611,     0, 20488|BT_COMP_FILTER,     0,     0,BT_SYNC_DELAY

#define DEFAULT_SPEECH_LOUDSPK_MODE_PARA \
    96,   224,  5256,    31, 57351, 24607,   400,   128,\
    84,  4325,   611,     0, 20488,     0,     0,     0

#define DEFAULT_SPEECH_CARKIT_MODE_PARA \
    96,   224,  5256,    31, 57351, 24607,   400,   132,\
    84,  4325,   611,     0, 20488,     0,     0,     0

#define DEFAULT_SPEECH_BT_CORDLESS_MODE_PARA \
     0,     0,     0,     0,     0,     0,     0,     0,\
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_SPEECH_AUX1_MODE_PARA \
     0,     0,     0,     0,     0,     0,     0,     0,\
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_SPEECH_AUX2_MODE_PARA \
     0,     0,     0,     0,     0,     0,     0,     0,\
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_SPEECH_COMMON_PARA \
     0, 55997, 31000, 10752, 32769,     0,     0,     0, \
     0,     0,     0,     0

#define DEFAULT_SPEECH_VOL_PARA \
     0,     0,     0,     0

#define DEFAULT_AUDIO_DEBUG_INFO \
     0,     0,     0,     0,     0,     0,     0,     0, \
     0,     0,     0,     0,     0,     0,     0,     0

#define DEFAULT_VM_SUPPORT FALSE

#define DEFAULT_AUTO_VM FALSE

#define DEFAULT_WB_SPEECH_NORMAL_MODE_PARA \
    96,   253, 16388,    29, 57607,   799,   400,    64,\
   400,  4325,   611,     0, 16392,   371,    23,  8192

#define DEFAULT_WB_SPEECH_EARPHONE_MODE_PARA \
     0,   189, 10756,    31, 57607,    31,   400,    64,\
    80,  4325,   611,     0, 16392,     0,     0,     0

#define DEFAULT_WB_SPEECH_BT_EARPHONE_MODE_PARA \
     0,   253, 10756,    31, 53511,    31,   400,     0,\
    80,  4325,   611,     0, 16392|BT_COMP_FILTER,     0,     0,BT_SYNC_DELAY

#define DEFAULT_WB_SPEECH_LOUDSPK_MODE_PARA \
    96,   224,  5256,    31, 57607, 24607,   400,   132,\
    84,  4325,   611,     0, 16392,     0,     0,     0

#define DEFAULT_WB_SPEECH_CARKIT_MODE_PARA \
 65187, 65099, 65246,   210,    23, 65175, 64894,    81,\
 65358, 65356, 64773,   304, 65385, 65392, 65444,   403

#define DEFAULT_WB_SPEECH_BT_CORDLESS_MODE_PARA \
   132,   108,     4, 65464, 65367, 65490,    13, 65528,\
 65516, 65501,    36, 65293, 65411, 65266,    41,    29

#define DEFAULT_WB_SPEECH_AUX1_MODE_PARA \
  1423, 64520,   179, 63859, 65237, 64428,   948, 63395,\
  4920, 58450,  2701, 59576, 23197, 23197, 59576,  2701

#define DEFAULT_WB_SPEECH_AUX2_MODE_PARA \
 58450,  4920, 63395,   948, 64428, 65237, 63859,   179,\
 64520,  1423,   403, 65444, 65392, 65385,   304, 64773

#define MICBAIS  1900

/* The Bluetooth PCM digital volume */
/* default_bt_pcm_in_vol : uplink, only for enlarge volume,
                       0x100 : 0dB  gain
                       0x200 : 6dB  gain
                       0x300 : 9dB  gain
                       0x400 : 12dB gain
                       0x800 : 18dB gain
                       0xF00 : 24dB gain             */

#define DEFAULT_BT_PCM_IN_VOL  0x100
/* default_bt_pcm_out_vol : downlink gain,
                       0x1000 : 0dB; maximum 0x7FFF  */
#define DEFAULT_BT_PCM_OUT_VOL  0x1000

#endif
