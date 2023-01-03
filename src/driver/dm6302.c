#include "dm6302.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <log/log.h>

#include "i2c.h"
#include "uart.h"
#include "dm5680.h"
#include "defines.h"
#include "../core/common.hh"

#define WAIT(ms)  usleep((ms)*1000)

// DM6302: RF receiver
/*  �����ź�:
        V536  --uart-->  DM5680  --(resetb)gpio-->  DM6302
        V536  --m_i2c-->  ��FPGA  --rf_spi-->  DM6302

    SPI:
        page: 3bit, addr: 12bit, data: 32bit

    ��FPGA reg��
        reg_90(self-clear):
                0x01 = Write cmd to Right DM6302
                0x02 = Write cmd to Left DM6302
                0x03 = Write cmd to Both DM6302
                0x10 = Read cmd to Both DM6302

        reg_91: addr[7:0]
        reg_92: {page[2:0],addr[11:8]}

        reg_93: data[7:0]
        reg_94: data[15:8]
        reg_95: data[23:16]
        reg_96: data[31:24]

        reg_98: read_data_right[7:0]    (read only)
        reg_99: read_data_right[15:8]   (read only)
        reg_9a: read_data_right[23:16]  (read only)
        reg_9b: read_data_right[31:24]  (read only)

        reg_9c: read_data_left[7:0]     (read only)
        reg_9d: read_data_left[15:8]    (read only)
        reg_9e: read_data_left[23:16]   (read only)
        reg_9f: read_data_left[31:24]   (read only)
*/
// RF_SPI������
/* dat0: ���Ҳ�DM6302����������
   dat1: ���Ҳ�DM6302����������*/
void SPI_Read (uint8_t page, uint16_t addr, uint32_t* dat0, uint32_t* dat1)
{
    uint8_t val;
    uint32_t rdat;

    //spi_addr
    val = addr & 0xFF;
    I2C_Write(ADDR_FPGA, 0x91, val);
    val = (page << 4) | (addr >> 8);
    I2C_Write(ADDR_FPGA, 0x92, val);

    //read cmd
    I2C_Write(ADDR_FPGA, 0x90, 0x10);

    //read dat
    rdat = I2C_Read(ADDR_FPGA, 0x9b);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0x9a);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0x99);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0x98);
    *dat0 = rdat;

    rdat = I2C_Read(ADDR_FPGA, 0x9f);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0x9e);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0x9d);
    rdat <<= 8;
    rdat |= I2C_Read(ADDR_FPGA, 0x9c);
    *dat1 = rdat;

#ifdef _DEBUG_DM6300
    LOGI("SPI READ: addr=%x  data=  %x  %x", addr, (*dat1), (*dat0));
#endif
}

// RF_SPIд����
/* sel: 0=д����DM6302
        1=д�Ҳ�DM6302
        2=д���DM6302*/
void SPI_Write(uint8_t sel, uint8_t page, uint16_t addr, uint32_t dat)
{
    uint8_t val;
    uint32_t r1=0,r0=0;

    //spi_addr
    val = addr & 0xFF;
    I2C_Write(ADDR_FPGA, 0x91, val);
    val = (page << 4) | (addr >> 8);
    I2C_Write(ADDR_FPGA, 0x92, val);

    //spi_wdat
    val = dat & 0xFF;
    I2C_Write(ADDR_FPGA, 0x93, val);
    val = (dat>>8) & 0xFF;
    I2C_Write(ADDR_FPGA, 0x94, val);
    val = (dat>>16) & 0xFF;
    I2C_Write(ADDR_FPGA, 0x95, val);
    val = (dat>>24) & 0xFF;
    I2C_Write(ADDR_FPGA, 0x96, val);

    //wrte cmd
    if(sel == 0)
        I2C_Write(ADDR_FPGA, 0x90, 0x03);
    else
        I2C_Write(ADDR_FPGA, 0x90, sel);

#ifdef _DEBUG_DM6300
    SPI_Read(page, addr, &r0, &r1);
    if(sel == 1){
        if(dat != r0)
            LOGE("                           --- W or R error !!   wdat=%x", dat);
    }
    else if(sel == 2){
        if(dat != r1)
            LOGE("                           --- W or R error !!   wdat=%x", dat);
    }
    else {
        if(dat != r1 || dat != r0)
            LOGE("                           --- W or R error !!   wdat=%x", dat);
    }
#endif
}

// ����Ƶ��
uint32_t tab[3][FREQ_NUM] = {
    {  0x3741,   0x379D,   0x37FA,   0x3856,   0x38B3,   0x390F,   0x396C,   0x39C8,   0x3840,   0x38A4},
    {    0x93,     0x94,     0x95,     0x96,     0x97,     0x98,     0x99,     0x9A,     0x96,     0x97},
    {0xB00000, 0x9D5555, 0x8AAAAB, 0x780000, 0x655555, 0x52AAAB, 0x400000, 0x2D5555, 0x000000, 0x155555}
};

void DM6302_SetChannel(uint8_t ch)
{
    SPI_Write(0, 0x6, 0xFF0, 0x00000018);
    SPI_Write(0, 0x3, 0x130, 0x00000013);
    SPI_Write(0, 0x3, 0x134, 0x00000013);
    SPI_Write(0, 0x3, 0x138, 0x00000370);
    SPI_Write(0, 0x3, 0x13C, 0x00000410);
    SPI_Write(0, 0x3, 0x140, 0x00000000);
    SPI_Write(0, 0x3, 0x144, 0x0D640735);
    SPI_Write(0, 0x3, 0x148, 0x01017F03);
    SPI_Write(0, 0x3, 0x14C, 0x022288A2);
    SPI_Write(0, 0x3, 0x150, 0x00FFCF33);
    SPI_Write(0, 0x3, 0x154, 0x1F0C3440);
    SPI_Write(0, 0x3, 0x128, 0x00008030);
    SPI_Write(0, 0x3, 0x120, tab[0][ch]);  //ch
    SPI_Write(0, 0x3, 0x11C, 0x00000002);
    SPI_Write(0, 0x3, 0x118, 0x00000001);
    SPI_Write(0, 0x3, 0x118, 0x00000000);
    SPI_Write(0, 0x3, 0x128, 0x00008030);
    SPI_Write(0, 0x3, 0x120, tab[0][ch]);  //ch
    SPI_Write(0, 0x3, 0x11C, 0x00000003);
    SPI_Write(0, 0x3, 0x118, 0x00000001);
    SPI_Write(0, 0x3, 0x118, 0x00000000);
    SPI_Write(0, 0x3, 0x150, 0x00FFCFB3);
    SPI_Write(0, 0x3, 0x104, tab[1][ch]);  //ch
    SPI_Write(0, 0x3, 0x108, tab[2][ch]);  //ch
    SPI_Write(0, 0x3, 0x100, 0x00000000);
    SPI_Write(0, 0x3, 0x100, 0x00000003);
    SPI_Write(0, 0x3, 0x150, 0x000333B3);
    SPI_Write(0, 0x3, 0x140, 0x07070000);
    SPI_Write(0, 0x3, 0x130, 0x00000010);
}

void DM6302_M0()
{
    uint32_t i;
    uint32_t dat[] = {
    0x0000C118,
    0x000000D5,
    0x00000135,
    0x0000010D,
    0x000003A0,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x000002B1,
    0x00000000,
    0x00000000,
    0x00000137,
    0x000002B5,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000175,
    0x00000000,
    0x00000000,
    0x00000139,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x46854803,
    0xF810F000,
    0x47004800,
    0x000002F5,
    0x0000C118,
    0x47004803,
    0xE7FEE7FE,
    0xE7FEE7FE,
    0xE7FEE7FE,
    0x000000C1,
    0x25014C06,
    0xE0054E06,
    0xCC0768E3,
    0x3C0C432B,
    0x34104798,
    0xD3F742B4,
    0xFFE2F7FF,
    0x00000368,
    0x00000388,
    0x4802B672,
    0x60480641,
    0xE7FDBF30,
    0x01233210,
    0x28014904,
    0x2000D003,
    0x628843C0,
    0x20004770,
    0x0000E7FB,
    0x00201080,
    0x47704770,
    0x2000B510,
    0xFFEEF7FF,
    0x07412021,
    0x21006008,
    0x43C94808,
    0x00496281,
    0x48076281,
    0x21016A42,
    0x430A0549,
    0x6A426242,
    0x6242438A,
    0xF7FF2001,
    0xBD10FFD9,
    0x00200700,
    0x00201080,
    0x2000B510,
    0xFFD0F7FF,
    0x21012018,
    0x60080749,
    0x48092100,
    0x610143C9,
    0x61010049,
    0x6A424807,
    0x04892101,
    0x6242430A,
    0x438A6A42,
    0x20016242,
    0xFFBAF7FF,
    0x0000BD10,
    0x00200740,
    0x00201080,
    0x2800B5F0,
    0x4938D03C,
    0x4C386809,
    0x0082B24B,
    0x250A58A1,
    0x17CD4369,
    0x18690F2D,
    0x4934110D,
    0x58892606,
    0x43711A59,
    0x50A11869,
    0xD02A2800,
    0x3C084C2D,
    0x4D2C6827,
    0x3D904C2E,
    0x2F1C2698,
    0x2800D024,
    0x4928D03A,
    0x68093908,
    0x01BF270F,
    0xD13A42B9,
    0x58894928,
    0xDD3D4299,
    0x4B224925,
    0x3B783118,
    0xD02C2800,
    0x6814461A,
    0x28002500,
    0x461AD029,
    0x28006015,
    0x4619D000,
    0xBDF0600C,
    0x3190491C,
    0x4C1BE7C0,
    0xE7D33488,
    0xDA232900,
    0x0F9217CA,
    0x22671851,
    0x43D21089,
    0x233F1A51,
    0x43DBB249,
    0xDD014291,
    0xDB004299,
    0x28004619,
    0x462CD000,
    0xBDF06021,
    0xBDF06026,
    0x3188490D,
    0x460AE7C3,
    0x460AE7D1,
    0x2800E7D4,
    0x4906D008,
    0x68093908,
    0xD0F02900,
    0xD0ED2800,
    0xE7EB462C,
    0x31884904,
    0x0000E7F5,
    0x00200BB0,
    0x0000C000,
    0x0000C008,
    0x00200920,
    0x0000C010,
    0x00004770,
    0x20FFB510,
    0x02004C05,
    0x20006120,
    0xFF78F7FF,
    0xF7FF2001,
    0x2000FF75,
    0xBD106120,
    0x00201000,
    0xC808E002,
    0xC1081F12,
    0xD1FA2A00,
    0x47704770,
    0xE0012000,
    0x1F12C101,
    0xD1FB2A00,
    0x00004770,
    0x69814816,
    0x12094B16,
    0x1E49B249,
    0x69806019,
    0xB2411400,
    0x30084618,
    0x49126001,
    0x1212698A,
    0x1E52B252,
    0x6989605A,
    0xB2491409,
    0x480F6041,
    0x6141490D,
    0x2207490E,
    0x021B69CB,
    0x04440A1B,
    0x61CB4323,
    0x61812100,
    0xB6626102,
    0x302420FF,
    0x07492101,
    0xBF306048,
    0x0000E7FD,
    0x00200940,
    0x0000C008,
    0x00200B40,
    0x000002FF,
    0xE000E000,
    0xE000ED04,
    0x00000388,
    0x0000C000,
    0x00000018,
    0x000002D4,
    0x000003A0,
    0x0000C018,
    0x00000100,
    0x000002E4,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x31323032,
    0x2D36302D,
    0x31203332,
    0x32323A30,
    0x2036313A
    };

    SPI_Write(0 ,0x6, 0xFF0, 0x00000000);
    for(i=0;i<237;i++){
        SPI_Write(0, 0x3, i<<2, dat[i]);
    }

    /*SPI_Write(0, 0x6, 0xFF0, 0x00000001);
    for(i=1024;i<1329;i++){
        SPI_Write(0, 0x3, i<<2, dat[i]);
    }*/

    SPI_Write(0, 0x6, 0x7FC, 0x00000000);
    //SPI_Write(0, 0x6, 0x7FC, 0x00000001);
}

void DM6302_Init0(uint8_t sel)
{
    //00_INIT
    SPI_Write(sel, 0x6, 0x7FC, 0x00000000);
    SPI_Write(sel, 0x6, 0xF1C, 0x00000001);
    SPI_Write(sel, 0x6, 0xF20, 0x0000FCD0);
    SPI_Write(sel, 0x6, 0xF04, 0x00004741); //0x00004741
    SPI_Write(sel, 0x6, 0xF08, 0x00000083);
    SPI_Write(sel, 0x6, 0xF08, 0x000000C3);
    SPI_Write(sel, 0x6, 0xF24, 0x00007000); //0x00007000
    SPI_Write(sel, 0x6, 0xF40, 0x00000003);
    SPI_Write(sel, 0x6, 0xF40, 0x00000001);
    SPI_Write(sel, 0x6, 0xFFC, 0x00000000);
    SPI_Write(sel, 0x6, 0xFFC, 0x00000001);
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
}

void DM6302_Init1(uint8_t sel)
{
    //01_BB_PLL_3456
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x2ac, 0x00000300);
    SPI_Write(sel, 0x3, 0x2B0, 0x00077777);//dcxo_pathbuf_sel
    SPI_Write(sel, 0x3, 0x230, 0x00000000);
    SPI_Write(sel, 0x3, 0x234, 0x10000000);
    SPI_Write(sel, 0x3, 0x238, 0x000000BF);
    SPI_Write(sel, 0x3, 0x23C, 0x73530610);//55530610,
    SPI_Write(sel, 0x3, 0x240, 0x3FFC0047);
    SPI_Write(sel, 0x3, 0x244, 0x00188A13);
    SPI_Write(sel, 0x3, 0x248, 0x00000000);
    SPI_Write(sel, 0x3, 0x24C, 0x0A121707);
    SPI_Write(sel, 0x3, 0x250, 0x017F0001);
    SPI_Write(sel, 0x3, 0x228, 0x0000807A);//coarse tune freq calibra
    SPI_Write(sel, 0x3, 0x220, 0x00002AE4);
    SPI_Write(sel, 0x3, 0x21C, 0x00000002);
    SPI_Write(sel, 0x3, 0x218, 0x00000001);
    SPI_Write(sel, 0x3, 0x218, 0x00000000);
    SPI_Write(sel, 0x3, 0x228, 0x0000807A);//fine tune freq calibra
    SPI_Write(sel, 0x3, 0x220, 0x00002AE4);
    SPI_Write(sel, 0x3, 0x21C, 0x00000003);
    SPI_Write(sel, 0x3, 0x218, 0x00000001);
    SPI_Write(sel, 0x3, 0x218, 0x00000000);
    SPI_Write(sel, 0x3, 0x244, 0x00188A17);
    SPI_Write(sel, 0x3, 0x204, 0x0000002D);//int div ratio
    SPI_Write(sel, 0x3, 0x208, 0x00000000);//fracn div ratio
    SPI_Write(sel, 0x3, 0x200, 0x00000000);//sdm en
    SPI_Write(sel, 0x3, 0x200, 0x00000003);
    SPI_Write(sel, 0x3, 0x240, 0x00030041);//pll close loop
    SPI_Write(sel, 0x3, 0x248, 0x00000404);
}

void DM6302_Init2(uint8_t sel, uint8_t freq)
{
    //02_RX1_PLL_11316(5658MHz)
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x130, 0x00000013); //0x00000013 10
    SPI_Write(sel, 0x3, 0x134, 0x00000013);
    SPI_Write(sel, 0x3, 0x138, 0x00000370);
    SPI_Write(sel, 0x3, 0x13C, 0x00000410);
    SPI_Write(sel, 0x3, 0x140, 0x00000000);
    SPI_Write(sel, 0x3, 0x144, 0x0D640735);
    SPI_Write(sel, 0x3, 0x148, 0x01017F03);
    SPI_Write(sel, 0x3, 0x14C, 0x022288A2); //0x021288A2
    SPI_Write(sel, 0x3, 0x150, 0x00FFCF33); //0x00FFCF33
    SPI_Write(sel, 0x3, 0x154, 0x1F0C3440); //0x1F3C3C40 0x1F0C3440
    SPI_Write(sel, 0x3, 0x128, 0x00008030);
    SPI_Write(sel, 0x3, 0x120, tab[0][freq]);  //ch
    SPI_Write(sel, 0x3, 0x11C, 0x00000002);
    SPI_Write(sel, 0x3, 0x118, 0x00000001);
    SPI_Write(sel, 0x3, 0x118, 0x00000000);
    SPI_Write(sel, 0x3, 0x128, 0x00008030);
    SPI_Write(sel, 0x3, 0x120, tab[0][freq]);  //ch
    SPI_Write(sel, 0x3, 0x11C, 0x00000003);
    SPI_Write(sel, 0x3, 0x118, 0x00000001);
    SPI_Write(sel, 0x3, 0x118, 0x00000000);
    SPI_Write(sel, 0x3, 0x150, 0x00FFCFB3);
    SPI_Write(sel, 0x3, 0x104, tab[1][freq]);  //ch
    SPI_Write(sel, 0x3, 0x108, tab[2][freq]);  //ch
    SPI_Write(sel, 0x3, 0x100, 0x00000000);
    SPI_Write(sel, 0x3, 0x100, 0x00000003);
    SPI_Write(sel, 0x3, 0x150, 0x000333B3);
    SPI_Write(sel, 0x3, 0x140, 0x07070000);
    SPI_Write(sel, 0x3, 0x130, 0x00000010);
}

void DM6302_Init3(uint8_t sel)
{
    //03_RX1_RF
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x480, 0x60FFFFFF);
    SPI_Write(sel, 0x3, 0x484, 0xFFFFF7FF);
    SPI_Write(sel, 0x3, 0x488, 0x0FFF7FE0);//0x0FFF7FE0
    SPI_Write(sel, 0x3, 0x48C, 0x00000001);
    SPI_Write(sel, 0x3, 0x490, 0x34460E01);
    SPI_Write(sel, 0x3, 0x494, 0x066727CC);//0x066427CC
    SPI_Write(sel, 0x3, 0x498, 0x00000002);//0x00001020
    SPI_Write(sel, 0x3, 0x49C, 0x00001020);
    SPI_Write(sel, 0x3, 0x4A0, 0x00001020);
    SPI_Write(sel, 0x3, 0x4A4, 0x00001030);
    SPI_Write(sel, 0x3, 0x4A8, 0x00001030);
    SPI_Write(sel, 0x3, 0x4AC, 0x8102040D);
    SPI_Write(sel, 0x3, 0x4B0, 0x00000964);
    SPI_Write(sel, 0x3, 0x4B4, 0x00000000);
    SPI_Write(sel, 0x3, 0x4B8, 0x00000000);
}

void DM6302_Init4(uint8_t sel)
{
    //04_RX1_BBF
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x500, 0x0F85FF49);
    SPI_Write(sel, 0x3, 0x504, 0x1488809A);
    SPI_Write(sel, 0x3, 0x508, 0x1488809A);
    SPI_Write(sel, 0x3, 0x50C, 0x1F616110);
    SPI_Write(sel, 0x3, 0x510, 0x00040C49);
    SPI_Write(sel, 0x3, 0x514, 0x11846130);
    SPI_Write(sel, 0x3, 0x518, 0x01B86E39);
    SPI_Write(sel, 0x3, 0x51C, 0x01F07C41);
    SPI_Write(sel, 0x3, 0x520, 0x02288A4B);
    SPI_Write(sel, 0x3, 0x524, 0x026C9B55);
    SPI_Write(sel, 0x3, 0x528, 0x82B8AE62);
    SPI_Write(sel, 0x3, 0x52C, 0x1310C470);
    SPI_Write(sel, 0x3, 0x530, 0x60300301);
    SPI_Write(sel, 0x3, 0x534, 0x40610613);
    SPI_Write(sel, 0x3, 0x538, 0x00C40C47);
    SPI_Write(sel, 0x3, 0x53C, 0x0188188F);
    SPI_Write(sel, 0x3, 0x540, 0x00010B00);
    SPI_Write(sel, 0x3, 0x544, 0x00090900);
    SPI_Write(sel, 0x3, 0x548, 0x0000000A);
    SPI_Write(sel, 0x3, 0x54C, 0x00000000);
    SPI_Write(sel, 0x3, 0x550, 0x0000002F);
    SPI_Write(sel, 0x3, 0x554, 0x00000100);
    SPI_Write(sel, 0x3, 0x55C, 0x00000000);
    SPI_Write(sel, 0x3, 0x560, 0x00000000);
    SPI_Write(sel, 0x3, 0x564, 0x00000007);
    SPI_Write(sel, 0x3, 0x568, 0x00000000);
}

void DM6302_Init5(uint8_t sel)
{
    //05_RX1_ADC
    SPI_Write(sel,  0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x728, 0xFFFFFFFE);
    SPI_Write(sel, 0x3, 0x72C, 0x22C42273);
    SPI_Write(sel, 0x3, 0x730, 0x141E81E8);
    SPI_Write(sel, 0x3, 0x734, 0x0040B1E8);
    SPI_Write(sel, 0x3, 0x738, 0x22C42273);
    SPI_Write(sel, 0x3, 0x73C, 0x141E81E8);
    SPI_Write(sel, 0x3, 0x740, 0x0040B1E8);
    SPI_Write(sel, 0x3, 0x744, 0x00A20001);
    SPI_Write(sel, 0x3, 0x748, 0x00004400);
    SPI_Write(sel, 0x3, 0x74C, 0x00000000);
}

void DM6302_Init6(uint8_t sel)
{
    //06_RX1_DFE
    SPI_Write(sel, 0x6, 0xFF0, 0x00000019);
    SPI_Write(sel, 0x3, 0x0E4, 0x0000000C);
    SPI_Write(sel, 0x3, 0x0E8, 0x00000003);

    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x254, 0x0055780F); //ADC1=ADC2=3456/8=432
    SPI_Write(sel, 0x3, 0x258, 0x00010002); //RBDP=3456/8=432
    SPI_Write(sel, 0x3, 0x908, 0x001FFF03); //0x001FFF03
    SPI_Write(sel, 0x3, 0x90C, 0xDE07E0F0); //ADC=432/2,FBCLK=ADC/4=54  0xDE07E0F0
    SPI_Write(sel, 0x3, 0x880, 0x0000001C); //GAIN
    SPI_Write(sel, 0x3, 0x938, 0x00000082); //AGC 0x00000082
}

void DM6302_Init7(uint8_t sel)
{
    //07_RX1_FIR
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x800, 0x000E000A);
    SPI_Write(sel, 0x3, 0x804, 0xFFEFFFF4);
    SPI_Write(sel, 0x3, 0x808, 0x00140010);
    SPI_Write(sel, 0x3, 0x80C, 0xFFE9FFEA);
    SPI_Write(sel, 0x3, 0x810, 0x0018001D);
    SPI_Write(sel, 0x3, 0x814, 0xFFE7FFDA);
    SPI_Write(sel, 0x3, 0x818, 0x0018002F);
    SPI_Write(sel, 0x3, 0x81C, 0xFFEAFFC5);
    SPI_Write(sel, 0x3, 0x820, 0x00110047);
    SPI_Write(sel, 0x3, 0x824, 0xFFF6FFAC);
    SPI_Write(sel, 0x3, 0x828, 0x00000062);
    SPI_Write(sel, 0x3, 0x82C, 0x000DFF8F);
    SPI_Write(sel, 0x3, 0x830, 0xFFE3007F);
    SPI_Write(sel, 0x3, 0x834, 0x0032FF73);
    SPI_Write(sel, 0x3, 0x838, 0xFFB6009B);
    SPI_Write(sel, 0x3, 0x83C, 0x0068FF59);
    SPI_Write(sel, 0x3, 0x840, 0xFF7600B0);
    SPI_Write(sel, 0x3, 0x844, 0x00B1FF49);
    SPI_Write(sel, 0x3, 0x848, 0xFF2100BA);
    SPI_Write(sel, 0x3, 0x84C, 0x0112FF48);
    SPI_Write(sel, 0x3, 0x850, 0xFEB400B0);
    SPI_Write(sel, 0x3, 0x854, 0x018EFF5F);
    SPI_Write(sel, 0x3, 0x858, 0xFE280089);
    SPI_Write(sel, 0x3, 0x85C, 0x022EFF9B);
    SPI_Write(sel, 0x3, 0x860, 0xFD6E0032);
    SPI_Write(sel, 0x3, 0x864, 0x030A0015);
    SPI_Write(sel, 0x3, 0x868, 0xFC5EFF87);
    SPI_Write(sel, 0x3, 0x86C, 0x046E0109);
    SPI_Write(sel, 0x3, 0x870, 0xFA61FE1B);
    SPI_Write(sel, 0x3, 0x874, 0x07BB035C);
    SPI_Write(sel, 0x3, 0x878, 0xF304F990);
    SPI_Write(sel, 0x3, 0x87C, 0x3B341183);
}

void DM6302_Init8(uint8_t sel)
{
    //08_RX1_AGC
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x880, 0x003C001C);
    SPI_Write(sel, 0x3, 0x884, 0x007C005C);
    SPI_Write(sel, 0x3, 0x888, 0x00BC009C);
    SPI_Write(sel, 0x3, 0x88C, 0x013C011C);
    SPI_Write(sel, 0x3, 0x890, 0x017C015C);
    SPI_Write(sel, 0x3, 0x894, 0x01BC019C);
    SPI_Write(sel, 0x3, 0x898, 0x023C01DC);
    SPI_Write(sel, 0x3, 0x89C, 0x027C025C);
    SPI_Write(sel, 0x3, 0x8A0, 0x02BC029C);
    SPI_Write(sel, 0x3, 0x8A4, 0x033C02DC);
    SPI_Write(sel, 0x3, 0x8A8, 0x037C035C);
    SPI_Write(sel, 0x3, 0x8AC, 0x03BC039C);
    SPI_Write(sel, 0x3, 0x8B0, 0x033403DC);
    SPI_Write(sel, 0x3, 0x8B4, 0x03740354);
    SPI_Write(sel, 0x3, 0x8B8, 0x03B40394);
    SPI_Write(sel, 0x3, 0x8BC, 0x032C03D4);
    SPI_Write(sel, 0x3, 0x8C0, 0x036C034C);
    SPI_Write(sel, 0x3, 0x8C4, 0x03AC038C);
    SPI_Write(sel, 0x3, 0x8C8, 0x032403CC);
    SPI_Write(sel, 0x3, 0x8CC, 0x03640344);
    SPI_Write(sel, 0x3, 0x8D0, 0x03A40384);
    SPI_Write(sel, 0x3, 0x8D4, 0x038303C4);
    SPI_Write(sel, 0x3, 0x8D8, 0x03C303A3);
    SPI_Write(sel, 0x3, 0x8DC, 0x03A20382);
    SPI_Write(sel, 0x3, 0x8E0, 0x032103C2);
    SPI_Write(sel, 0x3, 0x8E4, 0x03610341);
    SPI_Write(sel, 0x3, 0x8E8, 0x03A10381);
    SPI_Write(sel, 0x3, 0x8EC, 0x032003C1);
    SPI_Write(sel, 0x3, 0x8F0, 0x03600340);
    SPI_Write(sel, 0x3, 0x8F4, 0x03A00380);
    SPI_Write(sel, 0x3, 0x8F8, 0x03C103C0);
    SPI_Write(sel, 0x3, 0x8FC, 0x03C303C2);
    SPI_Write(sel, 0x3, 0x8FC, 0x03C503C4);
    SPI_Write(sel, 0x3, 0x900, 0x03C703C6);
    SPI_Write(sel, 0x3, 0x904, 0x03C903C8);
    SPI_Write(sel, 0x3, 0x93C, 0x0001FF00); //0x0002FF00
    SPI_Write(sel, 0x3, 0x944, 0x00004300);
    SPI_Write(sel, 0x3, 0x948, 0x00010000); //0x00001000
    SPI_Write(sel, 0x3, 0x958, 0x00FCFE00); //0x00FCFE00
    SPI_Write(sel, 0x3, 0x960, 0x0000B278); //0x0000B260
    SPI_Write(sel, 0x3, 0x938, 0x0000100A); //0x0000101A
    SPI_Write(sel, 0x3, 0x968, 0x00000020);
    SPI_Write(sel, 0x3, 0x974, 0x0058001F);
    SPI_Write(sel, 0x3, 0x978, 0x00000000);
    SPI_Write(sel, 0x3, 0x52C, 0x1310C470);
    SPI_Write(sel, 0x3, 0x530, 0x98300301);
    SPI_Write(sel, 0x3, 0x534, 0x40610613);
    SPI_Write(sel, 0x3, 0x4B0, 0x00000964);
    SPI_Write(sel, 0x3, 0x970, 0x0000003F);
    SPI_Write(sel, 0x3, 0x940, 0x00050A0A);
    SPI_Write(sel, 0x3, 0x94C, 0x10001000);
}

void DM6302_Init9(uint8_t sel)
{
    //09_RX2_RF
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x4C0, 0x6000F001);
    SPI_Write(sel, 0x3, 0x4C4, 0x00000000);
    SPI_Write(sel, 0x3, 0x4C8, 0x00006048);
    SPI_Write(sel, 0x3, 0x4CC, 0x00000001);
    SPI_Write(sel, 0x3, 0x4D0, 0x34460E01);
    SPI_Write(sel, 0x3, 0x4D4, 0x066727CC);//0x066427CC
    SPI_Write(sel, 0x3, 0x4D8, 0x00000002);//0x00001020
    SPI_Write(sel, 0x3, 0x4DC, 0x00001020);
    SPI_Write(sel, 0x3, 0x4E0, 0x00001020);
    SPI_Write(sel, 0x3, 0x4E4, 0x00001030);
    SPI_Write(sel, 0x3, 0x4E8, 0x00001030);
    SPI_Write(sel, 0x3, 0x4EC, 0x8102040D);
    SPI_Write(sel, 0x3, 0x4F0, 0x00000964);
    SPI_Write(sel, 0x3, 0x4F4, 0x00000000);
    SPI_Write(sel, 0x3, 0x4F8, 0x00000000);
}

void DM6302_Init10(uint8_t sel)
{
    //10_RX2_BBF
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x600, 0x0F85FF49);
    SPI_Write(sel, 0x3, 0x604, 0x1488809A);
    SPI_Write(sel, 0x3, 0x608, 0x1488809A);
    SPI_Write(sel, 0x3, 0x60C, 0x1F616110);
    SPI_Write(sel, 0x3, 0x610, 0x00040C49);
    SPI_Write(sel, 0x3, 0x614, 0x11846130);
    SPI_Write(sel, 0x3, 0x618, 0x01B86E39);
    SPI_Write(sel, 0x3, 0x61C, 0x01F07C41);
    SPI_Write(sel, 0x3, 0x620, 0x02288A4B);
    SPI_Write(sel, 0x3, 0x624, 0x026C9B55);
    SPI_Write(sel, 0x3, 0x628, 0x82B8AE62);
    SPI_Write(sel, 0x3, 0x62C, 0x1310C470);
    SPI_Write(sel, 0x3, 0x630, 0x60300301);
    SPI_Write(sel, 0x3, 0x634, 0x40610613);
    SPI_Write(sel, 0x3, 0x638, 0x00C40C47);
    SPI_Write(sel, 0x3, 0x63C, 0x0188188F);
    SPI_Write(sel, 0x3, 0x640, 0x00010B00);
    SPI_Write(sel, 0x3, 0x644, 0x00090900);
    SPI_Write(sel, 0x3, 0x648, 0x0000000A);
    SPI_Write(sel, 0x3, 0x64C, 0x00000000);
    SPI_Write(sel, 0x3, 0x650, 0x0000002F);
    SPI_Write(sel, 0x3, 0x654, 0x00000100);
    SPI_Write(sel, 0x3, 0x65C, 0x00000000);
    SPI_Write(sel, 0x3, 0x660, 0x00000000);
    SPI_Write(sel, 0x3, 0x664, 0x00000007);
    SPI_Write(sel, 0x3, 0x668, 0x00000000);
}

void DM6302_Init11(uint8_t sel)
{
    //11_RX2_ADC
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0x750, 0xFFFFFFFE);
    SPI_Write(sel, 0x3, 0x754, 0x22C42273);
    SPI_Write(sel, 0x3, 0x758, 0x141E81E8);
    SPI_Write(sel, 0x3, 0x75C, 0x0040B1E8);
    SPI_Write(sel, 0x3, 0x760, 0x22C42273);
    SPI_Write(sel, 0x3, 0x764, 0x141E81E8);
    SPI_Write(sel, 0x3, 0x768, 0x0040B1E8);
    SPI_Write(sel, 0x3, 0x76C, 0x00A20001);
    SPI_Write(sel, 0x3, 0x770, 0x00004400);
    SPI_Write(sel, 0x3, 0x774, 0x00000000);
}

void DM6302_Init12(uint8_t sel)
{
    //12_RX2_FIR
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0xA00, 0x000E000A);
    SPI_Write(sel, 0x3, 0xA04, 0xFFEFFFF4);
    SPI_Write(sel, 0x3, 0xA08, 0x00140010);
    SPI_Write(sel, 0x3, 0xA0C, 0xFFE9FFEA);
    SPI_Write(sel, 0x3, 0xA10, 0x0018001D);
    SPI_Write(sel, 0x3, 0xA14, 0xFFE7FFDA);
    SPI_Write(sel, 0x3, 0xA18, 0x0018002F);
    SPI_Write(sel, 0x3, 0xA1C, 0xFFEAFFC5);
    SPI_Write(sel, 0x3, 0xA20, 0x00110047);
    SPI_Write(sel, 0x3, 0xA24, 0xFFF6FFAC);
    SPI_Write(sel, 0x3, 0xA28, 0x00000062);
    SPI_Write(sel, 0x3, 0xA2C, 0x000DFF8F);
    SPI_Write(sel, 0x3, 0xA30, 0xFFE3007F);
    SPI_Write(sel, 0x3, 0xA34, 0x0032FF73);
    SPI_Write(sel, 0x3, 0xA38, 0xFFB6009B);
    SPI_Write(sel, 0x3, 0xA3C, 0x0068FF59);
    SPI_Write(sel, 0x3, 0xA40, 0xFF7600B0);
    SPI_Write(sel, 0x3, 0xA44, 0x00B1FF49);
    SPI_Write(sel, 0x3, 0xA48, 0xFF2100BA);
    SPI_Write(sel, 0x3, 0xA4C, 0x0112FF48);
    SPI_Write(sel, 0x3, 0xA50, 0xFEB400B0);
    SPI_Write(sel, 0x3, 0xA54, 0x018EFF5F);
    SPI_Write(sel, 0x3, 0xA58, 0xFE280089);
    SPI_Write(sel, 0x3, 0xA5C, 0x022EFF9B);
    SPI_Write(sel, 0x3, 0xA60, 0xFD6E0032);
    SPI_Write(sel, 0x3, 0xA64, 0x030A0015);
    SPI_Write(sel, 0x3, 0xA68, 0xFC5EFF87);
    SPI_Write(sel, 0x3, 0xA6C, 0x046E0109);
    SPI_Write(sel, 0x3, 0xA70, 0xFA61FE1B);
    SPI_Write(sel, 0x3, 0xA74, 0x07BB035C);
    SPI_Write(sel, 0x3, 0xA78, 0xF304F990);
    SPI_Write(sel, 0x3, 0xA7C, 0x3B341183);
}

void DM6302_Init13(uint8_t sel)
{
    //13_RX2_DFE
    SPI_Write(sel, 0x6, 0xFF0, 0x00000019);
    SPI_Write(sel, 0x3, 0x080, 0x1004210C);//RBDP=432/8,CLKOUT=432/4
    SPI_Write(sel, 0x3, 0x084, 0x00000004);
    SPI_Write(sel, 0x3, 0x088, 0x00000205);//TDD,2PORT,2T2R 0x00000005
    SPI_Write(sel, 0x3, 0x018, 0xE4F15E3C);
    SPI_Write(sel, 0x3, 0x01C, 0x0001C140);
    SPI_Write(sel, 0x3, 0x020, 0x0000000D);

    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0xB08, 0x001FFF03);//0x001FFF03
    SPI_Write(sel, 0x3, 0xB0C, 0xDE07E0F0);//0xDE07E0F0
    SPI_Write(sel, 0x3, 0xA80, 0x0000001C);//GAIN
    SPI_Write(sel, 0x3, 0xB38, 0x00000082);//AGC 0x00000082
}

void DM6302_Init14(uint8_t sel)
{
    //14_RX2_AGC
    SPI_Write(sel, 0x6, 0xFF0, 0x00000018);
    SPI_Write(sel, 0x3, 0xA80, 0x003C001C);
    SPI_Write(sel, 0x3, 0xA84, 0x007C005C);
    SPI_Write(sel, 0x3, 0xA88, 0x00BC009C);
    SPI_Write(sel, 0x3, 0xA8C, 0x013C011C);
    SPI_Write(sel, 0x3, 0xA90, 0x017C015C);
    SPI_Write(sel, 0x3, 0xA94, 0x01BC019C);
    SPI_Write(sel, 0x3, 0xA98, 0x023C01DC);
    SPI_Write(sel, 0x3, 0xA9C, 0x027C025C);
    SPI_Write(sel, 0x3, 0xAA0, 0x02BC029C);
    SPI_Write(sel, 0x3, 0xAA4, 0x033C02DC);
    SPI_Write(sel, 0x3, 0xAA8, 0x037C035C);
    SPI_Write(sel, 0x3, 0xAAC, 0x03BC039C);
    SPI_Write(sel, 0x3, 0xAB0, 0x033403DC);
    SPI_Write(sel, 0x3, 0xAB4, 0x03740354);
    SPI_Write(sel, 0x3, 0xAB8, 0x03B40394);
    SPI_Write(sel, 0x3, 0xABC, 0x032C03D4);
    SPI_Write(sel, 0x3, 0xAC0, 0x036C034C);
    SPI_Write(sel, 0x3, 0xAC4, 0x03AC038C);
    SPI_Write(sel, 0x3, 0xAC8, 0x032403CC);
    SPI_Write(sel, 0x3, 0xACC, 0x03640344);
    SPI_Write(sel, 0x3, 0xAD0, 0x03A40384);
    SPI_Write(sel, 0x3, 0xAD4, 0x038303C4);
    SPI_Write(sel, 0x3, 0xAD8, 0x03C303A3);
    SPI_Write(sel, 0x3, 0xADC, 0x03A20382);
    SPI_Write(sel, 0x3, 0xAE0, 0x032103C2);
    SPI_Write(sel, 0x3, 0xAE4, 0x03610341);
    SPI_Write(sel, 0x3, 0xAE8, 0x03A10381);
    SPI_Write(sel, 0x3, 0xAEC, 0x032003C1);
    SPI_Write(sel, 0x3, 0xAF0, 0x03600340);
    SPI_Write(sel, 0x3, 0xAF4, 0x03A00380);
    SPI_Write(sel, 0x3, 0xAF8, 0x03C103C0);
    SPI_Write(sel, 0x3, 0xAFC, 0x03C303C2);
    SPI_Write(sel, 0x3, 0xAFC, 0x03C503C4);
    SPI_Write(sel, 0x3, 0xB00, 0x03C703C6);
    SPI_Write(sel, 0x3, 0xB04, 0x03C903C8);
    SPI_Write(sel, 0x3, 0xB3C, 0x0001FF00);//0x0002FF00
    SPI_Write(sel, 0x3, 0xB44, 0x00004300);
    SPI_Write(sel, 0x3, 0xB48, 0x00010000);//0x00001000
    SPI_Write(sel, 0x3, 0xB58, 0x00FCFE00);//0x00FCFE00
    SPI_Write(sel, 0x3, 0xB60, 0x0000B278);//0x0000B260
    SPI_Write(sel, 0x3, 0xB38, 0x0000100A);//0x0000101A
    SPI_Write(sel, 0x3, 0xB68, 0x00000020);
    SPI_Write(sel, 0x3, 0xB74, 0x0058001F);
    SPI_Write(sel, 0x3, 0xB78, 0x00000000);
    SPI_Write(sel, 0x3, 0x62C, 0x1310C470);
    SPI_Write(sel, 0x3, 0x630, 0x98300301);
    SPI_Write(sel, 0x3, 0x634, 0x40610613);
    SPI_Write(sel, 0x3, 0x4F0, 0x00000964);
    SPI_Write(sel, 0x3, 0xB70, 0x0000003F);
    SPI_Write(sel, 0x3, 0xB40, 0x00050A0A);
    SPI_Write(sel, 0x3, 0xB4C, 0x10001000);

    SPI_Write(sel, 0x3, 0x90C, 0x9E22E0F0);
    SPI_Write(sel, 0x3, 0xB0C, 0x9E22E0F0);
    SPI_Write(sel, 0x3, 0x920, 0x00000098);
    SPI_Write(sel, 0x3, 0xB20, 0x00000098);
    SPI_Write(sel, 0x3, 0x908, 0x00000000);
    SPI_Write(sel, 0x3, 0xB08, 0x00000000);
    SPI_Write(sel, 0x3, 0x908, 0x001FFF03);
    SPI_Write(sel, 0x3, 0xB08, 0x001FFF03);
}

////////////////////////////////////////////////////////////////////////////////
// efuse rd amd imp
#define  EFUSE_NUM       12   // 12 macro
#define  EFUSE_SIZE      128  // 128*8=1024bit

typedef struct _MACRO0
{
    unsigned char  chip_name[8];
    unsigned char  chip_ver[4];
    unsigned char  chip_id[8];
    unsigned char  chip_grade[4];
    unsigned char  crc[4];
    unsigned char  efuse_ver[4];
    unsigned char  rsvd1[32];
    unsigned short mode;
    unsigned short band_num;
    unsigned char  rsvd2[60];
}MACRO0_T;

typedef struct _MACRO1
{
    unsigned short ical;
    unsigned short rcal;
    unsigned long   bandgap;
	unsigned long   tempsensor;
    unsigned char  rsvd[116];
}MACRO1_T;

typedef struct _RX_CAL
{
    unsigned short freq_start;
    unsigned short freq_stop;
    unsigned long   iqmismatch[3];
    unsigned long   im2;
    unsigned char  rsvd[12];
}RX_CAL_T;

typedef struct _TX_CAL
{
    unsigned short freq_start;
    unsigned short freq_stop;
    unsigned long   dcoc_i;
    unsigned long   dcoc_q;
    unsigned long   iqmismatch;
    unsigned char  rsvd[16];
}TX_CAL_T;

typedef struct _MACRO2
{
    RX_CAL_T rx1;
    TX_CAL_T tx1;
    RX_CAL_T rx2;
    TX_CAL_T tx2;
}MACRO2_T;

typedef union _EFUSE
{
    unsigned char dat[EFUSE_NUM][EFUSE_SIZE];
    struct
    {
        MACRO0_T m0;
        MACRO1_T m1;
        MACRO2_T m2[EFUSE_NUM-2];
        unsigned short band;
        unsigned short err;
    }macro;
}EFUSE_T;

EFUSE_T efuse0,efuse1;

void DM6302_EFUSE1(uint8_t SEL6302)
{
    int i, j;
    uint32_t r0,r1, rdat_sel;
    int efuse_rdy = 1;

    EFUSE_T* efuse_sel = (SEL6302==1) ? (&efuse0) : (&efuse1);

    memset((char*)efuse_sel, 0, sizeof(EFUSE_T));

    // EFUSE_RST = 1;
    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000019);
    SPI_Write(SEL6302, 0x3, 0x0E0, 0x00000001);

    LOGI("EFUSE1 %d, s1", SEL6302);

    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000018);
    for(j=66; j<68/*EFUSE_SIZE*/; j++) // read macro 0
    {
        // EFUSE_CFG = (0<<11) | (j<<4) | 0x1;
        SPI_Write(SEL6302, 0x3, 0x7D0, (j<<4) | 0x1);

        // while(EFUSE_RDY&1);
        while(efuse_rdy){
            SPI_Read(0x3, 0x7D4, &r0, &r1);
            rdat_sel = (SEL6302 == 1) ? r0 : r1;
            efuse_rdy = rdat_sel & 1;
        }
        efuse_rdy = 1;

        // efuse.data[0][j] = EFUSE_DAT;
        SPI_Read(0x3, 0x7D8, &r0, &r1);
        rdat_sel = (SEL6302 == 1) ? r0 : r1;
        efuse_sel->dat[0][j] = rdat_sel & 0xFF;
    }

    LOGI("EFUSE1 %d, s2", SEL6302);

    for(j=0; j<12/*EFUSE_SIZE*/; j++) // read macro 1
    {
        // EFUSE_CFG = (1<<11) | (j<<4) | 0x1;
        SPI_Write(SEL6302, 0x3, 0x7D0, (1<<11) | (j<<4) | 0x1);

        // while(EFUSE_RDY&1);
        while(efuse_rdy){
            SPI_Read(0x3, 0x7D4, &r0, &r1);
            rdat_sel = (SEL6302 == 1) ? r0 : r1;
            efuse_rdy = rdat_sel & 1;
        }
        efuse_rdy = 1;

        // efuse.data[1][j] = EFUSE_DAT;
        SPI_Read(0x3, 0x7D8, &r0, &r1);
        rdat_sel = (SEL6302 == 1) ? r0 : r1;
        efuse_sel->dat[1][j] = rdat_sel & 0xFF;
    }

    LOGI("EFUSE1 %d, s3, %d", SEL6302, efuse_sel->macro.m0.band_num);

    //for(i=2; i<efuse.macro.m0.band_num+2; i++) // read macro 2~11
    //efuse_sel->macro.m0.band_num = (efuse_sel->macro.m0.band_num >> 8) | (efuse_sel->macro.m0.band_num << 8);
    for(i=2; i<efuse_sel->macro.m0.band_num+2; i++) // read macro 2~11
    {
        for(j=0; j<20/*EFUSE_SIZE*/; j++){
            // EFUSE_CFG = (i<<11) | (j<<4) | 0x1;
            SPI_Write(SEL6302, 0x3, 0x7D0, (i<<11) | (j<<4) | 0x1);

            // while(EFUSE_RDY&1);
            while(efuse_rdy){
                SPI_Read(0x3, 0x7D4, &r0, &r1);
                rdat_sel = (SEL6302 == 1) ? r0 : r1;
                efuse_rdy = rdat_sel & 1;
            }
            efuse_rdy = 1;

            // efuse.data[i][j] = EFUSE_DAT;
            SPI_Read(0x3, 0x7D8, &r0, &r1);
            rdat_sel = (SEL6302 == 1) ? r0 : r1;
            efuse_sel->dat[i][j] = rdat_sel & 0xFF;

            if(j == 3){
                //efuse_sel->macro.m2[i-2].rx1.freq_start = (efuse_sel->macro.m2[i-2].rx1.freq_start >> 8) | (efuse_sel->macro.m2[i-2].rx1.freq_start << 8);
                //efuse_sel->macro.m2[i-2].rx1.freq_stop = (efuse_sel->macro.m2[i-2].rx1.freq_stop >> 8) | (efuse_sel->macro.m2[i-2].rx1.freq_stop << 8);
                if(efuse_sel->macro.m2[i-2].rx1.freq_start<5000 || efuse_sel->macro.m2[i-2].rx1.freq_stop>6000)
                    break;
            }
        }

        LOGI("EFUSE1 %d, s3-%i", SEL6302, i);

        for(j=64; j<84/*EFUSE_SIZE*/; j++){
            // EFUSE_CFG = (i<<11) | (j<<4) | 0x1;
            SPI_Write(SEL6302, 0x3, 0x7D0, (i<<11) | (j<<4) | 0x1);

            // while(EFUSE_RDY&1);
            while(efuse_rdy){
                SPI_Read(0x3, 0x7D4, &r0, &r1);
                rdat_sel = (SEL6302 == 1) ? r0 : r1;
                efuse_rdy = rdat_sel & 1;
            }
            efuse_rdy = 1;

            // efuse.data[i][j] = EFUSE_DAT;
            SPI_Read(0x3, 0x7D8, &r0, &r1);
            rdat_sel = (SEL6302 == 1) ? r0 : r1;
            efuse_sel->dat[i][j] = rdat_sel & 0xFF;

            if(j == 67){
                //efuse_sel->macro.m2[i-2].rx2.freq_start = (efuse_sel->macro.m2[i-2].rx2.freq_start >> 8) | (efuse_sel->macro.m2[i-2].rx2.freq_start << 8);
                //efuse_sel->macro.m2[i-2].rx2.freq_stop = (efuse_sel->macro.m2[i-2].rx2.freq_stop >> 8) | (efuse_sel->macro.m2[i-2].rx2.freq_stop << 8);
                if(efuse_sel->macro.m2[i-2].rx2.freq_start<5000 || efuse_sel->macro.m2[i-2].rx2.freq_stop>6000)
                    break;
            }
        }
    }

    LOGI("EFUSE1 %d, s4", SEL6302);

    // EFUSE_CFG = 0;
    SPI_Write(SEL6302, 0x3, 0x7D0, 0x00000000);
    // EFUSE_RST = 0;
    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000019);
    SPI_Write(SEL6302, 0x3, 0x0E0, 0x00000000);

#if(0)
    // application
    efuse_sel->macro.m1.bandgap = ((efuse_sel->macro.m1.bandgap >> 24) & 0xFF) |
                             ((efuse_sel->macro.m1.bandgap >> 8)  & 0xFF00) |
                             ((efuse_sel->macro.m1.bandgap << 8)  & 0xFF0000) |
                              ((efuse_sel->macro.m1.bandgap << 24) & 0xFF000000);
    efuse_sel->macro.m1.ical = (efuse_sel->macro.m1.ical >> 8) | (efuse_sel->macro.m1.ical << 8);
    efuse_sel->macro.m1.rcal = (efuse_sel->macro.m1.rcal >> 8) | (efuse_sel->macro.m1.rcal << 8);
#endif

    //LOGI("band_num=%x", efuse_sel->macro.m0.band_num);
    //LOGI("bandgap=%lx", efuse_sel->macro.m1.bandgap);
    //LOGI("ical=%x", efuse_sel->macro.m1.ical);
    //LOGI("rcal=%x", efuse_sel->macro.m1.rcal);

    r1 = ((efuse_sel->macro.m1.ical & 0x1F) << 3) | (efuse_sel->macro.m1.rcal & 0x7);
    //LOGI("rh=%lx", rh);

    SPI_Write(SEL6302, 0x6, 0xF14, efuse_sel->macro.m1.bandgap);
    SPI_Write(SEL6302, 0x6, 0xF18, r1);
}


void DM6302_EFUSE2(uint8_t SEL6302)
{
    int i;

    EFUSE_T* efuse_sel = (SEL6302==1) ? (&efuse0) : (&efuse1);

    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000018);
    SPI_Write(SEL6302, 0x3, 0x3AC, 0x00000012);
	for(i=0; i<efuse_sel->macro.m0.band_num; i++) // find match macro 5.8G
    //for(i=0; i<10; i++) // find match macro 5.8G
	{
        //efuse_sel->macro.m2[i].rx1.freq_start = (efuse_sel->macro.m2[i].rx1.freq_start >> 8) | (efuse_sel->macro.m2[i].rx1.freq_start << 8);
        //efuse_sel->macro.m2[i].rx1.freq_stop = (efuse_sel->macro.m2[i].rx1.freq_stop >> 8) | (efuse_sel->macro.m2[i].rx1.freq_stop << 8);

        //efuse_sel->macro.m2[i].rx2.freq_start = (efuse_sel->macro.m2[i].rx2.freq_start >> 8) | (efuse_sel->macro.m2[i].rx2.freq_start << 8);
        //efuse_sel->macro.m2[i].rx2.freq_stop = (efuse_sel->macro.m2[i].rx2.freq_stop >> 8) | (efuse_sel->macro.m2[i].rx2.freq_stop << 8);

        LOGI(" rx1: start=%x, stop=%x;  rx2: start=%x, stop=%x", efuse_sel->macro.m2[i].rx1.freq_start, efuse_sel->macro.m2[i].rx1.freq_stop,
                                                                       efuse_sel->macro.m2[i].rx2.freq_start, efuse_sel->macro.m2[i].rx2.freq_stop);

		if(efuse_sel->macro.m2[i].rx1.freq_start>=5000 && efuse_sel->macro.m2[i].rx1.freq_stop<=6000)
        {
#if(0)
            efuse_sel->macro.m2[i].rx1.iqmismatch[0] = ((efuse_sel->macro.m2[i].rx1.iqmismatch[0] >> 24) & 0xFF) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[0] >> 8)  & 0xFF00) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[0] << 8)  & 0xFF0000) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[0] << 24) & 0xFF000000);

            efuse_sel->macro.m2[i].rx1.iqmismatch[1] = ((efuse_sel->macro.m2[i].rx1.iqmismatch[1] >> 24) & 0xFF) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[1] >> 8)  & 0xFF00) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[1] << 8)  & 0xFF0000) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[1] << 24) & 0xFF000000);

            efuse_sel->macro.m2[i].rx1.iqmismatch[2] = ((efuse_sel->macro.m2[i].rx1.iqmismatch[2] >> 24) & 0xFF) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[2] >> 8)  & 0xFF00) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[2] << 8)  & 0xFF0000) |
                                                      ((efuse_sel->macro.m2[i].rx1.iqmismatch[2] << 24) & 0xFF000000);

            efuse_sel->macro.m2[i].rx1.im2 = ((efuse_sel->macro.m2[i].rx1.im2 >> 24) & 0xFF) |
                                            ((efuse_sel->macro.m2[i].rx1.im2 >> 8)  & 0xFF00) |
                                            ((efuse_sel->macro.m2[i].rx1.im2 << 8)  & 0xFF0000) |
                                            ((efuse_sel->macro.m2[i].rx1.im2 << 24) & 0xFF000000);
#endif

            SPI_Write(SEL6302, 0x3, 0x4AC, efuse_sel->macro.m2[i].rx1.im2);
            SPI_Write(SEL6302, 0x3, 0x990, efuse_sel->macro.m2[i].rx1.iqmismatch[0]);
            SPI_Write(SEL6302, 0x3, 0x994, efuse_sel->macro.m2[i].rx1.iqmismatch[1]);
            SPI_Write(SEL6302, 0x3, 0x998, efuse_sel->macro.m2[i].rx1.iqmismatch[2]);
            SPI_Write(SEL6302, 0x3, 0x980, 0x0000000C);

            LOGI("iqmismatch[0]=%lx", efuse_sel->macro.m2[i].rx1.iqmismatch[0]);
            LOGI("iqmismatch[1]=%lx", efuse_sel->macro.m2[i].rx1.iqmismatch[1]);
            LOGI("iqmismatch[2]=%lx", efuse_sel->macro.m2[i].rx1.iqmismatch[2]);
            LOGI("im2=%lx", efuse_sel->macro.m2[i].rx1.im2);
        }

        if(efuse_sel->macro.m2[i].rx2.freq_start>=5000 && efuse_sel->macro.m2[i].rx2.freq_stop<=6000)
        {
#if(0)
            efuse_sel->macro.m2[i].rx2.iqmismatch[0] = ((efuse_sel->macro.m2[i].rx2.iqmismatch[0] >> 24) & 0xFF) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[0] >> 8)  & 0xFF00) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[0] << 8)  & 0xFF0000) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[0] << 24) & 0xFF000000);

            efuse_sel->macro.m2[i].rx2.iqmismatch[1] = ((efuse_sel->macro.m2[i].rx2.iqmismatch[1] >> 24) & 0xFF) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[1] >> 8)  & 0xFF00) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[1] << 8)  & 0xFF0000) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[1] << 24) & 0xFF000000);

            efuse_sel->macro.m2[i].rx2.iqmismatch[2] = ((efuse_sel->macro.m2[i].rx2.iqmismatch[2] >> 24) & 0xFF) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[2] >> 8)  & 0xFF00) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[2] << 8)  & 0xFF0000) |
                                                      ((efuse_sel->macro.m2[i].rx2.iqmismatch[2] << 24) & 0xFF000000);

            efuse_sel->macro.m2[i].rx2.im2 = ((efuse_sel->macro.m2[i].rx2.im2 >> 24) & 0xFF) |
                                            ((efuse_sel->macro.m2[i].rx2.im2 >> 8)  & 0xFF00) |
                                            ((efuse_sel->macro.m2[i].rx2.im2 << 8)  & 0xFF0000) |
                                            ((efuse_sel->macro.m2[i].rx2.im2 << 24) & 0xFF000000);
#endif

            SPI_Write(SEL6302, 0x3, 0x4EC, efuse_sel->macro.m2[i].rx2.im2);
            SPI_Write(SEL6302, 0x3, 0xB90, efuse_sel->macro.m2[i].rx2.iqmismatch[0]);
            SPI_Write(SEL6302, 0x3, 0xB94, efuse_sel->macro.m2[i].rx2.iqmismatch[1]);
            SPI_Write(SEL6302, 0x3, 0xB98, efuse_sel->macro.m2[i].rx2.iqmismatch[2]);
            SPI_Write(SEL6302, 0x3, 0xB80, 0x0000000C);

            LOGI("iqmismatch[0]=%lx", efuse_sel->macro.m2[i].rx2.iqmismatch[0]);
            LOGI("iqmismatch[1]=%lx", efuse_sel->macro.m2[i].rx2.iqmismatch[1]);
            LOGI("iqmismatch[2]=%lx", efuse_sel->macro.m2[i].rx2.iqmismatch[2]);
            LOGI("im2=%lx", efuse_sel->macro.m2[i].rx2.im2);
        }
	}
}

void DM6302_DCOC(uint8_t SEL6302)
{
    uint32_t r0,r1, rdat_sel;
    uint32_t dat;

    //---------------- rx1_dcoc ----------------
	//write(0x201194, 0x1FFFF);         // clk enable
    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000019);
    SPI_Write(SEL6302, 0x3, 0x194, 0x0001FFFF);
	//write(0x200494, 0x66427C0);       // lna disable
    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000018);
    SPI_Write(SEL6302, 0x3, 0x494, 0x066727C0); //0x066427C0
    //write(0x200540, 0);               // clr dcoc i
    SPI_Write(SEL6302, 0x3, 0x540, 0x00000000);
    //write(0x200544, 0);               // clr dcoc q
    SPI_Write(SEL6302, 0x3, 0x544, 0x00000000);
    //write(0x200550, 0x7F);            // set dcoc wait time
    SPI_Write(SEL6302, 0x3, 0x550, 0x0000007F);
    //write(0x200554, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000000);

    //write(0x20054C, 1);               // i path tia
    SPI_Write(SEL6302, 0x3, 0x54C, 0x00000001);
    //write(0x200500, 0x1FE5FF49);      // set[28] [22] [21]
    SPI_Write(SEL6302, 0x3, 0x500, 0x1FE5FF49);
    //write(0x200554, 1);               // dcoc start
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000001);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200554, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000000);
    //dat = read(0x200558) & 0xFFFF;   // low 16bit
    SPI_Read(0x3, 0x558, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    dat = rdat_sel & 0xFFFF;
    //write(0x200540, dat);
    SPI_Write(SEL6302, 0x3, 0x540, dat);

    //write(0x20054C, 2);               // i path pab
    SPI_Write(SEL6302, 0x3, 0x54C, 0x00000002);
    //write(0x200500, 0x2FA5FF49);      // set[29] [21]
    SPI_Write(SEL6302, 0x3, 0x500, 0x2FA5FF49);
    //write(0x200554, 0x11);            // dcoc start
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000011);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200554, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000000);
    //dat |= read(0x200558)<<16;       // high 16bit
    SPI_Read(0x3, 0x558, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    rdat_sel = rdat_sel << 16;
    dat = dat | rdat_sel;
    //write(0x200540, dat);
    SPI_Write(SEL6302, 0x3, 0x540, dat);

    //write(0x20054C, 4);               // q path tia
    SPI_Write(SEL6302, 0x3, 0x54C, 0x00000004);
    //write(0x200500, 0x4F9DFF49);      // set[30] [20] [19]
    SPI_Write(SEL6302, 0x3, 0x500, 0x4F9DFF49);
    //write(0x200554, 0x21);            // dcoc start
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000021);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200554, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000000);
    //dat = read(0x200558) & 0xFFFF;   // low 16bit
    SPI_Read(0x3, 0x558, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    dat = rdat_sel & 0xFFFF;
    //write(0x200544, dat);
    SPI_Write(SEL6302, 0x3, 0x544, dat);

    //write(0x20054C, 8);               // q path pab
    SPI_Write(SEL6302, 0x3, 0x54C, 0x00000008);
    //write(0x200500, 0x8F8DFF49);      // set[30] [19]
    SPI_Write(SEL6302, 0x3, 0x500, 0x8F8DFF49);
    //write(0x200554, 0x31);            // dcoc start
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000031);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200554, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x554, 0x00000000);
    //dat |= read(0x200558)<<16;       // high 16bit
    SPI_Read(0x3, 0x558, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    rdat_sel = rdat_sel << 16;
    dat = dat | rdat_sel;
    //write(0x200544, temp);
    SPI_Write(SEL6302, 0x3, 0x544, dat);

    //write(0x20054C, 0);
    SPI_Write(SEL6302, 0x3, 0x54C, 0x00000000);
    //write(0x200500, 0xF85FFC9);       // dcoc disable
    SPI_Write(SEL6302, 0x3, 0x500, 0xF85FFC9);
	//write(0x200494, 0x66427C3);       // lna enable
    SPI_Write(SEL6302, 0x3, 0x494, 0x066727CC); //0x66427C3

    WAIT(10);

    //---------------- rx2_dcoc ----------------
	//write(0x201194, 0x1FFFF);         // clk enable
    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000019);
    SPI_Write(SEL6302, 0x3, 0x194, 0x0001FFFF);
	//write(0x2004D4, 0x66427C0);       // lna disable
    SPI_Write(SEL6302, 0x6, 0xFF0, 0x00000018);
    SPI_Write(SEL6302, 0x3, 0x4D4, 0x066727C0);//0x066427C0
    //write(0x200640, 0);               // clr dcoc i
    SPI_Write(SEL6302, 0x3, 0x640, 0x00000000);
    //write(0x200644, 0);               // clr dcoc q
    SPI_Write(SEL6302, 0x3, 0x644, 0x00000000);
    //write(0x200650, 0x7F);            // set dcoc wait time
    SPI_Write(SEL6302, 0x3, 0x650, 0x0000007F);
    //write(0x200654, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000000);

    //write(0x20064C, 1);               // i path tia
    SPI_Write(SEL6302, 0x3, 0x64C, 0x00000001);
    //write(0x200600, 0x1FE5FF49);      // set[28] [22] [21]
    SPI_Write(SEL6302, 0x3, 0x600, 0x1FE5FF49);
    //write(0x200654, 1);               // dcoc start
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000001);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200654, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000000);
    //temp = read(0x200658) & 0xFFFF;   // low 16bit
    SPI_Read(0x3, 0x658, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    dat = rdat_sel & 0xFFFF;
    //write(0x200640, dat);
    SPI_Write(SEL6302, 0x3, 0x640, dat);

    //write(0x20064C, 2);               // i path pab
    SPI_Write(SEL6302, 0x3, 0x64C, 0x00000002);
    //write(0x200600, 0x2FA5FF49);      // set[29] [21]
    SPI_Write(SEL6302, 0x3, 0x600, 0x2FA5FF49);
    //write(0x200654, 0x11);            // dcoc start
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000011);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200654, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000000);
    //dat |= read(0x200658)<<16;       // high 16bit
    SPI_Read(0x3, 0x658, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    rdat_sel = rdat_sel << 16;
    dat = dat | rdat_sel;
    //write(0x200640, dat);
    SPI_Write(SEL6302, 0x3, 0x640, dat);

    //write(0x20064C, 4);               // q path tia
    SPI_Write(SEL6302, 0x3, 0x64C, 0x00000004);
    //write(0x200600, 0x4F9DFF49);      // set[30] [20] [19]
    SPI_Write(SEL6302, 0x3, 0x600, 0x4F9DFF49);
    //write(0x200654, 0x21);            // dcoc start
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000021);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200654, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000000);
    //dat = read(0x200658) & 0xFFFF;   // low 16bit
    SPI_Read(0x3, 0x658, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    dat = rdat_sel & 0xFFFF;
    //write(0x200644, dat);
    SPI_Write(SEL6302, 0x3, 0x644, dat);

    //write(0x20064C, 8);               // q path pab
    SPI_Write(SEL6302, 0x3, 0x64C, 0x00000008);
    //write(0x200600, 0x8F8DFF49);      // set[30] [19]
    SPI_Write(SEL6302, 0x3, 0x600, 0x8F8DFF49);
    //write(0x200654, 0x31);            // dcoc start
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000031);
    //delay(500us);                     // wait dcoc finish
    WAIT(1);
    //write(0x200654, 0);               // dcoc stop
    SPI_Write(SEL6302, 0x3, 0x654, 0x00000000);
    //dat |= read(0x200658)<<16;       // high 16bit
    SPI_Read(0x3, 0x658, &r0, &r1);
    rdat_sel = (SEL6302 == 1) ? r0 : r1;
    rdat_sel = rdat_sel << 16;
    dat = dat | rdat_sel;
    //write(0x200644, temp);
    SPI_Write(SEL6302, 0x3, 0x644, dat);

    //write(0x20064C, 0);
    SPI_Write(SEL6302, 0x3, 0x64C, 0x00000000);
    //write(0x200600, 0xF85FFC9);       // dcoc disable
    SPI_Write(SEL6302, 0x3, 0x600, 0xF85FFC9);
	//write(0x2004D4, 0x66427C3);       // lna enable
    SPI_Write(SEL6302, 0x3, 0x4D4, 0x066727CC);//0x066427CC
}

// DM6302��ʼ��
int DM6302_init(uint8_t freq)
{
    int to_cnt = 0;
    uint32_t r0 = 1, r1 = 1;

    while(r0){
        DM5680_ResetRF(0);
        usleep(1000);
        DM5680_ResetRF(1);
        usleep(100000);

        DM6302_Init0(0);
        LOGI("Init0 done");

        SPI_Read(0x6, 0xFF0, &r0, &r1);
        if((r0 != 0x18) || (r1 != 0x18))
            r0 = 1;
        else
            r0 = 0;

        to_cnt++;
        if(to_cnt >= 10){
            LOGE("Error: DM6302s have no response.");
            return 1;
        }
    }

    DM6302_EFUSE1(1);
    LOGI("EFUSE1 1 done");

    DM6302_EFUSE1(2);
    LOGI("EFUSE1 2 done");

    DM6302_Init1(0);
    LOGI("Init1 done");

    DM6302_Init2(0,freq);
    LOGI("Init2 done");

    DM6302_Init3(0);
    LOGI("Init3 done");

    DM6302_Init4(0);
    LOGI("Init4 done");

    DM6302_Init5(0);
    LOGI("Init5 done");

    DM6302_Init6(0);
    LOGI("Init6 done");

    DM6302_Init7(0);
    LOGI("Init7 done");

    DM6302_Init8(0);
    LOGI("Init8 done");

    DM6302_Init9(0);
    LOGI("Init9 done");

    DM6302_Init10(0);
    LOGI("Init10 done");

    DM6302_Init11(0);
    LOGI("Init11 done");

    DM6302_Init12(0);
    LOGI("Init12 done");

    DM6302_Init13(0);
    LOGI("Init13 done");

    DM6302_Init14(0);
    LOGI("Init14 done");

    DM6302_EFUSE2(1);
    LOGI("EFUSE2 1 done");

    DM6302_EFUSE2(2);
    LOGI("EFUSE2 2 done");

    DM6302_DCOC(1);
    LOGI("DCOC 1 done");

    DM6302_DCOC(2);
    LOGI("DCOC 2 done");

    DM6302_M0();
    LOGI("M0 done");

    return 0;
}


// DM6302 Gain table
// ����ֵ��Χ: 0~60
uint8_t DM6302_gain_tab(uint32_t d)
{
    uint8_t i = 0;
    uint16_t tab[61] = {
        0x1C ,0x3C ,0x5C ,0x7C ,0x9C ,0xBC ,0x11C,0x13C,
        0x15C,0x17C,0x19C,0x1BC,0x1DC,0x23C,0x25C,0x27C,
        0x29C,0x2BC,0x2DC,0x33C,0x35C,0x37C,0x39C,0x3BC,
        0x3DC,0x334,0x354,0x374,0x394,0x3B4,0x3D4,0x32C,
        0x34C,0x36C,0x38C,0x3AC,0x3CC,0x324,0x344,0x364,
        0x384,0x3A4,0x3C4,0x383,0x3A3,0x3C3,0x382,0x3A2,
        0x3C2,0x321,0x341,0x361,0x381,0x3A1,0x3C1,0x320,
        0x340,0x360,0x380,0x3A0,0x3C0
    };

    for(i=0; i<61; i++){
        if((d & 0x3FF) == tab[i])
            break;
    }

    return i;
}

// ��ȡDM6302��������ȼ�(0~60)
void DM6302_get_gain(uint8_t* gain)
{
    uint32_t r0,r1;

    SPI_Write(0, 0x6, 0x7FC, 0x00000000);
    
    SPI_Write(0, 0x6, 0xFF0, 0x00000018);
    SPI_Read(0x3, 0x9A8, &r0, &r1);
    gain[0] = DM6302_gain_tab(r0); // 0~60
    gain[1] = DM6302_gain_tab(r1); // 0~60
    SPI_Read(0x3, 0xBA8, &r0, &r1);
    gain[2] = DM6302_gain_tab(r0); // 0~60
    gain[3] = DM6302_gain_tab(r1); // 0~60
}

// ������DM6302��MCU
// open: 0=����1=��
// ע�⣺�����κ�SPI_Read����ʱ��DM6302��MCU���봦�ڹ���״̬��
void DM6302_openM0(uint32_t open)
{
    SPI_Write(0, 0x6, 0x7FC, open);
}
