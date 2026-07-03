boolean updateError() {
  boolean returnValue = false;
    if (newError != errorCode) {        // Hat sich geändert?!
      
    errorCode = newError;
    returnValue = true;                     // Ja, hat sich geändert. 
    /* Fehlermeldung Setzen */
      switch (errorCode) {
        case -2:
          errorMessage = errorMessages[12];   // Kein Induktionskochfeld
          break; 
        case -1:
          errorMessage = errorMessages[11];    // Serielle Kommunikation gestört
          break;
        case 0:
          errorMessage = errorMessages[0];    // Kein Fehler
          returnValue = false;
          break;
        case 2:
          errorMessage = errorMessages[1];    // Kein Topf
          break;
        default:
          errorMessage = "Fehler: " + errorCode;          // Unbekannt
       }
       lcdPrintError();          
       Serial.println(errorMessage);
  } else {
    if (errorCode != 0) {
      returnValue = true;
    } else { returnValue = false; }    
  }  
  return returnValue;
}

void shutDown() {
  if (power != 0) {
    newPower = 0;  
    updatePower();    
    }
}

