[toc]

# 1. 小试牛刀

## 项目设置

```JSON
{
    "cortex-debug.gdbPath": "${config:STM32VSCodeExtension.cubeCLT.path}/GNU-tools-for-STM32/bin/arm-none-eabi-gdb",
    "cortex-debug.openocdPath": "/usr/local/Cellar/open-ocd/0.12.0_1/bin/openocd",

}

         {
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/build/Debug/mx_cmake_test.elf",
            "name": "Debug with OpenOCD",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "openocd",
            "configFiles": [
                "/usr/local/Cellar/open-ocd/0.12.0_1/share/openocd/scripts/interface/jlink.cfg",
                "/usr/local/Cellar/open-ocd/0.12.0_1/share/openocd/scripts/target/stm32f4x.cfg"
            ],
            "searchDir": [],
            "runToEntryPoint": "main",
            "showDevDebugOutput": "none"
        },



    "tasks": [
        {
            "type": "shell",
            "label": "Flash-test",
            "command": "openocd",
            "args": [
                "-f",
                "/usr/local/Cellar/open-ocd/0.12.0_1/share/openocd/scripts/interface/jlink.cfg",
                "-f",
                "/usr/local/Cellar/open-ocd/0.12.0_1/share/openocd/scripts/target/stm32f4x.cfg",
                "-c",
                "program ${workspaceRoot}/build/Debug/mx_cmake_test.elf  verify reset  exit",
            ],
            "problemMatcher": [
                "$armcc5",
                "$gcc"
            ],
            "group": "build"
        }
      ]
```

## 点灯大师

![image-20240528233340843](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240528233340843.png)

```c++
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, GPIO_PIN_RESET);
    HAL_Delay(500);
```

# 2. GPIO模块

## 按键检测

- 软件防抖

![image-20240529000426687](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240529000426687.png)



## GPIO的八大模式

![image-20240529000533513](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240529000533513.png)





![image-20240529000617364](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240529000617364.png)



1. 静电保护结构

![image-20240529000804735](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240529000804735.png)

2. 5v的静电保护GPIO口可以在芯片手册上查看到

![image-20240529000910437](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240529000910437.png)

3. 输入输出驱动器

   ![image-20240529001043557](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240529001043557.png)

3.1 输出驱动器

`P-MOS` 和 `N- MOS` 可以理解为2个开关。

`VDD`是高电压 `3.3V` , `VSS`是低电压 `0V`。

- 推挽输出
  - N 和 P 协同工作

![image-20240529001117768](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240529001117768.png)

# 3. 中断



# 4. 串口

## 1. 初步enable串口（轮询模式）

串口有2个端口，1个TX另一个是RX， 用来接受和发送，属于双工通信，同时有一个波特率的概念，用来定义发送和接受的频率。

接地可以确保`0v`的参考电压一致。

- 软件设置

![image-20240530111745003](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530111745003.png)

- 硬件原理图

![image-20240530111920220](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530111920220.png)

- 硬件连接

![image-20240530184744610](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530184744610.png)

- 代码设置

![image-20240530112136355](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530112136355.png)

- 串口接受

![image-20240530112233218](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530112233218.png)

## 2. 串口与中断

