/*  Signallaufzeiten */ 
const int SIGNAL_HIGH = 5120; 
const int SIGNAL_HIGH_TOL = 1500;
const int SIGNAL_LOW = 1280;  
const int SIGNAL_LOW_TOL = 500;
const int SIGNAL_START = 25;
const int SIGNAL_START_TOL = 10; 
const int SIGNAL_WAIT = 10;
const int SIGNAL_WAIT_TOL = 5;

/*  Binäre Signale für Induktionsplatte */
int CMD[6][33] = {
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},  // Aus
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},  // P1
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // P2
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // P3
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},  // P4
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0} };// P5
byte PWR_STEPS[] = {0,20,40,60,80,100}; // Prozentuale Abstufung zwischen den Stufen

long delayAfteroff = 120000;    // Verzögerung zum Ausschalten des Relais

long timeOutCommand = 2000;	    // TimeOut für Seriellen Befehl

long timeOutReaction = 2000;    // TimeOut für Induktionskochfeld

String errorMessages[13] = {
  "                ",
  "E0: Kein Topf   ",
  "E1: Stromkreisf.",
  "E2: ??          ",
  "E3: Überhitzung ",
  "E4: Temp.Sens.Fe",
  "E5: ??          ",
  "E6: ??          ",
  "E7: Niederspann.",
  "E8: Überspannung",
  "EC: Komm.Fehler ",
  "ES: Ser. Fehler ",
  "EI: Kein Kochf. "
  };

