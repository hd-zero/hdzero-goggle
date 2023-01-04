#include "self_test.h"

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <log/log.h>

#include "../driver/dm5680.h"
#include "../driver/TP2825.h"
#include "../driver/i2c.h"
#include "../driver/hardware.h"
#include "../driver/it66121.h"
#include "../driver/it66021.h"
#include "../driver/dm6302.h"
#include "ui/page_common.h"
#include "common.hh"


///////////////////////////////////////////////////////////////////////////////
// log
FILE* log_file;

#define UART_WAIT (100*1000)

void self_test()
{
    int i;
    uint32_t dat0,dat1;
    char* msg[2] = {"[Error]", "[Pass] "}; 
    
    if(!g_test_en) return;
    
    LOGI("==== Self Test ======================");
    //1. Read FPGA
     i = I2C_Read(ADDR_FPGA,  0xFF);
     LOGI("%sFPGA ver = %d ",msg[i!=0],i);
    
    //2. Check UART connection between V536 and DM5680 on left (UART2) and Right (UART1) 
    rx_status[0].rx_ver = 0;
    rx_status[1].rx_ver = 0;
    DM5680_req_ver();
    usleep(UART_WAIT);
    LOGI("%sUART1 communicate left  DM5680 ver=%x",msg[rx_status[0].rx_ver != 0],rx_status[0].rx_ver);
    LOGI("%sUART2 communicate right DM5680 ver=%x",msg[rx_status[1].rx_ver != 0],rx_status[1].rx_ver);

    //3. IT66121 (Right) HDMI_TX
    DM5680_ResetHDMI_TX(0);
    DM5680_ResetHDMI_TX(1);
    usleep(UART_WAIT);
    i = I2C_R_Read(ADDR_IT66121, 0x00); //Vender ID
    LOGI("%sIT66121(R, HDMI_TX) Vender ID = 0x%x ",msg[(i==0x54)],i);
    DM5680_ResetHDMI_TX(0);

    //4. IT66021 (Left)
    DM5680_ResetHDMI_RX(0);
    DM5680_ResetHDMI_RX(1);
    usleep(UART_WAIT);
    i = I2C_L_Read(ADDR_IT66021, 0x00); //Vender ID
    LOGI("%sIT66021(L, HDMI_RX) Vender ID = 0x%x ",msg[i==0x54],i);
    DM5680_ResetHDMI_RX(0);


    //5. AL FPGA
    i = I2C_Read(ADDR_AL, 0xFF);
    LOGI("%sAL ver = 0x%x ",msg[1],i);

    //6. TP2825
    TP2825_open();
    i = I2C_Read(ADDR_TP2825, 0x00);
    LOGI("%sTP2825 ver = 0x%x ",msg[i==0x11],i);
    TP2825_close();

    //7. DM6302s
    DM5680_ResetRF(0);
    DM5680_ResetRF(1);
    usleep(UART_WAIT);
    DM6302_Init0(0);
    SPI_Read (0x6, 0xFF0, &dat0, &dat1);
    LOGI("%sDM6302 (Left)  = 0x%x ",msg[(dat0==0x18)],dat0);
    LOGI("%sDM6302 (Right) = 0x%x ",msg[(dat1==0x18)],dat1);
    DM5680_ResetRF(0);

    //8. HAN Status
    i =  Get_HAN_status() & 1;
    LOGI("%sHAN Status. ",msg[i]);

    LOGI("==== Log  ======================");
}