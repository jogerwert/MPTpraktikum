/********************************************************************/
/*  Hochschule fuer Technik und Wirtschaft                          */
/*  Fakultät fuer Ingenieurwissenschaften                           */
/*  Labor fuer Eingebettete Systeme                                 */
/*  Mikroprozessortechnik                                           */
/********************************************************************/
/*                                                                  */
/*  Schaltuhr.C:                                                    */
/*	  Realisiert eine Schaltuhr mit Nutzerdialog via UART1          */
/*     																									            */
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

#include <LPC21xx.H>		/* LPC21xx Definitionen                   */
#include "defs.h" 			/* Definitionen fuer Schaltuhr 						*/

/*Funktions-Prototypen*/
void uartInit(void);
void tasterInit(void);
void timerInit(void);
void ioInit(void);
void menueAusgeben(void);
void startOderStop(void);
void restzeitAusgeben(void);
void zeichenSenden(char ch);
void stringSenden(char* string);
char zeichenLesen(void);
int asciiToZahl(char ch);
char zahlToAscii(int zahl);

/*ISRs*/
void taster_isr (void) __irq;
void timer_isr(void) __irq;
	
/*Globale Variablen*/
unsigned long segmente[10]={SEGMENTE_NULL, SEGMENTE_EINS, SEGMENTE_ZWEI, 
	SEGMENTE_DREI, SEGMENTE_VIER, SEGMENTE_FUENF, SEGMENTE_SECHS, SEGMENTE_SIEBEN,
	SEGMENTE_ACHT, SEGMENTE_NEUN};
volatile unsigned int sekunden = 0;



int main (void){
	char nutzerEingabe = 0;
	
	uartInit();				//Initalisierung UART1
	tasterInit();			//Initialisierung Taster
	timerInit();			//Initialisierung Timer
	ioInit();					//Initialisierung Ein-/Ausgabe
	menueAusgeben(); 	//Menü via UART1 ausgeben

   	//Endlosschleife	
   	while (1){
			nutzerEingabe = zeichenLesen();
			if(nutzerEingabe == 's' || nutzerEingabe == 'S'){
				startOderStop();
			}else if(nutzerEingabe == 'a' || nutzerEingabe == 'A'){
				if(TIMER_AKTIV){
					restzeitAusgeben();
				}else{
					stringSenden("Zeituhr nicht in Betrieb!\n");
				}
			}else{
				stringSenden("Falsche Eingabe!\n");
				menueAusgeben();
			}
		;
	}
}


/*--------------Initialisierungen--------------*/

void uartInit(void){
/*Initialisierung der UART1: 19200 Baud*/
	PINSEL0 = PINSEL0 | 0x50000;	//Pin0.8=Sendepin, Pin0.9=Empfangspin
	U1LCR = 0x83; 								//8 Datenbits, 1 Stoppbit, keine Paritaet,DLAB-Bit=1

	//Frequenzteiler -> 40 Dezimal
	U1DLL = 40 % 256; 						  //unterer Teil des Frequenzteilers
	U1DLM = 40 / 256; 					  	//oberer Teil des Frequenzteilers
	U1LCR = U1LCR & 0x7F; 			  	//DLAB-Bit loeschen
	
	U1FCR = U1FCR & 0x3F;				   	//RDA-Interrupt konfigurieren
	U1FCR = U1FCR | 0x07; 				  //FIFOs aktivieren und zuruecksetzen
}
	
	
void tasterInit(void){
	/*Initialisierung der Interrupt-Taste*/
	// EINT2 konfigurieren:
	EXTMODE  = EXTMODE | 0x04;   		//EINT2 flanken-abhaengig
	EXTPOLAR = EXTPOLAR & 0x0B;	 	  //EINT2 bei fallender Flanke ausloesen
	
	//Ausloesen des EINT2 durch Taster einstellen:
	PINSEL0 = PINSEL0 | 0x80000000; //Bit 31 in PINSEL0 auf 1 stellen
	PINSEL0 = PINSEL0 & 0xBFFFFFFF; //Bit 30 in PINSEL0 auf 0 stellen
	
	//Interrupt konfigurieren: Prioritaet 3 
	VICVectCntl3 = 0x30; 					  //aktiv fuer Kanal 16 (EINT2)
	VICVectAddr3 = (unsigned long) taster_isr;
	VICIntEnable = 0x00010000; 	    //IR durch Kanal 16 erlauben
}
	
	
void timerInit(void){
		unsigned int periode = 1000; /*Periodendauer in Millisekunden*/
	/*Initialisierung des Timer-Interrupt*/
	/*1 Millisec => 1 khz Taktrate = 1/12500 * P-Clock, wobei P-Clock == 12,5Mhz*/
		T0PR = 12500; /* Prescaler für 1 Millisec */
		T0TCR = 0x02; /* Timer rücksetzen, Zaehler bei 0 angehalten */
		T0MCR = 0x03; /* Bei Match mit T0MR0=> Interrupt und Reset */
	  T0MR0 = periode; /* Setzen der Periodendauer */
		T0TCR = TIMER_ANHALTEN;
		VICVectAddr2 = (unsigned long) timer_isr; /* Adresse der ISR */
		VICVectCntl2 = 0x24; /* aktiv für Kanal 4 (=TIMER0) */
		VICIntEnable = VICIntEnable | 0x10; /*Interrupt: Kanal 4 erlaubt, Bit4 = 4 */
}
	
	
void ioInit(){
  //Initialisierung der 7-Segment-Anzeige als Ausgabe == 1 
  //und des BCD-Drehschalters als Eingabe == 0 
	IODIR0 = ALLE_SEGMENTE;
	//8-LEDs als Ausgang
	IODIR1 = ALLE_LED;   					  
}


/*--------------Interrupt-Service-Routinen--------------*/
void taster_isr(void) __irq{
	startOderStop(); 
	EXTINT = 0x04;      // Ruecksetzen des interrupt flag
  VICVectAddr = 0x00; // VIC mitteilen, dass Interrupt vorbei
}


void timer_isr (void) __irq {
	unsigned int restMinuten;
	sekunden--;
	restMinuten = sekunden / 60;
	//Sekunden runterzaehlen, wenn bei 0, dann Timer-Stop und Ausgabe
	if(sekunden > 0){
		IOSET1 = MAX_LED;
		IOCLR0 = ALLE_SEGMENTE;
		IOSET0 = segmente[restMinuten];
		if(LED1_AKTIV){
			IOCLR1 = MIN_LED;
		}else{
			IOSET1 = MIN_LED;
		}
	}else{
		T0TCR  = TIMER_ANHALTEN;
		IOCLR1 = ALLE_LED;
		IOCLR0 = ALLE_SEGMENTE;
		stringSenden("Zeit abgelaufen\n");
	}
	
	T0IR |= 0x10; 			// Ruecksetzen des interrupt flag
	VICVectAddr = 0x00; // VIC mitteilen, dass Interrupt vorbei
}




/*--------------Hilfsfunktionen--------------*/
void menueAusgeben(void){
	stringSenden("Schaltuhr\n");
	stringSenden("\t Einstellung der Zeitdauer ueber Drehschalter\n");
	stringSenden("\t Start und Anhalten durch Druecken der Interrupt-Taste\n");
	stringSenden("\t S,s - Start und Anhalten\n");
	stringSenden("\t A,a - Anzeige der Restzeit\n");
}


void zeichenSenden(char ch){
	_Bool gesendet = 0;
	
	while(!gesendet){
		if(DATEN_VERSENDBAR){
			U1THR = ch;
			gesendet = 1;
		}
	}
}

void stringSenden(char* string){
	while(*string != '\0'){
		zeichenSenden(*string);
		string++;
	}
}

char zeichenLesen(void){
	char ch;
	_Bool gelesen = 0;
	
	while(!gelesen){
		if(DATEN_EMPFANGEN){
			ch = U1RBR;
			gelesen = 1;
		}
	}
	return ch;
}


void startOderStop(void){
	unsigned int bcdMinuten = (IOPIN0 & BCD_PINS) >> 10;
	if(TIMER_AKTIV){
		T0TCR  = TIMER_ANHALTEN;
		IOCLR1 = ALLE_LED;
		IOCLR0 = ALLE_SEGMENTE;
		stringSenden("Abbruch - ");
		restzeitAusgeben();
		sekunden = 0;
	}else{
		if(bcdMinuten >= 1 && bcdMinuten <= 9){
			sekunden = bcdMinuten * 60;
			stringSenden("Schaltuhr fuer ");
			zeichenSenden(zahlToAscii(bcdMinuten));	
			stringSenden(" Minuten eingestellt \n");	
			T0TCR = TIMER_STARTEN;
		}else{
			stringSenden("Falscher Wert bei BCD-Eingabe: ");
			zeichenSenden(zahlToAscii(bcdMinuten / 10));
			zeichenSenden(zahlToAscii(bcdMinuten % 10));
			stringSenden("\n");
			menueAusgeben();
		}
		
	}
}


void restzeitAusgeben(void){
	unsigned int restMinuten  = sekunden / 60;
	unsigned int restSekunden = sekunden % 60;
	
	stringSenden("Restzeit ");
	zeichenSenden(zahlToAscii(restMinuten));
	zeichenSenden(':');
	zeichenSenden(zahlToAscii(restSekunden / 10));
	zeichenSenden(zahlToAscii(restSekunden % 10));
	stringSenden("\n");
}


int asciiToZahl(char ch){
	int zahl = (int) ch;
	
	if(zahl >= '0' && zahl <= '9'){
	  zahl = zahl - '0';
	}else{
		if(zahl >= 'A' && zahl <= 'F'){
		  zahl = zahl - 'A' + 0xA;
		}else{
			zahl = zahl - 'a' + 0xA;
		}
	}
	return zahl;
}


char zahlToAscii(int zahl){
	char ch = (char) zahl;
	
	if(ch >= 0x0 && ch <= 0x9){
	  ch = ch + '0';
	}else{
		//Differenz zwischen Zahlwert und Grossbuchstabe addieren
		ch = ch + 0x37; 
	}
	return ch;
}


