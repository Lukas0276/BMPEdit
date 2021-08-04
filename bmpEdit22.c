/*
 * bmpEdit22.c
 * 
 * Copyright 2021 z0041x1k <z0041x1k@MD2GV8RC>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * Funktioniert!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * 
 * Farben: Blau Grün Rot [0,1,2]
 * 
 * bmpEdit22 input.bmp output.bmp n/h/x/y/s 100
 * 
 * n = negativ
 * h = Helligkeit
 * x = spiegeln horizontal
 * y = spiegeln vertikal
 * s = Farbe in Schwarz Weiß
 * 
 * 100 = Helligkeit (Prozentsatz)
 * 
 * Funktioniert aber nur mit nicht komprimierten Bildern!
 */


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *in;
	FILE *out;
	int temp = 0; //AusleseTemp
	int schreibTemp = 0; //HelligkeitsTemp
	int swTemp = 0; //Schwarz,Weiß Temp
	int zaehlera = 0; //Gesamtzaehler
	int zaehleri = 1; //Informationszaehler
	int zeilenzaehler = 0;
	int spaltenzaehler = 0;
	int pixelzaehler = 0;
	int breite, hoehe, compressed;
	int extraPixel = 0;
	
	int fileheader[54]; //Speicher für den Fileheader
	
	in = fopen(argv[1], "rb");
	
	fseek(in, 18, 0); //Die Breite suchen
	fread(&breite, sizeof(long), 1, in);
	fseek(in, 22, 0); //Die Hoehe suchen
	fread(&hoehe, sizeof(long), 1, in);
	fseek(in, 28, 0); //Die Hoehe suchen
	fread(&compressed, sizeof(long), 1, in);
	
	//printf("Compressed = %d\n", compressed); //Auf compression überprüfen
	int tempE = breite;
	//Extrapixel:
	while ((tempE%4) != 0) {
		tempE++;
		extraPixel++;
	}
	
	//printf("Es werden %d Extrapixel benoetigt!", extraPixel);
	
	fclose(in); //Die Datei schließen
	
	int bitmap[hoehe][breite][3]; //Bitmapmatrix
	
	//Die Bitmap mit 0 initialisieren
	for (int z = 0; z < hoehe; z++) {
		for (int s = 0; s < breite; s++) {
			for(int p = 0; p < 3; p ++) {
				bitmap[z][s][p] = 0;
			}
		}
	}
	
	in = fopen(argv[1], "rb"); //Wieder öffnen //Dadurch werden die vorherigen Schritte nicht gezählt --> fseek
	
	while( (temp = fgetc(in)) != EOF){

		if (zaehlera > 53) {

			//Auffüllen der Bitmatrix
			bitmap[zeilenzaehler][spaltenzaehler][pixelzaehler] = temp;
			pixelzaehler++; //Erhöhen des pixelzaehlers
			//}
			
			if ((zaehleri % (breite*3)) == 0) { //3* --> 3 Farben pro Pixel Eine Zeile auslesen
			
				//Ein Pixel hinzufügen--> damit das ganze durch 4 Teilbar ist 11*3 = 33/4 = geht nicht --> 33 + 3 = 36/4 = 9 --> geht!!!!!!!!!!!!!!!!!
				for (int ep = 0; ep < extraPixel; ep++) {
					
					temp = fgetc(in);
					temp = fgetc(in);
					temp = fgetc(in);
				}
				
				zeilenzaehler++; //Erhöhen des Zeilenzaehlers
				spaltenzaehler = 0; //Rücksetzen des Spaltenzaehlers
			}
			if ((zaehleri % 3) == 0) { //Ein Pixel auslesen
				
				pixelzaehler = 0; //Rücksetzen des Pixelzaehlers
				
				if ((zaehleri % (breite*3)) != 0) { //Nur wenn gerade kein Zeilenende ereicht wurde
					
					spaltenzaehler++; //Erhöhen des Spaltenzaehlers;
				}
			}
			
			zaehleri++; //Informationszaehler
		}
		else {
			fileheader[zaehlera] = temp; //Den Fileheader kopieren
		}
	
	zaehlera++; //Gesamtzaehler
	}
	//Schließen der Inputdatei
	fclose(in);
	
	//Schreiben des Fileheaders in eine Datei
	out = fopen(argv[2], "wb");
	
	for (int i = 0; i < 54; i++) {
		fputc(fileheader[i], out);
	}
	
	//unterschiedliche Bearbeitungsarten
	if (*argv[3] == 'x') {
		
		for (int z = hoehe-1; z >= 0; z--) {
			for (int s = 0; s < breite; s++) {
				for(int p = 0; p < 3; p++) {
					fputc(bitmap[z][s][p], out);
				}
			}
			//Einfügen des Aufüllpixels nach jeder Zeile
			for (int ep = 0; ep < extraPixel; ep++) {
			
				fputc(' ', out);
				fputc(' ', out);
				fputc(' ', out);
			}
		}
	}
	else if (*argv[3] == 'y') {
		
		for (int z = 0; z < hoehe; z++) {
			for (int s = breite-1; s >= 0; s--) {
				for(int p = 0; p < 3; p++) {
					fputc(bitmap[z][s][p], out);
				}
			}
			//Einfügen des Aufüllpixels nach jeder Zeile
			for (int ep = 0; ep < extraPixel; ep++) {
			
				fputc(' ', out);
				fputc(' ', out);
				fputc(' ', out);
			}
		}
	}
	else if (*argv[3] == 'h') {
		
		for (int z = 0; z < hoehe; z++) {
			for (int s = 0; s < breite; s++) {
				for(int p = 0; p < 3; p++) {
					
					schreibTemp = bitmap[z][s][p]*(atof(argv[4])/100.0); //Wert erhöhen
		
					if (schreibTemp > 255) {
			
						schreibTemp = 255; // Auf den Maximalwert setzen
					}
					fputc(schreibTemp, out);
				}
			}
			//Einfügen des Aufüllpixels nach jeder Zeile
			for (int ep = 0; ep < extraPixel; ep++) {
			
				fputc(' ', out);
				fputc(' ', out);
				fputc(' ', out);
			}
		}
	}
	else if (*argv[3] == 'n') {
		
		for (int z = 0; z < hoehe; z++) {
			for (int s = 0; s < breite; s++) {
				for(int p = 0; p < 3; p++) {
					//Berechnen des Gegenwertes eines Pixels
					fputc(255-bitmap[z][s][p], out);
				}
			}
			//Einfügen des Aufüllpixels nach jeder Zeile
			for (int ep = 0; ep < extraPixel; ep++) {
			
				fputc(' ', out);
				fputc(' ', out);
				fputc(' ', out);
			}
		}
	}
	else if (*argv[3] == 's') {
		
		for (int z = 0; z < hoehe; z++) {
			for (int s = 0; s < breite; s++) {
				for(int p = 0; p < 3; p++) {
					// Bilden des Mittelwertes der drei Pixel
					swTemp = (bitmap[z][s][0] + bitmap[z][s][1] + bitmap[z][s][2]) / 3;
					fputc(swTemp, out);
				}
			}
			//Einfügen des Aufüllpixels nach jeder Zeile
			for (int ep = 0; ep < extraPixel; ep++) {
			
				fputc(' ', out);
				fputc(' ', out);
				fputc(' ', out);
			}
		}
	}
	else {
		printf("Die Eingabe war leider falsch!\n");
	}
	
	//Schließen der Outputdatei
	fclose(out);
	return 0;
}

