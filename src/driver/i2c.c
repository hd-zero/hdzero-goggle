#include "i2c.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h> // for close
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <log/log.h>

#include "../core/common.hh"

pthread_mutex_t i2c_mutex;

void iic_init()
{
    pthread_mutex_init(&i2c_mutex, NULL);
}


static char* iic_ports[IIC_PORTS] = {
    "/dev/i2c-0",
    "/dev/i2c-1",
    "/dev/i2c-2",
    "/dev/i2c-3",
};

static int iic_open(char* port)
{
    //int i2c_fd = open ("/dev/i2c-0", O_RDONLY);
    int i2c_fd = open (port, O_RDONLY);
    if(i2c_fd <0) {
        LOGE("iic_open %s failed [1=Right,2=Main,3=Left]",port);
    }
    return i2c_fd;
}

static void iic_close(int i2c_fd)
{
    close(i2c_fd);
}

static uint8_t iic_read(int i2c_fd, uint8_t slave_address, uint16_t reg_address)
{
    struct i2c_rdwr_ioctl_data work_queue;
    struct i2c_msg msgs[2];

    uint8_t val = 0;
    int ret;

    work_queue.nmsgs = 2;
    work_queue.msgs = msgs;

    val =(unsigned char)reg_address;
    (work_queue.msgs[0]).len = 1;
    (work_queue.msgs[0]).flags = 0;
    (work_queue.msgs[0]).addr = slave_address;
    (work_queue.msgs[0]).buf = &val;

    (work_queue.msgs[1]).len = 1;
    (work_queue.msgs[1]).flags = 1;
    (work_queue.msgs[1]).addr = slave_address;
    (work_queue.msgs[1]).buf = &val;


    ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long) &work_queue);
    if(ret < 0) {
        //LOGI("iic_read[%x.%x] failed.",slave_address, reg_address);
        val = 0;
    }
    return val;
}

static void iic_read_n(int i2c_fd, uint8_t slave_address, uint16_t reg_address, uint8_t* reg_data, uint16_t len)
{
    struct i2c_rdwr_ioctl_data work_queue;
    struct i2c_msg msgs[2];
    uint8_t val = 0;

    work_queue.nmsgs = 2;
    work_queue.msgs = msgs;

    val =(unsigned char)reg_address;
    (work_queue.msgs[0]).len = 1;
    (work_queue.msgs[0]).flags = 0;
    (work_queue.msgs[0]).addr = slave_address;
    (work_queue.msgs[0]).buf = &val;

    (work_queue.msgs[1]).len = len;
    (work_queue.msgs[1]).flags = 1;
    (work_queue.msgs[1]).addr = slave_address;
    (work_queue.msgs[1]).buf = reg_data;


    ioctl(i2c_fd, I2C_RDWR, (unsigned long) &work_queue);
    //if(ret < 0) 
    //    LOGI("iic_read_n[%x.%x] failed.",slave_address, reg_address);

}
/*
static void iic_read_n(int i2c_fd, uint8_t slave_address, uint16_t reg_address, uint8_t* reg_data, uint16_t len)
{
    struct i2c_rdwr_ioctl_data work_queue;
    uint8_t val = 0;
    int ret;

    work_queue.nmsgs = 2;
    work_queue.msgs = (struct i2c_msg *)malloc(work_queue.nmsgs *sizeof(struct i2c_msg));

    if(!work_queue.msgs)
    {
        LOGE("Memery alloc error");
        return ;
    }

    val =(unsigned char)reg_address;

    (work_queue.msgs[0]).len = 1;
    (work_queue.msgs[0]).flags = 0;
    (work_queue.msgs[0]).addr = slave_address;
    (work_queue.msgs[0]).buf = &val;

    (work_queue.msgs[1]).len = len;
    (work_queue.msgs[1]).flags = 1;
    (work_queue.msgs[1]).addr = slave_address;
    (work_queue.msgs[1]).buf = reg_data;


    ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long) &work_queue);
    if(ret < 0)
    {
        LOGE("Error during I2C_RDWR ioctl with error code: %d", ret);
        val = 0;
    }

    free(work_queue.msgs);
}
*/

static int iic_write(int i2c_fd, uint8_t slave_address, uint16_t reg_address , uint16_t reg_val)
{
    struct i2c_rdwr_ioctl_data work_queue;
    struct i2c_msg msgs;
    uint8_t obuf[2];
    int ret;

    work_queue.nmsgs = 1;
    work_queue.msgs = &msgs;
    msgs.buf = obuf;

    (work_queue.msgs[0]).len = 2;
    (work_queue.msgs[0]).flags = 0;
    (work_queue.msgs[0]).addr = slave_address;
    (work_queue.msgs[0]).buf[0] = reg_address;
    (work_queue.msgs[0]).buf[1] = reg_val;


    ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long) &work_queue);
    if(ret < 0) {
        //LOGI("iic_write[%x.%x]<-%x failed.",slave_address, reg_address, reg_val);
        ret = 0;
    }
    return ret;
}

static int iic_write_n(int i2c_fd, uint8_t slave_address, uint8_t reg_address , uint8_t* reg_val, uint16_t len)
{
    struct i2c_rdwr_ioctl_data work_queue;
    struct i2c_msg msgs;
    int ret;
    uint16_t i;

    work_queue.nmsgs = 1;
    work_queue.msgs = &msgs;

    if ((work_queue.msgs[0].buf = (unsigned char *)malloc((len+1) * sizeof(unsigned char))) == NULL)
    {
        //LOGI("buf memery alloc error...");
        return -1;
    }

    (work_queue.msgs[0]).len = len+1;
    (work_queue.msgs[0]).flags = 0;
    (work_queue.msgs[0]).addr = slave_address;
    (work_queue.msgs[0]).buf[0] = reg_address;
    
    for(i=1;i<=len;i++)
        (work_queue.msgs[0]).buf[i]= reg_val[i-1];

    ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long) &work_queue);
    if(ret < 0) {
        //LOGI("iic_write_n[%x.%x] failed.",slave_address, reg_address);
        ret = 0;
    }
    free(work_queue.msgs[0].buf);
    return ret;
}
/*
static int iic_write_n(int i2c_fd, uint8_t slave_address, uint8_t reg_address , uint8_t* reg_val, uint16_t len)
{
    struct i2c_rdwr_ioctl_data work_queue;
    int ret;
    uint16_t i;

    work_queue.nmsgs = 2;
    work_queue.msgs = (struct i2c_msg *)malloc(work_queue.nmsgs * sizeof(struct i2c_msg));
    if(!work_queue.msgs)
    {
        LOGE("msgs memery alloc error");
        return -1;
    }
    if ((work_queue.msgs[0].buf = (unsigned char *)malloc((len+1) * sizeof(unsigned char))) == NULL)
    {
        LOGE("buf memery alloc error...");
        free(work_queue.msgs);
        return -1;
    }

    work_queue.nmsgs = 1;
    (work_queue.msgs[0]).len = len+1;
    (work_queue.msgs[0]).flags = 0;
    (work_queue.msgs[0]).addr = slave_address;
    (work_queue.msgs[0]).buf[0] = reg_address;
    
    for(i=1;i<=len;i++)
        (work_queue.msgs[0]).buf[i]= reg_val[i-1];

    ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long) &work_queue);
    if(ret < 0)
    {
        LOGE("Error during I2C_RDWR ioctl with error code: %d", ret);
    }
    free(work_queue.msgs[0].buf);
    free(work_queue.msgs);

    return ret;
}
*/

uint8_t i2c_read(int port, uint8_t slave_address, uint8_t addr)
{
    int fd;
    uint8_t val = 0;
    if(port >= IIC_PORTS) 
        return 0;

    pthread_mutex_lock(&i2c_mutex);
    fd = iic_open(iic_ports[port]);
    if(fd >= 0) {
        val = iic_read(fd, slave_address, addr);
        iic_close(fd);
    }
    pthread_mutex_unlock(&i2c_mutex);
    return val;
}


int8_t i2c_read_n(int port, uint8_t slave_address, uint8_t addr, uint8_t * data, uint16_t len)
{
    int8_t ret = -2;
    if(port >= IIC_PORTS)
        return -1;

    pthread_mutex_lock(&i2c_mutex);
    int fd = iic_open(iic_ports[port]);
    if(fd >= 0) {
        iic_read_n(fd, slave_address, addr,data,len);
        iic_close(fd);
        ret = 0;
    }
    pthread_mutex_unlock(&i2c_mutex);
    return ret;
}

/*
int8_t i2c_read_n(int port, uint8_t slave_address, uint8_t addr, uint8_t * data, uint16_t len)
{
    pthread_mutex_lock(&i2c_mutex);
    if(port >= IIC_PORTS)
    {
        return -1;
    }

    int fd = iic_open(iic_ports[port]);
    if(fd < 0)
    {
        return -2;
    }

    iic_read_n(fd, slave_address, addr,data,len);

    iic_close(fd);
    pthread_mutex_unlock(&i2c_mutex);
    return 0;
}*/

int i2c_write(int port, uint8_t slave_address, uint8_t addr, uint8_t val)
{
    int ret = -1;
    if(port >= IIC_PORTS)
        return -1;

    pthread_mutex_lock(&i2c_mutex);
    int fd = iic_open(iic_ports[port]);
    if(fd >= 0) {
        ret = iic_write(fd, slave_address, addr, val);
        iic_close(fd);
    }
    pthread_mutex_unlock(&i2c_mutex);
    return ret;
}


int8_t i2c_write_n(int port, uint8_t slave_address, uint8_t addr, uint8_t* val, uint16_t len)
{
    int ret = -2;
    if(port >= IIC_PORTS)
        return -1;

    pthread_mutex_lock(&i2c_mutex);
    int fd = iic_open(iic_ports[port]);
    if(fd >= 0) {
        iic_write_n(fd, slave_address, addr, val, len);
        iic_close(fd);
        ret = 0;
    }
    pthread_mutex_unlock(&i2c_mutex);
    return ret;
}
/*
int8_t i2c_write_n(int port, uint8_t slave_address, uint8_t addr, uint8_t* val, uint16_t len)
{
    pthread_mutex_lock(&i2c_mutex);
    if(port >= IIC_PORTS)
    {
        return -1;
    }

    int fd = iic_open(iic_ports[port]);
    if(fd < 0)
    {
        return -2;
    }

    int ret = iic_write_n(fd, slave_address, addr, val, len);

    iic_close(fd);
    pthread_mutex_unlock(&i2c_mutex);
    return 0;
}
*/