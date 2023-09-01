// Zombie Escape by Jeffrey Disse
// https://www.youtube.com/watch?v=XJBQz-13DXA

//Include the Liquid Crystal Library
#include <LiquidCrystal.h>

//Set pins for LCD display
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Set pins for Buzzer and Joystick button
const int buzzer = 8;
const int startPin = 9;

//Set pins for Joystick axes. 
#define joyX A0
#define joyY A1

//Set default variable for the program
int playerPos = -1;
int playerRow = 1;
int playersprite = 4;
int zombiePos = 12;
int zombieRow = 1;
int zombiesprite = 1;
int zombiespeed = 0;
int score = 0;
int lives = 3;
int keyLocation;
int doorLocation;
int beat = 0;
bool hasKey = false;
bool mapWon = false;
bool gamestarted = false;


//Set the arrays for the maps.
int maps[5][26] = {

  {
  0,0,7,0,0,0,0,0,0,0,0,8,219,
  0,0,0,0,219,0,0,0,219,0,0,0,0
  },

  {
  0,0,7,0,0,0,0,219,0,219,0,0,0,
  0,0,0,0,219,0,0,0,0,0,8,0,0
  },

  {
  7,0,219,0,0,0,0,219,0,0,0,0,8,
  0,0,0,0,219,0,0,0,0,0,219,0,0
  },
  {
  0,0,0,0,0,0,0,0,219,0,0,0,219,
  0,219,7,0,219,0,0,0,8,0,219,0,219
  },  
  {
  219,7,219,0,0,0,219,0,0,0,0,0,219,
  0,0,0,0,219,0,0,0,0,0,219,0,8
  }
};

//Create special charaters for the LCD display. 
byte zombieleft[8] = {
  0b00000,
  0b00110,
  0b01010,
  0b01110,
  0b11110,
  0b00010,
  0b00110,
  0b01010
};

byte zombieright[8] = {
  0b00000,
  0b01100,
  0b01010,
  0b01110,
  0b01111,
  0b01000,
  0b01100,
  0b01010
};

byte lookleft[8] = {
  0b01100,
  0b10100,
  0b11100,
  0b00100,
  0b11100,
  0b00100,
  0b01010,
  0b10001
};

byte lookright[8] = {
  0b00110,
  0b00101,
  0b00111,
  0b00100,
  0b00111,
  0b00100,
  0b01010,
  0b10001
};

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte trophy[8] = {
  0b11111,
  0b01110,
  0b01110,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b01110
};

byte door[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b10111,
  0b11111,
  0b11111,
  0b11111
};

byte key[8] = {
  0b01110,
  0b10001,
  0b10001,
  0b01110,
  0b00100,
  0b00100,
  0b00111,
  0b00111
};

//define the variables for the joystick. 
int xMap, yMap, xValue, yValue;

//Function for getting the position of the joystick.
int joystickPos() {

  xValue = analogRead(joyX);
  yValue = analogRead(joyY);

  int output = 0;

  if (yValue > 600) {
    output = 1;
    playersprite = 3;
  }
  if (yValue < 400) {
    output = 2;
    playersprite = 4;
  }
  if (xValue > 600) {
    output = 3;
  }
  if (xValue < 400) {
    output = 4;
  }

  return output;
}

//Function for playing sounds
void sound(int frequency, int duration) {

  tone(buzzer, frequency);

  if (duration > 0) {
    delay(duration);
    noTone(buzzer);
  }
}


//Function for moving the player
int movePlayer(int pos) {
  int newPos = -1;
  int oldPos = pos;
  int oldplayerRow = playerRow;
  int newSprite;
  int direction = joystickPos();
  int convertedPos;
  int oldConvertedPos;

  //If no position is set, move the player to the starting position.
  if (pos == -1) {
    newPos = 0;
    pos = 0;
  }


//Move the player based on the input from the joystick.
  if (direction == 1) {
    newPos = pos - 1;
  }
  if (direction == 2) {
    newPos = pos + 1;
  }
  if (direction == 3 && playerRow == 0) {
    playerRow = 1;
    newPos = oldPos;
  }
  if (direction == 4 && playerRow == 1) {
    playerRow = 0;
    newPos = oldPos;
  }

//Convert the actual position to a single number to make comparative statements. 
  convertedPos = convertPos(newPos, playerRow);
  oldConvertedPos = convertPos(oldPos, oldplayerRow);

//See if the new position is within the bounds of the maps
//As well as checking to see if a wall is blocking the move. 
  if (newPos >= 0 && newPos < 14 - 1 && maps[score][convertedPos] != 219 && direction != 0) {

    //look up the current position on the map to see what element needs to be placed once the player moves.
    lcd.setCursor(oldPos, oldplayerRow);

    if (maps[score][oldConvertedPos] > 0) {
      lcd.write(maps[score][oldConvertedPos]);
    } else {
      lcd.write(" ");
    }

    lcd.setCursor(newPos, playerRow);
    lcd.write(playersprite);

    //Check to see if they player is standing at the position of the Key. 
    //If so give the player the key.
    if (convertedPos == keyLocation && hasKey == false) {
      hasKey = true;
      sound(700, 150);
      sound(900, 150);
      maps[score][convertedPos] = 0;
    }

    //Check to see if they player is standing at the location of the door
    //If so, and they have the key, then increment the score end the level. 
    if (convertedPos == doorLocation && hasKey == true) {
      maps[score][keyLocation] = 8;
      sound(500, 400);
      sound(700, 400);
      score++;
      startgame();
      return;
    }

    delay(150);

    return newPos;
  } else {

    //If the new move places the player out of bounds
    //keep the player in the current position.
    playerRow = oldplayerRow;
    oldPos = pos;
    lcd.setCursor(oldPos, oldplayerRow);
    lcd.write(playersprite);

    return oldPos;
  }

}

//Function for moving the player
int moveZombie(int pos) {
  int newPos = -1;
  int oldPos = pos;
  int oldzombieRow = zombieRow;
  int newSprite;
  int convertedPos;
  int oldConvertedPos;

// Change the direction that the Zombie is running based on the positon of the player
  if (zombiePos > playerPos) {
    newPos = pos - 1;
    zombiesprite = 1;
  } else if (zombiePos < playerPos) {
    newPos = pos + 1;
    zombiesprite = 2;
  } else {
    newPos = pos;
  }

//If the Zombie is at the same position but on a different row
// then move the zombie to the same row as the player. 
  if (zombiePos == playerPos) {
    zombieRow = playerRow;
  }

//Convert the actual position to a single number to make comparative statements. 
  convertedPos = convertPos(newPos, zombieRow);
  oldConvertedPos = convertPos(oldPos, oldzombieRow);


//See if the new position is within the bounds of the map.
  if (newPos >= 0 && newPos < 14 - 1) {

    //Move the Zombie to the other row if a wall is blocking the move. 
    if (maps[score][convertedPos] == 219) {
      if (zombieRow == 1) {
        zombieRow = 0;
      } else {
        zombieRow = 1;
      }
    }

    //look up the current position on the map to see what element needs to be placed once the zombie moves.
    lcd.setCursor(oldPos, oldzombieRow);

    if (maps[score][oldConvertedPos] > 0) {
      lcd.write(maps[score][oldConvertedPos]);
    } else {
      lcd.write(" ");
    }

    lcd.setCursor(newPos, zombieRow);
    lcd.write(zombiesprite);

    delay(400);
    return newPos;

  } else {
    //If the Zombie's move is out of bounds
    //keep the zombie in the current position.
    zombieRow = oldzombieRow;
    oldPos = pos;
    lcd.setCursor(oldPos, oldzombieRow);
    lcd.write(zombiesprite);
    delay(400);
    return oldPos;
  }

}


//Function to convert the actual position to a single number to make comparative statements.
int convertPos(int col, int row) {

  int output = col;

  if (row == 1) {
    output = col + 13;
  }
  return output;
}

//Function to write the map to the screen. 
void generateMap() {

  //Iterate over each item in the map array.
  for (int i = 0; i < 26; i++) {
    //If the item is not a blank then proceed with writing the item to the screen.
    if (maps[score][i] > 0) {
      //If the item is on the first row.
      if (i < 12 + 1) {
        lcd.setCursor(i, 0);
        lcd.write(maps[score][i]);

        //If the item is the key, then set the keyLocation variable.
        if (maps[score][i] == 8) {
          keyLocation = i;
        }
        //If the item is the door, then set the doorLocation variable.
        if (maps[score][i] == 7) {
          doorLocation = i;
        }
      } else {
        //If the item is on the second row.
        lcd.setCursor(i - 13, 1);
        lcd.write(maps[score][i]);
        //If the item is the key, then set the keyLocation variable.
        if (maps[score][i] == 8) {
          keyLocation = i;
        }
        //If the item is the door, then set the doorLocation variable.
        if (maps[score][i] == 7) {
          doorLocation = i;
        }
      }
    }

  }

}

//Function to update the lives on the screen.
void updateLives() {
  lcd.setCursor(14, 0);
  lcd.write(5);
  lcd.setCursor(15, 0);
  lcd.print(lives);
}

//Function to update the score on the screen.
void updateScore() {
  lcd.setCursor(14, 1);
  lcd.write(6);
  lcd.setCursor(15, 1);
  lcd.print(score);
}

//Function to start or restart the level.
void startgame() {

  //If all levels have been won, then end the game and display the winning message and song.
  if (sizeof(maps) / sizeof(maps[0]) == score) {
    gamestarted = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("* YOU ESCAPED! *");
    lcd.setCursor(0, 1);
    lcd.print("* THE ZOMBIES! *");

    //Play a winning song
    int note = 400;

    for (int i = 0; i < 5; i++) {
      sound(note, 400);
      sound(note * 2, 400);
      sound(note * 3, 400);
      sound(note * 4, 400);
      note = note + 100;
      delay(1000);
    }

    //Reset the score and lives.
    delay(2000);
    lcd.clear();
    score = 0;
    lives = 3;
    return;
  }

  //Reset the game to start the level.
  playerPos = -1;
  playerRow = 1;
  playersprite = 4;
  zombiePos = 12;
  zombieRow = 1;
  zombiesprite = 1;
  hasKey = false;

  //Update the screen and send the map to the screen.
  lcd.clear();
  updateScore();
  updateLives();
  generateMap();

}

void setup() {

  //Set the pins 
  pinMode(startPin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  //initiate the screen and assign the special characters to the screen
  lcd.begin(16, 2);
  lcd.createChar(1, zombieleft);
  lcd.createChar(2, zombieright);
  lcd.createChar(3, lookleft);
  lcd.createChar(4, lookright);
  lcd.createChar(5, heart);
  lcd.createChar(6, trophy);
  lcd.createChar(7, door);
  lcd.createChar(8, key);

}

void loop() {

  //Pole the joystick
  int sensorReading = analogRead(A0);

  //If the game is not started, run the start screen.
  if (gamestarted == false) {

    lcd.setCursor(1, 0);
    lcd.print("ZOMBIE ESCAPE!!");

    //Play the theme song.
    sound(300, 150);
    sound(200, 150);
    sound(300, 150);
    sound(200, 150);
    delay(150);

    if (beat == 0) {
      lcd.setCursor(0, 1);
      lcd.write(2);
      sound(150, 150);
      beat = 1;
    } else {
      lcd.setCursor(0, 1);
      lcd.write(4);
      sound(100, 150);
      beat = 0;
    }
    delay(400);

    //Check to see if the Start Button is beening pressed.
    //If so start the game.
    if (digitalRead(startPin) == LOW) {
      sound(200, 400);
      sound(400, 400);
      sound(600, 400);
      gamestarted = true;
      startgame();
      return;
    }

    //Get the position of the Joystick.
    //Change the game difficulty if the user is requesting so.
    int selection = joystickPos();

    //Set the game mode to Easy if the joystick is moving down.
    if (selection == 3 || zombiespeed == 2) {
      zombiespeed = 2;
      lcd.setCursor(5, 2);
      lcd.print("-EASY-");
    }
    //Set the game mode to Hard if the joystick is moving up.
    if (selection == 4 || zombiespeed == 1) {
      zombiespeed = 1;
      lcd.setCursor(5, 2);
      lcd.print("-HARD-");
    }
    //Set the default game mode to Easy if no selection has been made yet.
    if (zombiespeed == 0) {
      zombiespeed = 2;
      lcd.setCursor(5, 2);
      lcd.print("-EASY-");
    }
  }

  //If the game has been started then proceed with the gameplay.
  if (gamestarted == true) {

    //Update the Zombies position.
    if (millis() % (zombiespeed * 1000) == 0) {
      zombiePos = moveZombie(zombiePos);
    }

    //Update the Players position.
    playerPos = movePlayer(playerPos);

    //If the player is at the same position as the Zombie
    //take a live from the player, then restart the level.
    if (convertPos(playerPos, playerRow) == convertPos(zombiePos, zombieRow)) {
      lives = lives - 1;
      updateLives();
      maps[score][keyLocation] = 8;
      //If the player has no lives left, then end the game.
      if (lives < 1) {
        gamestarted = false;
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("* GAME OVER *");
        sound(300, 300);
        sound(200, 300);
        sound(100, 300);
        delay(2000);
        score = 0;
        lives = 3;
        return 0;
      } else {
        sound(300, 400);
        sound(400, 400);
        startgame();
      }

    }

  }

}