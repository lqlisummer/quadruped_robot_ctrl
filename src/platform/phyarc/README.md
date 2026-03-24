# Platform Adapter

这里是项目内部对 PhyArc SDK 的适配层，不直接承载控制算法。

建议后续把这里拆成几类对象：

- `board_adapter`：主板初始化、网络配置、快速模式管理
- `motor_adapter`：单电机对象创建、模式切换、PVCT 收发
- `group_adapter`：电机组大包发送、同步触发
- `sdk_types`：把厂商类型映射成项目内部统一类型

目标是让上层控制代码依赖项目自己的接口，而不是散落依赖 `RobotControl.h`。

