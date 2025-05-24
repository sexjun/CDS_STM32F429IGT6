# CDS_STM32F429IGT6


# 环境配置
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
    - 选择


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





# 开发学习

cds的野火STM32F429IGT6学习代码仓库

**基础信息**

1. 开发板是：野火STM32F429IGT6 V1

## 1、 STM32CubeMX简单使用

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





## 3. 中断编程

1、使用 `HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup)` 函数配置中断优先级分组。一般默认是 `NVIC_PRIORITYGROUP_4` 分组 4。
2、使用` HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_tSubPriority)`函数配置具体外设中断通道的抢占优先级和子优先级。
3、使用 `HAL_NVIC_EnableIRQ` 函数使能中断请求



---

**cubemx实战：**

首先要配置系统时钟，然后要将系统debug方式打开。

![image-20211103233703692](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103233703.png)



![image-20211104104732833](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211104104732.png)

1. 将引脚设置为复用输出

![image-20211103220627213](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220627.png)

2. 选择上升沿触发

![image-20211103220800504](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220800.png)

3. 使能中断

![image-20211103220838051](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220838.png)

4. 生成代码，使用keil编译复写中断服务函数



![image-20211103225536010](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103225536.png)



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





























## git

```
git push origin HEAD:refs/for/master
```

