/**
 * @file ElecSolder.c
 * @brief Programa principal que controla un sistema de soldadura por calentamiento de la punta del cautín.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/adc.h"


#include "pwm.h"
#include "teclado.h"
#include "lcd.h"
#include "alarma.h"
#include "sensor.h"
#include "pid.h"



char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

uint cols[] = {6, 7, 8, 9};            ///< GPIO para las columnas del teclado
uint rows[] = {10, 11, 12, 13};        ///< GPIO para las filas del teclado

const char degree_celsius[8] = {0b11000,0b11000,0b00110,0b01001,0b01000,0b01000,0b01001,0b00110};  ///< Caracter especial de temperatura  

char TEM[4] = {'0', '\0', '\0', '\0'}; ///< Char para almacenar la temperatura que desea ingresar 
char TEM1[4] = {'5', '0', '\0', '\0'}; ///< Char para almacenar la temperatura ingresada
int TEM_USUARIO = 0;                    ///< Temperatura Actual del usuario 
int contador_tem = 0;                   ///< Contador para la los especios de el Char de temperatura
int TEM_LIMITE_ALCANZADO = 0;           ///< Bandera para limite maximo de la temperatura permitida 
int limpar_led = 0;                     ///< Limpiar el display unas sola vez
int control_lcd = 0;                    ///< Controla que se debe proyectar en el lcd


uint slice_num, chan;               
volatile uint pstart = 0;               ///< Variable para comparar la temperatura que quiere el usuario y la temperatura que se tiene  
float promTemp = 0;                     ///< Promedio del voltaje leido en el ADC
float resistencia = 0 ;                 ///< Variable para ver el valor de la resistencia del el sensor de temperatura
float temp = 0.0;                       ///< Temperatura del la punta del cauitin 
int PID_value = 100;                    ///< Crotolador para el PID

/**
 * @brief Manejador de interrupción para la actualización del PWM.
 */
void wrap_handler(){
    pwm_clear_irq(slice_num);
    //pwm_set_chan_level(slice_num, chan, wrap * pstart / 100);
    pwm_set_chan_level(slice_num, chan, wrap * PID_value / 100);
}

/**
 * @brief Función principal del programa.
 * Aqui se intrega los drives, en donde se realiza la lectura del teclado, luego se pasa al control pid y este da 
 * un valor para luego configurar el pwm y por ultimo se visualiza en el display lcd 16x02.
 * @return Devuelve 0 al finalizar el programa.
 */
int main(){
    stdio_init_all();

    setupPwm(&slice_num,&chan);
    pwm_set_freq_duty(slice_num,chan,20);                  /// funcion para la frecuencia 
    irq_set_exclusive_handler(PWM_IRQ_WRAP,wrap_handler);   /// Configura la interrupcion  -> sin reconocimiento 
    pwm_init(slice_num,&cfg,true);                          /// configuracion de los pines
    pwm_set_enabled(slice_num, true);                       /// habilita el PWM
    
    inicializarADC();
    init_timer();  
    lcd_init();
    iniciar_pines_teclado(rows, cols);  ///inicilizador de pines 


    while (1)
    {
        lectura_t(rows, cols, keymap, TEM, &contador_tem, TEM1, &control_lcd); ///Funcion para la lectura del teclado y almacenamiento de variables
        
        
        if (atoi(TEM1) > 350 && !TEM_LIMITE_ALCANZADO) {
            printf("TEMPERATURA MAXIMA 350\n");  
            TEM_USUARIO = 350;
            TEM1[0] ='3';
            TEM1[1] ='5';
            TEM1[2] ='0';
            TEM_LIMITE_ALCANZADO = 1;               /// Activa la bandera -> Se alcanzó el límite

        } else if (atoi(TEM1) <= 350) {
            TEM_LIMITE_ALCANZADO = 0;               /// Si el valor es menor a 350, se puede ajustar nuevamente
            TEM_USUARIO = atoi(TEM1);

            if (TEM_USUARIO < 50){
                TEM_USUARIO = 50;
                TEM1[0] ='5';
                TEM1[1] ='0';
                TEM1[2] =' ';
            }

        }


        pstart = TEM_USUARIO;

        sensorTermistor(&resistencia ,&promTemp, &temp);          /// Funcion para encontrar los valores de resistencia, voltaje y temperatura, lectura del sensor por el ADC
        controlPid(pstart, &temp, &PID_value);                    /// Funcion para realizar el contro PID y que la Temperatura no se desborde ni disminuya de la ingresada por el usuario


        if (alarm_fired) {

            
            printf("Voltaje Diferencial: %.2f \n", promTemp);
            printf("Temperatura: %.2f \n", temp);
            printf("resistencia: %.2f \n", resistencia);
            printf("PWM: %d \n", PID_value);
            printf("Start: %d \n", pstart);
            
           

            if(control_lcd ==1){

                if(limpar_led == 0) {
                    enviar_instru(0x01);
                    limpar_led = 1;
                }

                lcd_move_cursor(1, 0);
                lcd_puts("TEMP I:");
                lcd_move_cursor(1,10);
                lcd_puts(TEM); 
                lcd_load_custom_char(0, degree_celsius);
                lcd_move_cursor(1,13);
                lcd_putc(0);


                lcd_move_cursor(2, 0);
                lcd_puts("TEMP A:");
                lcd_move_cursor(2,10);
                lcd_puts(TEM1); 
                lcd_load_custom_char(0, degree_celsius);
                lcd_move_cursor(2,13);
                lcd_putc(0);
            }

            else{
                lcd_move_cursor(1, 1);
                lcd_puts("Welcome to"); 
                lcd_move_cursor(2, 5);
                lcd_puts("ElecSolder");   
            }

            alarm_fired = false;     /// Reiniciar la bandera de alarma
        
        }


    }
    
    return 0;
}

