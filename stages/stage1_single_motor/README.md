# Stage 1 - Single Motor

目标：完成单电机 bring-up，并建立最小闭环控制链路。

本阶段关注：

- 主板连接和网络配置
- 单电机对象创建
- 控制模式切换
- 位置/速度/电流闭环与 PVCT 读取
- 单电机安全限幅和日志记录

升格条件：

- 单电机能够稳定运行
- 关键反馈量能够被记录
- 限位与故障停机逻辑可验证

参考工程：

- 你已跑通的 `single_motor_ctrl` 非常适合作为本阶段实现基线
- 推荐迁移映射见 `reference_mapping.md`

当前状态：

- `stages/stage1_single_motor/app` 已替换为真实 CLI 入口
- `src/platform/phyarc` / `src/control/motor` / `src/runtime` / `src/safety` 已接入单电机实现
- 默认构建使用 stub 后端，便于在非控制主机上继续开发
- 在 Linux 控制主机上可用 `-DQRC_ENABLE_PHYARC_SDK=ON` 打开厂商 SDK 实机链路

常用命令：

```bash
./build/qrc_stage1_single_motor --config stages/stage1_single_motor/config/motor_cli.conf dump-config
./build/qrc_stage1_single_motor --config stages/stage1_single_motor/config/motor_cli.conf info
./build/qrc_stage1_single_motor --config stages/stage1_single_motor/config/motor_cli.conf get-id
./build/qrc_stage1_single_motor --config stages/stage1_single_motor/config/motor_cli.conf monitor 20 100 fast
./build/qrc_stage1_single_motor --config stages/stage1_single_motor/config/motor_cli.conf shell
```
