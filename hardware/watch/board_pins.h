/*
 * board_pins.h — Lyra 手表板引脚图（建议方案 / 待确认）
 *
 * 用法：把本文件放到 app/inc/，并在包含任何驱动头之前引入，例如在
 *       app/inc/conf.h 顶部加一行 #include "board_pins.h"。
 *       所有驱动里的引脚宏都写成 #ifndef / #define / #endif 形式，
 *       先定义者胜，所以驱动头文件一行都不用改 —— 开发板和手表板
 *       可以共用同一套驱动源码，靠是否引入本文件来切换。
 *
 * ⚠ 未确认项：下列引脚是按「优先硬件 I2C 物理脚 + 优先低位 + 三条总线分三个端口」
 *   的规则推出来的，尚未对照 K64 100-LQFP 引脚表核实。
 *   需确认 PTA11 / PTA12 / PTC10 / PTC11 在 100 脚封装上存在。
 *   全部是纯 GPIO，改引脚只需改本文件。
 *
 * 设计意图：三条位带 I2C 分别落在 I2C0 / I2C1 / I2C2 的物理引脚上。
 *   现在仍跑位带（三条独立总线避免争用：OLED 整屏 1024 字节在 400 kHz
 *   硬件 I2C 上约 23 ms，与 100 Hz 的两个传感器读取会冲突），
 *   但将来任何一条想改成硬件外设，改宏即可，板子不用重打。
 */

#ifndef __BOARD_PINS_H__
#define __BOARD_PINS_H__

/* ------------------------------------------------------------------
 * SSD1306 OLED 128x64  @0x3C   —  PTB0 / PTB1
 * PTB0 / PTB1 同时是 I2C0_SCL / I2C0_SDA 的物理引脚
 * （见 chip/inc/port_cfg.h:92-93 的备选列表）
 * 覆盖 dev/inc/oled_i2c.h:24-61 的 10 个宏
 * ------------------------------------------------------------------ */
#define OLED_I2C_PORT_INIT_SCL GPIO_Init(PTB0, GPO, 0)
#define OLED_I2C_PORT_INIT_SDA GPIO_Init(PTB1, GPO, 0)
#define OLED_I2C_DDR_OUT_SCL   GPIO_DDRX(PTB0, 1)
#define OLED_I2C_DDR_OUT_SDA   GPIO_DDRX(PTB1, 1)
#define OLED_I2C_DDR_IN_SDA    GPIO_DDRX(PTB1, 0)
#define OLED_I2C_SET_SCL_H     GPIO_SET(PTB0, 1)
#define OLED_I2C_SET_SCL_L     GPIO_SET(PTB0, 0)
#define OLED_I2C_SET_SDA_H     GPIO_SET(PTB1, 1)
#define OLED_I2C_SET_SDA_L     GPIO_SET(PTB1, 0)
#define OLED_I2C_GET_SDA_IN    GPIO_GET(PTB1)

/* ------------------------------------------------------------------
 * MPU6050 6 轴 IMU  @0x68   —  PTA12 / PTA11
 * 即 I2C2_SCL / I2C2_SDA 的物理引脚（port_cfg.h:100-101）
 * 覆盖 dev/inc/mpu6050.h:61-89 的 10 个宏
 * 注意这组宏名没有器件前缀，是历史命名
 * ------------------------------------------------------------------ */
#define I2C_PORT_INIT_SCL GPIO_Init(PTA12, GPO, 0)
#define I2C_PORT_INIT_SDA GPIO_Init(PTA11, GPO, 0)
#define I2C_DDR_OUT_SCL   GPIO_DDRX(PTA12, 1)
#define I2C_DDR_OUT_SDA   GPIO_DDRX(PTA11, 1)
#define I2C_DDR_IN_SDA    GPIO_DDRX(PTA11, 0)
#define I2C_SET_SCL_H     GPIO_SET(PTA12, 1)
#define I2C_SET_SCL_L     GPIO_SET(PTA12, 0)
#define I2C_SET_SDA_H     GPIO_SET(PTA11, 1)
#define I2C_SET_SDA_L     GPIO_SET(PTA11, 0)
#define I2C_GET_SDA_IN    GPIO_GET(PTA11)

/* ------------------------------------------------------------------
 * MAX30102 心率 / 血氧  @0x57   —  PTC10 / PTC11
 * 即 I2C1_SCL / I2C1_SDA 的物理引脚（port_cfg.h:96-97）
 * 覆盖 dev/inc/max30102.h:47-75 的 10 个宏
 * ------------------------------------------------------------------ */
#define MAX30102_I2C_PORT_INIT_SCL GPIO_Init(PTC10, GPO, 0)
#define MAX30102_I2C_PORT_INIT_SDA GPIO_Init(PTC11, GPO, 0)
#define MAX30102_I2C_DDR_OUT_SCL   GPIO_DDRX(PTC10, 1)
#define MAX30102_I2C_DDR_OUT_SDA   GPIO_DDRX(PTC11, 1)
#define MAX30102_I2C_DDR_IN_SDA    GPIO_DDRX(PTC11, 0)
#define MAX30102_I2C_SET_SCL_H     GPIO_SET(PTC10, 1)
#define MAX30102_I2C_SET_SCL_L     GPIO_SET(PTC10, 0)
#define MAX30102_I2C_SET_SDA_H     GPIO_SET(PTC11, 1)
#define MAX30102_I2C_SET_SDA_L     GPIO_SET(PTC11, 0)
#define MAX30102_I2C_GET_SDA_IN    GPIO_GET(PTC11)

/* ------------------------------------------------------------------
 * MAX30102 旁路信号   —  INT = PTB2，LED 驱动 = PTB3
 * 覆盖 app/inc/health.h:26-42 的 5 个宏
 *
 * 顺带修掉原固件的 PTA7 冲突：原来 IRD 是 PTA7 的 GPIO，
 * 而 RD 又定义成 FTM0_CH4（物理上也是 PTA7），
 * health.c:55-57 先初始化 PWM 再初始化 GPIO，GPIO 覆盖了 PWM，
 * 所以 RD_Duty 从来没起作用。新板只有一条网络，
 * 把 RD 的初始化定义成空，去掉这个误导性的 FTM 调用。
 * ------------------------------------------------------------------ */
#define MAX30102_PORT_INIT_INT GPIO_Init(PTB2, GPI, 0)
#define MAX30102_PORT_INIT_IRD GPIO_Init(PTB3, GPO, 0)
#define MAX30102_PORT_INIT_RD  /* 空：新板无独立 RD 网络，不再初始化 FTM0_CH4 */
#define MAX30102_SET_IRD_H     GPIO_SET(PTB3, 1)
#define MAX30102_SET_IRD_L     GPIO_SET(PTB3, 0)
#define MAX30102_GET_INT_IN    GPIO_GET(PTB2)

/* ------------------------------------------------------------------
 * 振动马达（决策 4，替代原有源蜂鸣器）  —  PTC4
 * 覆盖 app/inc/beep.h:23-32 的 3 个宏
 * beep.c 是纯 GPIO 电平翻转，不是 PWM，所以换马达固件逻辑零改动，
 * 只需在硬件上换驱动级：低边 N-MOSFET + 续流二极管 + 就近储能电容
 * ------------------------------------------------------------------ */
#define BEEP_PORT_INIT GPIO_Init(PTC4, GPO, 0)
#define BEEP_SET_H     GPIO_SET(PTC4, 1)
#define BEEP_SET_L     GPIO_SET(PTC4, 0)

#endif /* __BOARD_PINS_H__ */

/* ------------------------------------------------------------------
 * 本文件覆盖不到的两处，需要直接改源码：
 *
 * 1. 按键 ×4 —— board/src/maps_dock_key.c:43 是 C 数组而不是宏：
 *        PTXn Key_PTXn[KEY_MAX] = {PTB21, PTB22, PTB23, PTB20};
 *    改为：
 *        PTXn Key_PTXn[KEY_MAX] = {PTC0, PTC1, PTC2, PTC3};
 *    顺序对应 KEY0 进入/确认、KEY1 返回、KEY2 上一项/减、KEY3 下一项/加
 *
 * 2. UART4 调试测试点 —— chip/inc/port_cfg.h:43-44
 *    首选保持 PTC14 / PTC15；若 100-LQFP 上不可用，改成 PTE25 / PTE24。
 *    这是删掉 SPI2 之后唯一还受引脚复用约束的信号，且只是调试口，
 *    两组都不可用时不引出也能跑。
 * ------------------------------------------------------------------ */
