/*
 * partituras.h
 *
 *  Created on: 14/01/2014
 *      Author: Manolo
 */

#ifndef PRACTICA11_1516_3_PARTITURAS_H_
#define PRACTICA11_1516_3_PARTITURAS_H_


#define 	DO 12
#define 	REb	13
#define		RE	14
#define		MIb	15
#define		MI	16
#define		FA	17
#define		SOLb	18
#define		SOL	19
#define		LAb	20
#define		LA	21
#define		SIb	22
#define		SI	23
#define 	SIL 36


extern const unsigned int Nota[];
extern const char * Notas_N[];

//extern const char mario2[];
extern const char mario1[];



typedef  struct Cancion{
	const char *notas;		// Vector de notas y duraciones
	char *nombre;			// Nombre de la cancion
	unsigned int tempo;		// tempo recomendado
}Cancion;

//extern const Cancion Mario2;
extern const Cancion Mario1;


#endif /* PRACTICA11_1516_3_PARTITURAS_H_ */
