void lcdPrint() {
  lcd.backlight();
  lcd.setCursor(0,0); 
  lcd.print("M:SER");
    
  lcdPrintPower();
    
  lcd.setCursor(14,0);
  lcd.print("P");
    
  lcdPrintPowerLevel();
}

void lcdPrintPower() {
  lcd.setCursor(8,0);
  if ( power < 100 ) { lcd.print(" "); }
  if ( power < 10 )  { lcd.print(" "); }
  lcd.print(power);
  lcd.print("%");
}

void lcdPrintPowerLevel() {
  lcd.setCursor(15,0);
  if ( isPower != true && power > 0) { lcd.print(CMD_CUR-1); } 
  else  { lcd.print(CMD_CUR); }
}

void lcdPrintError() {
  lcd.setCursor(0,1);
  lcd.print(errorMessage);  
}

