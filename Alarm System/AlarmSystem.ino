#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {22, 24, 26, 30};
byte colPins[COLS] = {32, 34, 38, 40};
char customKey;

char password[] = "aaaaa";
char correct_password[] = "12345";

char password_m[] = "bbbb";
char correct_password_m[] = "9876";

int buzzer = 8;
int pirPin = 7;

bool initializeLCD = true;

String stare = "DISARMED";

int contrast = 30;

int counter1 = 0;
int counter2 = 0;

int value = 0;
bool toSound = false;

int interval = 15000;
int prevT = 0;
int startT = 0;

int contor_greseli = 0;

String inputString = "";
boolean stringComplete = false;

char caract;
String pass_blu = "aaaaa";

char password_b[] = "00000";
char password_m_b[]="zzzz";

int flag = 0;

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);
  lcd.begin(16, 2);

  analogWrite(6, contrast);
  pinMode(buzzer, OUTPUT);
  pinMode(pirPin, INPUT);
}


void loop() {

  disarmedLCD();

  bluetooth_function();

  keyPad_function();

  value = digitalRead(pirPin);
  buzzerFunction();


}


void disarmedLCD()
{
  if (initializeLCD)
  {
    initializeLCD = false;
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("STATE: ");
    lcd.print(stare);

    lcd.setCursor(0, 1);
    lcd.print("PRESS * to ARM.");
  }
}


void armedLCD()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STATE: ");
  lcd.print(stare);

  lcd.setCursor(0, 1);
  lcd.print("PASSWORD:");
  lcd.setCursor(9, 1);


  for (int i = 0; i < 5; i++) {
    if (password[i] == 'a') {
      lcd.setCursor(9 + i, 1);
      lcd.print(" ");
    } else {
      lcd.setCursor(9 + i, 1);
      lcd.print(password[i]);
    }
  }
}


void armedLCD_buzz()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STATE: ");
  lcd.print(stare);

  lcd.setCursor(0, 1);
  lcd.print("PASSWORD_M:");
  lcd.setCursor(11, 1);


  for (int i = 0; i < 4; i++) {
    if (password_m[i] == 'b') {
      lcd.setCursor(11 + i, 1);
      lcd.print(" ");
    } else {
      lcd.setCursor(11 + i, 1);
      lcd.print(password_m[i]);
    }
  }
}

void bluetooth_LCD()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STATE: ");
  lcd.print(stare);

  lcd.setCursor(0, 1);
  lcd.print("ENTER THE PASS");
}


void keyPad_function()
{
  customKey = customKeypad.getKey();
  if (customKey) {

    Serial.print("customKey: ");
    Serial.println(customKey);

    if (stare == "DISARMED" ) {
      flag=0;
      if (customKey == '*') {
        stare = "ARMED";
        armedLCD();
      }
    }
    else
    {

      if (toSound == true)
      {
        if (customKey == 'D')
        {
          if (strcmp(password_m, correct_password_m) == 0 ) {
            Serial.println("Se potrivesc cele doua parole master");
            // Disarm the system
            resetPassword_m();
            contor_greseli = 0;
            flag = 0;
            toSound = false;
            noTone(buzzer);
            stare = "DISARMED";
            initializeLCD = true;
            disarmedLCD();
          }
          else
          {
            Serial.println("Nu se potriveste");
            stare = "ARMED";
            resetPassword_m();
          }

        }
        else if (counter2 <= 3)
        {
          stare = "ARMED";
          password_m[counter2] = customKey;
          Serial.println(password_m);
          counter2++;
          armedLCD_buzz();
        }
      }
      else
      {
        if (customKey == '#')
        {
          if (strcmp(password, correct_password) == 0 ) {
            Serial.println("Se potrivesc cele doua parole");
            // Disarm the system
            resetPassword();
            stare = "DISARMED";
            initializeLCD = true;
            disarmedLCD();
          }
          else
          {
            stare = "ARMED";
            Serial.println("Nu se potriveste");
            contor_greseli++;
            resetPassword();
          }

        }
        else if (counter1 <= 4)
        {
          password[counter1] = customKey;
          Serial.println(password);
          counter1++;
          armedLCD();
        }
      }
    }
  }
}



void resetPassword()
{
  for (int i = 0; i < 5; i++)
  {
    password[i] = 'a';
  }
  counter1 = 0;
  armedLCD();
}

void resetPassword_m()
{
  for (int i = 0; i < 4; i++)
  {
    password_m[i] = 'b';
  }
  counter2 = 0;
  armedLCD_buzz();
}


void buzzerFunction()
{
  if (contor_greseli >= 3)
  {
    toSound = true;
  }
  int a = millis();

  if (flag == 0)
  {
    startT = a;
  }
  if (value > 0 && stare == "ARMED" ) {
    flag = 1;
  }
  if (flag == 1 && a - startT >= interval)
  {
   if(stare=="ARMED")
   {
    toSound = true;
   }
   
  }
  soundBuzzer();
}


void soundBuzzer()
{
  if (toSound == true)
  {
    tone(buzzer, 20);
    armedLCD_buzz();
  }
}

void serialEvent1()
{
  if (Serial1.available())
  {
    inputString = Serial1.readStringUntil('\n');
    stringComplete = true;
    //Serial.print(inputString);
  }
}

void bluetooth_function()
{
  if (stringComplete)
  {
    if (stare == "DISARMED")
    {
      flag=0;
      if (inputString[0] == 'A')
      {
        stare = "ARMED";
        armedLCD();
      }
      else
      {
        stare = "DISARMED";
        disarmedLCD();
      }
    }
    else
    {
      if (toSound == false)
      {
        if (inputString[0] == 'D')
        {
          pass_blu = inputString.substring(2, 7);
          for (int i = 0; i < 5; i++)
          {
            password_b[i] = pass_blu[i];
          }
          Serial.println(pass_blu);
          if (strcmp(password_b, correct_password) == 0)
          {
            stare = "DISARMED";
            //resetPassword();
            initializeLCD = true;
            disarmedLCD();
          }
          else
          {
            stare = "ARMED";
          }
        }
      }
      else
      {
        if (inputString[0] == 'M')
        {
          pass_blu = inputString.substring(2, 6);
          for (int i = 0; i < 4; i++)
          {
            password_m_b[i] = pass_blu[i];
          }

          if (strcmp(password_m_b, correct_password_m) == 0)
          {
            toSound = false;
            flag=0;
            noTone(buzzer);
            stare = "DISARMED";
            //contor_greseli=0;
            initializeLCD = true;
            disarmedLCD();
          }
          else
          {
            stare = "ARMED";
          }
        }
      }
    }
  }
}
