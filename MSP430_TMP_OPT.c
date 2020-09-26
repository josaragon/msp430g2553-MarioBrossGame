/*
 * MSP430_TMP_OPT.c
 *
 *  Created on: 12/11/2015
 *      Author: Manolo
 */
#include <msp430.h>
#include "MSP430_TMP_OPT.h"
#include <stdint.h>


volatile char I2C_RW=0;
char FinTx=0;
char I2C_DATO_RX;
char Numdatos;
char dato[3];
char BUFF_RX[2];

char P1SEL2_G,
	P1SEL_G,
	UCB0CTL1_G,
	UCB0CTL0_G,
	UCB0BR0_G,
	IE2_G;



void Conf_I2C_TMP006(void){
		P1SEL2 |=  BIT6 | BIT7;
	    P1SEL |=  BIT6 | BIT7;
	    UCB0CTL1 |= UCSWRST;
	    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
	    UCB0CTL1 = UCSSEL_2 | UCSWRST;
	    UCB0I2CSA = 80;
	    UCB0BR0 = 80;
	    UCB0CTL1 &= ~UCSWRST;
	    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG  );
        IE2 |= UCB0TXIE | UCB0RXIE;
        __bis_SR_register(GIE);
}



int Lee_Temp_TMP006(void){
	int Config;

	Esc_TMP006(0x2,0xF200);		//Reset, Modo continuo, Conv. rate: 2s/sec.
	_delay_cycles(6000);
   	Esc_TMP006(0x2,0x7200);		// Modo continuo, Conv. rate: 2s/sec.

   	do{
   		_delay_cycles(100);
   		Config=Lee_TMP006(0x02);
	 	}while(!(Config&0x0080));
   	Config=Lee_TMP006(0x01);

   	return(Config);
}

void Esc_TMP006(char dir, uint16_t dato_w)
{
	Numdatos=3;		//Dir: 1 byte, datos: 2 bytes
	UCB0I2CSA=0x40;
	dato[1]=(char)((dato_w&0xFF00)>>8);	// MSB del dato.
	dato[0]=(char)(dato_w&0x00FF);			// LSB del dato.
	dato[2]=dir;						//Direccion
	I2C_RW=0;					//Modo escritura
	UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start
	while(I2C_RW!=3);			//Espera al fin
}


uint16_t Lee_TMP006(char dir)
{
	UCB0I2CSA=0x40;
	Numdatos=1;		//Dir: 2 bytes.
	dato[0]=dir;
	I2C_RW=1;					//Modo lectura
	UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start
	while(I2C_RW!=3);
	return((BUFF_RX[1]<<8)+BUFF_RX[0]);
}

int Ints=0;

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR_HOOK(void)
{
	Ints++;


	if(IFG2&UCB0TXIFG){		//Rutina de Int. de transmision

		if(I2C_RW==0)		//SI CICLO DE ESCRITURA...
		{
			if (Numdatos) {      // Si quedan datos
				UCB0TXBUF = dato[Numdatos-1];  // Manda el dato
				Numdatos--;        // decrementa el índice
			}
			else {			//Si no quedan
				UCB0CTL1 |= UCTXSTP; // Manda un stop
				I2C_RW=3;		//Señaliza la var. aux.
			}
		}
		else if(I2C_RW==1)	//SI CICLO DE LECTURA...
		{
			if(Numdatos)	//si quedan datos...
			{
				UCB0TXBUF=dato[Numdatos-1];
				Numdatos--;
			}
			else	//Fin de la Tx de la dir. Pasamos a Rx...
			{
				Numdatos=2;
				UCB0CTL1&=~UCTR;		//Modo Rx
				UCB0CTL1 |= UCTXSTT; 	// Manda un start
				while(UCB0CTL1&UCTXSTT); //Espera a que salga...
		//		UCB0CTL1 |= UCTXSTP; 	// Manda un Stop (genera el nack automaticamente
			}
		}
		IFG2 &= ~UCB0TXIFG; // borra el flag de int. de Tx

	}
	else if(IFG2&UCB0RXIFG){// rutina de recepcion por interrupciones
		if(Numdatos){
			Numdatos--;
			BUFF_RX[Numdatos]=UCB0RXBUF;	// Lee el dato
		}
		if(Numdatos==1)	UCB0CTL1 |= UCTXSTP; 	// Manda un Stop (genera el nack automaticamente
		if(Numdatos==0){
			I2C_RW=3;				// Señaliza fin del proceso.

		}
	}



}


void guarda_conf(){

	P1SEL2_G=P1SEL2;
	P1SEL_G=P1SEL;
	UCB0CTL1_G=UCB0CTL1;
	UCB0CTL0_G=UCB0CTL0;
	UCB0BR0_G=UCB0BR0;
	IE2_G=IE2;
	}

void restaura_conf()
{
	P1SEL2=P1SEL2_G;
	P1SEL=P1SEL_G;
	UCB0CTL1=UCB0CTL1_G;
	UCB0CTL0=UCB0CTL0_G;
	UCB0BR0=UCB0BR0_G;
	IE2=IE2_G;

}


void Conf_I2C_OPT3001(void){
		P1SEL2 |=  BIT6 | BIT7;
	    P1SEL |=  BIT6 | BIT7;
	    UCB0CTL1 |= UCSWRST;
	    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
	    UCB0CTL1 = UCSSEL_2 | UCSWRST;
	    UCB0I2CSA = OPT3001_SLAVE_ADDRESS;
	    UCB0BR0 = 80;
	    UCB0CTL1 &= ~UCSWRST;
	    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG  );
        IE2 |= UCB0TXIE | UCB0RXIE;
        __bis_SR_register(GIE);
}


void Esc_OPT3001(char dir, uint16_t dato_w)
{
	Numdatos=3;		//Dir: 1 byte, datos: 2 bytes
	UCB0I2CSA=OPT3001_SLAVE_ADDRESS;
	dato[1]=(char)((dato_w&0xFF00)>>8);	// MSB del dato.
	dato[0]=(char)(dato_w&0x00FF);			// LSB del dato.
	dato[2]=dir;						//Direccion
	I2C_RW=0;					//Modo escritura
	UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start
	while(I2C_RW!=3);			//Espera al fin
}


uint16_t Lee_OPT3001(char dir)
{
	UCB0I2CSA=OPT3001_SLAVE_ADDRESS;
	Numdatos=1;		//Dir: 2 bytes.
	dato[0]=dir;
	I2C_RW=1;					//Modo lectura
	UCB0CTL1 |= UCTR + UCTXSTT; // I2C TX, start
	while(I2C_RW!=3);
	return((BUFF_RX[1]<<8)+BUFF_RX[0]);
}

void OPT3001_init()
{
	Conf_I2C_OPT3001();
		Esc_OPT3001(CONFIG_REG, DEFAULT_CONFIG_100);
}


unsigned long int OPT3001_getLux()
{

    uint16_t exponent = 0;
    uint32_t result = 0;
    int16_t raw;
    raw = Lee_OPT3001(RESULT_REG);
    /*Convert to LUX*/
    //extract result & exponent data from raw readings
    result = raw&0x0FFF;
    exponent = (raw>>12)&0x000F;
    //convert raw readings to LUX
    switch(exponent){
    case 0: //*0.015625
        result = result>>6;
        break;
    case 1: //*0.03125
        result = result>>5;
        break;
    case 2: //*0.0625
        result = result>>4;
        break;
    case 3: //*0.125
        result = result>>3;
        break;
    case 4: //*0.25
        result = result>>2;
        break;
    case 5: //*0.5
        result = result>>1;
        break;
    case 6:
        result = result;
        break;
    case 7: //*2
        result = result<<1;
        break;
    case 8: //*4
        result = result<<2;
        break;
    case 9: //*8
        result = result<<3;
        break;
    case 10: //*16
        result = result<<4;
        break;
    case 11: //*32
        result = result<<5;
        break;
    }
    return result;
}
