// --- Variables anti-double-tir (à placer en haut du fichier ou en global) ---
bool cible1Traitee = false;
bool cible2Traitee = false;
bool cible3Traitee = false;
bool cible4Traitee = false;
bool cible5Traitee = false;
bool cible6Traitee = false;
bool cible7Traitee = false;
bool cible8Traitee = false;
bool cible9Traitee = false;
bool cible10Traitee = false;

void EcranInitialisation() {
  lcd.print("Nb joueurs : " + String(nbJoueurs));
}

void InitGame() {
  FastLED.clear(); FastLED.show();
  myDFPlayer.stop();
  myDFPlayer.playMp3Folder(19);
  tourEnCours = 1;
  joueurEnCours = 1;
  resteEnCours = 3; // <<<--- Nb de tir
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

void trtPartieEnCours() {
  FastLED.clear(); FastLED.show();
  Temporisation();
  EcranEnJeu();
  while (resteEnCours > 0 && partieEnCours) {
    AcquisitionCapteurs();
    Penalite();
    if (partieFinie == true) {
      break;
    } else {
      killer[joueurEnCours] = 0;
    }
    delay(1);

    // --- PROTECTION ANTI-DOUBLE-TIR POUR CHAQUE CIBLE ---

    statusCible1 = digitalRead(cible1);
    if (statusCible1 == LOW && !cible1Traitee) {
      coursesCommencees = false;
      coursesFinies = false;
      fill_solid(leds, NUM_LEDS, CRGB::Red); FastLED.show();
      myDFPlayer.playMp3Folder(1); delay(3000);
      FastLED.clear(); FastLED.show();
      scores[joueurEnCours] = scores[joueurEnCours] + 0 + pointBonus0[joueurEnCours];
      pointBonus0[joueurEnCours] = pointBonus0[joueurEnCours] + 0;
      resetAllBonusExcept(0);
      String message = "J" + String(joueurEnCours -1) + " : " + String(0) + " : " + String((pointBonus0[joueurEnCours]) -1) + " : " + String(scores[joueurEnCours]);
      Serial1.println(message);
      Serial.println("📤 Envoi à ESP32: " + message);
      // Chrono spécial cible 1
      dernierCountdownEnvoye = 0.0;
      float tempsUtilise = tempsJeu;
      Serial1.print("COUNTDOWN:"); Serial1.println(0.000, 3);
      Serial1.print("END_MANCHE:"); Serial1.print(joueurEnCours - 1); Serial1.print(":"); Serial1.println(tempsUtilise, 3);
      tempsJeu += tempsManche;
      startTime = millis();
      dernierCountdownEnvoye = tempsJeu;
      resteEnCours--;
      cible1Traitee = true;
    }
    if (statusCible1 == HIGH) cible1Traitee = false;

    statusCible2 = digitalRead(cible2);
    if (statusCible2 == LOW && !cible2Traitee) {
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
      cible2Traitee = true;
    }
    if (statusCible2 == HIGH) cible2Traitee = false;

    statusCible3 = digitalRead(cible3);
    if (statusCible3 == LOW && !cible3Traitee) {
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
      cible3Traitee = true;
    }
    if (statusCible3 == HIGH) cible3Traitee = false;

    statusCible4 = digitalRead(cible4);
    if (statusCible4 == LOW && !cible4Traitee) {
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
      cible4Traitee = true;
    }
    if (statusCible4 == HIGH) cible4Traitee = false;

    statusCible5 = digitalRead(cible5);
    if (statusCible5 == LOW && !cible5Traitee) {
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
      cible5Traitee = true;
    }
    if (statusCible5 == HIGH) cible5Traitee = false;

    statusCible6 = digitalRead(cible6);
    if (statusCible6 == LOW && !cible6Traitee) {
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
      cible6Traitee = true;
    }
    if (statusCible6 == HIGH) cible6Traitee = false;

    statusCible7 = digitalRead(cible7);
    if (statusCible7 == LOW && !cible7Traitee) {
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
      cible7Traitee = true;
    }
    if (statusCible7 == HIGH) cible7Traitee = false;

    statusCible8 = digitalRead(cible8);
    if (statusCible8 == LOW && !cible8Traitee) {
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
      cible8Traitee = true;
    }
    if (statusCible8 == HIGH) cible8Traitee = false;

    statusCible9 = digitalRead(cible9);
    if (statusCible9 == LOW && !cible9Traitee) {
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
      cible9Traitee = true;
    }
    if (statusCible9 == HIGH) cible9Traitee = false;

    statusCible10 = digitalRead(cible10);
    if (statusCible10 == LOW && !cible10Traitee) {
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
      cible10Traitee = true;
    }
    if (statusCible10 == HIGH) cible10Traitee = false;
  }
  float tempsUtilise = tempsJeu - dernierCountdownEnvoye;
  Serial1.print("END_MANCHE:");
  Serial1.print(joueurEnCours - 1);
  Serial1.print(":");
  Serial1.println(tempsUtilise, 3);
  SMRAZ();
  GererInterruption();
}

void trtPartieFinie() {
  FinGame();
  Serial1.println("FIN_GAME");
  Serial.println("📤 Envoi à ESP32: FIN_GAME");
  Serial1.print("COUNTDOWN:");
  Serial1.println(0.000, 3);
  while (killer[1] == 1 || killer[2] == 1 || killer[3] == 1 || killer[4] == 1) {
    Serial.println("KILLER STATUS J1 " + String(killer[1]) + String(killer[2]) + String(killer[3]) + String(killer[4]));
    for (int i = 1; i <= nbJoueurs; i++) {
      if (killer[i] == 1) {
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
