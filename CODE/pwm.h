/**
 * @file pwm.h
 * @brief configura y genera el pwm.
 * A traves del controlador PID se asigna un valor de ciclo de trabajo el cual sera configurado en este archivo.
 *
 */

volatile uint32_t wrap; ///< Contador de envoltura para el valor de PWM.
pwm_config cfg;         ///< Configuración de la PWM.

/**
 * @brief Configura la frecuencia y el ciclo de trabajo de la PWM.
 * @param slice_num Número del slice PWM.
 * @param chan Canal de la PWM.
 * @param f Frecuencia deseada para la PWM.
 */
void pwm_set_freq_duty(uint slice_num,uint chan,uint32_t f)
{
    uint32_t clock = 125000000;                                           /// Frecuencia del reloj.
    uint32_t divider16 = clock / f / 4096 + (clock % (f * 4096) != 0);
    if (divider16 / 16 == 0) divider16 = 16;                              /// Asegurar que el divisor mínimo sea 16.
    wrap = clock * 16 / divider16 / f - 1;                                // Calcular el valor de envoltura para la PWM.
    /// Configurar la PWM.
    cfg =  pwm_get_default_config();
    pwm_config_set_phase_correct(&cfg,true);
    pwm_config_set_clkdiv_int_frac(&cfg, divider16/16,divider16 & 0xF);
    pwm_config_set_wrap(&cfg,wrap);
    /// Habilitar interrupciones de la PWM.
    pwm_set_irq_enabled(slice_num,true);
    irq_set_enabled(PWM_IRQ_WRAP,true);
}


/**
 * @brief Configura los pines GPIO para la PWM.
 * @param slice_num Puntero al número del slice PWM.
 * @param chan Puntero al canal de la PWM.
 */
void setupPwm(uint *slice_num, uint *chan){
    gpio_set_function(28, GPIO_FUNC_PWM);
    *slice_num = pwm_gpio_to_slice_num(28);
    *chan = pwm_gpio_to_channel(28);
}


