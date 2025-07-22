
// --- Variables anti-double-tir (à placer en haut du fichier) ---
bool msCible1Traitee = false;
bool msCible2Traitee = false;
bool msCible3Traitee = false;
bool msCible4Traitee = false;
bool msCible5Traitee = false;
bool msCible6Traitee = false;
bool msCible7Traitee = false;
bool msCible8Traitee = false;
bool msCible9Traitee = false;
bool msCible10Traitee = false;

void EcranInitialisationMS()
{
  lcd.print("Nb joueurs : " + String(nbJoueurs) + "      ");        
}

void InitGameMS()
{
  FastLED.clear(); FastLED.show();
  myDFPlayer.stop();
  myDFPlayer.playMp3Folder(19);
  tourEnCoursMS = 1;
  resteEnCoursMS = 1000000;
  joueurEnCours = 1;
  for(int i=1; i<=4; i++){
    scores[i] = 0;
    killer[i] = 0;
    levels[i] = 0;
    oldClassement[i] = 0;
    classement[i] = 0;
  }
  statusBoutonE = digitalRead(boutonE);

  if (nbJoueurs == 1){
    nbJoueurs = 1;
  }
  if (nbJoueurs != oldNbJoueurs){
    oldNbJoueurs=nbJoueurs;
  }
  if (statusBoutonE==LOW){ 
    for (int i = 1; i <= nbJoueurs ; i++) {
      killer[i] = 1;         
    }
  }
  initialisation = false;
  partieEnCours = true;
  EcranEnJeuMS();
  EcranGo();
}
  
void trtPartieEnCoursMS(){

    FastLED.clear(); FastLED.show();
    Temporisation();
    EcranEnJeuMS();            
    while (resteEnCoursMS > 0 && partieEnCours) {
      AcquisitionCapteurs();    
      PenaliteMS();     
      if (partieFinie==true){
        break;
      }
      else {      
          killer[joueurEnCours] = 0;
      }
      delay(1);

      // --- PATCH ANTI-DOUBLE-TIR SUR CHAQUE CIBLE ---
      statusCible1 = digitalRead(cible1);
      if (statusCible1 == LOW && !msCible1Traitee) {
        runCibleMS(0);
        msCible1Traitee = true;
      } else if (statusCible1 == HIGH) msCible1Traitee = false;

      statusCible2 = digitalRead(cible2);
      if (statusCible2 == LOW && !msCible2Traitee) {
        runCibleMS(5);
        msCible2Traitee = true;
      } else if (statusCible2 == HIGH) msCible2Traitee = false;

      statusCible3 = digitalRead(cible3);
      if (statusCible3 == LOW && !msCible3Traitee) {
        runCibleMS(10);
        msCible3Traitee = true;
      } else if (statusCible3 == HIGH) msCible3Traitee = false;

      statusCible4 = digitalRead(cible4);
      if (statusCible4 == LOW && !msCible4Traitee) {
        runCibleMS(15);
        msCible4Traitee = true;
      } else if (statusCible4 == HIGH) msCible4Traitee = false;

      statusCible5 = digitalRead(cible5);
      if (statusCible5 == LOW && !msCible5Traitee) {
        runCibleMS(25);
        msCible5Traitee = true;
      } else if (statusCible5 == HIGH) msCible5Traitee = false;

      statusCible6 = digitalRead(cible6);
      if (statusCible6 == LOW && !msCible6Traitee) {
        runCibleMS(50);
        msCible6Traitee = true;
      } else if (statusCible6 == HIGH) msCible6Traitee = false;

      statusCible7 = digitalRead(cible7);
      if (statusCible7 == LOW && !msCible7Traitee) {
        runCibleMS(100);
        msCible7Traitee = true;
      } else if (statusCible7 == HIGH) msCible7Traitee = false;

      statusCible8 = digitalRead(cible8);
      if (statusCible8 == LOW && !msCible8Traitee) {
        runCibleMS(150);
        msCible8Traitee = true;
      } else if (statusCible8 == HIGH) msCible8Traitee = false;

      statusCible9 = digitalRead(cible9);
      if (statusCible9 == LOW && !msCible9Traitee) {
        runCibleMS(200);
        msCible9Traitee = true;
      } else if (statusCible9 == HIGH) msCible9Traitee = false;

      statusCible10 = digitalRead(cible10);
      if (statusCible10 == LOW && !msCible10Traitee) {
        runCibleMS(250);
        msCible10Traitee = true;
      } else if (statusCible10 == HIGH) msCible10Traitee = false;
    }
}

void PenaliteMS() 
{ 
  if (!coursesCommencees)
  {
    EcranEnJeuMS();
    Moteur();
    myDFPlayer.playMp3Folder(19);
    coursesCommencees = true;
    startTime = millis();
  }      
  if(!coursesFinies && coursesCommencees)
    {
    float tempsrestant = tempstotalL - (millis() - startTime) / 1000.0;
    lcd.setCursor(11, 3);
    lcd.print("TR: ");
    if (tempsrestant > 0) {
      lcd.print(tempsrestant, 3);
      Serial1.print("COUNTDOWN:");
      Serial1.println(tempsrestant, 3);
      zeroEnvoye = false;
    }
    else if (!coursesFinies && tempsrestant <= 0) {
      lcd.print("0.000");
      Serial1.print("COUNTDOWN:");
      Serial1.println(0.000, 3);
      zeroEnvoye = true;
    }
    if(tempsrestant < 0)
     {
      coursesFinies = true;
      SMRAZTP();
      lcd.clear();
      clignotementLED(1);
      lcd.setCursor(3, 1);
      lcd.print("Temps ecoule !");
      clignotementLED(1);
      myDFPlayer.playMp3Folder(1);
      clignotementLED(1);
      Messages();
      clignotementLED(1);
      fill_solid(leds, NUM_LEDS, CRGB::Red);  FastLED.show();
      delay(2000);
      lcd.clear();
      FastLED.clear(); FastLED.show();
      coursesCommencees = false;
      coursesFinies = false;
      resteEnCoursMS = 1;
      resetAllBonuses();
      SMRAZ();
      GameOver();
      EcranEnJeuMS();
      partieFinie = true;
    }
  }
  else if (tempsrestant > 0) lcd.print(tempsrestant);
  else lcd.print("0.000");         
}

// --- Traitement unique d'un tir sur une cible MS ---
void runCibleMS(int points) {
  coursesCommencees = false;
  coursesFinies = false;
  // Animation & son selon points gagnés
  if (points == 0) { fill_solid(leds, NUM_LEDS, CRGB::Red); myDFPlayer.playMp3Folder(1); }
  else if (points == 5) { fill_solid(leds, NUM_LEDS, CRGB::Orange); myDFPlayer.playMp3Folder(2); }
  else if (points == 10) { fill_solid(leds, NUM_LEDS, CRGB::Yellow); myDFPlayer.playMp3Folder(3); }
  else if (points == 15) { fill_solid(leds, NUM_LEDS, CRGB::Green); myDFPlayer.playMp3Folder(5); }
  else if (points == 25) { fill_solid(leds, NUM_LEDS, CRGB::Blue); myDFPlayer.playMp3Folder(8); }
  else if (points == 50) { Cinquante(); }
  else if (points == 100) { Cent(); }
  else if (points == 150) { CentCinquante(); }
  else if (points == 200) { DeuxCent(); }
  else if (points == 250) { DeuxCentCinquante(); }
  delay(3000);
  FastLED.clear(); FastLED.show();
  // Gestion des scores et bonus
  if (points == 0) { scores[joueurEnCours] += pointBonus0[joueurEnCours]; pointBonus0[joueurEnCours]++; resetAllBonus0(); }
  if (points == 5) { scores[joueurEnCours] += 5 + pointBonus5[joueurEnCours]; pointBonus5[joueurEnCours]++; resetAllBonus5(); }
  if (points == 10) { scores[joueurEnCours] += 10 + pointBonus10[joueurEnCours]; pointBonus10[joueurEnCours]++; resetAllBonus10(); }
  if (points == 15) { scores[joueurEnCours] += 15 + pointBonus15[joueurEnCours]; pointBonus15[joueurEnCours]++; resetAllBonus15(); }
  if (points == 25) { scores[joueurEnCours] += 25 + pointBonus25[joueurEnCours]; pointBonus25[joueurEnCours]++; resetAllBonus25(); }
  if (points == 50) { scores[joueurEnCours] += 50 + pointBonus50[joueurEnCours]; pointBonus50[joueurEnCours]++; resetAllBonus50(); }
  if (points == 100) { scores[joueurEnCours] += 100 + pointBonus100[joueurEnCours]; pointBonus100[joueurEnCours]++; resetAllBonus100(); }
  if (points == 150) { scores[joueurEnCours] += 150 + pointBonus150[joueurEnCours]; pointBonus150[joueurEnCours]++; resetAllBonus150(); }
  if (points == 200) { scores[joueurEnCours] += 200 + pointBonus200[joueurEnCours]; pointBonus200[joueurEnCours]++; resetAllBonus200(); }
  if (points == 250) { scores[joueurEnCours] += 250 + pointBonus250[joueurEnCours]; pointBonus250[joueurEnCours]++; resetAllBonus250(); }
  // Envoi du message structuré pour le joueur
  String message = "J" + String(joueurEnCours -1) + " : " + String(points) + " : ";
  if      (points == 0)   message += String(pointBonus0[joueurEnCours]-1);
  else if (points == 5)   message += String(pointBonus5[joueurEnCours]-1);
  else if (points == 10)  message += String(pointBonus10[joueurEnCours]-1);
  else if (points == 15)  message += String(pointBonus15[joueurEnCours]-1);
  else if (points == 25)  message += String(pointBonus25[joueurEnCours]-1);
  else if (points == 50)  message += String(pointBonus50[joueurEnCours]-1);
  else if (points == 100) message += String(pointBonus100[joueurEnCours]-1);
  else if (points == 150) message += String(pointBonus150[joueurEnCours]-1);
  else if (points == 200) message += String(pointBonus200[joueurEnCours]-1);
  else if (points == 250) message += String(pointBonus250[joueurEnCours]-1);
  message += " : " + String(scores[joueurEnCours]);
  Serial1.println(message);
  Serial.println("📤 Envoi à ESP32: " + message);
  resteEnCoursMS--;
}

void GererInterruptionMS()
{
  if ( joueurEnCours == nbJoueurs && tourEnCoursMS == nbToursMS && resteEnCoursMS == 1 ){
    testKiller();
    coursesCommencees = false;
    coursesFinies = false;
    partieEnCours = false;
    partieFinie = true;
    SMRAZ();
  }
  else if (  joueurEnCours == nbJoueurs && resteEnCoursMS == 1 )
  {
    myDFPlayer.stop();
    testKiller();
    joueurEnCours = 1;            
    resteEnCoursMS = 1000000;
    tourEnCoursMS++;
    EcranTourSuivantMS();    
    EcranJoueurSuivant();
    Serial1.println("NEXT_TURN");
    Serial.println("📤 Envoi à ESP32: NEXT_TURN");
    Serial1.print("COUNTDOWN:");
    Serial1.println(0.000, 3);
    lcd.clear();
   while (statusBoutonE==HIGH){
        AcquisitionCapteurs();
        printBigNum(21, 13, 0);
        printBigNum(joueurEnCours,17,0);
        lcd.setCursor(0,0);
        lcd.print("Tour  : "+ String(tourEnCoursMS));
        lcd.setCursor(0,2);
        lcd.print("Quand pret");         
        lcd.setCursor(8,3);
        lcd.print("Start --> OK");
        FastLED.clear(); FastLED.show();              
   }
   EcranGo();                      
  }
  else if ( resteEnCoursMS != 1 ){
    resteEnCoursMS--; 
  }  
  else
  {
    myDFPlayer.stop();
    testKiller();
    joueurEnCours++;
    resteEnCoursMS = 1000000;
    EcranJoueurSuivant();
    Serial1.println("NEXT_PLAYER");
    Serial.println("📤 Envoi à ESP32: NEXT_PLAYER");
    Serial1.print("COUNTDOWN:");
    Serial1.println(0.000, 3);
    lcd.clear();
   while (statusBoutonE==HIGH){
        AcquisitionCapteurs();      
        printBigNum(21, 13, 0);
        printBigNum(joueurEnCours,17,0);
        lcd.setCursor(0,0);
        lcd.print("Tour  : "+ String(tourEnCoursMS));
        lcd.setCursor(0,2);
        lcd.print("Quand pret");        
        lcd.setCursor(8,3);
        lcd.print("Start --> OK");
        FastLED.clear(); FastLED.show();
   }
   EcranGo();    
  }  
}

void EcranEnJeuMS(){
  lcd1.clear();
  lcd1.setCursor(0,0);
  lcd1.print("J1 : " + String(scores[1]));
  Serial.println("J1 : " + String(scores[1]));
  lcd1.setCursor(0,1);
  lcd1.print("J2 : " + String(scores[2]));
  Serial.println("J2 : " + String(scores[2]));
  lcd1.setCursor(0,2);
  lcd1.print("J3 : " + String(scores[3]));
  Serial.println("J3 : " + String(scores[3]));
  lcd1.setCursor(0,3);
  lcd1.print("J4 : " + String(scores[4]));
  Serial.println("J4 : " + String(scores[4]));

  lcd.clear();
  lcd.setCursor(0,3);
  lcd.print("Joueur: " + String(joueurEnCours));
  Serial.println("Joueur: " + String(joueurEnCours));

  String scorestring = String(scores[joueurEnCours]);
  if (scorestring.length()==6){
    String Cmille  = String(scorestring.charAt(0));
    printBigNum(Cmille  .toInt(), 2, 1);      
    String Dmille  = String(scorestring.charAt(1));
    printBigNum(Dmille  .toInt(), 5, 1);      
    String millier = String(scorestring.charAt(2));
    printBigNum(millier.toInt(), 8, 1);      
    String centaine = String(scorestring.charAt(3));
    printBigNum(centaine.toInt(), 11, 1);
    String dizaine = String(scorestring.charAt(4));
    printBigNum(dizaine.toInt(), 14, 1);
    String unite = String(scorestring.charAt(5));
    printBigNum(unite.toInt(), 17, 1);
  } 
  else if (scorestring.length()==5){
    String Dmille  = String(scorestring.charAt(0));
    printBigNum(Dmille  .toInt(), 5, 1);      
    String millier = String(scorestring.charAt(1));
    printBigNum(millier.toInt(), 8, 1);      
    String centaine = String(scorestring.charAt(2));
    printBigNum(centaine.toInt(), 11, 1);
    String dizaine = String(scorestring.charAt(3));
    printBigNum(dizaine.toInt(), 14, 1);
    String unite = String(scorestring.charAt(4));
    printBigNum(unite.toInt(), 17, 1);
  }
  else if (scorestring.length()==4){
    String millier = String(scorestring.charAt(0));
    printBigNum(millier.toInt(), 8, 1);      
    String centaine = String(scorestring.charAt(1));
    printBigNum(centaine.toInt(), 11, 1);
    String dizaine = String(scorestring.charAt(2));
    printBigNum(dizaine.toInt(), 14, 1);
    String unite = String(scorestring.charAt(3));
    printBigNum(unite.toInt(), 17, 1);
  }      
  else if (scorestring.length()==3){
    String centaine = String(scorestring.charAt(0));
    printBigNum(centaine.toInt(), 11, 1);
    String dizaine = String(scorestring.charAt(1));
    printBigNum(dizaine.toInt(), 14, 1);
    String unite = String(scorestring.charAt(2));
    printBigNum(unite.toInt(), 17, 1);
  }    
  else if (scorestring.length()==2){
    String dizaine = String(scorestring.charAt(0));
    printBigNum(dizaine.toInt(), 14, 1);
    String unite = String(scorestring.charAt(1));
    printBigNum(unite.toInt(), 17, 1);
  }    
  else{
    String unite = String(scorestring.charAt(0));
    printBigNum(0, 14, 1);
    printBigNum(unite.toInt(), 17, 1);     
  }         
}

void EcranTourSuivantMS()
{
  EcranScores1();
  lcd.clear();
  printBigNum(31, 2, 1);
  printBigNum(26, 5, 1);
  printBigNum(32, 8, 1);
  printBigNum(29, 11, 1); 
  printBigNum(tourEnCoursMS,15,1);
  delay(20);
  EcranScores1();
}

void trtPartieFinieMS(){   
    FinGame();   
    while(killer[1]==1 || killer[2]==1 || killer[3]==1 || killer[4]==1 ){
      Serial.println("KILLER STATUS J1 " + String(killer[1]) + String(killer[2]) + String(killer[3]) + String(killer[4]));   
        for (int i = 1; i <= nbJoueurs ; i++){
          if (killer[i] == 1){
            joueurEnCours = i;
          }
        } 
    }
  partieFinie = false;
  partieEnCours = false;
  initialisation = true;
  oldNbJoueurs = 0;
  triclassement();
  AfficheFin();
}
// ---------------------------------FIN trtPartieFinie ----------------------------------
