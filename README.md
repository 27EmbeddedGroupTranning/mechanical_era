# Mechanical Era

STM32F103 麦克纳姆轮小车与四路舵机控制。控制代码使用 C++14，CubeMX 外设初始化和 ST 驱动保持生成的 C 代码。

## 阅读顺序

1. `src/main.cpp`：主循环，按键去抖与急停，采集输入、计算指令、执行输出、定时报告。
2. `src/ps2.cpp`：单个 PS2 接收器的 SPI 通信、模拟模式配置、轮询及超时；`ps2_protocol.cpp` 负责唯一一份报文解码。
3. `src/drive_control.cpp` / `src/arm_control.cpp`：将 `Ps2Frame` 转成电机/舵机百分比，不依赖 HAL。
4. `src/outputs.cpp`：方向脚、PWM、换向等待与舵机脉宽；`diagnostics.cpp`：串口状态和 M4 硬件诊断。
5. `inc/car_config.hpp`：轮位、极性、麦轮布局、速度和死区配置。

数据按 `ps2_update → ps2_read → drive_from_ps2 / arm_from_ps2 → apply_motors / apply_servos` 流动。PS2 报文只解码一次，程序中的按键位统一为 1 表示按下。调试统计不参与控制决策。

## 编译

固件需要 CMake、Ninja 和 PATH 中的 Arm GNU Toolchain：

```powershell
cmake --preset Debug
cmake --build --preset Debug
cmake --preset Release
cmake --build --preset Release
```

输出为 `build/Debug/mechanical_era.elf` 或 `build/Release/mechanical_era.elf`。

## 代码约定

- 公共类型用明确名称，如 `Ps2Frame`、`DriveCommand`、`ArmCommand`；函数用 `snake_case`。
- 配置集中在 `car_config` 命名空间；文件内部细节使用匿名命名空间。无需为每个模块再包一层通用 `Command` / `from_ps2`。
- 使用普通结构体和函数，无动态分配、继承层次或通用硬件抽象框架。
- 纯计算与 HAL 操作分开，`.hpp` 声明接口，`.cpp` 实现行为。`inc/app.h` 是供生成的 C 入口调用的薄桥接声明。
- 使用 `.clang-format` 统一自编代码格式；不要批量格式化 `Core/` 和 `Drivers/` 的生成代码。
- 生成文件中的手工入口保留在 `USER CODE` 区域。重新生成 CubeMX 后检查片选初值，并重新编译固件。
- `build/` 是可重新生成的本机输出，不纳入版本控制。

## 操作与接线

采用 O 型麦轮布局。M1 左前、M2 右前、M3 右后、M4 左后，M1/M2 反向。轮位和方向在 `inc/car_config.hpp` 中配置。

- 左摇杆控制前后、左右平移；方向键以 80% PWM 覆盖左摇杆输入。
- L1/R1 控制旋转；按住 L3 时右摇杆也参与底盘旋转。
- 右摇杆同时控制机械臂；Cross/Circle 控制第三路舵机偏移，松开回中。
- Cross/SELECT 按住时底盘停车；板上 Key 切换急停。
- 断线或有效报文超时 100 ms 时电机停止、舵机回中。
- START 加单个方向键用于电机校准：上/右/下/左对应 M1/M2/M3/M4，R2 反转，输出为 35% 原始通道占空比。

| PS2 信号 | STM32 引脚 |
| --- | --- |
| DAT | PB14 / SPI2_MISO |
| CMD | PB15 / SPI2_MOSI |
| ATT / CS | PB12 |
| CLK | PB13 / SPI2_SCK |
| GND | 与控制板共地 |

SPI2 使用 Mode 3、LSB 优先、256 分频，20 ms 轮询。接收器按模块规格供电，信号电平匹配 3.3 V。

TIM1 输出四路电机 PWM，TIM8 的 PC6～PC9 输出四路舵机 PWM。USART3_TX 使用 PC10，115200、8N1，每 500 ms 输出输入状态、电机指令及 M4 硬件诊断。
