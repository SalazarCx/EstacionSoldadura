/**
 * @file teclado.h
 * @brief Definiciones y funciones relacionadas con el manejo de un teclado matricial para la interfaz de usuario.
 */

#ifndef TECLADO_H
#define TECLADO_H

/**
 * @brief Inicializa los pines del teclado matricial.
 * @param rows Arreglo de pines para las filas del teclado.
 * @param cols Arreglo de pines para las columnas del teclado.
 */
void iniciar_pines_teclado(uint rows[],uint cols[]){
    ///@brief Configura los pines de las filas como salida
     for (int i = 0; i < 4; i++) {
        gpio_init(rows[i]);
        gpio_set_dir(rows[i], GPIO_OUT);
    }
    ///@brief Configura los pines de las columnas como entrada con pull-up.
    for (int i = 0; i < 4; i++) {
        gpio_init(cols[i]);
        gpio_set_dir(cols[i], GPIO_IN);
        gpio_pull_up(cols[i]);                
    }
}


/**
 * @brief Realiza la lectura de teclas presionadas en el teclado matricial.
 * @param rows Arreglo de pines para las filas del teclado.
 * @param cols Arreglo de pines para las columnas del teclado.
 * @param keymap Matriz que define las letras asociadas a cada tecla del teclado.
 * @param TEM Arreglo para almacenar la combinación actual de teclas presionadas.
 * @param contador_tem Puntero al contador de teclas presionadas.
 * @param TEM1 Arreglo para almacenar la combinación de teclas presionadas previamente.
 * @param control_lcd Puntero al control del LCD.
 */
void lectura_t(const uint rows[], const uint cols[], char keymap[4][4],
                          char TEM[],int *contador_tem,char TEM1[],int *control_lcd) {
    ///@brief Recorre las filas del teclado.                       
    for (int i = 0; i < 4; i++) {
        gpio_put(rows[i], 0);
        ///@brief Recorre las columnas del teclado.
        for (int j = 0; j < 4; j++) {
            if (!gpio_get(cols[j])) {
                char letra_presionada = keymap[i][j];   /// Almacena cada letra presiionada.
                ///@brief Maneja la lógica de las teclas según el estado del control del LCD
                if(*control_lcd == 1){
                    /// Si es una tecla válida y hay espacio en el arreglo, la agrega
                    if (*contador_tem < 3 && letra_presionada != 'A' && letra_presionada != 'B' && letra_presionada!= 'C' && letra_presionada != 'D' && letra_presionada != '*' && letra_presionada != '#' ) {
                        TEM[*contador_tem] = letra_presionada;
                        (*contador_tem)++;
                    } 
                    ///Maneja la tecla borrar
                    if( letra_presionada == '*'){
                        if(*contador_tem != 0){
                            (*contador_tem)--;  
                        }
                        TEM[*contador_tem] =' ';
                    }
                    ///Maneja la tecla '#' (confirmar)
                    if( letra_presionada == '#'){
                        strcpy(TEM1, TEM);        /// copia todo lo de TEM a TEM1
                        TEM[0] ='0';
                        TEM[1] =' ';
                        TEM[2] =' ';

                        ///Ajusta la variable TEM1 según la cantidad de teclas presionadas
                        if (*contador_tem == 1){
                            TEM1[1] =' ';
                            TEM1[2] =' ';
                            
                        }
                        
                        else if (*contador_tem == 2)
                        {
                            TEM1[2] =' ';
                        }

                        *contador_tem = 0;
                    }

                }


                *control_lcd = 1;  /// Activa el controlador lcd.
                sleep_ms(50);

                while (!gpio_get(cols[j]));
                sleep_ms(50);
            }
        }
        gpio_put(rows[i], 1);
    }
}

#endif  /// TECLADO_H