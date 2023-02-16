/******************************************************************
Universidad del Valle de Guatemala
IE2023 Programación de Microcontroladores
Autor: Fernando Donis
Compilador: XC8, MPLAB X IDE (v6.00)
Proyecto: Laboratorio 4
Hardware: PIC16F887
Creado: 09/02/2023
Última Modificación: 09/02/2023
******************************************************************/
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "I2C.h"
#include "LCD8.h"
#include <xc.h>
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 4000000
#define RS RC0
#define EN RC1
float vADC;
uint8_t ADC, t, va, seg, min, hor, dia, mes, an;
int ce, de, u;
char unid, decen, cent;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
char intachar(uint8_t n);
uint8_t deshacer(int p, uint16_t n);
uint16_t map(uint8_t v, uint8_t minen, uint8_t maxen, uint8_t minsa,uint16_t maxsa);
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    Lcd8_Init();        //Inicio lcd
    Lcd8_Clear();       //Limpiar lcd
    Lcd8_Set_Cursor(1,2);   
    Lcd8_Write_String("pot:");   //escribir S1
    while(1){
        I2C_Master_Start();         //Iniciar comunicación
        I2C_Master_Write(0x41);     //Dispositivo con el cual se comunica
        ADC = I2C_Master_Read(0);   //Leer Dato del Buffer
        I2C_Master_Stop();          //Parar comunicación
        PORTB = ADC;
        //Conversión ADC
        vADC = map(ADC, 0, 255, 0, 500);
        unid = intachar(deshacer(2, vADC));
        Lcd8_Set_Cursor(2,1);
        Lcd8_Write_Char(unid);  //Escribir unidades ADC
        Lcd8_Set_Cursor(2,2);
        Lcd8_Write_String(".");
        decen = intachar(deshacer(1, vADC));
        Lcd8_Set_Cursor(2,3);
        Lcd8_Write_Char(decen); //Escribir decenas ADC
        cent = intachar(deshacer(0, vADC));
        Lcd8_Set_Cursor(2,4);
        Lcd8_Write_Char(cent);  //Escribir centenas ADC
        __delay_ms(200);
        
        //Iniciar comunicación
        I2C_Master_Start();    
        I2C_Master_Write(0xD0);         //Dispositivo     
        I2C_Master_Write(0x00);         //Dirección del registro
        I2C_Master_RepeatedStart();     //Repetir inicio de comunicación
        I2C_Master_Write(0xD1);         //Leer dirección
        seg = I2C_Master_Read(0);       //Almacenar información
        
        Lcd8_Set_Cursor(1,14);          //Escribir dato
        Lcd8_Write_Char(seg);
        
        I2C_Master_Start();    
        I2C_Master_Write(0xD0);     
        I2C_Master_Write(0x01);        
        I2C_Master_RepeatedStart();    
        I2C_Master_Write(0xD1);  
        min = I2C_Master_Read(0);
        
        Lcd8_Set_Cursor(1,11);
        Lcd8_Write_Char(seg);
        
        I2C_Master_Start();    
        I2C_Master_Write(0xD0);     
        I2C_Master_Write(0x02);        
        I2C_Master_RepeatedStart();    
        I2C_Master_Write(0xD1);  
        hor = I2C_Master_Read(0);  
        
        Lcd8_Set_Cursor(1,8);
        Lcd8_Write_Char(seg);
        
        I2C_Master_Start();    
        I2C_Master_Write(0xD0);     
        I2C_Master_Write(0x04);        
        I2C_Master_RepeatedStart();    
        I2C_Master_Write(0xD1);  
        dia = I2C_Master_Read(0);  
        
        Lcd8_Set_Cursor(2,8);
        Lcd8_Write_Char(dia);
        
        I2C_Master_Start();    
        I2C_Master_Write(0xD0);     
        I2C_Master_Write(0x05);        
        I2C_Master_RepeatedStart();    
        I2C_Master_Write(0xD1);  
        mes = I2C_Master_Read(0);  
        
        Lcd8_Set_Cursor(2,11);
        Lcd8_Write_Char(mes);
        
        I2C_Master_Start();    
        I2C_Master_Write(0xD0);     
        I2C_Master_Write(0x06);        
        I2C_Master_RepeatedStart();    
        I2C_Master_Write(0xD1);  
        an = I2C_Master_Read(0);  
        
        Lcd8_Set_Cursor(2,14);
        Lcd8_Write_Char(an);
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0;
    TRISD = 0;
    PORTB = 0;
    PORTD = 0;
    TRISC = 0;
    PORTC = 0;
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
}

char intachar(uint8_t n){
    if(n == 0){
        return '0';
    }
    else if(n == 1){
        return '1';
    }
    else if(n == 2){
        return '2';
    }
    else if(n == 3){
        return '3';
    }
    else if(n == 4){
        return '4';
    }
    else if(n == 5){
        return '5';
    }
    else if(n == 6){
        return '6';
    }
    else if(n == 7){
        return '7';
    }
    else if(n == 8){
        return '8';
    }
    else if(n == 9){
        return '9';
    }
}

uint8_t deshacer(int p, uint16_t nume){
    uint8_t cente;
    uint8_t dec;
    uint8_t uni;
    cente = (nume/100);
    dec = (nume%100);
    uni = (dec%10);
    if(p == 2){
        return cente;
    }
    else if(p == 1){
        return (dec/10);
    }
    else if (p == 0){
        return uni;
    }  
    
}


uint16_t map(uint8_t v, uint8_t minen, uint8_t maxen, uint8_t minsa,uint16_t maxsa){
    uint16_t resultado;
    resultado = (((maxsa-minsa)* (long long)(v-minen))/maxen)+minsa;
    return resultado;
}