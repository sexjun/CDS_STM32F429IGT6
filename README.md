# CDS_STM32F429IGT6
cds的野火STM32F429IGT6学习代码仓库



**基础信息**

1. 开发板是：野火STM32F429IGT6 V1
2. CubeMx的版本是：6.3.0

![image-20211103210632489](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103210632.png)

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

1. 将引脚设置为复用输出

![image-20211103220627213](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220627.png)

2. 选择上升沿触发

![image-20211103220800504](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220800.png)

3. 使能中断

![image-20211103220838051](https://tu-chuang-1253216127.cos.ap-beijing.myqcloud.com/20211103220838.png)

4. 生成代码，使用keil编译复写中断服务函数

## git

```
git push origin HEAD:refs/for/master
```

