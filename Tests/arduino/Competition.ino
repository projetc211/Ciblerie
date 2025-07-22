
// --- Variables anti-double-tir (à placer en haut du fichier) ---
bool cptCible1Traitee = false;
bool cptCible2Traitee = false;
bool cptCible3Traitee = false;
bool cptCible4Traitee = false;
bool cptCible5Traitee = false;
bool cptCible6Traitee = false;
bool cptCible7Traitee = false;
bool cptCible8Traitee = false;
bool cptCible9Traitee = false;
bool cptCible10Traitee = false;

void EcranInitialisationCPT()
{
  lcd.print("Nb joueurs : " + String(nbJoueurs) + "      ");        
}

void InitGameCPT()
{
  FastLED.clear(); FastLED.show();
  myDFPlayer.stop();
  myDFPlayer.playMp3Folder(19);
  tourEnCoursCPT = 1;
  resteEnCoursCPT = 1000000;
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
  EcranEnJeuCPT();
  EcranGo();
}

void trtPartieFinieCPT(){   
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
    
void trtPartieEnCoursCPT(){

    FastLED.clear(); FastLED.show();
    Temporisation();
    EcranEnJeuCPT();            
    while (resteEnCoursCPT > 0 && partieEnCours) {
      AcquisitionCapteurs();    
      PenaliteCPT();     
      if (partieFinie==true){
        break;
      }
      else {      
          killer[joueurEnCours] = 0;
      }
      delay(1);

      // --- PATCH ANTI-DOUBLE-TIR SUR CHAQUE CIBLE ---
      statusCible1 = digitalRead(cible1);
      if (statusCible1 == LOW && !cptCible1Traitee) {
        runCibleCPT(0);
        cptCible1Traitee = true;
      } else if (statusCible1 == HIGH) cptCible1Traitee = false;

      statusCible2 = digitalRead(cible2);
      if (statusCible2 == LOW && !cptCible2Traitee) {
        runCibleCPT(5);
        cptCible2Traitee = true;
      } else if (statusCible2 == HIGH) cptCible2Traitee = false;

      statusCible3 = digitalRead(cible3);
      if (statusCible3 == LOW && !cptCible3Traitee) {
        runCibleCPT(10);
        cptCible3Traitee = true;
      } else if (statusCible3 == HIGH) cptCible3Traitee = false;

      statusCible4 = digitalRead(cible4);
      if (statusCible4 == LOW && !cptCible4Traitee) {
        runCibleCPT(15);
        cptCible4Traitee = true;
      } else if (statusCible4 == HIGH) cptCible4Traitee = false;

      statusCible5 = digitalRead(cible5);
      if (statusCible5 == LOW && !cptCible5Traitee) {
        runCibleCPT(25);
        cptCible5Traitee = true;
      } else if (statusCible5 == HIGH) cptCible5Traitee = false;

      statusCible6 = digitalRead(cible6);
      if (statusCible6 == LOW && !cptCible6Traitee) {
        runCibleCPT(50);
        cptCible6Traitee = true;
      } else if (statusCible6 == HIGH) cptCible6Traitee = false;

      statusCible7 = digitalRead(cible7);
      if (statusCible7 == LOW && !cptCible7Traitee) {
        runCibleCPT(100);
        cptCible7Traitee = true;
      } else if (statusCible7 == HIGH) cptCible7Traitee = false;

      statusCible8 = digitalRead(cible8);
      if (statusCible8 == LOW && !cptCible8Traitee) {
        runCibleCPT(150);
        cptCible8Traitee = true;
      } else if (statusCible8 == HIGH) cptCible8Traitee = false;

      statusCible9 = digitalRead(cible9);
      if (statusCible9 == LOW && !cptCible9Traitee) {
        runCibleCPT(200);
        cptCible9Traitee = true;
      } else if (statusCible9 == HIGH) cptCible9Traitee = false;

      statusCible10 = digitalRead(cible10);
      if (statusCible10 == LOW && !cptCible10Traitee) {
        runCibleCPT(250);
        cptCible10Traitee = true;
      } else if (statusCible10 == HIGH) cptCible10Traitee = false;
    }
}

void PenaliteCPT() 
{ 
  if (!coursesCommencees)
  {
    EcranEnJeuCPT();
    Moteur();
    myDFPlayer.playMp3Folder(19);
    coursesCommencees = true;
    startTime = millis();
  }      
  if(!coursesFinies && coursesCommencees)
    {
    float tempsrestant = tempstotalCPT - (millis() - startTime) / 1000.0;
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
      resteEnCoursCPT = 1;
      resetAllBonuses();
      SMRAZ();
      GameOver();
      EcranEnJeuCPT();
      partieFinie = true;
    } 
  }
  else if (tempsrestant > 0) lcd.print(tempsrestant);
  else lcd.print("0.000");       
}

// --- Traitement unique d'un tir sur une cible CPT ---
void runCibleCPT(int points) {
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
  resteEnCoursCPT--;
}

void GererInterruptionCPT()
{
  if ( joueurEnCours == nbJoueurs && tourEnCoursCPT == nbToursCPT && resteEnCoursCPT == 1 ){
    testKiller();
    coursesCommencees = false;
    coursesFinies = false;
    partieEnCours = false;
    partieFinie = true;
    SMRAZ();
  }
  else if (  joueurEnCours == nbJoueurs && resteEnCoursCPT == 1 )
  {
    myDFPlayer.stop();
    testKiller();
    joueurEnCours = 1;            
    resteEnCoursCPT = 1000000;
    tourEnCoursCPT++;
    EcranTourSuivantCPT();    
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
        lcd.print("Tour  : "+ String(tourEnCoursCPT));
        lcd.setCursor(0,2);
        lcd.print("Quand pret");         
        lcd.setCursor(8,3);
        lcd.print("Start --> OK");
        FastLED.clear(); FastLED.show();              
   }
   EcranGo();                      
  }
  else if ( resteEnCoursCPT != 1 ){
    resteEnCoursCPT--; 
  }  
  else
  {
    myDFPlayer.stop();
    testKiller();
    joueurEnCours++;
    resteEnCoursCPT = 1000000;
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
        lcd.print("Tour  : "+ String(tourEnCoursCPT));
        lcd.setCursor(0,2);
        lcd.print("Quand pret");        
        lcd.setCursor(8,3);
        lcd.print("Start --> OK");
        FastLED.clear(); FastLED.show();
   }
   EcranGo();    
  }  
}

void EcranTourSuivantCPT()
{
  EcranScores1();
  lcd.clear();
  printBigNum(31, 2, 1);
  printBigNum(26, 5, 1);
  printBigNum(32, 8, 1);
  printBigNum(29, 11, 1); 
  printBigNum(tourEnCoursCPT,15,1);
  delay(20);
  EcranScores1();
}

void EcranEnJeuCPT(){
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
