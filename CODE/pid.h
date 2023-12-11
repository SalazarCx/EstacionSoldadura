/**
 * @file pid.h
 * @brief Control PID para el mantenimiento de temperatura en un cautín.
 *
 * Este archivo contiene el código para implementar un control PID que mantiene la temperatura
 * de un cautín en un valor objetivo. Se definen variables para el control PID, constantes ajustadas
 * a través de MATLAB, y una función `controlPid` que realiza el cálculo del control PID.
 *
 */

float PID_error = 0, previous_error = 0;           ///< Variables para el control de error.
uint32_t current_time=0, timePrev=0, elapsedTime=0;///< Variables para el manejo de tiempo para el control derivativo.
int kp = 9.1;   int ki = 0.3;   int kd = 1.8;      ///< Varibles de las constantes para el control proporcional, integral y derivativo.
int PID_p = 0;    int PID_i = 0;    int PID_d = 0; ///< Variables para almacenar el valor calculado para cada control.

/**
 * @brief Realiza el control PID para mantener la temperatura en un valor deseado.
 * @param setTemp Temperatura objetivo.
 * @param tempCautin Puntero a la temperatura actual del cautín.
 * @param PID_value Puntero al valor de salida del control PID (rango entre 0 y 100).
 */
void controlPid(uint setTemp, float *tempCautin, int *PID_value){
  PID_error = setTemp - *tempCautin;    
  PID_p = kp * PID_error;               /// Calcula el control porcional; ajustal al velocidad en la que actua el sensor, en funcion de la diferencia


  /// Calcula PID_i, control integral en un ragno de +-3
  if(-3 < PID_error <3)
  {
    PID_i = PID_i + (ki * PID_error);   /// corrige el error acomulado, mas precision.
  }

  /// Para el término derivativo, necesitamos el tiempo real para calcular la tasa de cambio de velocidad
  timePrev = current_time;                            /// El tiempo anterior se almacena antes de leer el tiempo actual

  current_time = time_us_32();
  elapsedTime = (current_time - timePrev) / 1000; 

  /// Ahora podemos calcular el valor de D
  PID_d = kd*((PID_error - previous_error)/elapsedTime);   ///ajusta la velecidad con la en la que esta cambiando la temperatura, Taza de cambio de la temperatura 

  
  ///El valor total final del PID es la suma de P + I + D
  *PID_value = PID_p + PID_i + PID_d;

  ///Definimos el rango PWM entre 0 y 100
  if(*PID_value < 0)
  {    *PID_value = 0;    }
  if(*PID_value > 100)  
  {    *PID_value = 100;  }


  previous_error = PID_error;     /// Recuerda almacenar el error anterior para el próximo bucle. lazo de retroalimentacion
}