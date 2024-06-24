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

  ```c
  // 轮询模式
  
  // 发送数据  
  HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), 100);
  // 接受数据
  HAL_UART_Receive(&huart1, (uint8_t*)rec, 2, HAL_MAX_DELAY);
  ```

  

![image-20240530112136355](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530112136355.png)

- 串口接受

![image-20240530112233218](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530112233218.png)

## 2. 串口与中断

使用中断发送数据的方式与使用轮询发送数据的方式非常类似：

```c
 // 轮询模式

// 发送数据  
HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message), 100);
// 接受数据
HAL_UART_Receive(&huart1, (uint8_t*)rec, 2, HAL_MAX_DELAY);


// 中断模式
		   // 数据接收
     HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
       // 数据发送
     HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
```



![image-20240530203646817](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530203646817.png)





可以看到在 自动生成的代码中，已经为该中断生成了中断处理函数。我们进入 中断处理函数，i

![image-20240530204328530](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530204328530.png)



根据函数的定义我们可以了解到，该函数主要的作用就是分类，因为该中断线中有很多触发源， UArt只是其中一种。

```c
/**
  * @brief  This function handles UART interrupt request.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
```

我们可以在该下面找到特定的，针对uart出发的中断callback函数，并且是`__weak`定义，我们可以在自己的代码中实现，并处理。

```c
/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
__weak void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxHalfCpltCallback could be implemented in the user file
   */
}
```





## 3. 串口与SMDA

> 1. 接受不定长
> 2. 彻底释放CPU？

- 串口与SDMA

```c
// 使用DMA进行串口数据传输
HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size)
HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
  
  
// 使用空闲中断，当空闲发生的时候，则认为数据发送完毕，这个时候通过DMA将数据进行读取。
  // 最后的参数size是指一次性最大读取的字符数，一般是缓存空间的大小，防止溢出。
  HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
```

需要注意的是，空闲中断函数和uart中断处理函数不是一个函数：



- 空闲中断函数

```c
/**
  * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
  * @param  huart UART handle
  * @param  Size  Number of data available in application reception buffer (indicates a position in
  *               reception buffer until which, data are available)
  * @retval None
  */
__weak void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  UNUSED(Size);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UARTEx_RxEventCallback can be implemented in the user file.
   */
}
```





## 4. 串口与printf

```c
// 在main.c中实现该函数，后续printf就会使用uart进行打印。
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

```

# 5. 蓝牙

> 蓝牙模块与数据打包

分类：

- 经典蓝牙
  - 耳机
- 低功耗蓝牙
  - BLE： 手环

手机充当主机 -----> STM从机

![image-20240530221233620](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/image-20240530221233620.png)

蓝牙串口透传模块：

1. 复杂的蓝牙协议转化为串口透传
2. STM串口发送给其的数据，原封不动的转发给主机。
3. 主机发给蓝牙的数据，原封不动的通过串口发给从机。
