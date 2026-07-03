void loop() {
  /*--- Befehl von CBPi empfangen ---*/
  while(Serial.available() > 0) {
    newPower = Serial.parseInt();
    timeLastCommand = millis();
    timeLastReaction = millis();
    if (errorCode < 0 ) { newError = 0; }
  }

  /*--- Serielle kommunikation Überwachen ---*/
  if ((timeLastCommand + timeOutCommand) < millis()) { newError = -1; }

  /*--- Induktionskochfeld Überwachen ---*/
  // if ((timeLastReaction + timeOutReaction) < millis()) { newError = -2; } Für Kontrolle, ob Induktionskochfeld zurücksendet. Derzeit ungenutzt.
    
  /*--- Schauen ob Fehler vorliegt ---*/ 
  if (updateError()) {  goto continueWithError; }
  else               {  goto continueWithoutError; }

  continueWithoutError:
    /*--- Befehl auswerten ---*/
    updatePower();

    /*--- Relais an oder ausschalten ---*/
    isRelayon = updateRelay();  

    /*--- Befehl senden ---*/
    updateCommand();

    /*--- Fehlerhandling Überspringen ---*/
    goto Ende;    

  continueWithError:
    shutDown();    

  Ende:
    isRunning = true;
// Ende des Loops
}

