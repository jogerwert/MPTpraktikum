/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultät fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  defs.H:                                     	                  */
/*	  Enthaelt Defines fuer Laborversuch 3                          */
/*    Die Defines werden u. a. zur Maskierung bestimmter Bits       */
/*    verwendet 																			      				*/
/*                                                                  */
/********************************************************************/
/*  Aufgaben-Nr.:        *  1, Laborversuch 3                       */
/*                       *                                          */
/********************************************************************/
/*  Gruppen-Nr.: 	       *  29                                      */
/*                       *                                          */
/********************************************************************/
/*  Name / Matrikel-Nr.: *	Blau, Michelle 	  / 3726118					    */
/*                       *	Gerwert, Johannes / 3726223				     	*/
/*                       *                                          */
/********************************************************************/
/* 	Abgabedatum:         *  24.01.2019                              */
/*                       *                                          */
/********************************************************************/


#ifndef __defs_H
#define __defs_H


#define PIN_25	   	  	 0x02000000	 //Bit-Position 25 des Kippschalters
#define NOT_PIN_25		   0xFDFFFFFF	 //NICHT Bit-Pos. 25 des Kippsch.

//7-Segment-Anzeige
#define ALLE_SEGMENTE	   0x01FC0000		//Alle 7 Segmente der 7-Seg-Anzg.
#define SEGMENTE_NULL	   0x00FC0000 	//Segmente der Zahl 0
#define SEGMENTE_EINS	   0x00180000 	//Segmente der Zahl 1
#define SEGMENTE_ZWEI	   0x016C0000 	//Segmente der Zahl 2
#define SEGMENTE_DREI	   0x013C0000 	//Segmente der Zahl 3
#define SEGMENTE_VIER	   0x01980000 	//Segmente der Zahl 4
#define SEGMENTE_FUENF   0x01B40000 	//Segmente der Zahl 5
#define SEGMENTE_SECHS   0x01F40000 	//Segmente der Zahl 6
#define SEGMENTE_SIEBEN  0x001C0000 	//Segmente der Zahl 7
#define SEGMENTE_ACHT	   0x01FC0000 	//Segmente der Zahl 8
#define SEGMENTE_NEUN	   0x01BC0000 	//Segmente der Zahl 9

//BCD-Drehschalter
#define BCD_PINS				 0x00003C00		//P0.10 bis P0.13

//LEDs
#define ALLE_LED				 0x00FF0000		//LED 1 bis 8, P1.16-1.23
#define MIN_LED 				 0x00010000		//LED 1
#define MAX_LED 				 0x00800000   //LED 8
#define LED1_AKTIV			 (IOPIN1 & MIN_LED)  //true -> LED1 ist aktiv

//Serielle Schnittstelle
#define CARRIAGE_RETURN	 0x0D					//ASCII-Wert fuer CR
#define LINEFEED 				 0x0A					//ASCII-Wert fuer LF
#define DATEN_EMPFANGEN 	((U1LSR & 0x01) != 0) //true -> Empfangsreg. lesbar
#define DATEN_VERSENDBAR	((U1LSR & 0x20) != 0) //true -> Sendereg. frei

//Timer
#define TIMER_AKTIV				(T0TCR == 0x01)	//true -> Timer0 ist aktiv
#define TIMER_STARTEN		 0x01
#define TIMER_ANHALTEN	 0x00

#endif  // __definitionen_H

