# Stage 1 Reference Mapping

这个文档用于把已经跑通的 `single_motor_ctrl` 工程映射到当前主仓库架构中。

## 参考工程中已经验证过的能力

- 主板网络连接
- 单电机创建与销毁
- 控制模式切换
- 位置/速度/电流/PD 指令下发
- 电机型号、固件、ID 查询
- 遥测监控与交互式 CLI
- 清故障、使能、置零、校准、强拖

这些都说明 `Stage 1` 的功能边界已经很清楚，不需要再从零摸索。

## 迁移建议

### 可以直接继承的分层思想

- `board_client`：保留为板级连接对象
- `motor_driver`：保留为电机硬件访问对象
- `single_motor_controller`：保留为单电机控制器
- `cli_app`：保留为 `Stage 1` 的现场调试入口

### 迁移到主仓库时的落位

| 参考工程文件族 | 主仓库建议位置 |
| --- | --- |
| `include/device/*` `src/device/*` | `src/platform/phyarc/*` |
| `include/driver/*` `src/driver/*` | `src/platform/phyarc/*` |
| `include/domain/motor_types.h` | `src/model/motor/motor_types.*` |
| `include/domain/single_motor_controller.h` `src/domain/*` | `src/control/motor/*` |
| `include/control/safety_manager.h` `src/control/safety_manager.cpp` | `src/safety/*` |
| `include/control/telemetry_monitor.h` `src/control/telemetry_monitor.cpp` | `src/runtime/*` |
| `configs/motor_cli.conf` | `stages/stage1_single_motor/config/*` |
| `src/app/cli_app.cpp` `src/main.cpp` | `stages/stage1_single_motor/app/*` |

## 为什么不原样照搬整个目录

- 原工程适合单电机调试，但不能自然长成并联驱动单元、单腿和整机控制。
- `device / driver / domain / control` 的思想很好，但需要把“平台层”和“控制层”再分得更清楚。
- 未来 `Stage 2~4` 需要复用底层 SDK 适配和安全逻辑，所以必须提前抽出共享底座。

## 下一步最自然的接法

1. 先把 `single_motor_ctrl` 的 `board_client` 和 `motor_driver` 吸收到 `src/platform/phyarc`。
2. 把 `motor_types` 和 `single_motor_controller` 分别落到 `src/model/motor` 与 `src/control/motor`。
3. 保留 CLI 入口在 `stages/stage1_single_motor/app`，作为现场 bring-up 工具。
4. 在此基础上再定义 `ParallelDriveUnit`，就能自然过渡到 `Stage 2`。

