/*
 * partituras.c
 *
 *  Created on: 16/12/2015
 *      Author: Manolo
 */
#include "partituras.h"

const unsigned int Nota[]={
		61156,57724,54480,51428,48540,45816,43244,40818,38528,36364,34336,32404,
		30578,28862,27240,25714,24270,22908,21622,20408,19264,18182,17168,16202,
		15289,14431,13620,12857,12135,11454,10811,10204,9632,9091,8584,8101,1
};

const char * Notas_N[]={"DO0  ", "REb0 ","RE0  ","MIb0 ", "MI0  ", "FA0  ","SOLb0","SOL0 ", "LAb0 ","LA0  ","SIb0 ", "SI0  ",
		"DO   ", "REb  ","RE   ","MIb  ", "MI   ", "FA   ","SOLb ","SOL  ", "LAb  ","LA   ","SIb  ", "SI   ",
		"DO1 ", "REb1","RE1 ","MIb1", "MI1 ", "FA1 ","SOLb1","SOL1 ", "LAb1","LA1 ","SIb1", "SI1 ", "-----"
};


/* PARTITURAS para el segundo apartado */

//underground
//const char mario2[]={
	//	DO,1,DO+12,1,DO,1,LA,1,DO,1,SI,1,SIL,2,DO,1,DO+12,1,DO,1,LA,1,DO,1,SI,1,SIL,2,
		//DO,1,SIL,1,MI,1,SIL,1,RE,1,SIL,1,DO,1,SIL,1,DO,1,SIL,1,RE,1,SIL,1,
		//LA,1,SIL,1,DO+12,1,SIL,1,SI,1,SIL,1,FA,1,SIL,1,MI,1,SIL,1,SI,1,SIL,1,
		//DO,1,SOL,1,FA,1,SIL,1,MI,1,SI,1,LA,1,SIL,1,LAb,1,MI,1,
		//DO,1,SIL,1,SI-12,1,LA-12,1,LA-12,1,SIL,1,LA,1,MIb+12,1,
		//RE+12,1,SIL,1,DO+12,1,SOL+12,1,FA+12,1,SIL,1,FA+12,1,DO+12,1,SOL,1,SIL,1,
		//SOL,1,FA,1,FA,1,SIL,1,

//0};

//world
const char mario1[]={
		MI+12,1,MI+12,1,SIL,1,MI+12,1,SIL,1,DO+12,1,MI+12,2,SOL+12,1,SIL,2,SOL,1,SIL,2,
		DO+12,1,SOL,1,SIL,2,MI,1,MI,1,SIL,1,LA,1,SIL,1,SI,1,SI,1,SIb,1,SIL,1,LA,1,SIL,1,
		SOL,1,MI+12,1,SOL+12,1,LA,1,SIL,1,FA+12,1,SOL+12,1,SIL,1,MI+12,1,SIL,1,
		DO+12,1,SIL,1,RE+12,1,SIL,1,SI,1,SIL,1,DO+12,1,SIL,1,RE+12,1,SIL,1,SOL,1,SIL,2,
		RE,1,RE,1,SIL,1,LA,1,SIL,1,SI,1,SI,1,SIb,1,SIL,1,LA,1,SIL,1,SOL,1,MI+12,1,SOL+12,1,
		LA+12,1,SIL,1,FA+12,1,SOL+12,1,SIL,1,MI+12,1,SIL,1,DO+12,1,SIL,1,RE+12,1,SIL,1,SI,1,SIL,2,
		SOL+12,1,FA+12,1,FA+12,1,SIL,1,MIb+12,1,SIL,1,MIb+12,1,SIL,1,SOL,1,LA,1,DO+12,1,SIL,1,
		LA,1,DO+12,1,RE,1,SIL,2,SOL+12,1,FA+12,1,FA+12,1,SIL,1,MI+12,1,SIL,1,MI+12,1,SIL,1,SI+12,1,
		SIL,1,SI+12,1,SI+12,2,SIL,2,
0};



//const Cancion Mario2={
	//	mario2,
		//"Mario Underground",
	//	2150
//};

const Cancion Mario1={
		mario1,
		"Mario Bros",
		800
};




/* ESCALA para usar en el tercer apartado */

const char ESCALA[]={DO,REb,RE,MIb,MI,FA,SOLb,SOL,LAb,LA,SIb,SI,DO+12};

