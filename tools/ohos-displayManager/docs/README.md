# ohos-displayManager

## 概述

ohos-displayManager 是一个用于管理显示设备亮度的命令行工具，通过封装 DisplayPowerMgrClient 内部接口，提供便捷的亮度调节能力。

## 功能列表

- 设置显示设备亮度（支持连续调节模式）

## 依赖

- 系统能力：SystemCapability.PowerManager.DisplayPowerManager
- 权限：系统应用身份（Permission::IsSystem()）。本工具不涉及 ohos.permission.XXX 权限声明，需以系统应用身份运行。

## 基本用法

```bash
ohos-displayManager <command> [options]
```

## 命令列表

| 命令 | 说明 | 参数 | 权限 | 前置依赖 |
|------|------|------|------|----------|
| set-brightness | 设置显示设备亮度 | --value (必填), --continuous (可选) | 系统应用身份 | 无 |

## 示例

```bash
# 查看全局帮助信息
ohos-displayManager --help

# 查看 set-brightness 子命令帮助信息
ohos-displayManager set-brightness --help

# 设置亮度为 128
ohos-displayManager set-brightness --value 128

# 设置亮度为 100，启用连续调节模式
ohos-displayManager set-brightness --value 100 --continuous

# 设置亮度为 255，启用连续调节模式
ohos-displayManager set-brightness --value 255 --continuous
```
