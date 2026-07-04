# Lyra 代码 Review & 优化 — 会话交接文档

> 用途：在新的 Kiro 会话中，先让 AI 读取此文件（"请阅读 .kiro/HANDOFF.md 恢复上下文"），
> 即可无缝接续之前的代码 review 与修复工作。

---

## 项目概况

- **项目**：Lyra —— 基于 MK64FX512VLQ12 / MK64FN1M0VLQ12 的多功能智能手表固件
- **结构**：`app/`（应用层）、`board/`（MAPS Dock 板级驱动）、`chip/`（K64 外设驱动）、
  `dev/`（MPU6050 / MAX30102 / OLED 传感器驱动）、`utils/`（动画等工具）
- **主循环**：`app/src/main.c`，10ms 周期读 MPU6050 + 滤波，100Hz 采样，PIT0/PIT1 中断驱动

---

## Git 状态与推送方式

- 分支：`main`，远端 `origin` = `https://github.com/gorgeousbubble/Lyra.git`
- **推送有障碍**：企业网络对 HTTPS 做 SSL 拦截（间歇性 `errno 10054`），SSH 无可用密钥。
  - **可用的推送命令**（重试几次通常能成功）：
    ```
    git -c http.sslVerify=false push origin main
    ```
  - 网络重置时多试几次；不要用后台复用的旧终端进程推（工作目录可能过期）。
- `.vscode/settings.json` 有本地改动，**不要提交**。
- 每次 commit 会有 "Committer: Yuxing He" 的 identity 警告，可忽略。

---

## 已完成的修复（按时间顺序，全部已 commit）

### 第一批 code review 修复（已推送）
1. RTC_Time_Now / Stop_Watch_Now 结构体非原子读写竞争 → 临界区保护
2. Flash 手动保存节流 + CRC-8 有效性校验（activity_history）
3. （同 2）Flash 有效性检测加校验
4. Kalman 初始化零值判断 bug → 加 `initialized` flag
5. Yaw 角无 wrap → 每步 Normalize_Angle
6. FreeFall `elapsed_ms` uint32 wrap 误触发 → 相对时长计数器
7. 9 文件重复的 framebuffer helper → 提取 `app/inc/framebuf.h` + `framebuf.c`
8. `alarm.c` 空实现 → 实现 Alarm_Tick/Dismiss/Is_Ringing + 接入主循环和按键
9. Health Score SpO2 90-94% 计算超上限 → 分段线性重写
10. 每帧 1KB 栈 framebuffer → 全局 `g_fb`
11. `init.c` 注释残留调试代码 → 清理
12. 睡眠 `session_start_s` 从未赋值 → Toggle/Clear 传 rtc 赋值
13. `watch.c` Calc_* malloc/free 像素链表 → 静态 scratch 数组

### 第二批性能/重构修复（部分已推送，最后 5 个可能待推送）
- **B** `MAPS_Dock_KEY_Delay` 100ms 阻塞 → 非阻塞 cooldown（`Key_Ms_Tick`）**[已推送]**
- **C** MPU6050 6 次独立 I2C → `MPU_Get_All` 单次 burst **[已推送 5f8002f]**
- **F** W25Q80 固定 2 秒 DELAY → 轮询 BUSY（`Wait_Busy`）**[已推送 b2821f4]**
- **D** MAX30102 word 读原子化 **[commit 7dca927]**
- **A** 菜单 3 个 16-case switch → 表驱动 menu_scroll_icon/menu_display_bmp **[commit 09443d3]**
- **G** 删除 animation 未用的 malloc 死代码 **[commit bb548db]**
- **I** SpO2/HR 算法节流到每秒一次 **[commit 20eb8f3]**
- **J** 计步器固定阈值 → 自适应包络阈值 **[commit 1d01933]**

> ⚠️ 确认最后 5 个 commit（7dca927 起）是否已推送：`git log --oneline origin/main..HEAD`
> 若非空则需 push。

### 第三批（新一轮 review，逐个修复）
- **L** ✅ `app/src/health_monitor.c` SpO2 无效分支 `snprintf(buf,...,"--%")` 格式串以
  单个 `%` 结尾（不完整转换说明符，UB）→ 改为 `"--%%"`，输出字面 "--%"（与有效分支
  `"%ld%%"` 一致）。
- **M** ✅ `app/src/watch.c` `Calc_World_Clock_Time` 世界时区 UTC 偏移显示：原用
  `char ch[5][2]` 单字符格子 + 固定 x（97+i*6），`snprintf(ch[4],2,"%d",offset)` 对两位数
  偏移（悉尼 +10）截断成 "1"，显示 "UTC+1"。→ 改为构造完整字符串 `"UTC%c%d"` 并按右边缘
  右对齐绘制（`utc_x = 127 - len*6`）：单位数时 x=97 与原来完全一致，两位数时左移一格
  （x=91）正确显示 "UTC+10"，不越屏。
- **N** ✅ MPU6050 热插拔支持：杜邦线运行中松动再插回后无法恢复读数。根因——(1) 掉线时
  只是 GPIO 位带 I2C（不会死锁），但 `MPU_Get_All` 的返回值被忽略，失败时旧值冻结；
  (2) 若 VCC/GND 也断，重连后 MPU6050 回到默认 SLEEP 态，而 `MPU6050_Init` 只在开机跑一次，
  无人再唤醒它；(3) 掉线可能把从机卡在半字节、总线被 SDA 拉低。
  修复：`dev/src/mpu6050.c` 新增 `MPU6050_Bus_Recovery`（放 SDA 为输入、拨 9 个 SCL 解卡 + STOP）、
  `MPU6050_Present`（读 WHO_AM_I==0x68）、`MPU6050_Recover`（解卡→确认在线→重新 Init 唤醒配置）。
  `app/src/main.c` 10ms 槽改为检查 `MPU_Get_All` 返回值：成功才喂滤波/计步/倾角/陀螺/自由落体；
  失败则跳过（不喂脏数据）并调 `MPU6050_Recover()`，重连后自动恢复。闹钟/ADC 块不依赖 MPU，仍每 10ms 运行。
  遗留小项：`SleepMonitor_Update` 掉线时仍读到冻结的 acc（无害，表现为"静止"），未纳入本次范围。**[已由 Q 解决]**
- **O** ✅ MAX30102 热插拔支持（与 N 同理，心率/血氧传感器）。MAX30102 同为 GPIO 位带 I2C
  （PTE10/11），但 `MAX30102_ReadFIFO` 原返回 void 丢弃底层读取成败，主循环也不重init，
  掉线重连后（断电回到 reset/LED 关）卡住。修复：`dev/src/max30102.c` 的 `ReadFIFO` 改为返回
  uint8（失败保留出参不变）；新增 `MAX30102_Bus_Recovery` / `MAX30102_Present`(读 PART_ID==0x15)
  / `MAX30102_Recover`（解卡→确认→重新 Init）。`app/src/main.c` MAX30102 槽改为：读成功才
  Collect/HealthMonitor_Update，失败调 `MAX30102_Recover()`。重连后自动恢复。
  说明（分析 #1）：硬件 I2C 外设的 `I2C_Wait` 无超时死等只影响 MAPS-dock 24C02 EEPROM 路径，
  而该路径当前无任何调用方（潜在/未触发）；看门狗在 `system_init.c` 被 `WDOG_Disable()` 关闭，
  重新启用需硬件实测标定超时（SD 读图/flash 擦写等长操作），暂不盲改。三个在用传感器
  (MPU6050/MAX30102/OLED) 均为带 NACK 检测的位带 I2C，不会真死锁。
- **P** ✅ SPI 死等加超时（分析 #2，LCD/W25Q80 flash 走 SPI，是实际卡死风险点）。
  `chip/inc/spi.h` 新增 `SPI_WAIT_TIMEOUT`(1e6 次，远大于单字节耗时、远小于无限)，并把
  `SPI_EOQF_WAIT` 宏改为有界。`chip/src/spi.c` 新增 `SPI_SPIN_WHILE(cond)` 有界自旋宏 +
  `SPI_Clear_Flags()`（合并原三处相同的清标志 do-while 并加计数上限）。把 `SPI_Xfer_CMD`/
  `SPI_Xfer_Data` 的 RFDF 等待、`SPI_Send` 的 TCF 等待、三处 flag-clear 全部改为有界。
  函数签名保持 void（尽力而为：超时则放弃本次传输而非永久卡死），调用方 LCD/flash 不受影响。
  权衡：硬件真故障时可能拿到不完整数据，但系统不再假死（看门狗当前关闭，这点尤其重要）。

---

## 剩余未修复的问题（低优先级）

- **H** ✅ 已修复：`chip/src/spi.c` 的 `SPI_MOSI` / `SPI_MOSI_CMD` 4 路 NULL 组合重复分支
  → 提取 `SPI_Xfer_CMD` / `SPI_Xfer_Data` 两个静态 helper，内联 NULL 检查
  （NULL MO 发 0x00，NULL MI 丢弃接收）。顺带修复 CMD_Len>0 且 Len==0 时数据阶段
  `SPI_Len-1` 的 uint32 下溢（helper 内 `if (len == 0) return;` 保护）。约 -200 行。
- **K** ✅ 已修复：`app/src/watch.c` 的 11 个 `Calc_*` 函数原各自在栈上分配
  `uint8 clock[64][16]`（1KB）再整块拷贝进 `array` 出参。
  → 将 `clock` 改为指向 `array` 的指针别名 `uint8 (*clock)[16] = (uint8 (*)[16])array;`
  （所有 `clock[y][x]` 索引不变），函数开头 `memset(array,0,64*16)` 清零，删除末尾
  1024 字节拷贝循环。省 11×1KB 栈 + 11×1024 字节拷贝。
  注意：所有调用方（watch.c `Render_*` 与 maps_dock_key.c `Refresh_Dynamic_Animation_Cache`）
  都传 `g_fb`；`Refresh` 会连续两次向同一 `g_fb` 调 `Calc_*`，因此每个 `Calc_*` 内部
  必须自清零（原来靠局部 `{0x00}`）——已由内部 memset 保证独立性。

---

## 工作方法备注

- 大文件（watch.c / maps_dock_key.c / animation.c）含超长坐标常量数组，
  用 grep_search 定位函数，或用 PowerShell 脚本做批量精确替换（正则 + MatchEvaluator
  避免 `$` 转义问题）；临时 .ps1 脚本用完记得删除，勿提交。
- 终端输出常有字符回显噪声，关注最后的实际结果行即可。
- **Q** ✅ 睡眠监测掉线数据修复（分析 #8，收尾 N 的遗留项）。`SleepMonitor_Update` 原本在
  `if (MPU6050_Read_Flag)` 里、但在读取成功/失败分支之外，掉线时会拿到冻结的加速度值
  （被误判为"完全静止"→深睡，污染睡眠统计）。改为把该调用移进读取成功分支（紧跟 FreeFall_Update），
  掉线时直接跳过而不喂假数据。闹钟块和 ADC 示波器块不依赖 MPU，仍每 10ms 运行。

> 关于分析 #3（字库越界钳制）：复查后判定为低收益/高改动——约 30 处近似的 `c=ch[j]-32`，
> 且需按 6x8/8x16/12x24 各自字库大小分别钳制（12x24 "参数输入"字库覆盖字符更少）；而所有
> 调用方传入的都是受控可打印 ASCII（snprintf 数字、固定标签、城市名），实际无暴露。暂缓，
> 若日后有通用字符串绘制 API 接收外部文本再补。
- **R** ✅ 删除 `Render_*` 与 `Refresh_Dynamic_Animation_Cache` 里冗余的 `memset(g_fb,...)`
  （分析 #6）。自 K 修复起每个 `Calc_*` 内部已自清零 `g_fb`，调用前再清一次是每帧多余的全帧
  memset。移除 `app/src/watch.c` 的 11 处 + `board/src/maps_dock_key.c` 的 1 处（注释改为
  "Calc_* zeroes it"）。已确认每处 memset 后到 Calc 之间无人读 g_fb，早退路径也不使用 g_fb，
  故移除安全。纯清理，正常路径行为不变。
- **S** ✅ 重新启用看门狗（分析 #1）。`startup/src/system_init.c` 的 `Start()` 里把
  `WDOG_Disable()` 换成 `WDOG_Init(WDOG_TIMEOUT_MS)`（新增宏，2000ms；1kHz LPO、PRESC=0 →
  1 count=1ms）。**必须在 boot 阶段配置**：driver 的 `WDOG_Unlock` 结束会重新开中断，而 K64
  要求解锁后在窗口内完成重配置、不能被 ISR 打断；`Start()` 此时外设中断尚未启用，安全。
  喂狗点：`app/src/main.c` 的 `AllInit()` 后、`Activity_Load()` 后、SD 载图后各一次，以及
  `for(;;)` 每次迭代开头一次（并 `#include "wdog.h"`）。挂死超过 ~2s 不喂狗即复位。
  ⚠️ **需硬件实测**：确认 (1) boot（尤其 SD 挂载+载图）不超过 2s 否则会 boot-loop；
  (2) 运行时没有单次主循环迭代（菜单切换动画 / 临时 SD 载图等）超过 2s。若有，调大
  `WDOG_TIMEOUT_MS` 或在该长操作内部补 `WDOG_Feed()`。`WDOG_Feed()` 内部已保护刷新序列
  （DisableInterrupts 包裹），运行期任意调用安全。另注：`WDOG_Init` 设了 STOPEN/WAITEN，
  调试器 halt 时可能触发复位。
- **T** ✅ 修复 5 个文件里多余的文件作用域 `}`（编译阻断，最高优先）。framebuf 重构当初把
  各文件私有的 static 绘制 helper 换成 `#include framebuf.h` + 宏别名时，漏删了旧 helper 块
  收尾的 `}`，导致 `#define ds6(...)` 之后多出一个文件作用域右花括号 → 语法错误、编译不过。
  受影响：`health_monitor.c` / `gyro_dash.c` / `adc_scope.c` / `health_score.c` / `activity_history.c`。
  各删除该多余 `}`。花括号计数复核：修复前 5 文件各 close 比 open 多 1（同风格的
  pedometer.c/freefall.c 本就平衡）；修复后全部 open==close。framebuf.h 已确认平衡、不会吸收该括号。
- **U** ✅ 修复 `app/src/freefall.c` 蜂鸣器可能一直响。蜂鸣在 FALLING→IMPACT 确认自由落体时
  开启（Beep_On + beep_ms=300ms），但倒计时 + Beep_Off 原本只在 IMPACT 分支里。撞击检测后
  state→IDLE，此时 beep_ms 通常还 >0（撞击一般在 300ms 内发生），而 IDLE/FALLING 不递减
  beep_ms → 蜂鸣器一直响到下次自由落体或 FreeFall_Clear()。修复：把蜂鸣倒计时块从 IMPACT
  分支移到 `switch` 之前，每次调用都跑、与状态无关，确保 300ms 后一定 Beep_Off。花括号复核
  仍 21/21 平衡。
- **V** ✅ 修复 `board/src/maps_dock_key.c` 的 `menu_scroll_icon` 静态初始化非法（编译错误）。
  该表（表驱动菜单重构 A 引入）的 `.len` 字段用 `LCM_*_coordinate_length` 初始化，而这些是
  `extern const int` 运行期变量、不是编译期常量表达式 → C 语言 "initializer element is not
  constant"，IAR/ARMCC 报错。（`.coord` 用数组名，是地址常量，合法。）修复：把结构体 `len`
  字段改为 `const int *`，初始化用 `&LCM_*_coordinate_length`（地址常量，合法静态初始化），
  4 处调用点改为解引用 `*menu_scroll_icon[i].len`。
- **W** ✅ 修复 `app/inc/watch.h` 头文件不自包含（Coord 未定义）。watch.h 用了 `Coord`
  （`extern const Coord LCM_*_coordinate[]` 及各 Calc_*/Render_* 原型）却只 include `common.h`，
  `Coord` 定义在 `utils/inc/animation.h`。此前能编译只因所有 includer 恰好先 include 了
  animation.h（alarm.c 等场景可能先失败）。同理 watch.h 还用了 `MAPS_WorldClock_Time`
  （定义在 main.h）。修复：watch.h 补上 `#include "main.h"` 和 `#include "animation.h"`，
  头文件自包含。已确认 main.h/animation.h 都只 include common.h、不回指 watch.h，无循环包含。
- **X** ✅ 修复 `g_fb` 链接未定义。`app/src/framebuf.c`（framebuf 重构时新建，定义了 `g_fb`
  及 `fb_pixel`/`fb_hline`/`fb_vline`/`fb_char6`/`fb_str6`/`fb_clear`）**从未被加入 IAR 工程**
  → 所有引用 g_fb 及 fb_* 的地方在链接期报未定义符号。修复：把 `framebuf.c` 加入 `prj/IAR/lyra.ewp`
  和 `lyra.ewt` 的 app 组（filter.c 之后）。`lyra.dep` 是依赖缓存会自动重建，无需改。
  注：这是 framebuf 重构遗留的一批"从未编译验证"问题之一（同批还有 T/V/W）。
- **Y** ✅ 消除 `app/src/freefall.c` 的 IAR 警告 Pe188 "enumerated type mixed with another type"。
  `FFDet` 初始化里 `events` 字段用 `{{0}}`，而 `FreeFall_Event.type` 是枚举
  `FreeFall_EventType`，等于用 int 字面量 0 显式初始化枚举字段 → Pe188。改为
  `{{FF_EVENT_FREEFALL}}`（该枚举常量值也是 0，其余元素仍隐式零初始化，不触发警告）。