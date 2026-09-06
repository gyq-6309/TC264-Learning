
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"

#define CHANNEL_NUMBER          (4)

#define PWM_CH1                 (ATOM1_CH5_P20_9)
#define PWM_CH2                 (ATOM0_CH7_P20_8)
#define PWM_CH3                 (ATOM0_CH3_P21_5)
#define PWM_CH4                 (ATOM0_CH2_P21_4)

int16 duty = 0;
int16 duty_temp = 0;
uint8 channel_index = 0;
pwm_channel_enum channel_list[CHANNEL_NUMBER] = {PWM_CH1, PWM_CH2, PWM_CH3, PWM_CH4};

int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

    pwm_init(PWM_CH1, 17000, 0);                                                // 初始化 PWM 通道 频率 17KHz 初始占空比 0%
    pwm_init(PWM_CH2, 17000, 0);                                                // 初始化 PWM 通道 频率 17KHz 初始占空比 0%
    pwm_init(PWM_CH3, 17000, 0);                                                // 初始化 PWM 通道 频率 17KHz 初始占空比 0%
    pwm_init(PWM_CH4, 17000, 0);                                                // 初始化 PWM 通道 频率 17KHz 初始占空比 0%

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码

        for(duty = 0; duty <= PWM_DUTY_MAX / 2; duty ++)                        // 输出占空比递增到 50%
        {
			// 呼吸流水灯
            for(channel_index = 0; channel_index < CHANNEL_NUMBER; channel_index++) 
            {
                duty_temp = (duty + channel_index * PWM_DUTY_MAX / 8) % (PWM_DUTY_MAX / 2) + (PWM_DUTY_MAX / 2); 
                pwm_set_duty(channel_list[channel_index], duty_temp);           // 更新对应通道占空比
            }
            system_delay_us(200);
        }

        // 此处编写需要循环执行的代码
    }
}

#pragma section all restore

