
// --- Variables anti-double-tir (à placer en haut du fichier) ---
bool ljCible1Traitee = false;
bool ljCible2Traitee = false;
bool ljCible3Traitee = false;
bool ljCible4Traitee = false;
bool ljCible5Traitee = false;
bool ljCible6Traitee = false;
bool ljCible7Traitee = false;
bool ljCible8Traitee = false;
bool ljCible9Traitee = false;
bool ljCible10Traitee = false;

void EcranInitialisationLJ() {
  lcd.print("Nb joueurs : " + String(nbJoueurs));
}

void InitGameLJ() {
  FastLED.clear(); FastLED.show();
  myDFPlayer.stop();
  myDFPlayer.playMp3Folder(19);
  tourEnCours = 1;
  joueurEnCours = 1;
  resteEnCoursLJ = 1000000; // <<<--- Nb de tir ( illimité )
  tempsJeu = tempsManche; // Utilise le temps selon le mode choisi
  dernierCountdownEnvoye = tempsJeu;
  TTRST();
  statusBoutonE = digitalRead(boutonE);
  if (nbJoueurs == 1) {
    nbJoueurs = 1;
  }
  if (nbJoueurs != oldNbJoueurs) {
    oldNbJoueurs = nbJoueurs;
  }
  initialisation = false;
  partieEnCours = true;
  EcranEnJeu();
  EcranGo();
}

void trtPartieEnCoursLJ()
{
  FastLED.clear(); FastLED.show();
  Temporisation();
  EcranEnJeuLJ();            
  while (resteEnCoursLJ > 0 && partieEnCours) {
    AcquisitionCapteurs();    
    PenaliteLJ();     
    if (partieFinie==true){
      break;
    }
    else {      
      killer[joueurEnCours] = 0;
    }
    delay(1);

    // --- PROTECTION ANTI-DOUBLE-TIR POUR CHAQUE CIBLE ---

    statusCible1 = digitalRead(cible1);
    if (statusCible1 == LOW && !ljCible1Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;                  
      fill_solid(leds, NUM_LEDS, CRGB::Red);  FastLED.show();
      myDFPlayer.playMp3Folder(1); delay(3000);
      FastLED.clear(); FastLED.show();
      String message = "J" + String(joueurEnCours -1) + " : " + String(0) + " : " + String((pointBonus0[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      // Chrono spécial cible 1
      dernierCountdownEnvoye = 0.0;
      tempsJeu += tempsManche;
      startTime = millis();
      dernierCountdownEnvoye = tempsJeu;
      ljCible1Traitee = true;
      resteEnCoursLJ = 1;
      SMRAZ();
      EcranEnJeuLJ();
      GererInterruptionLJ(); // 🔄 Passe au joueur suivant immédiatement
      resetAllBonus0();
      resetAllBonusExcept(0);
    } else if (statusCible1 == HIGH) ljCible1Traitee = false;

    statusCible2 = digitalRead(cible2);
    if (statusCible2 == LOW && !ljCible2Traitee) {
      coursesCommencees = false;
      coursesFinies = false;
      fill_solid(leds, NUM_LEDS, CRGB::Orange); FastLED.show();
      myDFPlayer.playMp3Folder(2); delay(3000);
      FastLED.clear(); FastLED.show();
      scores[joueurEnCours] = scores[joueurEnCours] + 5 + pointBonus5[joueurEnCours];
      pointBonus5[joueurEnCours] = pointBonus5[joueurEnCours] + 1;
      resetAllBonusExcept(5);
      String message = "J" + String(joueurEnCours -1) + " : " + String(5) + " : " + String((pointBonus5[joueurEnCours]) -1)+ " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32 : " + message);
      resteEnCours--;
      ljCible2Traitee = true;
    } else if (statusCible2 == HIGH) ljCible2Traitee = false;

    statusCible3 = digitalRead(cible3);
    if (statusCible3 == LOW && !ljCible3Traitee) {
      coursesCommencees = false;
      coursesFinies = false;
      fill_solid(leds, NUM_LEDS, CRGB::Yellow); FastLED.show();
      myDFPlayer.playMp3Folder(3); delay(3000);
      FastLED.clear(); FastLED.show();
      scores[joueurEnCours] = scores[joueurEnCours] + 10 + pointBonus10[joueurEnCours];
      pointBonus10[joueurEnCours] = pointBonus10[joueurEnCours] + 2;
      resetAllBonusExcept(10);
      String message = "J" + String(joueurEnCours -1) + " : " + String(10) + " : " + String((pointBonus10[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      resteEnCours--;
      ljCible3Traitee = true;
    } else if (statusCible3 == HIGH) ljCible3Traitee = false;

    statusCible4 = digitalRead(cible4);
    if (statusCible4 == LOW && !ljCible4Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;                              
      fill_solid( leds, NUM_LEDS, CRGB::Green);  FastLED.show();
      myDFPlayer.playMp3Folder(5); delay(3000);
      FastLED.clear (); FastLED.show();
      scores[joueurEnCours] = scores[joueurEnCours]+15+pointBonus15[joueurEnCours];
      pointBonus15[joueurEnCours] = pointBonus15[joueurEnCours]+3;
      resetAllBonusExcept(15);
      String message = "J" + String(joueurEnCours -1) + " : " + String(15) + " : " + String((pointBonus15[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      resteEnCours--;
      ljCible4Traitee = true;
    } else if (statusCible4 == HIGH) ljCible4Traitee = false;

    statusCible5 = digitalRead(cible5);
    if (statusCible5 == LOW && !ljCible5Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;
      fill_solid(leds, NUM_LEDS, CRGB::Blue);  FastLED.show();
      myDFPlayer.playMp3Folder(8); delay(3000);
      FastLED.clear(); FastLED.show();
      scores[joueurEnCours] = scores[joueurEnCours]+25+pointBonus25[joueurEnCours];
      pointBonus25[joueurEnCours] = pointBonus25[joueurEnCours]+4;
      resetAllBonusExcept(25);
      String message = "J" + String(joueurEnCours -1) + " : " + String(25) + " : " + String((pointBonus25[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message); 
      resteEnCours--;
      ljCible5Traitee = true;
    } else if (statusCible5 == HIGH) ljCible5Traitee = false;

    statusCible6 = digitalRead(cible6);
    if (statusCible6 == LOW && !ljCible6Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;
      Cinquante();               
      scores[joueurEnCours] = scores[joueurEnCours]+50+pointBonus50[joueurEnCours];
      pointBonus50[joueurEnCours] = pointBonus50[joueurEnCours]+5;
      resetAllBonusExcept(50);
      String message = "J" + String(joueurEnCours -1) + " : " + String(50) + " : " + String((pointBonus50[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      resteEnCours--;
      ljCible6Traitee = true;
    } else if (statusCible6 == HIGH) ljCible6Traitee = false;

    statusCible7 = digitalRead(cible7);
    if (statusCible7 == LOW && !ljCible7Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;
      Cent();
      scores[joueurEnCours] = scores[joueurEnCours]+100+pointBonus100[joueurEnCours];
      pointBonus100[joueurEnCours] = pointBonus100[joueurEnCours]+10;
      resetAllBonusExcept(100);
      String message = "J" + String(joueurEnCours -1) + " : " + String(100) + " : " + String((pointBonus100[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      resteEnCours--;
      ljCible7Traitee = true;
    } else if (statusCible7 == HIGH) ljCible7Traitee = false;

    statusCible8 = digitalRead(cible8);
    if (statusCible8 == LOW && !ljCible8Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;
      CentCinquante();
      scores[joueurEnCours] = scores[joueurEnCours]+150+pointBonus150[joueurEnCours];
      pointBonus150[joueurEnCours] = pointBonus150[joueurEnCours]+15;
      resetAllBonusExcept(150);
      String message = "J" + String(joueurEnCours -1) + " : " + String(150) + " : " + String((pointBonus150[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      resteEnCours--;
      ljCible8Traitee = true;
    } else if (statusCible8 == HIGH) ljCible8Traitee = false;

    statusCible9 = digitalRead(cible9);
    if (statusCible9 == LOW && !ljCible9Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;
      DeuxCent();
      scores[joueurEnCours] = scores[joueurEnCours]+200+pointBonus200[joueurEnCours];
      pointBonus200[joueurEnCours] = pointBonus200[joueurEnCours]+20;
      resetAllBonusExcept(200);
      String message = "J" + String(joueurEnCours -1) + " : " + String(200) + " : " + String((pointBonus200[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      resteEnCours--;
      ljCible9Traitee = true;
    } else if (statusCible9 == HIGH) ljCible9Traitee = false;

    statusCible10 = digitalRead(cible10);
    if (statusCible10 == LOW && !ljCible10Traitee) {
      coursesCommencees = false; 
      coursesFinies = false;
      DeuxCentCinquante();
      scores[joueurEnCours] = scores[joueurEnCours]+250+pointBonus250[joueurEnCours];
      pointBonus250[joueurEnCours] = pointBonus250[joueurEnCours]+25;
      resetAllBonusExcept(250);
      String message = "J" + String(joueurEnCours -1) + " : " + String(250) + " : " + String((pointBonus250[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      resteEnCours--;
      ljCible10Traitee = true;
    } else if (statusCible10 == HIGH) ljCible10Traitee = false;

  }
  float tempsUtilise = tempsJeu - dernierCountdownEnvoye;
  Serial1.print("END_MANCHE:");
  Serial1.print(joueurEnCours - 1);
  Serial1.print(":");
  Serial1.println(tempsUtilise, 3);
  SMRAZ();
  GererInterruptionLJ();
}

void PenaliteLJ() 
{
  static bool bleuMontre = false; // Signal bleu bref avant le vrai décompte
  float tempsrestant = 0;
  if (!coursesCommencees)
  {
    EcranEnJeuLJ();
    myDFPlayer.playMp3Folder(19);
    coursesCommencees = true;
    // Réinitialisation des couleurs et états
    memset(colorIndexes, 0, sizeof(colorIndexes)); // Réinitialise tous les index de couleur
    memset(lastColorChanges, 0, sizeof(lastColorChanges));
    startTime = millis();
    tempsGenere = false;  // ✅ Réinitialise l'état pour la prochaine manche
    bleuMontre = false; // pour chaque nouvelle manche
  }
  if(!coursesFinies && coursesCommencees) {
    // --- EFFET BLEU AVANT DECOMPTE SI PAS DE 5s ---
    if (!bleuMontre && tempsManche < 5.0) {
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      FastLED.show();
      delay(500); // 0.5 seconde
      FastLED.clear(); FastLED.show();
      bleuMontre = true;
      // On attend le prochain cycle
      return;
    }
    if (!tempsGenere) {
      randomSeed(analogRead(10));
      TempsAleatoireLJ = random(2.250, 4.250);  // ⏳ Temps entre 1 et 5 secondes
      Period1LJ = TempsAleatoireLJ * 1000;
      Period2LJ = TempsAleatoireLJ * 1000;
      tempsGenere = true;  // ✅ Marque que le temps a été généré
      // ✅ Affichage immédiat du temps restant pour éviter qu'il ne soit manquant
      lcd.print(TempsAleatoireLJ);
      // 🎨 Affichage du code couleur correspondant
      if (TempsAleatoireLJ == 1) fill_solid(leds, NUM_LEDS, CRGB::Red); // 🔴 Rouge
      else if (TempsAleatoireLJ >= 2 && TempsAleatoireLJ <= 4.250) { 
        int index = TempsAleatoireLJ - 2;
        fill_solid(leds, NUM_LEDS, colorArrays[index][colorIndexes[index]]); // 🟠 Orange <- 🟡 Jaune <- 🟢 Vert <- 🔵 Bleu ( Sens de décomptage )
        lastColorChanges[index] = millis();
      }
      FastLED.show();
      Moteur();  // ✅ Activation immédiate
      // ✅ Démarre le délai non bloquant pour éteindre les LEDs après 1 seconde
      ledsAllumeesMillis = millis();
      ledsAllumees = true;
    }
     // ✅ Éteindre les LEDs après 1 seconde sans bloquer le code
     if (ledsAllumees && millis() - ledsAllumeesMillis >= 500) {
      FastLED.clear(); FastLED.show(); // ⭕ Éteint les LEDs après 1 seconde
      ledsAllumees = false;
     }
    // ✅ Changement de couleur toutes les secondes si TempsAleatoireLJ entre 2 et 5
    if (TempsAleatoireLJ >= 2 && TempsAleatoireLJ <= 4.250) {
      int index = TempsAleatoireLJ - 2;
      if (millis() - lastColorChanges[index] >= 1000) {
        colorIndexes[index] = (colorIndexes[index] + 1) % colorSizes[index];
        fill_solid(leds, NUM_LEDS, colorArrays[index][colorIndexes[index]]); //  🔵 Bleu -> 🟢 Vert -> 🟡 Jaune -> 🟠 Orange ( Sens réel )
        FastLED.show();
        lastColorChanges[index] = millis();
      }
    }
    // ⏳ Temps écoulé, on passe au joueur/tour suivant
    float tempsrestant = TempsAleatoireLJ - (millis() - startTime) / 1000.0;
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
    // Temps écoulé, fin de manche
    if(tempsrestant < 0)
     {
      float tempsUtilise = tempsJeu - dernierCountdownEnvoye;
      Serial1.print("END_MANCHE:");
      Serial1.print(joueurEnCours - 1);
      Serial1.print(":");
      Serial1.println(tempsUtilise, 3);
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
      resteEnCoursLJ = 1;
      resetAllBonuses();
      SMRAZ();
      EcranEnJeuLJ();
      GererInterruptionLJ();  
    }
  }
  else if (tempsrestant > 0) lcd.print(tempsrestant);
  else lcd.print("0.000");        
}

void GererInterruptionLJ()
{
  if (joueurEnCours == nbJoueurs && tourEnCoursLJ == nbToursLJ && resteEnCoursLJ == 1){
    testKiller();
    coursesCommencees = false;
    coursesFinies = false;
    partieEnCours = false;
    partieFinie = true;
    SMRAZ();
  }
  else if (joueurEnCours == nbJoueurs && resteEnCoursLJ == 1)
  {
    myDFPlayer.stop();
    testKiller();
    joueurEnCours = 1;            
    resteEnCoursLJ = 1000000; // <<<--- Nb de tir ( illimité )
    tourEnCoursLJ++;
    EcranTourSuivantLJ();    
    EcranJoueurSuivant();
    // === ENVOI "NEXT_TURN" ===
    Serial1.println("NEXT_TURN");
    Serial.println("📤 Envoi à ESP32: NEXT_TURN");
    Serial1.print("COUNTDOWN:");
    Serial1.println(0.000, 3);
    lcd.clear();
    attenteJoueurOuTour = true;
    while (digitalRead(boutonE) == HIGH && !boutonVirtuelE) {
      AcquisitionCapteurs();
      printBigNum(21, 13, 0);
      printBigNum(joueurEnCours,17,0);
      lcd.setCursor(0,0);
      lcd.print("Tour  : "+ String(tourEnCoursLJ));
      lcd.setCursor(0,2);
      lcd.print("Quand pret");         
      lcd.setCursor(8,3);
      lcd.print("Start --> OK");
      disableLedAnimation = false;
      disableSonAnimation = false;
      CodeLedAleatoire();
      SonsAleatoires();
      communications();
      delay(1);           
    }
    boutonVirtuelE = false;
    attenteJoueurOuTour = false;
    EcranGo();                      
  }
  else if (resteEnCoursLJ != 1){
    resteEnCoursLJ--; 
  }  
  else
  {
    myDFPlayer.stop();
    testKiller();
    joueurEnCours++;
    resteEnCoursLJ = 1000000; // <<<--- Nb de tir ( illimité )
    EcranJoueurSuivant();
    // === ENVOI "NEXT_PLAYER" ===
    Serial1.println("NEXT_PLAYER");
    Serial.println("📤 Envoi à ESP32: NEXT_PLAYER");
    Serial1.print("COUNTDOWN:");
    Serial1.println(0.000, 3);
    lcd.clear();
    attenteJoueurOuTour = true;
    while (digitalRead(boutonE) == HIGH && !boutonVirtuelE) {
      AcquisitionCapteurs();      
      printBigNum(21, 13, 0);
      printBigNum(joueurEnCours,17,0);
      lcd.setCursor(0,0);
      lcd.print("Tour  : "+ String(tourEnCoursLJ));
      lcd.setCursor(0,2);
      lcd.print("Quand pret");        
      lcd.setCursor(8,3);
      lcd.print("Start --> OK");
      disableLedAnimation = false;
      disableSonAnimation = false;
      CodeLedAleatoire();
      SonsAleatoires();
      communications();
      delay(1);
    }
    boutonVirtuelE = false;
    attenteJoueurOuTour = false;
    EcranGo();   
  }  
}

void EcranEnJeuLJ()
{
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
  lcd.setCursor(0,0);
  lcd.print("Tour  : "+ String(tourEnCoursLJ));
  Serial.println("Tour  : "+ String(tourEnCoursLJ));
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
    printBigNum(dizaine.toInt(), 14, 1); // 12
    String unite = String(scorestring.charAt(1));
    printBigNum(unite.toInt(), 17, 1); // 15
  }
  else{
    String unite = String(scorestring.charAt(0));
    printBigNum(0, 14, 1); // 12
    printBigNum(unite.toInt(), 17, 1); // 15     
  }         
}

void EcranTourSuivantLJ()
{
  EcranScores1();
  lcd.clear();
  printBigNum(31, 2, 1);
  printBigNum(26, 5, 1);
  printBigNum(32, 8, 1);
  printBigNum(29, 11, 1); 
  printBigNum(tourEnCoursLJ,15,1);
  delay(2000);
  EcranScores1();
}

void trtPartieFinieLJ(){
  FinGame();
  Serial1.println("FIN_GAME");
  Serial.println("📤 Envoi à ESP32: FIN_GAME");
  Serial1.print("COUNTDOWN:");
  Serial1.println(0.000, 3); 
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
