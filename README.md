# Quadruped Robot Control

这个仓库按“一条主线、四级递进”来组织四足机器人控制项目，目标是让控制系统沿着同一条演进路径，从单电机逐步长成整机控制，而不是每一阶段都重新搭一套工程。

## 主线

1. `Stage 1` 单电机控制
2. `Stage 2` 并联驱动单元控制
3. `Stage 3` 单腿控制
4. `Stage 4` 四足机器人控制

四个阶段共享同一个底座：

- `src/common` 通用类型、打印、基础工具
- `src/runtime` 控制循环、任务调度、日志与状态机
- `src/platform/phyarc` 厂商 SDK 适配层
- `src/model` 电机/驱动单元/腿/整机的模型层
- `src/control` 与四级递进对应的控制器层
- `src/safety` 限幅、故障处理、状态切换

## 推荐目录

```text
.
├── CMakeLists.txt
├── docs
│   ├── architecture.md
│   ├── hardware
│   └── mechanism
├── src
│   ├── common
│   ├── runtime
│   ├── platform
│   ├── model
│   ├── control
│   └── safety
├── stages
│   ├── stage1_single_motor
│   ├── stage2_parallel_drive_unit
│   ├── stage3_single_leg
│   └── stage4_quadruped
├── third_party
│   └── phyarc
└── tools
```

## 设计原则

- 一条主线：所有代码都服务于最终四足控制，不为临时 demo 单独分叉。
- 四级递进：每一阶段都必须有独立入口、配置和测试焦点。
- 共享底座：通信、模型、日志、安全等公共能力不重复实现。
- 厂商隔离：第三方 SDK 固定在 `third_party`，业务代码只依赖适配层。
- 可迁移：从实机 bring-up 到整机控制，接口尽量保持稳定。

## 当前资产归位

- 厂商 Linux SDK：`third_party/phyarc/linux_sdk`
- 电机与控制盒手册：`docs/hardware/phyarc/manuals`
- 单腿机构资料：`docs/mechanism/single_leg_mechanism_kinematics.docx`

## 构建骨架

```bash
cmake -S . -B build
cmake --build build
```

构建后会生成四个 stage 的占位可执行文件，用于确认目录和演进关系已经接通。

## 接下来怎么推进

1. 先在 `Stage 1` 打通单电机通信、模式切换、PVCT 闭环和日志。
2. 在 `Stage 2` 把多个电机抽象成一个并联驱动单元，加入同步与力矩分配。
3. 在 `Stage 3` 引入单腿运动学、雅可比和足端轨迹控制。
4. 在 `Stage 4` 叠加 gait、状态估计、整机协调与安全状态机。

更详细的分层和演进规则见 `docs/architecture.md`。

