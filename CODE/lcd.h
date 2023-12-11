/**
 * @file lcd.h
 * @brief Contiene definiciones y funciones relacionadas con el control de un LCD.
 *
 * Este archivo proporciona funciones para controlar un LCD utilizando pines 
 * específicos (RS, EN, D4, D5, D6, D7) en un sistema embebido.
 */

#ifndef LCD_H
#define LCD_H


/**
 * @brief Define el pin asociado al pin RS del LCD.
 */
#define RS_PIN 0

/**
 * @brief Define el pin asociado al pin EN del LCD.
 */
#define EN_PIN 1

/**
 * @brief Define el pin asociado al pin D4 del LCD.
 */
#define D4_PIN 2

/**
 * @brief Define el pin asociado al pin D5 del LCD.
 */
#define D5_PIN 3

/**
 * @brief Define el pin asociado al pin D6 del LCD.
 */
#define D6_PIN 4

/**
 * @brief Define el pin asociado al pin D7 del LCD.
 */
#define D7_PIN 5


/**
 * @brief Envia un nibble (4 bits) al LCD.
 *
 * @param bits El nibble que se enviará al LCD.
 * 
 * Esta función envía un nibble al LCD a través de los pines de datos D4-D7.
 */
void enviar_bits(uint8_t bits) {
    ///(D7,D6,D5,D4) se pone en alto o bajo los pines dependiendo del valor de bits por ejemplo (si es (0001) (D7_bajo,D6_bajo,D5_bajo,D4_alto) )
    /// recordar que que que bits es de 8 bits pero solo usamos 4 ya que lo configuramos para usar o enviar en dos partes, parte alta(mas significativos) y baja(manos sinificativos)
    gpio_put(D4_PIN, (bits & 0x01) != 0); /// Coloca el estado del pin D4_PIN basado en el bit menos significativo de bits. Si este bit es 1, el pin se establece en alto, si es 0, se establece en bajo.
    gpio_put(D5_PIN, (bits & 0x02) != 0); /// Similar al paso anterior, pero para el segundo bit menos significativo y el pin D5_PIN.
    gpio_put(D6_PIN, (bits & 0x04) != 0); /// Similar al paso anterior, pero para el tercer bit menos significativo y el pin D6_PIN.
    gpio_put(D7_PIN, (bits & 0x08) != 0); /// Similar al paso anterior, pero para el cuarto bit menos significativo y el pin D7_PIN.
    
    /// cuando se tiene los valores de D4_PIN a D7_PIN, se hace un pulso (1 y luego 0) con EN_PIN para habilitar la intruccion, idica a lcd que esta habilitado, los datos listos
    
    gpio_put(EN_PIN, 1);
    sleep_us(1);
    gpio_put(EN_PIN, 0);
    
    sleep_us(100); /// espera 100 us por recomendacion del fabricante 
}

/**
 * @brief Envia una instrucción al LCD.
 *
 * @param command La instrucción que se enviará al LCD.
 * 
 * Esta función envía una instrucción al LCD, como configuraciones de inicialización.
 */
void enviar_instru(uint8_t command) {
    /// (D7,D6,D5,D4) = (D3,D2,D1,D0)
    gpio_put(RS_PIN, 0);  /// Pone el pin RS_PIN en bajo (0). Esto indica que se va a enviar una instrucción a la pantalla LCD en lugar de datos.
    /// parte 1 - alta (D7,D6,D5,D4)
    enviar_bits(command >> 4); /// Envia los 4 bits más significativos de la variable command. La expresión command >> 4 desplaza los bits 4 lugares hacia la derecha, dejando solo los bits más significativos.
    /// parte 2 - baja (D3,D2,D1,D0)
    enviar_bits(command & 0x0F); /// Envia los 4 bits menos significativos de la variable command. La expresión command & 0x0F aplica una máscara para mantener solo los bits menos significativos.
    /// se hace lo anterior porque la indicamos al lcd por medio de un comando que ibamos a usarlo en modo de 4 bits es decir (enviara los datos en dos partes la parte alta (4 mas significativos) y luego parte baja (4 menos significativos)).
    sleep_ms(2); /// recomendacion del fabricante 
}

/**
 * @brief Inicializa el LCD y configura los pines.
 * 
 * Esta función inicializa los pines asociados con el LCD y lleva a cabo la secuencia 
 * de inicialización del dispositivo.
 */
void lcd_init() {
    ///Inicializa los pin definido como X_PIN para su uso.
    gpio_init(RS_PIN); 
    gpio_init(EN_PIN);
    gpio_init(D4_PIN);
    gpio_init(D5_PIN);
    gpio_init(D6_PIN);
    gpio_init(D7_PIN);

    gpio_set_dir(RS_PIN, GPIO_OUT); /// Configura el pin RS_PIN como salida para enviar señales de control a la pantalla LCD.
    gpio_set_dir(EN_PIN, GPIO_OUT); /// Configura el pin EN_PIN como salida para enviar señales de habilitación a la pantalla LCD. con un pulso 1 y luego 0
    gpio_set_dir(D4_PIN, GPIO_OUT); /// salida para enviar datos a la pantalla LCD.
    gpio_set_dir(D5_PIN, GPIO_OUT); /// salida para enviar datos a la pantalla LCD.
    gpio_set_dir(D6_PIN, GPIO_OUT); /// salida para enviar datos a la pantalla LCD.
    gpio_set_dir(D7_PIN, GPIO_OUT); /// salida para enviar datos a la pantalla LCD.

    enviar_bits(0x03);  /// comando SET (00000011) se debe enviar tres veces seguidas para inicilizar el lcd, el comando SET(pone D4 y D5 en 1 y D6,D7 mantienen en 0)
    sleep_ms(5);        /// Cuanto se manda el comando SET la primera vez se debe esperar un minimo de 4.1ms 
    enviar_bits(0x03);
    sleep_us(100);      /// Ya para la segunda y tercera vez se debe esperar un minimo de 100uS
    enviar_bits(0x03);
    sleep_us(100);
    enviar_bits(0x02); /// se manda el comando (00000010) para que D5 este en alto "1" y los demas en "0" para indicar que se va a manejar la lcd en formato de 4 bits, la lcd siempre necesita intrucciones de 8bit, lo que hace configurar para que reciba de 4bits es que se ignorara los pines D0 a D3 y enviara los datos en dos partes la parte alta (4 mas significativos) y luego parte baja (4 menos significativos).
    enviar_instru(0x28); /// instruccion (00101000) para configurar la interfaz de la pantalla LCD (modo de 4 bits activado en la intrucion anterior, 2 líneas logicas del led y 5x8 puntos que indica el tamaño matricial de cada led tamaño del caracter).
    enviar_instru(0x0C); /// instruccion o comando SHOW (00001100) para activar la pantalla (sin cursor, sin parpadeo). recorda que se manda en dos partes parte alta(0000) y parte baja(1100), (D7,D6,D5,D4)
    enviar_instru(0x01); /// instrucción (00000001) para borrar la pantalla o conocida como comando clear display.
    sleep_ms(2);         /// por recomendacion se debe esperar 2 ms
    enviar_instru(0x06); /// instrucción (00000110) para configurar el modo de entrada (cursor se mueve hacia la derecha) o conocido como Entry mode, recordar que sera en dos partes (0000)(0110).
}

/**
 * @brief Coloca un caracter en el LCD.
 *
 * @param character El caracter que se mostrará en el LCD.
 * 
 * Esta función coloca un caracter en la posición actual del cursor en el LCD.
 */
void lcd_putc(char character) {
    gpio_put(RS_PIN, 1); ///RS_PIN en alto (1) para indicar que va a enviar infromacion, datos, no instruciones 
    /// la variable character es un caracter que se reprensenta en 8bits, formato ASCII
    /// se envia la primera parte (parte alta) del caracter (4bits mas significativos)
    enviar_bits(character >> 4);
    /// se envia la segunda parte (parte baja) del caracter (4bits menos significativos)
    enviar_bits(character & 0x0F);
}

/**
 * @brief Coloca una cadena de caracteres en el LCD.
 *
 * @param string La cadena que se mostrará en el LCD.
 * 
 * Esta función coloca una cadena de caracteres a partir de la posición actual del cursor.
 */
void lcd_puts(const char *string) {
    while (*string) {         /// Este es un bucle while que se ejecutará mientras el valor apuntado por string no sea nulo. En otras palabras, este bucle continuará hasta que se llegue al final de la cadena de caracteres.
        lcd_putc(*string);    /// En cada iteración del bucle, se llama a la función lcd_putc con el carácter apuntado por string. Esto significa que se está enviando cada carácter de la cadena a la pantalla LCD.
        string++;             /// Después de enviar el carácter actual, el puntero string se incrementa para apuntar al siguiente carácter en la cadena.
    }
}
///Este bucle recorre la cadena de caracteres y envía cada uno de ellos a la pantalla LCD utilizando la función lcd_putc. Cuando se alcanza el final de la cadena (cuando se llega a un carácter nulo \0, que marca el final de una cadena de caracteres en C), el bucle se detiene y la función lcd_puts termina su ejecución.
///Esta función es útil para mostrar texto en la pantalla LCD y permite imprimir cadenas de caracteres completas en la pantalla desde la posicion actual del cursos en el lcd hasta el final de la cadena.

/**
 * @brief Mueve el cursor a una posición específica en el LCD.
 *
 * @param row La fila a la que se moverá el cursor (1 o 2).
 * @param col La columna a la que se moverá el cursor (0 a 15).
 * 
 * Esta función mueve el cursor a la posición especificada por los argumentos `row` y `col`.
 */
void lcd_move_cursor(uint8_t row, uint8_t col) {
    ///Aquí está la parte de la memoria DDRAM que normalmente se usa para mostrar caracteres en la pantalla LCD
    /// ----------------------------------------
    /// | Dirección DDRAM | Fila | Columna |    |
    /// ----------------------------------------
    /// |      0x80       |  1   |   1     |    |
    /// |      0x81       |  1   |   2     |    |
    /// |      0x82       |  1   |   3     |    |
    /// |      0x83       |  1   |   4     |    |
    /// |      0x84       |  1   |   5     |    |
    /// |      0x85       |  1   |   6     |    |
    /// |      0x86       |  1   |   7     |    |
    /// |      0x87       |  1   |   8     |    |
    /// |      0x88       |  1   |   9     |    |
    /// |      0x89       |  1   |   10    |    |
    /// |      0x8A       |  1   |   11    |    |
    /// |      0x8B       |  1   |   12    |    |
    /// |      0x8C       |  1   |   13    |    |
    /// |      0x8D       |  1   |   14    |    |
    /// |      0x8E       |  1   |   15    |    |
    /// |      0x8F       |  1   |   16    |    |
    /// |      0xC0       |  2   |   1     |    |
    /// |      0xC1       |  2   |   2     |    |
    /// |      0xC2       |  2   |   3     |    |
    /// |      0xC3       |  2   |   4     |    |
    /// |      0xC4       |  2   |   5     |    |
    /// |      0xC5       |  2   |   6     |    |
    /// |      0xC6       |  2   |   7     |    |
    /// |      0xC7       |  2   |   8     |    |
    /// |      0xC8       |  2   |   9     |    |
    /// |      0xC9       |  2   |   10    |    |
    /// |      0xCA       |  2   |   11    |    |
    /// |      0xCB       |  2   |   12    |    |
    /// |      0xCC       |  2   |   13    |    |
    /// |      0xCD       |  2   |   14    |    |
    /// |      0xCE       |  2   |   15    |    |
    /// |      0xCF       |  2   |   16    |    |
    /// ----------------------------------------

    uint8_t command = 0x80; /// comando para poner el cursor en la primera linea (10000000), puede ser 0x00 0 0x80 para DDRAM

    if (row == 1) {
        command += col; /// simplemente agrega el valor de la columna (col) al comando. Dado que la primera fila comienza en la dirección 0x80, simplemente se suma la columna para determinar la posición exacta de la celda 0 columna que se requiera.
    } else if (row == 2) {
        command += col + 0x40; /// comando para poner el cursor en la segunda linea(11000000) (0x80+0x40=0xC0) puede ser 0x40 0 0xC0 depende de como se configure la primera fila
    }

    enviar_instru(command);
}


/**
 * @brief Carga un caracter personalizado en el LCD.
 *
 * @param char_num El número de caracter personalizado (0 a 7).
 * @param char_data Los datos del caracter personalizado (8 bytes).
 * 
 * Esta función permite cargar un caracter personalizado en el LCD para su posterior uso.
 */
void lcd_load_custom_char(uint8_t char_num, const char *char_data) {
    
    uint8_t command = 0x40 | (char_num << 3); /// crean un valor de comando específico para indicar a la pantalla LCD que se está definiendo un carácter personalizado, y que este carácter se almacenará en una ubicación de memoria determinada.
    enviar_instru(command);

    for (int i = 0; i < 8; i++) {
        lcd_putc(char_data[i]);             /// bucle for que se ejecutará 8 veces, ya que los caracteres personalizados de 5x8 píxeles.
    }
}


#endif // LCD_H
