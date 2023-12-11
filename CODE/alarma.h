/**
 * @file alarma.h
 * @brief alarmas para el manejo de temporizadores.
 */

#define ALARM_NUM 0              ///<  Define el número de alarma como 0. Esto se utiliza más adelante para configurar la alarma específica del temporizador.
#define ALARM_IRQ TIMER_IRQ_0    ///<  Define el número de interrupción asociado con la alarma

static volatile bool alarm_fired;  ///< indica si la alarma se ha disparado

/**
 * @brief Establece el tiempo en microsegundos para la próxima ocurrencia de la alarma.
 * @param delay_us Retardo en microsegundos.
 */
void alarm_in_us(uint32_t delay_us) {
    uint64_t target = timer_hw->timerawl + delay_us;
    timer_hw->alarm[ALARM_NUM] = (uint32_t) target;  /// Calcula el tiempo de destino sumando el tiempo actual del temporizador (timer_hw->timerawl) con el retardo especificado en microsegundos.
}

/**
 * @brief Función de interrupción asociada con la alarma. Se ejecuta cuando la alarma se dispara.
 */
static void alarm_irq(void) {
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM); /// Limpia el bit de interrupción asociado con la alarma.
    alarm_fired = true;                              /// Marca que la alarma ha sido disparada.
    alarm_in_us(1000000);                            /// Configura la próxima ocurrencia de la alarma para 1 segundo
}


/**
 * @brief Función de inicialización del temporizador.
 */
void init_timer(void) {
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);   /// Asocia la función alarm_irq con la interrupción de la alarma.
    irq_set_enabled(ALARM_IRQ, true);                  /// Habilita las interrupciones 
    alarm_in_us(1000000);                              /// Configura la primera ocurrencia de la alarma para 1 segundo
}
