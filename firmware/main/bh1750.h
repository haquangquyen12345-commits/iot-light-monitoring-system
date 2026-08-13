#ifndef BH1750_H
#define BH1750_H

// Dùng thư viện I2C chuẩn mới của ESP-IDF v5.2+
#include "driver/i2c_master.h" 

#define BH1750_ADDR         0x23 //nối đất-> 0x23, nối lên vcc-> 0x5C
#define I2C_MASTER_SCL_IO   22     // định nghĩa chân cắm
#define I2C_MASTER_SDA_IO   21
#define I2C_MASTER_NUM      I2C_NUM_0   // có 2 bộ điều khiển
#define I2C_MASTER_FREQ_HZ  100000   
#define BH1750_CMD_POWER_DOWN   0x00
#define BH1750_CMD_POWER_ON     0x01
#define BH1750_CMD_RESET        0x07

#define BH1750_CMD_CONT_H_RES   0x10
#define BH1750_CMD_CONT_H_RES_2 0x11
#define BH1750_CMD_CONT_L_RES   0x13

#define BH1750_CMD_ONCE_H_RES   0x20
#define BH1750_CMD_ONCE_H_RES_2 0x21
#define BH1750_CMD_ONCE_L_RES   0x23

void bh1750_init(void);
float bh1750_read_lux(void);

#endif
