# PhyArc Vendor Assets

这个目录只存放厂商提供的 SDK、示例和二进制库。

- `linux_sdk`：原始 Linux SDK、头文件、库文件和示例代码

建议约束：

- 不在 vendor 目录直接写项目业务逻辑
- 项目代码通过 `src/platform/phyarc` 做二次封装
- 如果必须修改 vendor 代码，单独记录修改原因和版本差异

