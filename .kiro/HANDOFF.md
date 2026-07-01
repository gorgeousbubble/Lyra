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
