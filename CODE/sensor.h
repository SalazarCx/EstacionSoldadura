/**
 * @file sensor.h
 * @brief Lectura de un sensor de termistor mediante un ADC con comentarios Doxygen.
 *
 * Este archivo contiene funciones para inicializar el ADC y leer un sensor de termistor,
 * realizando conversiones de valores ADC a resistencia y temperatura.
 */
#define SENSOR_TERMISTOR 26   //< Declaracion del GPIO para el ADC
#define V_REF 4.96             //< Voltaje de referencia para hacer las transformaciones 


float promedio_Suma = 0.0;    ///< Variable sumatoria para realizar el promedio 
int muestras = 100;           ///< Muestras para realizar el promedio 
float correccion = 0.02;      ///< Para ajustar el valor entregador por el ADC y el obtenido con el multimetro


/**
 * @brief Inicializa el ADC para la lectura del sensor de termistor.
 */
void inicializarADC(){
    adc_init();
    adc_gpio_init(SENSOR_TERMISTOR );
};


/**
 * @brief Lee el sensor de termistor y calcula la resistencia y la temperatura.
 * @param resistencia Puntero para almacenar el valor de resistencia calculado.
 * @param promTemp Puntero para almacenar el valor promedio de temperatura calculado.
 * @param temp Puntero para almacenar el valor de temperatura calculado.
 */
void sensorTermistor(float *resistencia ,float *promTemp,float *temp){
    ///@brief se estan tomando 100 muestras con el fin de mejorar la precision de la lectura.
    for (int i = 0; i < muestras; i++)
    {
        uint16_t Temperatura = adc_read();
        float voltage_t = (Temperatura / 4095.0) * 3.3;     /// El valor leido por el adc se pasa a voltaje.
        promedio_Suma += voltage_t;                         /// Se suma el valor de voltaje con el fin de luego ser promediado.
    }

    *promTemp = ((promedio_Suma/muestras)-correccion);      /// Se obtiene el valor final de voltaje y se le resta el valor de correccion para disminuir el error.
    
    ///@brief ese voltaje se convierte a un valor de resistencia con el objetivo de tener la relacion con la temperatura.
    float promedio_configurado = (2.40 + *promTemp);         
    float resistance = -((promedio_configurado * 51)/(promedio_configurado-V_REF))-1;
    *resistencia=resistance;
    *temp = (2.201f * resistance) - 81.818f;
    //*temp = (2.4429 * resistance) - 114.9;
    //*temp = (3.2198 * resistance) - 140.06;
    //*temp = (2.5116 * resistance) - 104.53;
    //*temp =  (2.4403 * resistance) - 105.44;
    promedio_Suma = 0;

}

    