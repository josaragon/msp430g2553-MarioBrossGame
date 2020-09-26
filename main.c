#include <msp430.h> 
#include "grlib.h"
#include "Crystalfontz128x128_ST7735.h"
#include "HAL_MSP430G2_Crystalfontz128x128_ST7735.h"
#include <stdio.h>
#include "partituras.h"
#include "MSP430_TMP_OPT.h"

/*
 * main.c
 */
unsigned int i,j;
char n,s,p;//variables de la duracion de las notas y la reproduccion del fondo
int Temp, Temp_d;// variables usadas para obtener un valor numerico de la temperatura
float fTemp;// variable de la temperatura
int DeviceID;//variable de los sensores
unsigned long int Luz;//variable de la cantidad de luz
char x,y,x_ant,y_ant,mueve,t=0,duracion,noche;
Cancion cancionguay;
char *cancion;
int ejex;
int sentido;
//void obstaculos(void);
void fondo(void);//funcion que usamos para representar el fondo del juego en funcion de los sensores
void frio(void);//funcion que usamos cuando el sensor de temperatura es inferior a cierta temperatura
void calor(void);//funcion que usamos cuando el sensor de temperatura es superior a cierta temperatura
void musica(void);
void salto_mario(char x, char y,char y_ant, char x_ant, int sentido);
void dibuja_mario(char x,char y, int sentido);
void borra_mario(char x,char y,char x_ant,char y_ant, int sentido);
void conf_reloj(char VEL){
	BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;
	switch(VEL){
	case 1:
		if (CALBC1_1MHZ != 0xFF) {
			DCOCTL = 0x00;
			BCSCTL1 = CALBC1_1MHZ;      /* Set DCO to 1MHz */
			DCOCTL = CALDCO_1MHZ;
		}
		break;
	case 16:
		if (CALBC1_16MHZ != 0xFF) {
			__delay_cycles(100000);
			DCOCTL = 0x00;
			BCSCTL1 = CALBC1_16MHZ;     /* Set DCO to 16MHz */
			DCOCTL = CALDCO_16MHZ;
		}
		break;
	}
	BCSCTL1 |= XT2OFF | DIVA_0;
	BCSCTL3 = XT2S_0 | LFXT1S_2 | XCAP_1;
}

void lee_ch(char canal){
	ADC10CTL0 &= ~ENC;					//deshabilita el ADC
	ADC10CTL1&=(0x0fff);				//Borra canal anterior
	ADC10CTL1|=canal<<12;				//selecciona nuevo canal
	ADC10CTL0|= ENC;					//Habilita el ADC
	ADC10CTL0|=ADC10SC;					//Empieza la conversión
	LPM0;								//Espera fin en modo LPM0
}

void inicia_ADC(char canales){
	ADC10CTL0 &= ~ENC;		//deshabilita ADC
	ADC10CTL0 = ADC10ON | ADC10SHT_3 | SREF_0|ADC10IE; //enciende ADC, S/H lento, REF:VCC, con INT
	ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_0 | SHS_0 | INCH_0;
	//Modo simple, reloj ADC, sin subdivision, Disparo soft, Canal 0
	ADC10AE0 = canales; //habilita los canales indicados
	ADC10CTL0 |= ENC; //Habilita el ADC
}
Graphics_Context g_sContext;
char x_pos,y_pos,x,y,x_m,y_m;
const char mario[16][12]={{0,0,0,0,1,1,1,1,0,0,0,0},
		{0,0,0,1,1,1,1,1,1,1,1,0},
		{0,0,3,3,3,4,4,6,4,0,0,0},
		{0,3,4,3,4,4,4,6,4,4,4,0},
		{0,3,4,3,3,4,4,4,3,4,4,4},
		{0,3,3,4,4,4,4,3,3,3,3,0},
		{0,0,0,4,4,4,4,4,4,4,0,0},
		{0,0,1,1,1,1,1,1,1,0,0,0},
		{0,1,1,1,2,1,1,2,1,1,1,0},
		{1,1,1,1,2,2,2,2,1,1,1,1},
		{4,4,1,2,5,2,2,5,2,1,4,4},
		{4,4,4,2,2,2,2,2,2,4,4,4},
		{4,4,2,2,2,2,2,2,2,2,4,4},
		{0,0,2,2,2,0,0,2,2,2,0,0},
		{0,3,3,3,0,0,0,0,3,3,3,0},
		{3,3,3,3,0,0,0,0,3,3,3,3}};

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	conf_reloj(16);
	P2DIR|=BIT6;
	P2SEL|=BIT6;
	P2SEL2&=~BIT6;
	P2SEL&=~BIT7;
	P2SEL2&=~BIT7;
	TA0CCTL1=OUTMOD_7;
	TA0CTL=TASSEL_2 | MC_1;
	TA0CCR0=15999;
	TA0CCR1=7999>>8;
	i=0; //Esta variable nos permite recorrer el vector de notas de cada cancion
	j=1;
	p=1;
	noche=0;
	Crystalfontz128x128_Init();
	/* Set default screen orientation */
	Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
	/* Initializes graphics context */
	Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
	Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
	Graphics_clearDisplay(&g_sContext);
	P1OUT = BIT1;
	P1REN = BIT1;
	//P1IES = BIT1;
	//P1IFG = 0;
	//P1IE = BIT1;
	P1OUT = BIT2;
	P1REN = BIT2;
	P1IES = BIT2;
	P1IFG = 0;
	P1IE = BIT2;
	x=20;
	x_ant=19;
	y_ant=120;
	sentido=0;
	y=120;
	inicia_ADC(BIT3);
	TA1CCTL0 = CM_0 | CCIS_0 | OUTMOD_4 | CCIE;
	TA1CTL=TASSEL_1| MC_1;		//ACLK, DIV=1, UP
	TA1CCTL1=OUTMOD_0;
	TA1CCR0=800;		//periodo=100*12
	cancionguay=Mario1; //Escogemos la cancion que sonara de antemano

	__bis_SR_register(GIE); //Activamos las interrupciones
	while(1){
		TA0CCR0=Nota[cancionguay.notas[i]]; //La frecuencia de cada nota será recorrida de esta forma
		if(cancionguay.notas[i]==SIL) TA0CCR1=1; //Si hay un silencio, bajamos el volumen al minimo
		else{
			TA0CCR1=TA0CCR0>>6; //Ante cualquier otra nota, escogemos un volumen de esta forma
		}
		duracion=cancionguay.notas[j]; //Con esto recorremos la duracion de cada nota individual
		i=i+2;
		j=j+2;
		t=0;
		LPM0; //Entramos en bajo consumo
		if(!(P1IN&BIT1)){
			if(mueve==1 && sentido==0){mueve=0;x-=1;}
			if(mueve==1 && sentido==-1){mueve=0;x+=1;}
			salto_mario(x,y,x_ant,y_ant,sentido);
		}

		if(t>=duracion){lee_ch(0);}
		else{lee_ch(0); LPM0;}
		if(cancionguay.notas[i]==0){i=0; j=1;} //En el momento que termine la cancion, vuelve a comenzar
		//obstaculos();
	}
}

void dibuja_mario(char x,char y, int sentido){
	char x_pos,y_pos,x_m,y_m;
	for(y_pos=y-15,y_m=0; y_m<15; y_pos++, y_m++){
		for(x_pos=x-11,x_m=0; x_m<11; x_pos++, x_m++){
			switch(mario[y_m][-1*sentido*11+(2*sentido+1)*x_m]){
			case 0: //Transparente
				break;

			case 1: //Rojo
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_RED);
				Graphics_drawPixel(&g_sContext,x_pos,y_pos);
				break;

			case 2: //Azul
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_BLUE);
				Graphics_drawPixel(&g_sContext,x_pos,y_pos);
				break;

			case 3: //Marron
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_BROWN);
				Graphics_drawPixel(&g_sContext,x_pos,y_pos);
				break;

			case 4: //Carne
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_SALMON);
				Graphics_drawPixel(&g_sContext,x_pos,y_pos);
				break;

			case 5: //Amarillo
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_YELLOW);
				Graphics_drawPixel(&g_sContext,x_pos,y_pos);
				break;

			case 6: //Negro
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_BLACK);
				Graphics_drawPixel(&g_sContext,x_pos,y_pos);
				break;
			}
		}
	}
}


void borra_mario(char x,char y,char x_ant,char y_ant, int sentido){
	char yvar;
	if (sentido == 0){
		if(noche==0){
			for(yvar=y_ant;yvar>y_ant-16;yvar--){
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_WHITE);
				Graphics_drawLine(&g_sContext,x_ant,yvar,x_ant-12,yvar);
			}
		}
		else{

			for(yvar=y_ant;yvar>y_ant-16;yvar--){
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_ORANGE_RED);
				Graphics_drawLine(&g_sContext,x_ant,yvar,x_ant-12,yvar);
			}
		}}
	else{
		if(noche==0){
			for(yvar=y;yvar>y-16;yvar--){
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_WHITE);
				Graphics_drawLine(&g_sContext,x-10,yvar,x+10,yvar);
			}}
		else{
			for(yvar=y;yvar>y-16;yvar--){
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_ORANGE_RED);
				Graphics_drawLine(&g_sContext,x-10,yvar,x+10,yvar);
			}
		}
	}
}



#pragma vector=ADC10_VECTOR
__interrupt void ConvertidorAD(void)
{
	ejex=ADC10MEM;
	if(ejex>800){
		sentido=0;
		x+=7;
		mueve=1;
	}
	if(ejex<200){
		if(sentido == 0) {borra_mario(x,y,x_ant,y_ant,sentido); sentido = -1; dibuja_mario(x,y,sentido);}
		else{
			sentido=-1;
			x-=7;
			if(x<=10) x+=7;
			mueve=1;
		}
	}
	borra_mario(x,y,x_ant,y_ant,sentido);
	dibuja_mario(x,y,sentido);
	x_ant=x;
	y_ant=y;
}

void salto_mario(char x_actual, char y_actual ,char y_ant, char x_ant, int sentido){
	char y_salto,x_salto;
	if (ejex>200 && ejex<800){
		for (y_salto=y_actual;y_salto>y_actual-20;y_salto-=1){
			borra_mario(x_actual,y_salto,x_ant,y_ant,0*sentido);
			dibuja_mario(x_actual,y_salto,sentido);
			__delay_cycles(30000);
			x_ant=x_actual;
			y_ant=y_salto;
		}
		for(y_salto=y_actual-20;y_salto<=y_actual;y_salto+=1){
			borra_mario(x_actual,y_ant,x_ant,y_ant,0*sentido);
			dibuja_mario(x_actual,y_salto,sentido);
			__delay_cycles(30000);
			x_ant=x_actual;
			y_ant=y_salto;
		}
	}
	if(ejex>800){
		for(y_salto=y,x_salto=x;y_salto>y-20;y_salto-=1,x_salto+=1){
			borra_mario(x_salto,y_salto,x_ant,y_ant,sentido);
			dibuja_mario(x_salto,y_salto,sentido);
			__delay_cycles(30000);
			x_ant=x_salto;
			y_ant=y_salto;
		}
		for(y_salto=y-20;y_salto<=y;y_salto+=1,x_salto+=1){
			borra_mario(x_salto,y_ant,x_ant,y_ant,sentido);
			dibuja_mario(x_salto,y_salto,sentido);
			__delay_cycles(30000);
			x_ant=x_salto;
			y_ant=y_salto;
		}
		x=x_salto;
	}
	if(ejex<200){
		for(y_salto=y,x_salto=x;y_salto>y-20;y_salto-=1,x_salto-=1){
			borra_mario(x_salto,y_salto,x_ant,y_ant,sentido);
			dibuja_mario(x_salto,y_salto,sentido);
			__delay_cycles(30000);
			x_ant=x_salto;
			y_ant=y_salto;
		}
		for(y_salto=y-20;y_salto<=y;y_salto+=1,x_salto-=1){
			borra_mario(x_salto,y_salto-7,x_ant,y_ant,sentido);
			dibuja_mario(x_salto,y_salto,sentido);
			__delay_cycles(30000);
			x_ant=x_salto;
			y_ant=y_salto;
		}
		x=x_salto;
	}
}

/*void obstaculos(void){
	char mario_obs[10]={0,0,0,1,2,1,2,3,0,0};
	char x_ob,y_ob,m;
	for(x_ob=4,m=0;m<=10;x_ob+=12,m++){
		if(mario_obs[m]>0) {
			for(y_ob=0;y_ob<=mario_obs[m]*12;y_ob++){
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_DARK_GOLDENROD);
				Graphics_drawLineH(&g_sContext,x_ob,x_ob+12,120-y_ob);
			}
		}
	}
}*/
//interrupcion boton 2
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR_HOOK(void)
{
	//if(mueve==1 && sentido==0){mueve=0;x-=1;}
	//if(mueve==1 && sentido==-1){mueve=0;x+=1;}
	//salto_mario(x,y,x_ant,y_ant,sentido);
	P1IFG&=~BIT2;
	while(!(P1IN&BIT2));
	fondo();

}


void fondo (void){
	CS_HIGH;			//Deshabilita la pantalla
	guarda_conf();		//Almacena la config. de la USCI (para la pantalla)
	Conf_I2C_TMP006();	//Configura la usci en modo I2C, apuntando al TMP006
	Temp=Lee_Temp_TMP006();	//Lee la temperatura del sensor
	Temp=Temp>>2;		//Convierte el valor a grados, con 3 decimales
	fTemp=(float)Temp/32.0;
	OPT3001_init();		//Configura el I2C apuntando al OPT3001
	DeviceID=Lee_OPT3001(DEVICEID_REG);
	Luz=OPT3001_getLux();	//Lee la luminosidad
	Temp=(int)fTemp;
	restaura_conf();

	CS_LOW;				//Habilita la pantalla
	if(Luz<30){
		Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
		Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_ORANGE_RED);
		Graphics_clearDisplay(&g_sContext);
		noche=1;
		if(Temp>=25.100)//comprobamos si la temperatura supera la temperatura indicada en grados
		{

			for(n=2;n<=60;n=n+6){
				for(s=2;s<=30;s=s+6){
					Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_ORANGE_RED);
					Graphics_drawPixel(&g_sContext,2*n,2*s);
					calor();


				}}
		}
		else{

			Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_ORANGE_RED);//dibujamos un sol amarillo
			Graphics_fillCircle(&g_sContext,24,24,20);
			frio();
		}
	}

	else{
		Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
		Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
		Graphics_clearDisplay(&g_sContext);
		noche=0;
		if(Temp>=25.100)//comprobamos si la temperatura supera la temperatura indicada en grados
		{

			for(n=2;n<=60;n=n+6){
				for(s=2;s<=30;s=s+6){
					Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_WHITE);
					Graphics_drawPixel(&g_sContext,2*n,2*s);}}
			calor();}
		else{

			Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_WHITE);//dibujamos un sol amarillo
			Graphics_fillCircle(&g_sContext,24,24,20);
			frio();
		}
	}
	CS_LOW;
}




void calor(void){

	//limpiamos el fondo del frio si se ha dibujado previamente

	Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_YELLOW);//dibujamos un sol amarillo
	Graphics_fillCircle(&g_sContext,24,24,20);
}




void frio(void){
	//Hacemos mediante la variable p el efecto de caida de la nieve dibujando alternamente 2 y 3 filas de pixeles
	for(n=2;n<=60;n=n+6){
		for(s=2;s<=30;s=s+12){//filas impares
			if(p%2!=0){
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_BLUE);
				Graphics_drawPixel(&g_sContext,2*n,2*s);
			}
			else{
				if(noche==0){
					Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_WHITE);
					Graphics_drawPixel(&g_sContext,2*n,2*s);
				}
				else{
					Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_ORANGE_RED);
					Graphics_drawPixel(&g_sContext,2*n,2*s);
				}
			}
		}}
	for(n=2;n<=60;n=n+6){//filas pares
		for(s=8;s<=30;s=s+12){
			if(p%2!=1){
				Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_BLUE);
				Graphics_drawPixel(&g_sContext,2*n,2*s);
			}
			else{
				if(noche==0){

					Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_WHITE);
					Graphics_drawPixel(&g_sContext,2*n,2*s);
				}
				else{
					Graphics_setForegroundColor(&g_sContext,GRAPHICS_COLOR_ORANGE_RED);
					Graphics_drawPixel(&g_sContext,2*n,2*s);
				}
			}
		}}
	p++;



}



#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR_HOOK(void)
{
	t++;
	LPM0_EXIT;
}
