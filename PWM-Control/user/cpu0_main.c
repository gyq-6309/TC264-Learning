#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"

/*
 * 电机驱动接线（默认，可按实际接线修改）：
 * MOTOR_PWM_CH  -> 电机驱动模块 PWM/EN
 * MOTOR_DIR_IN1 -> 电机驱动模块 IN1
 * MOTOR_DIR_IN2 -> 电机驱动模块 IN2
 *
 * 驱动模块与 TC264D 核心板必须共地；TC264 GPIO 为 3.3 V 电平。
 */
#define MOTOR_PWM_CH               (ATOM0_CH2_P21_4)
#define MOTOR_DIR_IN1              (P21_2)
#define MOTOR_DIR_IN2              (P21_3)

#define MOTOR_PWM_FREQ             (17000)
#define MOTOR_DUTY_TARGET          (4000)
#define MOTOR_DUTY_STEP            (20)
#define MOTOR_RAMP_DELAY_US        (7500)
#define MOTOR_DIRECTION_DEADTIME_MS (10)

static void motor_stop(void)
{
    pwm_set_duty(MOTOR_PWM_CH, 0);
    gpio_set_level(MOTOR_DIR_IN1, GPIO_LOW);
    gpio_set_level(MOTOR_DIR_IN2, GPIO_LOW);
}

static void motor_set_direction(uint8 forward)
{
    /* 先关闭 PWM，再改变方向，避免 H 桥切换时直通。 */
    pwm_set_duty(MOTOR_PWM_CH, 0);
    gpio_set_level(MOTOR_DIR_IN1, forward ? GPIO_HIGH : GPIO_LOW);
    gpio_set_level(MOTOR_DIR_IN2, forward ? GPIO_LOW : GPIO_HIGH);
    system_delay_ms(MOTOR_DIRECTION_DEADTIME_MS);
}

/*
 * 单方向运行约 3 秒：
 * 0 -> 4000 用时 1.5 秒，4000 -> 0 用时 1.5 秒。
 */
static void motor_run_for_three_seconds(uint8 forward)
{
    int16 duty;

    motor_set_direction(forward);

    for (duty = 0; duty < MOTOR_DUTY_TARGET; duty += MOTOR_DUTY_STEP)
    {
        pwm_set_duty(MOTOR_PWM_CH, duty);
        system_delay_us(MOTOR_RAMP_DELAY_US);
    }
    pwm_set_duty(MOTOR_PWM_CH, MOTOR_DUTY_TARGET);

    for (duty = MOTOR_DUTY_TARGET; duty > 0; duty -= MOTOR_DUTY_STEP)
    {
        pwm_set_duty(MOTOR_PWM_CH, duty);
        system_delay_us(MOTOR_RAMP_DELAY_US);
    }

    motor_stop();
}

int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口

    pwm_init(MOTOR_PWM_CH, MOTOR_PWM_FREQ, 0);
    gpio_init(MOTOR_DIR_IN1, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(MOTOR_DIR_IN2, GPO, GPIO_LOW, GPO_PUSH_PULL);

    cpu_wait_event_ready();         // 等待所有核心初始化完毕

    while (TRUE)
    {
        motor_run_for_three_seconds(TRUE);   // 正转：0 -> 4000 -> 0，约 3 秒
        motor_run_for_three_seconds(FALSE);  // 反转：0 -> 4000 -> 0，约 3 秒
    }
}

#pragma section all restore
