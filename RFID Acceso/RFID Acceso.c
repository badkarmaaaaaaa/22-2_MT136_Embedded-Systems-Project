#include <16f877a.h>
#fuses HS,NOWDT,NOPROTECT,NOPUT,NOLVP,NOBROWNOUT
#use delay(clock=20M)
#use standard_io(A)
#use standard_io(D)
#use standard_io(E)

#define led_access PIN_E0                       // Pines de los leds indicadores
#define led_error PIN_E1
#define led_prueba PIN_E2

#define LCD_DB4   PIN_D4                        // Pines de la pantalla LCD
#define LCD_DB5   PIN_D5
#define LCD_DB6   PIN_D6
#define LCD_DB7   PIN_D7
#define LCD_RS    PIN_D2
#define LCD_E     PIN_D3

#define use_servo_1           // Utiliza el servo 1
#define servo_1 PIN_D0        // Pin del servo 1
#define use_servo_2
#define servo_2 PIN_D1
#include <servo_st.c>         // Libreria para el manejo de servomotores

#define MFRC522_CS  PIN_A0                      // Pin SDA
#define MFRC522_SCK PIN_A1                      // Pin SCK
#define MFRC522_SI  PIN_A2                      // Pin MOSI
#define MFRC522_SO  PIN_A3                      // Pin MISO
#define MFRC522_RST PIN_A4                      // Pin RST
#include <RC522.h>                              // Libreria para el manejo del modulo MFRC522

#include <LCD_16X2.c>                           // Libreria para el manejo de la pantalla LCD
#define tiempo_delay 2000

int   angulo=90;
int   capacidad=3;
//float distancia = 0; 

char user[5] = {0x13,0xE2,0xE6,0x11,0x06};      // ID para acceder

char i;                                         // Contador
char UID[10];                                   // Almacena los digitos de la ID
unsigned int TagType;                           // Variable de verificacion de tag

void secuencia_acceso () {
   servo_1_write(0);
   delay_ms(500);
   servo_1_write(angulo);      // 90�
   delay_ms(tiempo_delay);
   servo_1_write(0);
   delay_ms(500);
}

void secuencia_salida () {
   servo_1_write(0);
   delay_ms(500);
   servo_1_write(angulo);      // 90�
   delay_ms(tiempo_delay);
   servo_1_write(0);
   delay_ms(500);
}

void main()
{
   output_low(led_access);                      // Inicializan los leds indicadores apagados
   output_low(led_error);
   lcd_init();                                  // Inicializa la pantalla LCD
   MFRC522_Init();                              // Inicializa el modulo RFID MFRC522
   servo_init();                                // Inicializa el servo
   
   while(true){
         lcd_gotoxy(1,1);                          // Imprime un texto breve en la pantalla LCD
         lcd_putc("Puerta 5-UNI");
         lcd_gotoxy(1,2);
         printf(lcd_putc,"Espacios disp: %d",capacidad);
         delay_ms(100);

         if(MFRC522_isCard(&TagType))              // Verificacion si hay un tag disponible
         {
            if(MFRC522_ReadCardSerial(&UID))       // Lectura y verificacion si encontro algun tag
            {
               lcd_clear();
               lcd_gotoxy(1,1);
               lcd_putc("ID: ");
               lcd_gotoxy(5,1);
               for(i=0; i<5; i++)                  // Imprime la ID en la pantalla LCD
               {
                  printf(lcd_putc, "%X", UID[i]);
               }
               
               if(MFRC522_Compare_UID(UID, user))  // Compara la ID del usuario si es correcta
               {  if(capacidad==0) {
                     lcd_clear();
                     lcd_gotoxy(1,1);                          // Imprime un texto breve en la pantalla LCD
                     lcd_putc("Estacionamiento ");
                     lcd_gotoxy(1,2);
                     lcd_putc("Lleno.");
                     delay_ms(2000);
                     lcd_clear();
                  }
                  else {
                  output_high(led_access);
                  output_low(led_error);
                  lcd_gotoxy(1,2);
                  lcd_putc("Acceso Correcto");
              
                  secuencia_acceso();
                  
                  output_low(led_access);
                  output_low(led_error);
                  lcd_clear();
                  capacidad--;
                  }}
               else                                // Si la ID no es correcta, el usuario no tiene acceso
               {  output_high(led_error);
                  output_low(led_access);
                  lcd_gotoxy(1,2);
                  lcd_putc("Acceso Denegado");
                  delay_ms(tiempo_delay);
                  output_low(led_access);
                  output_low(led_error);
                  lcd_clear();
               }
               MFRC522_Clear_UID(UID);             // Limpia temporalmente la ID
               delay_ms(100);
            }
            MFRC522_Halt();                        // Apaga la antena
         }
         delay_ms(500);
         output_high(led_prueba);
         delay_ms(1000);
         output_low(led_prueba);
         delay_ms(500);
         
         
         
   }
}
