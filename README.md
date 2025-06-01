# CDS_STM32F429IGT6

很多年前买了野火的开发板，一直吃灰(有其他重要的事情)

最近抽出一点时间，打算系统的学习一下这个开发板，主要是我个人想要开发一个小器件。

文档是出于记录的目的，防止忘记。



**本文参考：**

视频参考的是B站keysking和野火官方2个up主， 文档就比较多了，不在详细记录。






# 一、环境配置
## 1. 环境配置vscode篇幅

### 1.1 vscode插件下载

首先参考ST官网的教程，下载vscode插件。[web链接](https://marketplace.visualstudio.com/items?itemName=stmicroelectronics.stm32-vscode-extension)

![](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250524223633227.png)



下载之后在vscode侧边栏可以看到stm32插件的菜单。， 点击`user guide` 则可以看该插件配置教程。

![image-20250524224144945](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250524224144945.png)

### 1.2 flash和debug

- 下载openocd
    - 下载vscode插件![](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250524203048732.png)
    - 侧边栏自动配置![image-20250524203108684](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250524203108684.png)

- debug



配置 lanuch.json

```json
{
  // Use IntelliSense to learn about possible attributes.
  // Hover to view descriptions of existing attributes.
  // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
  "version": "0.2.0",
  "configurations": [
    {
      "type": "stlinkgdbtarget",
      "request": "launch",
      "name": "STM32Cube: STM32 Launch ST-Link GDB Server",
      "origin": "snippet",
      "cwd": "${workspaceFolder}",
      "runEntry": "main",
      "imagesAndSymbols": [
        {
          "imageFileName": "${command:st-stm32-ide-debug-launch.get-projects-binary-from-context1}"
        }
      ]
    }
  ]
}
```

然后按下F5进行debug， 如果有问题，则可以点击STM32的vscode插件，配置下环境：

![image-20250601223024247](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250601223024247.png)



[STM32论坛这里也有问答关于这个问题的](https://community.st.com/t5/stm32cube-for-visual-studio-code/missing-cmsis-pack-root/td-p/804489)






## 2. 环境配置clion

参考[clion 官网的guide即可](https://www.jetbrains.com/help/clion/openocd-support.html#opecocd-debug)





## 3. pyocd



- 安装
    - `pip install pyocd`

```shell


# 列出设备
pyocd list

# 擦除芯片
pyocd erase -t  stm32f429xg --chip

# 下载程序
pyocd flash -t stm32f429xg D:\github\CDS_STM32F429IGT6\cmake-build-debug\yehuo_STM32F429IGT6_base_preject.elf

# 复位芯片
pyocd reset -t stm32f429xg

# 组合操作（擦除、下载、复位）
pyocd flash -t stm32f429xg --erase chip D:\github\CDS_STM32F429IGT6\cmake-build-debug\yehuo_STM32F429IGT6_base_preject.elf -R


```



## 4. 串口工具

1. vscode插件：Serial monitor
2. pyqt写的开源工具： comtool
3. 开源可视化工具： Serial Studio

# 二、开发学习

cds的野火STM32F429IGT6学习代码仓库

**基础信息**

1. 开发板是：野火STM32F429IGT6 V1



我还有一块STM32F7的开发板，CUBEMX截至2025年5月24日是不支持这种有2核的MCU的。 所以暂时先用F429进行开发吧。



## 1、 STM32CubeMX简单使用

有2个重要的设置需要注意：

![image-20211104104732833](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211104104732.png)

![image-20211103225536010](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103225536.png)



1. 安装，这个全网都是教程，跳过。
2. 新建项目，无脑下一步，没有什么难度。
3. `Ctrl + R`快捷键可以快速导出项目的配置。给别人分享超级方便。

![image-20211103210422544](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103210422.png)





4. 老样子，开机直接来一个点灯大师。

先看一下野火开发板的原理图：

- key

<img src="https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103210832.png" alt="image-20211103210832496" style="zoom:50%;" />

- LED

![image-20211103210921037](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103210921.png)

- cubeMx的GPIO配置如下图：

![image-20211103210735141](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103210735.png)



- 生成项目编译进去

在main.c的死循环里面，写入下面一个简单的程序，来通过key2 翻转led灯。

```c
void key2_toggle_green_led_pin()
{
    if (HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin)) {
        while (HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin)) {
            // do nothings
        }
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
    }
}
```

keil编译下载进去，效果还可以。



## 2、 时钟树

### 1. 简介

**RCC ：reset clock control 复位和时钟控制器。**

- 作用

设置系统时钟 SYSCLK、设置 AHB 分频因子（决定 HCLK 等于多少）、设置 APB2 分频因子（决定 PCLK2 等于多少）、设置 APB1 分频因子（决定 PCLK1 等于多少）、设置各个外设的分频因子；控制 AHB、APB2 和 APB1 这三条总线时钟的开启、控制每个外设的时钟的开启。对于 SYSCLK、HCLK、PCLK2、PCLK1 这四个时钟的配置一般是：HCLK =SYSCLK=PLLCLK = 180M，PCLK1=HCLK/2 = 90M，PCLK1=HCLK/4 = 45M。这个时钟配置也是库函数的标准配置，我们用的最多的就是这个。

- 一般设置(F429举例)

HCLK = SYSCLK=PLLCLK= 180M，PCLK1=HCLK/2 = 90M，PCLK1=HCLK/4 = 45M

- 时钟树

![image-20211030104405634](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211030104405.png)

**时钟的分类：**

1. HSE高速外部时钟信号
2. 锁相环 PLL
3. 系统时钟 SYSCLK
4. AHB总线时钟 HCLK
5. APB1总线时钟 HCLK1
6. APB2总线时钟 HCLK2

这里分类的顺序分别对应上图时钟树的标号。



**1. HSE**

- 主要特性
    1. 外部时钟，不是系统自身的时钟源。
    2. 如果使用HSE作为系统时钟的话，或者使用HSE被锁相环失踪PLL倍频后的时钟作为系统时钟源的话，当HSE出现故障，系统会自动切换到HSI时钟，直到HSE恢复。

**2. PLL 锁相环时钟**、

- 主要特性
     	1. 倍频时钟
     	2. 有主锁相环PLL、 专用的 PLLI2S二个，都有HSE或者HSI提供时钟源。
     	3. **主PLL锁相环时钟**
               	1. 总共有2路输出
                          	1. 第一路：系统时钟
                          	2. 第二路：用于 USB OTG FS 的时钟（48M）、RNG 和 SDIO 时钟（<=48M）专用的 PLLI2S 用于生成精确时钟，给 I2S 提供时钟





### 2. 代码部分

![在这里插入图片描述](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103215348)

- 跟时钟相关的都在这个hal文件里面。

`Drivers\STM32F4xx_HAL_Driver\Inc\stm32f4xx_hal_rcc.h`

- cubemx IDE配置相关

要选中外部HSE时钟，就必须先配置RCC，开启HSE。

![image-20211103215613094](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103215613.png)

主要是配置这二个结构体分别配置：

1. 系统时钟源头。
2. RCC System, AHB and APB busses clock configuration

```c
/**
  * @brief  RCC Internal/External Oscillator (HSE, HSI, LSE and LSI) configuration structure definition
  * 配置系统振荡器的属性
  */
typedef struct
{                                                 */
}RCC_OscInitTypeDef;

/**
  * @brief  RCC System, AHB and APB busses clock configuration structure definition
  * RCC系统，AHB和APB总线时钟配置结构定义
  */
typedef struct
{
}RCC_ClkInitTypeDef;
```



#### 2.1 开启GPIO口的时钟

```c
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
```

### 3. cubemx的设置

这里会有最大值，直接设置最大值后，CUBEMX会自动设置其它参数的。

![image-20250524232707755](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250524232707755.png)；；；；llklkkktyfty





## 3. 中断编程

1、使用 `HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup)` 函数配置中断优先级分组。一般默认是 `NVIC_PRIORITYGROUP_4` 分组 4。
2、使用` HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_tSubPriority)`函数配置具体外设中断通道的抢占优先级和子优先级。
3、使用 `HAL_NVIC_EnableIRQ` 函数使能中断请求



---

**cubemx实战：**

首先要配置系统时钟

![image-20211103233703692](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103233703.png)





1. 将引脚设置为复用输出

![image-20211103220627213](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220627.png)

2. 选择上升沿触发

![image-20211103220800504](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220800.png)

3. 使能中断

![image-20211103220838051](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220838.png)

4. 生成代码，使用keil编译复写中断服务函数







在上面我们可以看到设置的是![image-20211104104925272](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211104104925.png)

中断线0的中断，我们可以在中断向量表中查询该中断的响应函数为：`EXTI0_IRQHandler`

> MDK-ARM\startup_stm32f429xx.s 中断向量表查询路径

```c
__Vectors       DCD     __initial_sp               ; Top of Stack
DCD     EXTI0_IRQHandler                  ; EXTI Line0
```

然后可以在中断服务函数里找到该函数

> Core\Src\stm32f4xx_it.c

```c
/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}
```

可以看到这个函数将中断事件进行了分发，分发给了函数`HAL_GPIO_EXTI_IRQHandler` 我们点开这个函数再看一眼：

> Drivers\STM32F4xx_HAL_Driver\Src\stm32f4xx_hal_gpio.c

```c
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
  /* EXTI line interrupt detected */
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != RESET)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    HAL_GPIO_EXTI_Callback(GPIO_Pin);
  }
}
```

我们看到这里再次对中断进行了一次判断，然后分发给了函数`HAL_GPIO_EXTI_Callback`这个函数就是统一的中断处理函数了。我们对中断的代码逻辑加在这一部分就可以了。我在这里对红色的led灯进行了翻转。

> Drivers\STM32F4xx_HAL_Driver\Src\stm32f4xx_hal_gpio.c

```c
__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  key1_toggle_red_led_pin();
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}
```

烧录进入开发板一切正常。

第二次重新生成工程的适合发现，**我写的代码没有了？？？？？？**

我们仔细看这个函数的注释：这个函数不应该被修改，当这个回调需要的适合用户可以自己实现在自己的文件中。

```c
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
```

然后仔细一看这个函数是个 **弱函数**， 所以下来我们要自己实现。

1. 首先新建一个文件：`Core\Src\cds_gpio_exit.c` 包含头文件`\#include "stm32f4xx_hal.h"`
2. 复写函数：`void HAL_GPIO_EXTI_Callback(uint16_t *GPIO_Pin*)`
3. 添加到keil编译环境中
4. 烧录下载验证，一切ok。

![image-20211104111503230](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211104111503.png)



我们来梳理一下流程：

1. 配置系统时钟
2. 配置debug方式
3. 引脚设置为复用输出
4. 选择沿触方式
5. 生成代码，使用keil编译复写中断服务函数。
    1. GPIO的中断服务响应函数定义在`stm32f4xx_hal_gpio.c`文件中，然后该文件将这个中断信号通过函数调用的方式分发给了`stm32f4xx_hal_gpio.c`文件
    2. 我们实际的GPIO中断逻辑就都可以写在该文件路径下。但是会代码耦合。
    3. 重新定义回调函数，并实现。



## 4. freertos



这部分没什么说的，只需要勾选了操作系统即可：

![image-20250525002141247](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250525002141247.png)









## 4. UART通讯

在这里我们使用开发板上的  `USART3` 作为例子

![image-20250525002158699](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250525002158699.png)

有一个vscode的串口收发插件特别好用：

![image-20250525213431810](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250525213431810.png)





### 4.1 CUBEMX设置

![image-20250525213454902](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250525213454902.png)



### 4.2 基础的uart发送

![image-20250525213526913](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250525213526913.png)



### 4.2 使能uart收发中断

![image-20250525230343719](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250525230343719.png)



在 `startup_stm32f429xx.s` 文件中找到ta的中断处理函数

![image-20250526213808187](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250526213808187.png)



在代码中可以看到：

```c++
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}
```



这里稍微有点没搞明白， 会在中断函数

```c++
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
```

再次调用，这个函数是一个weak函数， 可以重新定义为：

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        HAL_UART_Transmit_IT(&huart1, uart3_rx_buffer, 2); // Echo back received data
        // Check the received command and toggle the corresponding LED
        if(uart3_rx_buffer[0] == 'R') {
            HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin); // Toggle red LED
        } else if(uart3_rx_buffer[0] == 'G') {
            HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin); // Toggle green LED
        } else if(uart3_rx_buffer[0] == 'B') {
            HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin); // Toggle blue LED
        } else {
            uint8_t response[] = "Invalid Command";
            HAL_UART_Transmit_IT(huart, response, sizeof(response));
        }
        HAL_UART_Receive_IT(huart, uart3_rx_buffer, 2); // Re-enable the interrupt for next reception
    }
}
```

### 4.3 使用DMA，接受发送任意长度的数据

![image-20250526214749475](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250526214749475.png)





使用DMA只需要将

```C
// 中断接受数据
HAL_UART_Transmit_IT()

// DMA接受数据
HAL_UART_Transmit_DMA()
```

在开启DMA情况下， 串口的中断就不会在被触发了， 因为 `当 UART 配置为 DMA 模式时，数据直接从 UART 缓冲区传输到内存，**RXNE 标志不会被置位**（因为 DMA 自动清除了它），导致 RXNE 中断无法触发。`, 我们什么时候知道串口接受或者发送完成了呢？

答案就是当系统的`idle`中断时候， 则说明uart发送或者接受完了， 所以我们在idle的中断函数里处理uart发送完成和接受完成的情况。



这里有一个坑就是 DMA 传输过半中断也会激活系统的`idle`中断， 所以需要关闭DMA传输过半中断

```c
// 关闭DMA传输过半中断（HAL库默认开启，但我们只需要接收完成中断）
__HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
```

### 4.4 printf打印串口数据



[参考guide](http://www.emcu.eu/how-to-implement-printf-for-send-message-via-usb-on-stm32-nucleo-boards-using-atollic/)

- 添加对浮点数据的支持

```cmake
# Add compile options
# Note: The -u _printf_float option is used to enable floating-point support in printf/scanf functions.
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -u _printf_float
)
```





## 5. I2C协议

我看的up主是使用I2C协议控制OLED屏幕，我没有OLED屏幕......

只有一块RGB屏幕，是要走比较复杂的协议LTDC， 我先学习下I2C协议吧。





## 6. SPI

FLSAH 存储器又称闪存，它与 EEPROM 都是掉电后数据不丢失的存储器，但 FLASH存储器容量普遍大于 EEPROM，现在基本取代了它的地位。我们生活中常用的 U 盘、SD卡、SSD 固态硬盘以及我们 STM32 芯片内部用于存储程序的设备，都是 FLASH 类型的存储器。在存储控制上，最主要的区别是 FLASH 芯片只能一大片一大片地擦写，而在“I2C章节”中我们了解到 EEPROM 可以单个字节擦写。







# 外设篇

## 温度外设-普通GPIO

温度传感器为： DS18B20

每一个芯片都有自己独立的ID，本模块需连接的引脚仅三根，除去基础的 VCC 电源线以及 GND 地线以外，只剩下 DATA 数据线，

将数据线连接至任意 GPIO，保证该 IO 口收发数据正常即可完成对 DHT11 模块的控制。

![image-20250529215453876](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250529215453876.png)



所以驱动温度外设就特别简单，2个引脚是正负极。另外一个引脚是GPIO，设置为输入输出模式，用于读取温度和芯片ID即可。

![image-20250529215612987](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250529215612987.png)

看图可知：

![image-20250529220055128](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250529220055128.png)



PE2口就是我们要读取的口，根据该口配置相关IO即可。



## EEPROM外设-I2C



![image-20250529220254164](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com//202408/image-20250529220254164.png)





型号是： AT24C02。 芯片手册在立创商城搜索这个型号就可以下载。

是I2C1的接口， 通过这个接口就可以读写。



EEPROM是是可以存储数据的，断电数据不会消失。









### DWT精确定时

DWT 是 ARM Cortex-M 内核中提供的一个调试和性能分析模块，具有以下用途：

- 周期计数（高精度延时）
- 指令统计
- 性能监控（如睡眠次数、中断次数等
-
