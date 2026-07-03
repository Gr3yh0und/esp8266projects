/*
 * Sketch zur Kommunikation zwischen einem CBPi und einem GGM Induktionskochfeld.
 * v.0.2 vom 23.09.2018
 * M.Sc.
 * 
 * Benutzung auf eigene gefahr.
 * 
 */
 
/*############## INCLUDES ###################*/
#include <Wire.h>                 // i2C
#include <LiquidCrystal_I2C.h>    // i2C Display
#include "config.h"               // Config-Datei

/*############## KONSTANTEN ##################*/

LiquidCrystal_I2C lcd(0x27,20,4); // Instanz Display

/*############## Variablen ##################*/

/*-----  Induktionskochfeld  -----*/
byte CMD_CUR = 0;                 // Aktueller Befehl
boolean isRelayon = false;        // Systemstatus: ist das Relais in der Platte an?
boolean isInduon = false;         // Systemstatus: ist Power > 0?

boolean isError = false;          // Systemstatus: Fehlermeldung von der Platte?
int errorCode = 0;                // Ziffer der Fehlermeldung
int newError = 0;                 // Empfangene Fehlermeldung
String  errorMessage = "";        // Fehlermeldung String
byte inputBuffer[33];             // Buffer für Rückantwort der Platte
byte inputCurrent = 0;            // Hilfvariable für Rückantwort (Zählvariable)
bool inputStarted = false;        // Starbit
unsigned long lastInterrupt;      // Zeitmessung
unsigned long timeLastReaction;   // Timeout Induktionskochfeld

unsigned long timeTurnedoff;      // Für Messung Verzögerung Abschaltung Relais

int power = 0;                    // Prozent Power Aktuell
int newPower = 0;                 // Prozent Power Neu
long powerSampletime = 20000;     // Dauer eines Schaltzyklus
long powerHigh = powerSampletime; // Dauer des "HIGH"-Anteils im Schaltzyklus
long powerLow = 0;                // Dauer des "LOW"-Anteils im Schaltzyklus
bool isPower = false;             // High oder Low Senden
unsigned long powerLast;          // Zeitmessung für High oder Low

/*-----  CBPi  -----*/
unsigned long timeLastCommand;    // Letzer empfangener Command von CBPi
boolean isRunning = true;

/*############## PIN-Zuordnung ##################*/

/*-----  Induktionskochfeld  -----*/
const byte PIN_WHITE = 3;       // RELAIS
const byte PIN_YELLOW = 5;      // AUSGABE AN PLATTE
const byte PIN_INTERRUPT = 2;   // EINGABE VON PLATTE

