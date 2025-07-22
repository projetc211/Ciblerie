
// Fonctions spécifiques pour le mode Classic avec gestion des bonus, chrono, transitions

void Penalite() {
  static bool bleuMontre = false;
  float tempsrestant = 0;

  if (!coursesCommencees)
  {
    EcranEnJeu();
    myDFPlayer.playMp3Folder(19);
    coursesCommencees = true;
    memset(colorIndexes, 0, sizeof(colorIndexes));
    memset(lastColorChanges, 0, sizeof(lastColorChanges));
    startTime = millis();
    tempsGenere = false;
    bleuMontre = false;
  }
  if(!coursesFinies && coursesCommencees) {
    if (!bleuMontre && tempsManche < 5.0) {
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      FastLED.show();
      delay(500);
      FastLED.clear(); FastLED.show();
      bleuMontre = true;
      return;
    }

    if (!tempsGenere) {
      tempsJeu = tempsManche;
      Period1 = tempsJeu * 1000;
      Period2 = tempsJeu * 1000;
      dernierCountdownEnvoye = tempsJeu;
      tempsGenere = true;
      if ((int)tempsJeu == 1) fill_solid(leds, NUM_LEDS, CRGB::Red);
      else if ((int)tempsJeu >= 2 && (int)tempsJeu <= 5) {
        int index = (int)tempsJeu - 2;
        fill_solid(leds, NUM_LEDS, colorArrays[index][colorIndexes[index]]);
        lastColorChanges[index] = millis();
      }
      FastLED.show();
      Moteur();
      ledsAllumeesMillis = millis();
      ledsAllumees = true;
    }
    if (ledsAllumees && millis() - ledsAllumeesMillis >= 900) {
      FastLED.clear(); FastLED.show();
      ledsAllumees = false;
    }
    if ((int)tempsJeu >= 2 && (int)tempsJeu <= 5) {
      int index = (int)tempsJeu - 2;
      if (millis() - lastColorChanges[index] >= 1000) {
        colorIndexes[index] = (colorIndexes[index] + 1) % colorSizes[index];
        fill_solid(leds, NUM_LEDS, colorArrays[index][colorIndexes[index]]);
        FastLED.show();
        lastColorChanges[index] = millis();
      }
    }
    tempsrestant = tempsJeu - (millis() - startTime) / 1000.0;
    lcd.setCursor(11, 3);
    lcd.print("TR: ");
    if (tempsrestant > 0) {
      lcd.print(tempsrestant, 3);
      Serial1.print("COUNTDOWN:");
      Serial1.println(tempsrestant, 3);
      dernierCountdownEnvoye = tempsrestant;
      zeroEnvoye = false;
    }
    else if (!coursesFinies && tempsrestant <= 0) {
      lcd.print("0.000");
      Serial1.print("COUNTDOWN:");
      Serial1.println(0.000, 3);
      dernierCountdownEnvoye = 0.0;
      zeroEnvoye = true;
    }
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
      resetAllBonuses();
      SMRAZ();
      EcranEnJeu();
      GererInterruption();
    }
  }
  else if (tempsrestant > 0) lcd.print(tempsrestant);
  else lcd.print("0.000");
}

void GererInterruption() {
  if (joueurEnCours == nbJoueurs && tourEnCours == nbTours && resteEnCours == 1) {
    testKiller();
    coursesCommencees = false;
    coursesFinies = false;
    partieEnCours = false;
    partieFinie = true;
    SMRAZ();
  }
  else if (joueurEnCours == nbJoueurs && resteEnCours == 1) {
    myDFPlayer.stop();
    testKiller();
    joueurEnCours = 1;
    resteEnCours = 3;
    tourEnCours++;
    EcranTourSuivant();
    EcranJoueurSuivant();
    Serial1.println("NEXT_TURN");
    Serial.println("📤 Envoi à ESP32: NEXT_TURN");
    Serial1.print("COUNTDOWN:");
    Serial1.println(0.000, 3);
    lcd.clear();
    attenteJoueurOuTour = true;
    while (digitalRead(boutonE) == HIGH && !boutonVirtuelE) {
      AcquisitionCapteurs();
      printBigNum(21, 13, 0);
      printBigNum(joueurEnCours, 17, 0);
      lcd.setCursor(0,0);
      lcd.print("Tour  : " + String(tourEnCours));
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
  else if (resteEnCours != 1) {
    resteEnCours--;
  }
  else {
    testKiller();
    joueurEnCours++;
    resteEnCours = 3;
    EcranJoueurSuivant();
    Serial1.println("NEXT_PLAYER");
    Serial.println("📤 Envoi à ESP32: NEXT_PLAYER");
    Serial1.print("COUNTDOWN:");
    Serial1.println(0.000, 3);
    lcd.clear();
    attenteJoueurOuTour = true;
    while (digitalRead(boutonE) == HIGH && !boutonVirtuelE) {
      AcquisitionCapteurs();
      printBigNum(21, 13, 0);
      printBigNum(joueurEnCours, 17, 0);
      lcd.setCursor(0,0);
      lcd.print("Tour  : " + String(tourEnCours));
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
