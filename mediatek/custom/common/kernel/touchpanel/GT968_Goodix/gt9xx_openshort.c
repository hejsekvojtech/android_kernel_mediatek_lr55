/* drivers/input/touchscreen/gt9xx_shorttp.c
 * 
 * 2010 - 2012 Goodix Technology.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be a reference 
 * to you, when you are integrating the GOODiX's CTP IC into your system, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details.
 * 
 * Version:1.0
 * Author: meta@goodix.com
 * Accomplished Date:2012/10/20
 * Revision record:
 *
 */

#include "gt9xx_openshort.h"

//************** Customer Config Start ***********************//
// short test
#define GTP_SHORT_GND
#define GTP_VDD         28      // 2.8V

// open test
#define DEFAULT_TEST_ITEMS  (_MAX_TEST | _MIN_TEST /*| _KEY_MAX_TEST | _KEY_MIN_TEST | _UNIFORMITY_TEST*/)

u16 max_limit_value = 2575;     // screen max limit
u16 min_limit_value = 855;     // screen min limit
u16 max_limit_key = 0;       // key_val max limit
u16 min_limit_key = 0;        // key_val min limit
u16 uniformity_lmt = 70;        // screen uniformity in percent

#define DSP_SHORT_BURN_CHK          256 // burn short chuck size
#define _SHORT_INFO_MAX             50  // short test max show 50 pairs short channels
#define _BEYOND_INFO_MAX            20  // open test max show 20 infos for each test item
#define GTP_OPEN_SAMPLE_NUM         16  // open test raw data sampled count
#define GTP_TEST_INFO_MAX           200 // test info lines max count

#define OPEN_TEST_CFG_GROUP {\
	0x00,0xD0,0x02,0x00,0x05,0x05,0x05,0x00,0x01,0x08,\
	0x1E,0x0E,0x46,0x32,0x03,0x05,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x88,0x07,0x0A,\
	0x33,0x00,0xD3,0x07,0x00,0x00,0x00,0x22,0x03,0x1D,\
	0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x08,0x0A,0x0C,0x0E,0x10,0x12,0x14,0x16,\
	0x18,0x1A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x02,0x04,0x05,0x06,0x08,0x0A,0x0C,\
	0x1D,0x1E,0x1F,0x20,0x22,0x24,0x28,0xFF,0xFF,0xFF,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x32,0x32,0x32,0x32,0x14,0x14,\
	0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x14,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0xCD,0x01\
	}

//****************** Customer Config End ***********************//

// porting for touch driver start
//add raw data to file
#define tp_opentest_raw_file  "/sdcard/tp_opentest_raw_file.txt"
#define tp_shorttest_raw_file  "/sdcard/tp_shorttest_raw_file.txt"
// porting for touch driver end

extern s32 gtp_send_cfg(struct i2c_client *client);
extern s32 gtp_i2c_read(struct i2c_client *, u8 *, s32);
extern s32 gtp_i2c_write(struct i2c_client *, u8 *, s32);
extern int i2c_write_bytes(struct i2c_client *client, u16 addr, u8 *txbuf, int len);
extern int i2c_read_bytes(struct i2c_client *client, u16 addr, u8 *rxbuf, int len);
extern void gtp_reset_guitar(struct i2c_client*, s32);
extern s32 gup_enter_update_mode(struct i2c_client *);
extern s32 gup_leave_update_mode(void);
extern s32 gtp_send_cfg(struct i2c_client *client);
extern s32 gtp_read_version(struct i2c_client *client, u16* version);
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);

extern u8 gtp_rawdiff_mode;
extern struct i2c_client * i2c_client_point;
extern u8 cfg_len;
extern int tpd_halt;
extern u8 cfg_len;

u8  gt9xx_drv_num = MAX_DRIVER_NUM; // default driver and sensor number
u8  gt9xx_sen_num = MAX_SENSOR_NUM;
u16 gt9xx_pixel_cnt = MAX_DRIVER_NUM * MAX_SENSOR_NUM;
u16 gt9xx_sc_pxl_cnt = MAX_DRIVER_NUM * MAX_SENSOR_NUM;
struct gt9xx_short_info *short_sum; 

u8 chip_type_gt9f = 0;
u8 have_key = 0;
u8 gt9xx_sc_drv_num;
u8 key_is_isolated; // 0: no, 1: yes
u8 key_iso_pos[5];

struct kobject *goodix_debug_kobj;
static u8  rslt_buf_idx = 0;
static s32 *test_rslt_buf;  
static struct gt9xx_open_info *touchpad_sum;
    
#define _MIN_ERROR_NUM      (GTP_OPEN_SAMPLE_NUM * 9 / 10)

static char *result_lines[GTP_TEST_INFO_MAX];
static char tmp_info_line[80];
static u16 RsltIndex;

// porting for touch driver start
//add SET_INFO_LINE_FTM
static char *result_lines_FTM[GTP_TEST_INFO_MAX];
static char tmp_info_line_FTM[80];
static u16 RsltIndex_FTM;

static int gtp_open_test_init(struct i2c_client *client)
{
	int ret = 1;
	int retry = 0;

	u8 open_test_cfg_group[] =  OPEN_TEST_CFG_GROUP;
	u8 opentest_config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
    = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

	memset(&opentest_config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&opentest_config[GTP_ADDR_LENGTH], open_test_cfg_group, cfg_len);

	for (retry = 0; retry < 5; retry++)
    {
		ret = gtp_i2c_write(client, opentest_config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);

        if (ret > 0)
        {
            break;
        }
		msleep(10);
    }
	msleep(300);
	return ret;
}

static void append_info_line_FTM(void)
{
    if (strlen(tmp_info_line_FTM) != 0)
    {
        result_lines_FTM[RsltIndex_FTM] = (char *)kzalloc(strlen(tmp_info_line_FTM)+1, GFP_KERNEL);
        memcpy(result_lines_FTM[RsltIndex_FTM], tmp_info_line_FTM, strlen(tmp_info_line_FTM));
    }
    if (RsltIndex_FTM != (GTP_TEST_INFO_MAX-1))
        ++RsltIndex_FTM;
    else {
        kfree(result_lines_FTM[RsltIndex_FTM]);
    }
}
#define SET_INFO_LINE_FTM(fmt, args...)       do{ memset(tmp_info_line_FTM, '\0', 80);\
                                                   sprintf(tmp_info_line_FTM, fmt"\n", ##args);\
                                                   GTP_INFO(fmt, ##args);\
                                                append_info_line_FTM();} while(0)
// porting for touch driver end

static void append_info_line(void)
{
    if (strlen(tmp_info_line) != 0)
    {
        result_lines[RsltIndex] = (char *)kzalloc(strlen(tmp_info_line)+1, GFP_KERNEL);
        memcpy(result_lines[RsltIndex], tmp_info_line, strlen(tmp_info_line));
    }
    if (RsltIndex != (GTP_TEST_INFO_MAX-1))
        ++RsltIndex;
    else {
        kfree(result_lines[RsltIndex]);
    }
}


                  
#define SET_INFO_LINE_INFO(fmt, args...)       do{ memset(tmp_info_line, '\0', 80);\
                                                   sprintf(tmp_info_line, "<Sysfs-INFO>"fmt"\n", ##args);\
                                                   GTP_INFO(fmt, ##args);\
                                                append_info_line();} while(0)
                                                   
#define SET_INFO_LINE_ERR(fmt, args...)        do { memset(tmp_info_line, '\0', 80);\
                                                   sprintf(tmp_info_line, "<Sysfs-ERROR>"fmt"\n", ##args);\
                                                   GTP_ERROR(fmt, ##args);\
                                                   append_info_line();}while(0)


static u8 cfg_drv_order[MAX_DRIVER_NUM];
static u8 cfg_sen_order[MAX_SENSOR_NUM];

static void gtp_irq_disable(void)
{
// porting for touch driver start
    //mt65xx_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
    mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
// porting for touch driver end
}

static void gtp_irq_enable(void)
{
// porting for touch driver start
    //mt65xx_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
    mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
// porting for touch driver end
}
/*
 * Initialize cfg_drv_order and cfg_sen_order, which is used for report short channels
 *
 */

s32 gt9xx_short_parse_cfg(void)
{
    u8 i = 0;
    u8 drv_num = 0, sen_num = 0;
    
    u8 config[256] = {(u8)(GTP_REG_CONFIG_DATA >> 8), (u8)GTP_REG_CONFIG_DATA, 0};
    
    if (gtp_i2c_read(i2c_client_point, config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH) <= 0)
    {
        SET_INFO_LINE_ERR("Failed to read config!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Failed to read config!");
        // porting for touch driver end
        return FAIL;
    }
    
    drv_num = (config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT-GT9_REG_CFG_BEG] & 0x1F)
                        + (config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT+1 -GT9_REG_CFG_BEG] & 0x1F);
    sen_num = (config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT+2-GT9_REG_CFG_BEG] & 0x0F) 
                        + ((config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT+2-GT9_REG_CFG_BEG]>>4) & 0x0F);

    if (drv_num < MIN_DRIVER_NUM || drv_num > MAX_DRIVER_NUM)
    {
        GTP_ERROR("driver number error!");
        return FAIL;
    }
    if (sen_num < MIN_SENSOR_NUM || sen_num > MAX_SENSOR_NUM)
    {
        GTP_ERROR("sensor number error!");
        return FAIL;
    }
    // get sensor and driver order 
    memset(cfg_sen_order, 0xFF, MAX_SENSOR_NUM);
    for (i = 0; i < sen_num; ++i)
    {
        cfg_sen_order[i] = config[GTP_ADDR_LENGTH + GT9_REG_SEN_ORD - GT9_REG_CFG_BEG + i];
    }

    memset(cfg_drv_order, 0xFF, MAX_DRIVER_NUM);
    for (i = 0; i < drv_num; ++i)
    {
        cfg_drv_order[i] = config[GTP_ADDR_LENGTH + GT9_REG_DRV_ORD - GT9_REG_CFG_BEG + i];
    }
    
    return SUCCESS;
}

/*
 * @param:
 *      phy_chnl: ic detected short channel, is_driver: it's driver or not
 * @Return:
 *      0xff: the ic channel is not used, otherwise: the tp short channel
 */
u8 gt9_get_short_tp_chnl(u8 phy_chnl, u8 is_driver)
{
    u8 i = 0;
    if (is_driver) {
        for (i = 0; i < MAX_DRIVER_NUM; ++i)
        {
            if (cfg_drv_order[i] == phy_chnl) {
                return i;
            }
            else if (cfg_drv_order[i] == 0xFF) {
                return 0xFF;
            }
        }
    }
    else 
    {
        for (i = 0; i < MAX_SENSOR_NUM; ++i)
        {
            if (cfg_sen_order[i] == phy_chnl) {
                return i;
            }
            else if (cfg_sen_order[i] == 0xFF) {
                return 0xFF;
            }
        }
    }
    return 0xFF;
}


static s32 gtp_i2c_end_cmd(struct i2c_client *client)
{
    u8  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    s32 ret = 0;
    
    ret = gtp_i2c_write(client, end_cmd, 3);
    if (ret < 0)
    {
        SET_INFO_LINE_INFO("I2C write end_cmd  error!"); 
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("I2C write end_cmd  error!"); 
        // porting for touch driver end
    }
    return ret;
}

static void gtp_hopping_switch(struct i2c_client *client, s32 on)
{
    s32 j = 0;
    static u8 hopping_enabled = 0;
    u8 chksum = 0;
    u8 config[256] = {(u8)(GTP_REG_CONFIG_DATA >> 8), (u8)GTP_REG_CONFIG_DATA, 0};

    if (gtp_i2c_read(client, config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH) < 0)
    {
        SET_INFO_LINE_ERR("Failed to read config!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Failed to read config!");
        // porting for touch driver end
        return;
    }
    if (!on)
    {
        // disable hopping
        if (config[GTP_ADDR_LENGTH + 0x807D - GTP_REG_CONFIG_DATA] & 0x80)
        {
            GTP_INFO("Disable hopping.");
            config[GTP_ADDR_LENGTH + 0x807D - GTP_REG_CONFIG_DATA] &= 0x7F;
            // calculate checksum
            for (j = 0; j < (cfg_len-2); ++j)
            {
                chksum += config[GTP_ADDR_LENGTH + j];
            }
            config[cfg_len] = (~chksum) + 1;
            config[cfg_len+1] = 0x01;
            
            gtp_i2c_write(client, config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);
            hopping_enabled = 1;
        }
    }
    else if (hopping_enabled)
    {
        if (0x00 == (config[GTP_ADDR_LENGTH + 0x807D - GTP_REG_CONFIG_DATA] & 0x80))
        {
            GTP_INFO("Enable hopping.");
            config[GTP_ADDR_LENGTH + 0x807D - GTP_REG_CONFIG_DATA] |= 0x80; 
            // calculate checksum
            for (j = 0; j < (cfg_len-2); ++j)
            {
                chksum += config[GTP_ADDR_LENGTH + j];
            }
            config[cfg_len] = (~chksum) + 1;
            config[cfg_len+1] = 0x01;
            
            gtp_i2c_write(client, config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);
            hopping_enabled = 0;
        }
    }
}

s32 gtp_parse_config(struct i2c_client *client)
{
    u8 i = 0;
    u8 key_pos = 0;
    u8 key_val = 0;
    u8 config[256] = {(u8)(GTP_REG_CONFIG_DATA >> 8), (u8)GTP_REG_CONFIG_DATA, 0};
    u8 type_buf[12] = {0x80, 0x00};
    
    if (gtp_i2c_read(client, config, GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH) <= 0)
    {
        SET_INFO_LINE_ERR("Failed to read config!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Failed to read config!");
        // porting for touch driver end
        return FAIL;
    }
    
    gt9xx_drv_num = (config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT-GT9_REG_CFG_BEG] & 0x1F)
                    + (config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT+1 -GT9_REG_CFG_BEG] & 0x1F);
    gt9xx_sen_num = (config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT+2-GT9_REG_CFG_BEG] & 0x0F) 
                    + ((config[GTP_ADDR_LENGTH + GT9_REG_SEN_DRV_CNT+2-GT9_REG_CFG_BEG]>>4) & 0x0F);

    GTP_INFO("Driver num: %d, Sensor Num: %d", gt9xx_drv_num, gt9xx_sen_num);
    if (gt9xx_drv_num < MIN_DRIVER_NUM || gt9xx_drv_num > MAX_DRIVER_NUM)
    {
        SET_INFO_LINE_ERR("driver number error!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("driver number error!");
        // porting for touch driver end
        return FAIL;
    }
    if (gt9xx_sen_num < MIN_SENSOR_NUM || gt9xx_sen_num > MAX_SENSOR_NUM)
    {
        SET_INFO_LINE_ERR("sensor number error!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("sensor number error!");
        // porting for touch driver end
        return FAIL;
    }
    gt9xx_sc_pxl_cnt = gt9xx_pixel_cnt = gt9xx_drv_num * gt9xx_sen_num;
    
    gtp_i2c_read(client, type_buf, 12);
    if (!memcmp(&type_buf[2], "GOODIX_GT9", 10))
    {
        chip_type_gt9f = 0;
        GTP_INFO("Chip type: GT9XX");
    }
    else
    {
        chip_type_gt9f = 1;
        GTP_INFO("Chip type: GT9XXF");
    }
    
    have_key = config[0x804E - GT9_REG_CFG_BEG + GTP_ADDR_LENGTH] & 0x01;
    
    if (!have_key)
    {
        GTP_INFO("No key");
        return SUCCESS;
    }
    GTP_INFO("Have Key");
    gt9xx_sc_drv_num = gt9xx_drv_num - 1;
    
    for (i = 0; i < 5; ++i)
    {
        key_iso_pos[i] = 0;
    }
    
    key_is_isolated = 0;
    for (i = 0; i < 4; ++i)
    {
        // all keys are multiples of 0x08 -> isolated keys
        key_val = config[GTP_ADDR_LENGTH + GT9_REG_KEY_VAL - GT9_REG_CFG_BEG + i];
        key_pos = key_val%0x08;
        GTP_DEBUG("key_val[%d] = 0x%02x", i+1, key_val);
        if ((key_pos != 0))
        {
            key_is_isolated = 0;
            GTP_DEBUG("Key is not isolated!");
            break;
        }
        else if (key_val == 0x00)       // no more key
        {
            continue;
        }
        else
        {
            key_iso_pos[0]++;       // isolated key count
            key_iso_pos[i+1] = key_val/0x08 - 1;
            key_is_isolated = 1;
        }
    }
    
    gt9xx_sc_pxl_cnt = gt9xx_pixel_cnt - (gt9xx_drv_num-gt9xx_sc_drv_num) * gt9xx_sen_num;
    GTP_DEBUG("drv num: %d, sen num: %d, sc drv num: %d", gt9xx_drv_num, gt9xx_sen_num, gt9xx_sc_drv_num);
    if (key_is_isolated)
    {
        GTP_DEBUG("Isolated [%d key(s)]: %d, %d, %d, %d", key_iso_pos[0], key_iso_pos[1], key_iso_pos[2], key_iso_pos[3], key_iso_pos[4]);
    }
    
    return SUCCESS;
}



/*
 * Function:
 *      write one byte to specified register
 * Input:
 *      reg: the register address
 *      val: the value to write into
 * Return:
 *      i2c_write function return 
 */
s32 gtp_write_register(struct i2c_client * client, u16 reg, u8 val)
{
    s32 ret = 0;
    u8 buf[3];
    buf[0] = (u8) (reg >> 8);
    buf[1] = (u8) reg;
    buf[2] = val;
    //return gtp_i2c_write(client, buf, 3);
    ret = i2c_write_bytes(client, reg, &buf[2], 1);
    
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return 1;
    }
}
/*
 * Function: 
 *      read one byte from specified register into buf
 * Input:
 *      reg: the register
 *      buf: the buffer for one byte
 * Return:
 *      i2c_read function return
 */
s32 gtp_read_register(struct i2c_client * client, u16 reg, u8* buf)
{
    s32 ret = 0;
    buf[0] = (u8)(reg >> 8);
    buf[1] = (u8)reg;
    //return gtp_i2c_read(client, buf, 3);
    
    ret = i2c_read_bytes(client, reg, &buf[2], 1);
    
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return 2;
    }
}

/* 
 * Function:
 *      burn dsp_short code
 * Input:
 *      i2c_client
 * Return:
 *      SUCCESS: burning succeed, FAIL: burning failed
 */
s32 gtp_burn_dsp_short(struct i2c_client *client)
{
    s32 ret = 0;
    u8 *opr_buf;
    u16 i = 0;
    u16 addr = GTP_REG_DSP_SHORT;
    u16 opr_len = 0;
    u16 left = 0;
    u16 retry = 0;
    u8 read_buf[3] = {0x00};

    GTP_DEBUG("Start writing dsp_short code");
    opr_buf = (u8*)kmalloc(sizeof(u8) * (DSP_SHORT_BURN_CHK+2), GFP_KERNEL);
    if (!opr_buf)
    {
        SET_INFO_LINE_ERR("failed to allocate memory for check buffer!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("failed to allocate memory for check buffer!");
        // porting for touch driver end
        return FAIL;
    }
    
    left = sizeof(dsp_short);
    while (left > 0)
    {
        opr_buf[0] = (u8)(addr >> 8);
        opr_buf[1] = (u8)(addr);
        if (left > DSP_SHORT_BURN_CHK)
        {
            opr_len = DSP_SHORT_BURN_CHK;
        }
        else
        {
            opr_len = left;
        }
        memcpy(&opr_buf[2], &dsp_short[addr-GTP_REG_DSP_SHORT], opr_len);
       
        ret = gtp_i2c_write(client, opr_buf, 2 + opr_len);
        if ( ret < 0 )
        {
            GTP_ERROR("write dsp_short code failed!");
            kfree(opr_buf);
            return FAIL;
        }
        addr += opr_len;
        left -= opr_len;
    }
    
    // check code: 0xC000~0xCFFF
    GTP_DEBUG("Start checking dsp_short code");
    addr = GTP_REG_DSP_SHORT;
    left = sizeof(dsp_short);
    while (left > 0)
    {
        memset(opr_buf, 0, opr_len + 2);
        opr_buf[0] = (u8)(addr >> 8);
        opr_buf[1] = (u8)(addr);
        
        
        if (left > DSP_SHORT_BURN_CHK)
        {
            opr_len = DSP_SHORT_BURN_CHK;
        }
        else
        {
            opr_len = left;
        }
        
        ret = gtp_i2c_read(client, opr_buf, opr_len+2);
        if (ret < 0)
        {
            kfree(opr_buf);
            return FAIL;
        }
        for (i = 0; i < opr_len; ++i)
        {
            if (opr_buf[i+2] != dsp_short[addr-GTP_REG_DSP_SHORT+i])
            {
                GTP_ERROR("check dsp_short code failed!");
                
                gtp_write_register(client, addr + i, dsp_short[addr-GTP_REG_DSP_SHORT+i]);
                
                GTP_DEBUG("(%d)Location: %d, 0x%02X, 0x%02X", retry+1, addr - GTP_REG_DSP_SHORT + i, opr_buf[i+2], dsp_short[addr-GTP_REG_DSP_SHORT+i]);
                
                msleep(1);
                gtp_read_register(client, addr + i, read_buf);
                opr_buf[i+2] = read_buf[2];
                i--;
                retry++;
                if (retry >= 200)
                {
                    GTP_DEBUG("Burn dsp retry timeout!");
                    kfree(opr_buf);
                    return FAIL;
                }
            }
        }
        
        addr += opr_len;
        left -= opr_len;
    }
    kfree(opr_buf);
    return SUCCESS;
}
/*
 * Function: 
 *      check the resistor between shortlike channels if less than threshold confirm as short
 * INPUT:
 *      Short like Information struct pointer
 * Returns:
 *      SUCCESS: it's shorted FAIL: otherwise
 */
s32 gtp_short_resist_check(struct gt9xx_short_info *short_node)
{
    s32 short_resist = 0;
    struct gt9xx_short_info *node = short_node;
    u8 master = node->master;
    u8 slave = node->slave;
    u8 chnnl_tx[4] = { GT9_DRV_HEAD|13, GT9_DRV_HEAD|28,
                    GT9_DRV_HEAD|29, GT9_DRV_HEAD|42 };
    s32 numberator = 0;
    u32 amplifier = 1000;  // amplify 1000 times to emulate float computing
    
    
    // Tx-ABIST & Tx_ABIST
    if ((((master > chnnl_tx[0]) && (master <= chnnl_tx[1])) &&
        ((slave > chnnl_tx[0]) && (slave <= chnnl_tx[1])) ) ||
        (((master >= chnnl_tx[2]) && (master <= chnnl_tx[3])) &&
        ((slave >= chnnl_tx[2]) && (slave <= chnnl_tx[3]))))
    {
        numberator = node->self_data * 40 * amplifier;
        short_resist = numberator/(node->short_code) - 40 * amplifier;
    }
    // Receiver is Rx-odd(1,3,5)
    else if ((node->slave & (GT9_DRV_HEAD | 0x01)) == 0x01)
    {
        numberator = node->self_data * 60 * amplifier;
        short_resist = numberator/node->short_code - 40 * amplifier; 
    }
    else
    {
        numberator = node->self_data * 60 * amplifier;
        short_resist = numberator / node->short_code - 60 * amplifier;
    }
    GTP_DEBUG("self_data = %d" ,node->self_data);
    GTP_DEBUG("master = 0x%02X, slave = 0x%02X", node->master, node->slave);
    GTP_DEBUG("short_code = %d, short_resist = %d", node->short_code, short_resist);
    
    if (short_resist < 0)
    {
        short_resist = 0;
    }
    
    if (short_resist < (gt900_resistor_threshold * amplifier))
    {
        node->impedance = short_resist / amplifier;
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}



/*
 * Function: 
 *      compute the result, whether there are shorts or not
 * Input:
 *      i2c_client
 * Return:
 *      SUCCESS
 */
s32 gtp_compute_rslt(struct i2c_client *client)
{
    u16 short_code;
    u8 i = 0, j = 0;
    u16 result_addr;
    u8 *result_buf;
    u16 *self_data;
    s32 ret = 0;
    u16 data_len = 3 + (MAX_DRIVER_NUM + MAX_SENSOR_NUM) * 2 + 2; // a short data frame length
    struct gt9xx_short_info short_node;
    u16 node_idx = 0; // short_sum index: 0~_SHORT_INFO_MAX
    
    u8 tx_short_num = 0;
    u8 rx_short_num = 0;
    
    u8 master, slave;
    
    self_data = (u16*)kmalloc(sizeof(u16) * ((MAX_DRIVER_NUM + MAX_SENSOR_NUM)), GFP_KERNEL);
    result_buf = (u8*)kmalloc(sizeof(u8) * (data_len+2), GFP_KERNEL);
    short_sum = (struct gt9xx_short_info *) kmalloc(sizeof(struct gt9xx_short_info) * _SHORT_INFO_MAX, GFP_KERNEL);

    if (!self_data || !result_buf || !short_sum)
    {
        SET_INFO_LINE_ERR("allocate memory for short result failed!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("allocate memory for short result failed!");
        // porting for touch driver end
        if (self_data)
        {
            kfree(self_data);
        }
        if (result_buf)
        {
            kfree(self_data);
        }
        if (short_sum)
        {
            kfree(short_sum);
        }
        return FAIL;
    }   
    
    // Get Selfdata
    result_buf[0] = 0xA4;
    result_buf[1] = 0xA1;
    gtp_i2c_read(client, result_buf, 2 + 144);
    for (i = 0, j = 0; i < 144; i += 2)
    {
        self_data[j++] = (u16)(result_buf[2+i] << 8) + (u16)(result_buf[2+i+1]);
    }
    GTP_DEBUG("Self Data:");
    GTP_DEBUG_ARRAY(result_buf+2, 144);
    
    
    // Get TxShortNum & RxShortNum
    result_buf[0] = 0x88;
    result_buf[1] = 0x02;
    gtp_i2c_read(client, result_buf, 2 + 2);
    tx_short_num = result_buf[2];
    rx_short_num = result_buf[3];
    
    GTP_DEBUG("Tx Short Num: %d, Rx Short Num: %d", tx_short_num, rx_short_num);
    
    // 
    result_addr = 0x8860;
    data_len = 3 + (MAX_DRIVER_NUM + MAX_SENSOR_NUM) * 2 + 2;
    for (i = 0; i < tx_short_num; ++i)
    {        
        result_buf[0] = (u8) (result_addr >> 8);
        result_buf[1] = (u8) (result_addr);
        ret = gtp_i2c_read(client, result_buf, data_len+2);
        if (ret < 0)
        {
            SET_INFO_LINE_ERR("read result data failed!");
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("read result data failed!");
            // porting for touch driver end
        }
        GTP_DEBUG("Result Buffer: ");
        GTP_DEBUG_ARRAY(result_buf+2, data_len);
        
        short_node.master_is_driver = 1;
        short_node.master = result_buf[2];
        
        // Tx - Tx
        for (j = i + 1; j < MAX_DRIVER_NUM; ++j)
        {
            short_code = (result_buf[2+3+j*2] << 8) + result_buf[2+3+j*2+1];
            if (short_code > gt900_short_threshold)
            {
                short_node.slave_is_driver = 1;
                short_node.slave = ChannelPackage_TX[j] | GT9_DRV_HEAD;
                short_node.self_data = self_data[j];
                short_node.short_code = short_code;
                
                ret = gtp_short_resist_check(&short_node);  
                if (ret == SUCCESS)
                {
                    if (node_idx < _SHORT_INFO_MAX)
                    {
                        short_sum[node_idx++] = short_node;
                    }
                }
            }
        }
        // Tx - Rx
        for (j = 0; j < MAX_SENSOR_NUM; ++j)
        {
            short_code = (result_buf[2+3+84+j*2] << 8) + result_buf[2+3+84+j*2+1];
            
            if (short_code > gt900_short_threshold)
            {
                short_node.slave_is_driver = 0;
                short_node.slave = j | GT9_SEN_HEAD;
                short_node.self_data = self_data[MAX_DRIVER_NUM + j];
                short_node.short_code = short_code;
                
                ret = gtp_short_resist_check(&short_node);
                if (ret == SUCCESS)
                {
                    if (node_idx < _SHORT_INFO_MAX)
                    {
                        short_sum[node_idx++] = short_node;
                    }
                }
            }
        }
        
        result_addr += data_len;
    }
    
    result_addr = 0xA0D2;
    data_len = 3 + MAX_SENSOR_NUM * 2 + 2;
    for (i = 0; i < rx_short_num; ++i)
    {
        result_buf[0] = (u8) (result_addr >> 8);
        result_buf[1] = (u8) (result_addr);
        ret = gtp_i2c_read(client, result_buf, data_len + 2);
        if (ret < 0)
        {
            SET_INFO_LINE_ERR("read result data failed!");
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("read result data failed!");
            // porting for touch driver end
        }
        
        GTP_DEBUG("Result Buffer: ");
        GTP_DEBUG_ARRAY(result_buf+2, data_len);
        
        short_node.master_is_driver = 0;
        short_node.master = result_buf[2];
        
        // Rx - Rx
        for (j = 0; j < MAX_SENSOR_NUM; ++j)
        {
            if ((j == i) || ( (j < i) && (j & 0x01) == 0))
            {
                continue;
            }
            short_code = (result_buf[2+3+j*2] << 8) + result_buf[2+3+j*2+1];
            
            if (short_code > gt900_short_threshold)
            {
                short_node.slave_is_driver = 0;
                short_node.slave = j | GT9_SEN_HEAD;
                short_node.self_data = self_data[MAX_DRIVER_NUM + j];
                short_node.short_code = short_code;
                
                ret = gtp_short_resist_check(&short_node);
                if (ret == SUCCESS)
                {
                    if (node_idx < _SHORT_INFO_MAX)
                    {
                        short_sum[node_idx++] = short_node;
                    }
                }
            }
        }
        
        result_addr += data_len;
    }
    
    if (node_idx == 0)
    {
        ret = SUCCESS;
    }
    else
    {
        for (i = 0, j = 0; i < node_idx; ++i)
        {
            GTP_DEBUG("Orignal Shorted Channels: %s%d, %s%d",
                (short_sum[i].master_is_driver) ? "Drv" : "Sen", short_sum[i].master & (~GT9_DRV_HEAD),
                (short_sum[i].slave_is_driver) ? "Drv" : "Sen", short_sum[i].slave & (~GT9_DRV_HEAD));
                    
            if ((short_sum[i].master_is_driver))
            {
                master = gt9_get_short_tp_chnl(short_sum[i].master-GT9_DRV_HEAD, 1);
            }
            else
            {
                master = gt9_get_short_tp_chnl(short_sum[i].master, 0);
            }
            
            if ((short_sum[i].slave_is_driver))
            {
                slave = gt9_get_short_tp_chnl(short_sum[i].slave-GT9_DRV_HEAD, 1);
            }
            else
            {
                slave = gt9_get_short_tp_chnl(short_sum[i].slave, 0);
            }
            
            if (master == 0xFF && slave == 0xFF)
            {
                GTP_DEBUG("unbonded channel (%d, %d) shorted!", short_sum[i].master, short_sum[i].slave);
                continue;
            }
            else
            {
                short_sum[j].slave = slave;
                short_sum[j].master = master;
                short_sum[j].slave_is_driver = short_sum[i].slave_is_driver;
                short_sum[j].master_is_driver = short_sum[i].master_is_driver;
                short_sum[j].impedance = short_sum[i].impedance;
                short_sum[j].self_data = short_sum[i].self_data;
                short_sum[j].short_code = short_sum[i].short_code;
                ++j;
            }
        }
        node_idx = j;
        if (node_idx == 0)
        {
            ret = SUCCESS;
        }
        else
        {
            for (i = 0; i < node_idx; ++i)
            {
                SET_INFO_LINE_INFO("  %s%02d & %s%02d Shorted! (R = %dKOhm)",
                (short_sum[i].master_is_driver) ? "Drv" : "Sen", short_sum[i].master,
                (short_sum[i].slave_is_driver) ? "Drv" : "Sen", short_sum[i].slave,
                short_sum[i].impedance);
                // porting for touch driver start
                //add SET_INFO_LINE_FTM
                SET_INFO_LINE_FTM("  %s%02d & %s%02d Shorted! (R = %dKOhm)",
                (short_sum[i].master_is_driver) ? "Drv" : "Sen", short_sum[i].master,
                (short_sum[i].slave_is_driver) ? "Drv" : "Sen", short_sum[i].slave,
                short_sum[i].impedance);
                // porting for touch driver end
            }
            ret = FAIL;
        }
    }
    kfree(self_data);
    kfree(short_sum);
    kfree(result_buf);
    return ret;
}

s32 gt9_test_gnd_vdd_short(struct i2c_client *client)
{
    u8 *data;
    s32 ret = 0;
    s32 i = 0;
    u16 len = (MAX_DRIVER_NUM + MAX_SENSOR_NUM) * 2;
    u16 short_code = 0;
    s32 r = -1;
    u32 short_res = 0;
    u8 short_chnl = 0;
    u16 amplifier = 1000;
    
    data = (u8 *)kmalloc(sizeof(u8) * (len + 2), GFP_KERNEL);
    if (NULL == data)
    {
       SET_INFO_LINE_ERR("failed to allocate memory for gnd vdd test data buffer");
       // porting for touch driver start
       //add SET_INFO_LINE_FTM
       SET_INFO_LINE_FTM("failed to allocate memory for gnd vdd test data buffer");
       // porting for touch driver end
       return FAIL;
    }
    
    data[0] = 0xA5;
    data[1] = 0x31;
    gtp_i2c_read(client, data, 2 + len);
    
    GTP_DEBUG_ARRAY(data+2, len);
    ret = SUCCESS;
    for (i = 0; i < len; i += 2)
    {
        short_code = (data[2+i] << 8) + (data[2 + i + 1]);
        if (short_code == 0)
        {
            continue;
        }
        if ((short_code & 0x8000) == 0)        // short with GND
        {
        #ifdef GTP_SHORT_GND
            r = 5266285 * 10 / (short_code & (~0x8000)) - 40 * amplifier;
        #endif
        }
        else    // short with VDD
        {
            //r = ( 1/(((float)(short_code&(~0x8000)))/0.9*0.7/1024/(sys.avdd-0.9)/40) ) -40;
        #ifdef GTP_VDD
            r = 40*9*1024*(100*GTP_VDD - 900)/((short_code&(~0x8000))*7) - 40*1000;
            GTP_DEBUG("vdd short_code: %d", short_code & (~0x8000));
        #endif
        }
        GTP_DEBUG("resistor: %d, short_code: %d", r, short_code);
        
        short_res = (r >= 0) ? r : 0xFFFF;
        if (short_res == 0xFFFF)
        {
        }
        else 
        {
            if (short_res < (gt900_gnd_resistor_threshold * amplifier))
            {
                if (i < MAX_DRIVER_NUM * 2)       // driver 
                {
                    short_chnl = gt9_get_short_tp_chnl(ChannelPackage_TX[i/2], 1);
                    GTP_INFO("driver%02d & gnd/vdd shorted!", short_chnl);
                    if (short_chnl == 0xFF)
                    {
                        GTP_INFO("unbonded channel");
                    }
                    else
                    {
                        SET_INFO_LINE_INFO("  Drv%02d & GND/VDD Shorted! (R = %dKOhm)", short_chnl, short_res/amplifier);
                        // porting for touch driver start
                        //add SET_INFO_LINE_FTM
                        SET_INFO_LINE_FTM("  Drv%02d & GND/VDD Shorted! (R = %dKOhm)", short_chnl, short_res/amplifier);
                        // porting for touch driver end
                    }
                } 
                else
                {
                    short_chnl = gt9_get_short_tp_chnl((i/2) - MAX_DRIVER_NUM, 0);
                    GTP_INFO("sensor%02d & gnd/vdd shorted!", short_chnl);
                    if (short_chnl == 0xFF)
                    {
                        GTP_INFO("unbonded channel");
                    }
                    else
                    {
                        SET_INFO_LINE_INFO("  Sen%02d & GND/VDD Shorted! (R = %dKOhm)", short_chnl, short_res/amplifier);
                        // porting for touch driver start
                        //add SET_INFO_LINE_FTM
                        SET_INFO_LINE_FTM("  Sen%02d & GND/VDD Shorted! (R = %dKOhm)", short_chnl, short_res/amplifier);
                        // porting for touch driver end
                    }
                }
                ret = FAIL;
            }
        }
    }
    return ret;
}


/*
 * leave short test 
 */
void gt9xx_leave_short_test(struct i2c_client *client)
{
    
    gtp_reset_guitar(client, 20);
    msleep(100);

    gtp_send_cfg(client);
    SET_INFO_LINE_INFO("");
    SET_INFO_LINE_INFO("---gtp short test end---");
}


/*
 * Function:
 *      gt9 series ic short test function
 * Input:
 *      I2c_client, i2c device
 * Return:
 *      SUCCESS: test succeed, FAIL: test failed
 */
s32 gt9xx_short_test(struct i2c_client * client)
{
    s32 ret = 0;
    s32 ret2 = 0;
    u8 i = 0;
    u8 opr_buf[60] = {0};
    u8 retry = 0;
    u8 drv_sen_chksum = 0;
    u8 retry_load = 0;
    u8 old_i2c_addr = client->addr;
    
    gtp_irq_disable();
    //disable_irq(client->irq);
#if GTP_ESD_PROTECT
    tpd_halt = 1;     // suspend esd
#endif
    // step 1: reset guitar, delay 1ms,  hang up ss51 and dsp
    SET_INFO_LINE_INFO("---gtp short test---");
    SET_INFO_LINE_INFO("Step 1: reset guitar, hang up ss51 dsp");
    
    ret = gt9xx_short_parse_cfg();
    if (FAIL == ret)
    {
        SET_INFO_LINE_ERR("You May check your IIC connection.");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("You May check your IIC connection.");
        // porting for touch driver end
        goto short_test_exit;
    }
    
load_dsp_again:
    // RST output low last at least 2ms
    
    
    GTP_GPIO_OUTPUT(GTP_RST_PORT, 0);
    msleep(30);
    
    // select I2C slave addr,INT:0--0xBA;1--0x28.
    GTP_GPIO_OUTPUT(GTP_INT_PORT, client->addr == 0x14);
    msleep(2);
    
    // RST output high reset guitar
    GTP_GPIO_OUTPUT(GTP_RST_PORT, 1);
    msleep(6);
    
    msleep((retry_load)*10);
    
    for (i = 0; i < 200; ++i)
    {
        // Hold ss51 & dsp
        ret = gtp_write_register(client, _rRW_MISCTL__SWRST_B0_, 0x0C);
        // DSP_CK and DSP_ALU_CK PowerOn
        ret2 = gtp_write_register(client, 0x4010, 0x00);
        
        if ((ret > 0) && (ret2 > 0))
        {
            break;
        }
        msleep(2);
    }
    if (i >= 200)
    {
        SET_INFO_LINE_ERR("Failed to init short test mode");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Failed to init short test mode");
        // porting for touch driver end
        goto short_test_exit;
    }
    msleep(10);
    
    while(retry++ < 200)
    {        
        // Confirm hold
        opr_buf[GTP_ADDR_LENGTH] = 0x00;
        ret = gtp_read_register(client, _rRW_MISCTL__SWRST_B0_, opr_buf);
        if(ret <= 0)
        {
            GTP_DEBUG("Hold ss51 & dsp I2C error,retry:%d", retry);
            gtp_reset_guitar(client, 10);
            continue;
        }
        if(0x0C == opr_buf[GTP_ADDR_LENGTH])
        {
            GTP_DEBUG("Hold ss51 & dsp confirm SUCCESS");
            break;
        }        
        ret = gtp_write_register(client, _rRW_MISCTL__SWRST_B0_, 0x0C);
        if( (ret <= 0))
        {
            GTP_DEBUG("Hold ss51 & dsp I2C error,retry:%d", retry);
            gtp_reset_guitar(client, 10);
            continue;
        }
        GTP_DEBUG("Hold ss51 & dsp confirm 0x4180 failed,value:%d", opr_buf[GTP_ADDR_LENGTH]);
        msleep(2);
    }
    if(retry >= 200)
    {
        SET_INFO_LINE_ERR("Enter update Hold ss51 failed.");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Enter update Hold ss51 failed.");
        // porting for touch driver end
        goto short_test_exit;
    }
    
    // step2: burn dsp_short code
    GTP_INFO("step 2: burn dsp_short code");
    gtp_write_register(client, _bRW_MISCTL__TMR0_EN, 0x00); // clear watchdog
    gtp_write_register(client, _bRW_MISCTL__CACHE_EN, 0x00); // clear cache
    gtp_write_register(client, _rRW_MISCTL__BOOTCTL_B0_, 0x02); // boot from sram
    //gtp_write_register(client, _bWO_MISCTL__CPU_SWRST_PULSE, 0x01); // reset software
    
    gtp_write_register(client, _bRW_MISCTL__SRAM_BANK, 0x00); // select bank 0
    gtp_write_register(client, _bRW_MISCTL__MEM_CD_EN, 0x01); // allow AHB bus accessing code sram
   
    // ---: burn dsp_short code
    ret = gtp_burn_dsp_short(client);
    
    if (ret != SUCCESS)
    {
        if(retry_load++ < 5)
        {
            GTP_ERROR("Load dsp failed,times %d retry load!", retry_load);
            goto load_dsp_again;
        }
        else
        {
            SET_INFO_LINE_INFO("Step 2: burn dsp_short code");
            SET_INFO_LINE_ERR("burn dsp_short Timeout!");
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("burn dsp_short Timeout!");
            // porting for touch driver end
            goto short_test_exit;
        }
    } 
    
    SET_INFO_LINE_INFO("Step 2: burn dsp_short code");
    
    // step3: run dsp_short, read results
    SET_INFO_LINE_INFO("Step 3: run dsp_short code, confirm it's runnin'");
    gtp_write_register(client, _rRW_MISCTL__SHORT_BOOT_FLAG, 0x00); // clear dsp_short running flag
    gtp_write_register(client, _rRW_MISCTL__BOOT_OPT_B0_, 0x03);//set scramble
    
    gtp_write_register(client, _bWO_MISCTL__CPU_SWRST_PULSE, 0x01); // reset software
    
    gtp_write_register(client, _rRW_MISCTL__SWRST_B0_, 0x08);   // release dsp
    
    client->addr = 0x14;    // for constant iic address
    msleep(50);
    // confirm dsp is running
    i = 0;
    while (1)
    {
        opr_buf[2] = 0x00;
        gtp_read_register(client, _rRW_MISCTL__SHORT_BOOT_FLAG, opr_buf);
        if (opr_buf[2] == 0xAA)
        {
            break;
        }
        ++i;
        if (i >= 20)
        {
            SET_INFO_LINE_ERR("step 3: dsp is not running!");
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("step 3: dsp is not running!");
            // porting for touch driver end
            goto short_test_exit;
        }
        msleep(10);
    }
    // step4: host configure ic, get test result
    SET_INFO_LINE_INFO("Step 4: host config ic, get test result");
    // Short Threshold
    GTP_DEBUG(" Short Threshold: 10");
    opr_buf[0] = 0x88;
    opr_buf[1] = 0x04;  
    opr_buf[2] = 0;
    opr_buf[3] = 10;
    gtp_i2c_write(client, opr_buf, 4);
    
    // ADC Read Delay
    GTP_DEBUG(" ADC Read Delay: 150");
    opr_buf[0] = 0x88;
    opr_buf[1] = 0x06;
    opr_buf[2] = (u8)(150 >> 8);
    opr_buf[3] = (u8)(150);
    gtp_i2c_write(client, opr_buf, 4);
    
    // DiffCode Short Threshold
    GTP_DEBUG(" DiffCode Short Threshold: 20");
    opr_buf[0] = 0x88;
    opr_buf[1] = 0x51;
    opr_buf[2] = (u8)(20 >> 8);
    opr_buf[3] = (u8)(20);
    gtp_i2c_write(client, opr_buf, 4);
    
    // Config Driver & Sensor Order
#if GTP_DEBUG_ON
    printk("<<-GTP-DEBUG->> Driver Map:\n");
    printk("IC Driver:");
    for (i = 0; i < MAX_DRIVER_NUM; ++i)
    {
        printk(" %2d", cfg_drv_order[i]);
    }
    printk("\n");
    printk("TP Driver:");
    for (i = 0; i < MAX_DRIVER_NUM; ++i)
    {
        printk(" %2d", i);
    }
    printk("\n");
    
    printk("<<-GTP-DEBUG->> Sensor Map:\n");
    printk("IC Sensor:");
    for (i = 0; i < MAX_SENSOR_NUM; ++i)
    {
        printk(" %2d", cfg_sen_order[i]);
    }
    printk("\n");
    printk("TP Sensor:");
    for (i = 0; i < MAX_SENSOR_NUM; ++i)
    {
        printk(" %2d", i);
    }
    printk("\n");
#endif

    opr_buf[0] = 0x88;
    opr_buf[1] = 0x08;
    for (i = 0; i < MAX_DRIVER_NUM; ++i)
    {
        opr_buf[2 + i] = cfg_drv_order[i];
        drv_sen_chksum += cfg_drv_order[i];
    }
    gtp_i2c_write(client, opr_buf, MAX_DRIVER_NUM + 2);
    
    opr_buf[0] = 0x88;
    opr_buf[1] = 0x32;
    for (i = 0; i < MAX_SENSOR_NUM; ++i)
    {
        opr_buf[2+i] = cfg_sen_order[i];
        drv_sen_chksum += cfg_sen_order[i];
    }
    gtp_i2c_write(client, opr_buf, MAX_SENSOR_NUM + 2);
    
    opr_buf[0] = 0x88;
    opr_buf[1] = 0x50;
    opr_buf[2] = 0 - drv_sen_chksum;
    gtp_i2c_write(client, opr_buf, 2 + 1);
    
    // clear waiting flag, run dsp
    gtp_write_register(client, _rRW_MISCTL__SHORT_BOOT_FLAG, 0x04);
    
    // inquirying test status until it's okay
    for (i = 0;;++i)
    {
        gtp_read_register(client, 0x8800, opr_buf);
        if (opr_buf[2] == 0x88)
        {
            break;
        }
        msleep(50);
        if ( i > 100 )
        {
            SET_INFO_LINE_ERR("step 4: inquiry test status timeout!");
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("step 4: inquiry test status timeout!");
            // porting for touch driver end
            goto short_test_exit;
        }
    }
    
    // step 5: compute the result
    /* short flag: 
          bit0: Rx & Rx 
          bit1: Tx & Tx 
          bit2: Tx & Rx
          bit3: Tx/Rx & GND/VDD
    */
    gtp_read_register(client, 0x8801, opr_buf);
    GTP_DEBUG("short_flag = 0x%02X", opr_buf[2]);
    SET_INFO_LINE_INFO("");
    SET_INFO_LINE_INFO("Short Test Result:");
    // porting for touch driver start
    //add SET_INFO_LINE_FTM
    SET_INFO_LINE_FTM("Short Test Result:");
    // porting for touch driver end
    if ((opr_buf[2] & 0x0f) == 0)
    {
        SET_INFO_LINE_INFO("  PASS!");
        ret = SUCCESS;
    }
    else 
    {
        ret2 = SUCCESS;
        if ((opr_buf[2] & 0x08) == 0x08)
        {
            ret2 = gt9_test_gnd_vdd_short(client);
        }
        ret = gtp_compute_rslt(client);
        if (ret == SUCCESS && ret2 == SUCCESS)
        {
            SET_INFO_LINE_INFO("  PASS!");
            ret = SUCCESS;
        }
        else
        {
            ret = FAIL;
        }
    }
    // boot from rom and download code from flash to ram
    gtp_write_register(client, _rRW_MISCTL__BOOT_CTL_, 0x99);
    gtp_write_register(client, _rRW_MISCTL__BOOTCTL_B0_, 0x08);
    
    client->addr = old_i2c_addr;
    gt9xx_leave_short_test(client);
    gtp_irq_enable();
    //enable_irq(client->irq);

#if GTP_ESD_PROTECT
    tpd_halt = 0;     // resume esd
#endif
    return ret;
    
short_test_exit:
    // boot from rom and download code from flash to ram
    gtp_write_register(client, _rRW_MISCTL__BOOT_CTL_, 0x99);
    gtp_write_register(client, _rRW_MISCTL__BOOTCTL_B0_, 0x08);
    
    client->addr = old_i2c_addr;
    gt9xx_leave_short_test(client);
    gtp_irq_enable();
    //enable_irq(client->irq);
#if GTP_ESD_PROTECT
    tpd_halt = 0;     // resume esd
#endif
    return FAIL;    
}

u32 endian_mode(void)
{
    union {s32 i; s8 c;}endian;

    endian.i = 1;

    if (1 == endian.c)
    {
        return MYBIG_ENDIAN;
    }
    else
    {
        return MYLITLE_ENDIAN;
    }
}
/*
*********************************************************************************************************
* Function: 
*   send read rawdata cmd
* Input:
*   i2c_client* client: i2c device
* Return:
*   SUCCESS: send process succeed, FAIL: failed
*********************************************************************************************************
*/
s32 gt9_read_raw_cmd(struct i2c_client* client)
{
    u8 raw_cmd[3] = {(u8)(GTP_REG_READ_RAW >> 8), (u8)GTP_REG_READ_RAW, 0x01};
    s32 ret = -1;
    GTP_DEBUG("Send read raw data command");
    ret = gtp_i2c_write(client, raw_cmd, 3);
    if(ret <= 0)
    {
        SET_INFO_LINE_ERR("i2c write failed.");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("i2c write failed.");
        // porting for touch driver end
        return FAIL;
    }
    msleep(10); 
    return SUCCESS;
}

s32 gt9_read_coor_cmd(struct i2c_client *client)
{
    u8 raw_cmd[3] = {(u8)(GTP_REG_READ_RAW >> 8), (u8)GTP_REG_READ_RAW, 0x0};
    s32 ret = -1;
    
    ret = gtp_i2c_write(client, raw_cmd, 3);
    if (ret < 0)
    {
        SET_INFO_LINE_ERR("i2c write coor cmd failed!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("i2c write coor cmd failed!");
        // porting for touch driver end
        return FAIL;
    }
    msleep(10);
    return SUCCESS;
}
/*
*********************************************************************************************************
* Function: 
*   read rawdata from ic registers
* Input:
*   u16* data: rawdata buffer
*   i2c_client* client: i2c device
* Return:
*   SUCCESS: read process succeed, FAIL:  failed
*********************************************************************************************************
*/
s32 gtp_read_rawdata(struct i2c_client* client, u16* data)
{
    s32 ret = -1;
    u16 retry = 0;
    u8 read_state[3] = {(u8)(GTP_REG_RAW_READY>>8), (u8)GTP_REG_RAW_READY, 0};
    u16 i = 0, j = 0;
    u8 *read_rawbuf;
    u8 tail, head;

    read_rawbuf = (u8*)kmalloc(sizeof(u8) * (gt9xx_drv_num*gt9xx_sen_num * 2 + GTP_ADDR_LENGTH), GFP_KERNEL);

    if (NULL == read_rawbuf)
    {
        SET_INFO_LINE_ERR("failed to allocate for read_rawbuf");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("failed to allocate for read_rawbuf");
        // porting for touch driver end
        return FAIL;
    }
    
    if(data == NULL)
    {
        SET_INFO_LINE_ERR("Invalid raw buffer.");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Invalid raw buffer.");
        // porting for touch driver end
        goto have_error;
    }
    
    msleep(10);
    while (retry++ < GTP_WAIT_RAW_MAX_TIMES)
    {
        ret = gtp_i2c_read(client, read_state, 3);
        if(ret <= 0)
        {
            SET_INFO_LINE_ERR("i2c read failed.return: %d", ret);
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("i2c read failed.return: %d", ret);
            // porting for touch driver end
            continue;
        }
        if ((read_state[GTP_ADDR_LENGTH] & 0x80) == 0x80)
        {
            GTP_DEBUG("Raw data is ready.");
            break;
        } 
        if ((retry%20) == 0)
		{
            GTP_DEBUG("(%d)read_state[2] = 0x%02X", retry, read_state[GTP_ADDR_LENGTH]);
			if (retry == 100)
			{
				gt9_read_raw_cmd(client);
			}
		}
        msleep(5);
    }
    if (retry >= GTP_WAIT_RAW_MAX_TIMES)
    {
        SET_INFO_LINE_ERR("Wait raw data ready timeout.");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Wait raw data ready timeout.");
        // porting for touch driver end
        goto have_error;
    }
    
    if (chip_type_gt9f)
    {
        read_rawbuf[0] = (u8)( GTP_REG_RAW_DATA_GT9F >> 8);
        read_rawbuf[1] = (u8)( GTP_REG_RAW_DATA_GT9F );
    }
    else
    {
        read_rawbuf[0] = (u8)( GTP_REG_RAW_DATA >> 8);
        read_rawbuf[1] = (u8)( GTP_REG_RAW_DATA );
    }
    
    ret = gtp_i2c_read(client, read_rawbuf, GTP_ADDR_LENGTH + ((gt9xx_drv_num*gt9xx_sen_num)*2));
    if(ret <= 0)
    {
        SET_INFO_LINE_ERR("i2c read rawdata failed.");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("i2c read rawdata failed.");
        // porting for touch driver end
        goto have_error;
    }
    gtp_i2c_end_cmd(client);    // clear buffer state
    
    if (endian_mode() == MYBIG_ENDIAN)
    {
        head = 0;
        tail =1;
        GTP_DEBUG("Big Endian.");
    }
    else
    {
        head = 1;
        tail = 0;
        GTP_DEBUG("Little Endian.");
    }
    
    for(i=0,j = 0; i < ((gt9xx_drv_num*gt9xx_sen_num)*2); i+=2)
    {
        data[i/2] = (u16)(read_rawbuf[i+head+GTP_ADDR_LENGTH]<<8) + (u16)read_rawbuf[GTP_ADDR_LENGTH+i+tail];
    #if GTP_DEBUG_ARRAY_ON
        printk("%4d ", data[i/2]);
        ++j;
        if((j%10) == 0)
            printk("\n");
    #endif
    }
    
    kfree(read_rawbuf);
    return SUCCESS;
have_error:
    kfree(read_rawbuf);
    return FAIL;
}
/*
*********************************************************************************************************
* Function: 
*   rawdata test initilization function
* Input:
*   u32 check_types: test items
*********************************************************************************************************
*/
static s32 gtp_raw_test_init(void)
{
    u16 i = 0;
    
    test_rslt_buf = (s32*) kmalloc(sizeof(s32)*GTP_OPEN_SAMPLE_NUM, GFP_ATOMIC);    
    touchpad_sum = (struct gt9xx_open_info*) kmalloc(sizeof(struct gt9xx_open_info) * (4 * _BEYOND_INFO_MAX + 1), GFP_ATOMIC);
    if (NULL == test_rslt_buf || NULL == touchpad_sum)
    {
        return FAIL;
    }
    memset(touchpad_sum, 0, sizeof(struct gt9xx_open_info) * (4 * _BEYOND_INFO_MAX + 1));
    
    for (i = 0; i < (4 * _BEYOND_INFO_MAX); ++i)
    {
        touchpad_sum[i].driver = 0xFF;
    }
        
    for (i = 0; i < GTP_OPEN_SAMPLE_NUM; i++)
    {
        test_rslt_buf[i] = _CHANNEL_PASS;
    }
    return SUCCESS;
}

/*
*********************************************************************************************************
* Function: 
*   touchscreen rawdata min limit test
* Input:
*   u16* raw_buf: rawdata buffer
*********************************************************************************************************
*/
static void gtp_raw_min_test(u16 *raw_buf)
{
    u16 i, j=0;
    u8 driver, sensor;
    u8 sum_base = 1 * _BEYOND_INFO_MAX;
    u8 new_flag = 0;
    
    for (i = 0; i < gt9xx_sc_pxl_cnt; i++)
    {
        if (raw_buf[i] < min_limit_value)
        {
            test_rslt_buf[rslt_buf_idx] |= _BEYOND_MIN_LIMIT;       
            driver = (i/gt9xx_sen_num); 
            sensor = (i%gt9xx_sen_num);
            new_flag = 0;
            for (j = sum_base; j < (sum_base+_BEYOND_INFO_MAX); ++j)
            {
                if (touchpad_sum[j].driver == 0xFF)
                {
                    new_flag = 1;
                    break;
                }
                if ((driver == touchpad_sum[j].driver) && (sensor == touchpad_sum[j].sensor))
                {
                    touchpad_sum[j].times++;
                    new_flag = 0;
                    break;
                }
            }
            if (new_flag)   // new one
            {
                touchpad_sum[j].driver = driver;
                touchpad_sum[j].sensor = sensor;
                touchpad_sum[j].beyond_type |= _BEYOND_MIN_LIMIT;
                touchpad_sum[j].raw_val = raw_buf[i];
                touchpad_sum[j].times = 1;    
                GTP_DEBUG("[%d, %d]rawdata: %d, raw min limit: %d", driver, sensor, raw_buf[i], min_limit_value);
            }
            else
            {
                continue;
            }
        }
    }
}

/*
*********************************************************************************************************
* Function: 
*   touchscreen rawdata max limit test
* Input:
*   u16* raw_buf: rawdata buffer
*********************************************************************************************************
*/
static void gtp_raw_max_test(u16 *raw_buf)
{
    u16 i, j;
    u8 driver, sensor;
    u8 sum_base = 0 * _BEYOND_INFO_MAX;
    u8 new_flag = 0;
    
    for (i = 0; i < gt9xx_sc_pxl_cnt; i++)
    {
        if (raw_buf[i] > max_limit_value)
        {
            test_rslt_buf[rslt_buf_idx] |= _BEYOND_MAX_LIMIT;
            driver = (i/gt9xx_sen_num);
            sensor = (i%gt9xx_sen_num);
            new_flag = 0;
            for (j = sum_base; j < (sum_base+_BEYOND_INFO_MAX); ++j)
            {
                if (touchpad_sum[j].driver == 0xFF)
                {
                    new_flag = 1;
                    break;
                }
                if ((driver == touchpad_sum[j].driver) && (sensor == touchpad_sum[j].sensor))
                {
                    touchpad_sum[j].times++;
                    new_flag = 0;
                    break;
                }
            }
            if (new_flag)   // new one
            {
                touchpad_sum[j].driver = driver;
                touchpad_sum[j].sensor = sensor;
                touchpad_sum[j].beyond_type |= _BEYOND_MAX_LIMIT;
                touchpad_sum[j].raw_val = raw_buf[i];
                touchpad_sum[j].times = 1;    
                GTP_DEBUG("[%d, %d]rawdata: %d, raw max limit: %d", driver, sensor, raw_buf[i], max_limit_value);
            }
            else
            {
                continue;
            }
        }
    }
}

/*
*********************************************************************************************************
* Function: 
*   key rawdata max limit test
* Input:
*   u16* raw_buf: rawdata buffer
*********************************************************************************************************
*/
static void gtp_key_max_test(u16 *raw_buf)
{
    u16 i = 0, j = 1, k = 0;
    u8 key_cnt = key_iso_pos[0];
    u8 driver, sensor;
    u8 sum_base = 2 * _BEYOND_INFO_MAX;
    u8 new_flag = 0;
    
    driver = gt9xx_drv_num-1;
    for (i = gt9xx_sc_pxl_cnt; i < gt9xx_pixel_cnt; ++i)
    {
        sensor = ((i)%gt9xx_sen_num); 
        if (key_is_isolated)
        { 
            if ((key_iso_pos[j] != sensor) || (key_cnt == 0))
            {
                continue;
            }
            else    // only test key pixel rawdata
            {
                --key_cnt;
                ++j;
            }
        }
        if (raw_buf[i] > max_limit_key)
        {
            test_rslt_buf[rslt_buf_idx] |= _BEYOND_KEY_MAX_LMT;
            new_flag = 0;
            for (k = sum_base; k < (sum_base+_BEYOND_INFO_MAX); ++k)
            {
                if (touchpad_sum[k].driver == 0xFF)
                {
                    new_flag = 1;
                    break;
                }
                if (touchpad_sum[k].sensor == sensor)
                {
                    touchpad_sum[k].times++;
                    new_flag = 0;
                    break;
                }
            }
            if (new_flag)   // new one
            {
                touchpad_sum[k].driver = driver;
                touchpad_sum[k].sensor = sensor;
                touchpad_sum[k].beyond_type |= _BEYOND_KEY_MAX_LMT;
                touchpad_sum[k].raw_val = raw_buf[i];
                touchpad_sum[k].times = 1;
                if (key_is_isolated)
                {
                    touchpad_sum[k].key = j-1;
                }    
                GTP_DEBUG("[%d, %d]key rawdata: %d, key max limit: %d", driver,sensor, raw_buf[i], max_limit_key);
            }
            else
            {
                continue;
            }
        }
    }
}
/*
*********************************************************************************************************
* Function: 
*   key rawdata min limit test
* Input:
*   u16* raw_buf: rawdata buffer
*********************************************************************************************************
*/
static void gtp_key_min_test(u16 *raw_buf)
{
    u16 i = 0, j = 1, k = 0;
    u8 key_cnt = key_iso_pos[0];
    u8 driver, sensor;
    u8 sum_base = 3 * _BEYOND_INFO_MAX;
    u8 new_flag = 0;
    
    driver = gt9xx_drv_num-1;
    for (i = gt9xx_sc_pxl_cnt; i < gt9xx_pixel_cnt; ++i)
    {
        sensor = (i%gt9xx_sen_num); 
        if (key_is_isolated)
        {
            //GTP_DEBUG("sensor: %d, key_iso_pos[j]: %d", sensor, key_iso_pos[j]);
            if ((key_iso_pos[j] != sensor) || (key_cnt == 0))
            {
                continue;
            }
            else    // only test key pixel rawdata
            {
                --key_cnt;
                ++j;
            }
        }
    
        if (raw_buf[i] < min_limit_key)
        {
            test_rslt_buf[rslt_buf_idx] |= _BEYOND_KEY_MIN_LMT;
            new_flag = 0;
            for (k = sum_base; k < (sum_base + _BEYOND_INFO_MAX); ++k)
            {
                if (touchpad_sum[k].driver == 0xFF)
                {
                    new_flag = 1;
                    break;
                }
                if (sensor == touchpad_sum[k].sensor)
                {
                    touchpad_sum[k].times++;
                    break;
                }
            }
            if (new_flag)   // new one
            {
                touchpad_sum[k].driver = driver;
                touchpad_sum[k].sensor = sensor;
                touchpad_sum[k].beyond_type |= _BEYOND_KEY_MIN_LMT;
                touchpad_sum[k].raw_val = raw_buf[i];
                touchpad_sum[k].times = 1;
                if (key_is_isolated)
                {
                    touchpad_sum[k].key = j-1;
                }            
                GTP_DEBUG("[%d, %d]key rawdata: %d, key min limit: %d", driver, sensor, raw_buf[i], min_limit_key);

            }
            else
            {
                continue;
            }
        }
    }
}

static void gtp_uniformity_test(u16 *raw_buf)
{    
    u16 i = 0;
    u8 sum_base = 4 * _BEYOND_INFO_MAX;
    u16 min_val = 0, max_val = 0;
    u16 uniformity = 0;
    
    min_val = raw_buf[0];
    max_val = raw_buf[0];
    for (i = 1; i < gt9xx_sc_pxl_cnt; i++)
    {
        if (raw_buf[i] > max_val)
        {
            max_val = raw_buf[i];
        }
        if (raw_buf[i] < min_val)
        {
            min_val = raw_buf[i];
        }
    }
    
    if (0 == max_val)
    {
        uniformity = 0;
    }
    else
    {
        uniformity = (min_val * 100) / max_val;
    }
     GTP_DEBUG("min_val: %d, max_val: %d, tp uniformity: %d%%", min_val, max_val, uniformity);
     if (uniformity < uniformity_lmt)  
     {
        test_rslt_buf[rslt_buf_idx] |= _BEYOND_UNIFORMITY_LMT;
        touchpad_sum[sum_base].beyond_type |= _BEYOND_UNIFORMITY_LMT;
        touchpad_sum[sum_base].times++;
        touchpad_sum[sum_base].raw_val += uniformity;
        GTP_INFO("min_val: %d, max_val: %d, uniformity: %d%%, times: %d", min_val, max_val, uniformity, touchpad_sum[sum_base].times);
     }
}


/*
*********************************************************************************************************
* Function: 
*   analyse rawdata retrived from ic registers
* Input:
*   u16 *raw_buf, buffer for rawdata, 
*   u32 check_types, test items
* Return:
*   SUCCESS: test process succeed, FAIL: failed
*********************************************************************************************************
*/
static u32 gtp_raw_test(u16 *raw_buf, u32 check_types)
{   
    if (raw_buf == NULL)
    {
        GTP_DEBUG("Invalid raw buffer pointer!");
        return FAIL;
    } 

    if (check_types & _MAX_TEST)
    {
        gtp_raw_max_test(raw_buf);
    }

    if (check_types & _MIN_TEST)    
    {
        gtp_raw_min_test(raw_buf);
    }
    if (have_key)
    {    
        if (check_types & _KEY_MAX_TEST)
        {
            gtp_key_max_test(raw_buf);
        }
        if (check_types & _KEY_MIN_TEST)
        {
            gtp_key_min_test(raw_buf);
        }
    }
    if (check_types & _UNIFORMITY_TEST)     // 20130814, for acer
    {
        gtp_uniformity_test(raw_buf);
    }
    return SUCCESS;
} 


/*
====================================================================================================
* Function: 
*   output the test result
* Return: 
*   return the result. if result == 0, the TP is ok, otherwise list the beyonds
====================================================================================================
*/

static s32 gtp_get_test_result(void)
{
    u16 i = 0, j = 0;
    u16 beyond_max_num = 0;         // beyond max limit test times
    u16 beyond_min_num = 0;         // beyond min limit test times
    u16 beyond_key_max = 0;         // beyond key max limit test times
    u16 beyond_key_min = 0;         // beyond key min limit test times

    u16 beyond_uniformity = 0;      // uniformity test failed times
    
    s32 result = _CHANNEL_PASS;
    
#if GTP_DEBUG_ON
    for (i = 0; i < 4 * _BEYOND_INFO_MAX; ++i)
    {
        printk("(%2d, %2d)[%2d] ", touchpad_sum[i].driver, touchpad_sum[i].sensor, touchpad_sum[i].times);
        if (i && ((i+1) % 5 == 0))
        {
            printk("\n");
        }
    }
    printk("\n");
#endif

    for (i = 0; i < GTP_OPEN_SAMPLE_NUM; ++i)
    {
        if (test_rslt_buf[i] & _BEYOND_MAX_LIMIT) 
        {
            beyond_max_num++;
        }
        if (test_rslt_buf[i] & _BEYOND_MIN_LIMIT)
        {
            beyond_min_num++;
        }
        
        if (have_key)
        {
            if (test_rslt_buf[i] & _BEYOND_KEY_MAX_LMT)
            {
                beyond_key_max++;
            }
            if (test_rslt_buf[i] & _BEYOND_KEY_MIN_LMT)
            {
                beyond_key_min++;
            }
        }
        
        if (test_rslt_buf[i] & _BEYOND_UNIFORMITY_LMT)
        {
            beyond_uniformity++;
        }
    }
    if (beyond_max_num > _MIN_ERROR_NUM)
    {
        result |= _BEYOND_MAX_LIMIT;
        j = 0;
        SET_INFO_LINE_INFO("Beyond Max Limit Points Info: ");
        for (i = 0; i < _BEYOND_INFO_MAX; ++i)
        {
            if (touchpad_sum[i].driver == 0xFF)
            {
                break;
            }
            SET_INFO_LINE_INFO("  Drv: %d, Sen: %d RawVal: %d", touchpad_sum[i].driver, touchpad_sum[i].sensor, touchpad_sum[i].raw_val);
        }
    }
    if (beyond_min_num > _MIN_ERROR_NUM)
    {
        result |= _BEYOND_MIN_LIMIT;
        SET_INFO_LINE_INFO("Beyond Min Limit Points Info:");
        j = 0;
        for (i = _BEYOND_INFO_MAX; i < (2*_BEYOND_INFO_MAX); ++i)
        {
            if (touchpad_sum[i].driver == 0xFF)
            {
                break;
            }
            SET_INFO_LINE_INFO("  Drv: %d, Sen: %d RawVal: %d", touchpad_sum[i].driver, touchpad_sum[i].sensor, touchpad_sum[i].raw_val);
        }
    }
    
    if (have_key)
    {
        if (beyond_key_max > _MIN_ERROR_NUM)
        {
            result |= _BEYOND_KEY_MAX_LMT;
            SET_INFO_LINE_INFO("Beyond Key Max Limit Key Info:");
            for (i = 2*_BEYOND_INFO_MAX; i < (3*_BEYOND_INFO_MAX); ++i)
            {
                if (touchpad_sum[i].driver == 0xFF)
                {
                    break;
                }
                SET_INFO_LINE_INFO("  Drv: %d, Sen: %d RawVal: %d", touchpad_sum[i].driver, touchpad_sum[i].sensor, touchpad_sum[i].raw_val);
            }
        }
        if (beyond_key_min > _MIN_ERROR_NUM)
        {
            result |= _BEYOND_KEY_MIN_LMT;       
            SET_INFO_LINE_INFO("Beyond Key Min Limit Key Info:");
            for (i = 3*_BEYOND_INFO_MAX; i < (4*_BEYOND_INFO_MAX); ++i)
            {
                if (touchpad_sum[i].driver == 0xFF)
                {
                    break;
                }
                SET_INFO_LINE_INFO("  Drv: %d, Sen: %d RawVal: %d", touchpad_sum[i].driver, touchpad_sum[i].sensor, touchpad_sum[i].raw_val);
            }
        }
    }

    if (beyond_uniformity > _MIN_ERROR_NUM)
    {
        result |= _BEYOND_UNIFORMITY_LMT;
        SET_INFO_LINE_INFO("Beyond Uniformity Limit Info: ");
        SET_INFO_LINE_INFO("  Uniformity Limit: %d%%, Tp Uniformity: %d%%", uniformity_lmt, touchpad_sum[4*_BEYOND_INFO_MAX].raw_val / touchpad_sum[4 * _BEYOND_INFO_MAX].times);
    }
    
    if (result == 0)
    {
        SET_INFO_LINE_INFO("[TEST SUCCEED]: ");
        SET_INFO_LINE_INFO("\tThe TP is OK!");
        return result;
    }
    SET_INFO_LINE_INFO("[TEST FAILED]:");
    // porting for touch driver start
    //add SET_INFO_LINE_FTM
    SET_INFO_LINE_FTM("[TEST FAILED]:");
    // porting for touch driver end
    if (result & _BEYOND_MAX_LIMIT)
    {
        SET_INFO_LINE_INFO("  Beyond Raw Max Limit [Max Limit: %d]", max_limit_value);
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("  Beyond Raw Max Limit [Max Limit: %d]", max_limit_value);
        // porting for touch driver end
    }
    if (result & _BEYOND_MIN_LIMIT)
    {
        SET_INFO_LINE_INFO("  Beyond Raw Min Limit [Min Limit: %d]", min_limit_value);
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("  Beyond Raw Min Limit [Min Limit: %d]", min_limit_value);
        // porting for touch driver end
    }
    
    if (have_key)
    {
        if (result & _BEYOND_KEY_MAX_LMT)
        {
            SET_INFO_LINE_INFO("  Beyond KeyVal Max Limit [Key Max Limit: %d]", max_limit_key);
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("  Beyond KeyVal Max Limit [Key Max Limit: %d]", max_limit_key);
            // porting for touch driver end
        }
        if (result & _BEYOND_KEY_MIN_LMT)
        {
            SET_INFO_LINE_INFO("  Beyond KeyVal Min Limit [Key Min Limit: %d]", min_limit_key);
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("  Beyond KeyVal Min Limit [Key Min Limit: %d]", min_limit_key);
            // porting for touch driver end
        }
    }
   
    if (result & _BEYOND_UNIFORMITY_LMT)
    {
        SET_INFO_LINE_INFO("  Beyond Uniformity Limit [Uniformity Limit: %d%%]", uniformity_lmt);
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("  Beyond Uniformity Limit [Uniformity Limit: %d%%]", uniformity_lmt);
        // porting for touch driver end
    }
    return result;
}

/*
 ===================================================
 * Function: 
 *      test gt9 series ic open test
 * Input:
 *      client, i2c_client
 * Return:
 *      SUCCESS: test process success, FAIL, test process failed
 *
 ===================================================
*/
s32 gt9xx_open_test(struct i2c_client * client)
{
    u16 i = 0;
    s32 ret = FAIL; // SUCCESS, FAIL
    u16 *raw_buf = NULL;

// porting for touch driver start
//add raw data to file
	u16 j = 0, k = 0;
	char selftest_buf[127]="";
	struct file *filp;
	mm_segment_t fs;
// porting for touch driver end

    gtp_irq_disable();
#if GTP_ESD_PROTECT
    tpd_halt = 1;     // suspend esd
#endif
    gtp_rawdiff_mode = 1;

	ret = gtp_open_test_init(client);
	if(ret < 0)
    {
        SET_INFO_LINE_ERR("failed to send opentest config...");
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("failed to send opentest config...");
        ret = FAIL;
        goto open_test_exit;
    }
    
    SET_INFO_LINE_INFO("---gtp open test---");
    GTP_INFO("Parsing configuration...");
//    gtp_hopping_switch(client, 0);
    ret = gtp_parse_config(client);
    if (ret == FAIL)
    {
        SET_INFO_LINE_ERR("failed to parse config...");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("failed to parse config...");
        // porting for touch driver end
        ret = FAIL;
        goto open_test_exit;
    }
    raw_buf = (u16*)kmalloc(sizeof(u16)* (gt9xx_drv_num*gt9xx_sen_num), GFP_KERNEL);
    if (NULL == raw_buf)
    {
        SET_INFO_LINE_ERR("failed to allocate mem for raw_buf!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("failed to allocate mem for raw_buf!");
        // porting for touch driver end
        ret = FAIL;
        goto open_test_exit;
    }

    GTP_INFO("Step 1: Send Rawdata Cmd");
    
    ret = gtp_raw_test_init();
    if (FAIL == ret)
    {
        SET_INFO_LINE_ERR("Allocate memory for open test failed!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Allocate memory for open test failed!");
        // porting for touch driver end
        ret = FAIL;
        goto open_test_exit;
    }
    ret = gt9_read_raw_cmd(client);
    if (ret == FAIL)
    {
        SET_INFO_LINE_ERR("Send Read Rawdata Cmd failed!");
        // porting for touch driver start
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("Send Read Rawdata Cmd failed!");
        // porting for touch driver end
        ret = FAIL;
        goto open_test_exit;
    }
    GTP_INFO("Step 2: Sample Rawdata");

    for (i = 0; i < GTP_OPEN_SAMPLE_NUM; ++i)
    {   
        //gtp_rawdiff_mode = 1;
        rslt_buf_idx = i;
        ret = gtp_read_rawdata(client, raw_buf);
        if (ret == FAIL)
        {
            SET_INFO_LINE_ERR("Read Rawdata failed!");
            // porting for touch driver start
            //add SET_INFO_LINE_FTM
            SET_INFO_LINE_FTM("Read Rawdata failed!");
            // porting for touch driver end
            ret = FAIL;
            goto open_test_exit;
        }

// porting for touch driver start
//add raw data to file
//write raw data to file start
	GTP_INFO("[#23]Write raw data to file start. gt9xx_sen_num = %d, gt9xx_drv_num = %d", gt9xx_sen_num, gt9xx_drv_num);
	filp = filp_open(tp_opentest_raw_file, O_CREAT | O_RDWR, 0666);
	if(!IS_ERR(filp))
	{
		//printk("open return=%ld\n", IS_ERR(filp));
		GTP_INFO("[#23]open return=%ld\n", IS_ERR(filp));
		fs=get_fs();
		set_fs(KERNEL_DS);
		//filp->f_op->llseek(filp, 0, SEEK_SET);
		filp->f_op->llseek(filp, 0, SEEK_END);

		sprintf(selftest_buf,"Raw data:\n");
		filp->f_op->write(filp, (char *)selftest_buf, strlen(selftest_buf),&filp->f_pos);

		for(k=0,j = 0; k < (gt9xx_drv_num*gt9xx_sen_num); k++){
			sprintf(selftest_buf,"%4d ", raw_buf[k]);
			filp->f_op->write(filp, (char *)selftest_buf, strlen(selftest_buf),&filp->f_pos);

			//printk("%4d ", raw_buf[k]);
			++j;
			if((j%gt9xx_sen_num) == 0){
				//printk("\n");
				sprintf(selftest_buf,"\n");
				filp->f_op->write(filp, (char *)selftest_buf, strlen(selftest_buf),&filp->f_pos);
			}
		}

		set_fs(fs);
		filp_close(filp,NULL);
	}else{
		//printk("open file fail, return=%ld\n", IS_ERR(filp));
		GTP_INFO("[#23]open file fail, return=%ld\n", IS_ERR(filp));
		//test_result |= 0x4;  //flag for Fail\nCan not save tha raw data!
	}
	GTP_INFO("[#23]Write raw data to file end");
//write raw data to file end
// porting for touch driver end

        ret = gtp_raw_test(raw_buf, DEFAULT_TEST_ITEMS);
        if (ret == FAIL)
        {
            gtp_i2c_end_cmd(client);
            continue;
        }
    }

	ret = gtp_send_cfg(client);
	if(ret < 0)
    {
        SET_INFO_LINE_ERR("failed to send normal config...");
        //add SET_INFO_LINE_FTM
        SET_INFO_LINE_FTM("failed to send normal config...");
        ret = FAIL;
		goto open_test_exit;
    }

    GTP_INFO("Step 3: Analyse Result");
    SET_INFO_LINE_INFO("Total %d Sample Data, Max Show %d Info for each Test Item", GTP_OPEN_SAMPLE_NUM, _BEYOND_INFO_MAX);
    if (0 == gtp_get_test_result())
    {
        ret = SUCCESS; 
    }
    else 
    {
        ret = FAIL;
    }
    
open_test_exit:
    if (raw_buf)
    {
        kfree(raw_buf);
    }
    if (test_rslt_buf)
    {
        kfree(test_rslt_buf);
        test_rslt_buf = NULL;
    }
    if (touchpad_sum)
    {
        kfree(touchpad_sum);
        touchpad_sum = NULL;
    }
    gtp_irq_enable();
#if GTP_ESD_PROTECT
    tpd_halt = 0;     // resume esd
#endif
    gtp_rawdiff_mode = 0;
    gt9_read_coor_cmd(client);	// back to read coordinates data 
    SET_INFO_LINE_INFO("---gtp open test end---");
//    gtp_hopping_switch(client, 1);
    return ret;
}

static ssize_t gtp_sysfs_shorttest_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    u8 index;
    ssize_t len;

// porting for touch driver start
//add Short test result to file start
	char selftest_buf[255]="";
	struct file *filp;
	mm_segment_t fs;
	s32 ret = FAIL; // SUCCESS = 1, FAIL = 0
//add Short test result to file end

// mark start
    //gt9xx_short_test(i2c_client_point);
// mark end

// add start
    ret = gt9xx_short_test(i2c_client_point);
    GTP_INFO("[#23]gtp_sysfs_shorttest_show() ret = %d", ret);
// add end

//add Short test result to file start
	GTP_INFO("[#23]Write Short Test result to file start.");
	filp = filp_open(tp_shorttest_raw_file, O_CREAT | O_RDWR, 0666);
	if(!IS_ERR(filp))
	{
		GTP_INFO("[#23]open return=%ld\n", IS_ERR(filp));
		fs=get_fs();
		set_fs(KERNEL_DS);
		//filp->f_op->llseek(filp, 0, SEEK_SET);
		filp->f_op->llseek(filp, 0, SEEK_END);

		sprintf(selftest_buf,"\Short test result:\n");
		filp->f_op->write(filp, (char *)selftest_buf, strlen(selftest_buf),&filp->f_pos);

		for (index = 0; index < RsltIndex; ++index){
			sprintf(selftest_buf,"%s", result_lines[index]);
			filp->f_op->write(filp, (char *)selftest_buf, strlen(selftest_buf),&filp->f_pos);
			//if(ret)// if test pass, free data
				kfree(result_lines[index]);
			}
		//if(ret) // if test pass, reset RsltIndex
			RsltIndex = 0;

		set_fs(fs);
		filp_close(filp,NULL);
	}else{
		GTP_INFO("[#23]open file fail, return=%ld\n", IS_ERR(filp));
	}
	GTP_INFO("[#23]Write Short Test result to file end");
//add Short test result to file end

// add start
	if(ret){
		//PASS
		len = 0;
		len += sprintf(&buf[len], "Pass\n");
		GTP_INFO("[#23]gtp_sysfs_shorttest_show() PASS, len = %d", len);
		for (index = 0; index < RsltIndex_FTM; ++index){
			kfree(result_lines_FTM[index]);
			}
		RsltIndex_FTM = 0;
		return len;
		}
	else{
		//FAIL
		//show fail message
		//for (index = 0, len = 0; index < RsltIndex; ++index){
		//	sprintf(&buf[len], "%s", result_lines[index]);
		//	len += strlen(result_lines[index]);
		//	kfree(result_lines[index]);
		//	}
		//RsltIndex = 0;

		for (index = 0, len = 0; index < RsltIndex_FTM; ++index){
			sprintf(&buf[len], "%s", result_lines_FTM[index]);
			len += strlen(result_lines_FTM[index]);
			kfree(result_lines_FTM[index]);
			}
		RsltIndex_FTM = 0;
		GTP_INFO("[#23]gtp_sysfs_shorttest_show() FAIL, len = %d", len);		
		return len;
		}
// add end

// mark start
#if 0
    for (index = 0, len = 0; index < RsltIndex; ++index)
    {
        sprintf(&buf[len], "%s", result_lines[index]);
        len += strlen(result_lines[index]);
        kfree(result_lines[index]);
    }
    RsltIndex = 0;
    return len;
#endif
// mark end
// porting for touch driver end
}

static ssize_t gtp_sysfs_shorttest_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t count)
{
    return -EPERM;
}

static ssize_t gtp_sysfs_opentest_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    s32 index;
    ssize_t len;

// porting for touch driver start
//add Open test result to file start
	char selftest_buf[255]="";
	struct file *filp;
	mm_segment_t fs;
	s32 ret = FAIL; // SUCCESS = 1, FAIL = 0
//add Open test result to file end

// mark start
    //gt9xx_open_test(i2c_client_point);
// mark end

// add start
    ret = gt9xx_open_test(i2c_client_point);
    GTP_INFO("[#23]gtp_sysfs_opentest_show() ret = %d", ret);
// add end

//add Open test result to file start
	GTP_INFO("[#23]Write Open Test result to file start.");
	filp = filp_open(tp_opentest_raw_file, O_CREAT | O_RDWR, 0666);
	if(!IS_ERR(filp))
	{
		GTP_INFO("[#23]open return=%ld\n", IS_ERR(filp));
		fs=get_fs();
		set_fs(KERNEL_DS);
		//filp->f_op->llseek(filp, 0, SEEK_SET);
		filp->f_op->llseek(filp, 0, SEEK_END);

		sprintf(selftest_buf,"\nOpen test result:\n");
		filp->f_op->write(filp, (char *)selftest_buf, strlen(selftest_buf),&filp->f_pos);

		for (index = 0; index < RsltIndex; ++index){
			sprintf(selftest_buf,"%s", result_lines[index]);
			filp->f_op->write(filp, (char *)selftest_buf, strlen(selftest_buf),&filp->f_pos);
			//if(ret)// if test pass, free data
				kfree(result_lines[index]);
			}
		//if(ret) // if test pass, reset RsltIndex
			RsltIndex = 0;

		set_fs(fs);
		filp_close(filp,NULL);
	}else{
		GTP_INFO("[#23]open file fail, return=%ld\n", IS_ERR(filp));
	}
	GTP_INFO("[#23]Write Open Test result to file end");
//add Open test result to file end

// add start
	if(ret){
		//PASS
		len = 0;
		len += sprintf(&buf[len], "Pass\n");
		GTP_INFO("[#23]gtp_sysfs_opentest_show() PASS, len = %d", len);
		for (index = 0; index < RsltIndex_FTM; ++index){
			kfree(result_lines_FTM[index]);
			}
		RsltIndex_FTM = 0;
		return len;
		}
	else{
		//FAIL
		//show fail message
		//for (index = 0, len = 0; index < RsltIndex; ++index){
		//	sprintf(&buf[len], "%s", result_lines[index]);
		//	len += strlen(result_lines[index]);
		//	kfree(result_lines[index]);
		//	}
		//RsltIndex = 0;

		for (index = 0, len = 0; index < RsltIndex_FTM; ++index){
			sprintf(&buf[len], "%s", result_lines_FTM[index]);
			len += strlen(result_lines_FTM[index]);
			kfree(result_lines_FTM[index]);
			}
		RsltIndex_FTM = 0;
		GTP_INFO("[#23]gtp_sysfs_opentest_show() FAIL, len = %d", len);
		return len;
		}
// add end

// mark start
#if 0
    for (index = 0, len = 0; index < RsltIndex; ++index)
    {
        sprintf(&buf[len], "%s", result_lines[index]);
        len += strlen(result_lines[index]);
        kfree(result_lines[index]);
    }
    RsltIndex = 0;
    return len;
#endif
// mark end
// porting for touch driver end
}

static ssize_t gtp_sysfs_opentest_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t count)
{
    return -EPERM;
}
// porting for touch driver start
//add Device attribute
static ssize_t gtp_sysfs_max_limit_value_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    return sprintf(buf,"%d\n", max_limit_value);
}

static ssize_t gtp_sysfs_max_limit_value_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t count)
{
    max_limit_value = simple_strtoul(buf,NULL,10);
    return strlen(buf);
}

static ssize_t gtp_sysfs_min_limit_value_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    return sprintf(buf,"%d\n", min_limit_value);
}

static ssize_t gtp_sysfs_min_limit_value_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t count)
{
    min_limit_value = simple_strtoul(buf,NULL,10);
    return strlen(buf);
}
// porting for touch driver end
static DEVICE_ATTR(shorttest, S_IRUGO|S_IWUSR, gtp_sysfs_shorttest_show, gtp_sysfs_shorttest_store);
static DEVICE_ATTR(opentest, S_IRUGO|S_IWUSR, gtp_sysfs_opentest_show, gtp_sysfs_opentest_store);
// porting for touch driver start
//add Device attribute
static DEVICE_ATTR(max_limit_value, S_IRUGO|S_IWUSR, gtp_sysfs_max_limit_value_show, gtp_sysfs_max_limit_value_store);
static DEVICE_ATTR(min_limit_value, S_IRUGO|S_IWUSR, gtp_sysfs_min_limit_value_show, gtp_sysfs_min_limit_value_store);
// porting for touch driver end
/*******************************************************
Description:
    Goodix debug sysfs init function.

Parameter:
    none.
    
return:
    Executive outcomes. 0---succeed.
*******************************************************/
s32 gtp_test_sysfs_init(void)
{
    s32 ret ;
    
    goodix_debug_kobj = kobject_create_and_add("gtp_test", NULL) ;
    SET_INFO_LINE_INFO("Starting initializing gtp_debug_sysfs");
    if (goodix_debug_kobj == NULL)
    {
        GTP_ERROR("%s: subsystem_register failed\n", __func__);
        return -ENOMEM;
    }

    ret = sysfs_create_file(goodix_debug_kobj, &dev_attr_shorttest.attr);
    if (ret)
    {
        GTP_ERROR("%s: sysfs_create_version_file failed\n", __func__);
        return ret;
    }
    ret = sysfs_create_file(goodix_debug_kobj, &dev_attr_opentest.attr);
    if (ret)
    {
        GTP_ERROR("%s: sysfs_create_version_file failed\n", __func__);
        return ret;
    }
// porting for touch driver start
//add Device attribute
    ret = sysfs_create_file(goodix_debug_kobj, &dev_attr_max_limit_value.attr);
    if (ret)
    {
        GTP_ERROR("%s: sysfs create max_limit_value failed\n", __func__);
        return ret;
    }
	ret = sysfs_create_file(goodix_debug_kobj, &dev_attr_min_limit_value.attr);
    if (ret)
    {
        GTP_ERROR("%s: sysfs create min_limit_value failed\n", __func__);
        return ret;
    }
// porting for touch driver end

    GTP_INFO("Goodix debug sysfs create success!\n");
    return 0 ;
}

void gtp_test_sysfs_deinit(void)
{
    sysfs_remove_file(goodix_debug_kobj, &dev_attr_shorttest.attr);
    sysfs_remove_file(goodix_debug_kobj, &dev_attr_opentest.attr);
	sysfs_remove_file(goodix_debug_kobj, &dev_attr_max_limit_value.attr);
	sysfs_remove_file(goodix_debug_kobj, &dev_attr_min_limit_value.attr);
    kobject_del(goodix_debug_kobj);
}
