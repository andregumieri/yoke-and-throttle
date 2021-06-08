// https://github.com/MHeironimus/ArduinoJoystickLibrary

#include <Joystick.h>

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, 
  0, // Número de botões
  0, // Número de hatswitches
  true, // Eixo X
  true, // Eixo Y
  false, // Eixo Z 
  false, // Eixo Rx
  false, // Eixo Ry
  false, // Eixo Rz
  false, // Rudder
  true, // Throttle
  true, // Accelerator
  false, // Brake
  false //Steering
);

int X_MAX = 925;
int X_MIN = 233;
int X_MID = 580;
int X_DEAD_ZONE_STEPS = 35;

int Y_MAX = 657;
int Y_MIN = 357;
int Y_MID = 508;
int Y_DEAD_ZONE_STEPS = 35;


// Variaveis de controle
int val = 0;

int mapX = 0;
int readX = 0;
int readXOriginal = 0;

int standby = 1;
int standbyCount = 0;
int standbyLastX = X_MAX + 1;
int standbyLastY = Y_MAX + 1;

int mapY = 0;
int readY = 0;
int readYOriginal = 0;

int foi = 0;

/** 
 * Setup
 */
void setup() {  
  // Inicia o serial
  Serial.begin(9600);

  Joystick.setXAxisRange(-127, 127);
  Joystick.setYAxisRange(-127, 127);
  Joystick.setThrottleRange(0, 255);
  Joystick.setAcceleratorRange(0, 255);

  // Initialize Joystick Library
  Joystick.begin();
}

/** 
 * Retorna a leitura o analogico do Y
 */
int analogY() {
  return analogRead(1);
}

/** 
 * Retorna a leitura o analogico do X
 */
int analogX() {
  return analogRead(0);
}

/**
 * Retorna a leitura analogica do throttle
 */
int analogThrottle() {
  return analogRead(2);
}

/**
 * Retorna a leitura analogica do throttle
 */
int analogAccelerator() {
  return analogRead(3);
}

/** 
 * Retorna a posição de joystick do X
 */
int x(int do_debug = 0) {
  readX = analogX();
  readXOriginal = readX;

  if(isDeadX() == 1 && standby == 1) {
    readX = X_MID;
  }

  if(readX < X_MIN) {
    readX = X_MIN;
  }

  if(readX > X_MAX) {
    readX = X_MAX;
  }
  
  mapX = map(readX, X_MIN, X_MAX, -127, 127);

  if(do_debug == 1 && standby == 0) {
    Serial.println("== X ==");
    debug(readX, readXOriginal, mapX);
  }

  return mapX;
}

/** 
 * Retorna a posição de joystick do Y
 */
int y(int do_debug = 0) {
  readY = analogY();
  readYOriginal = readY;

  if(isDeadY() == 1 && standby == 1) {
    readY = Y_MID;
  }

  if(readY < Y_MIN) {
    readY = Y_MIN;
  }

  if(readY > Y_MAX) {
    readY = Y_MAX;
  }
  
  mapY = map(readY, Y_MIN, Y_MAX, -127, 127);

  if(do_debug == 1 && standby == 0) {
    Serial.println("== Y ==");
    debug(readY, readYOriginal, mapY);
  }

  return mapY;
}

/** 
 * Verifica se o Y está em zona morta
 */
int isDeadY() {
  if(readY >= Y_MID - Y_DEAD_ZONE_STEPS && readY <= Y_MID + Y_DEAD_ZONE_STEPS) {
    return 1;
  }
  return 0;
}

/** 
 * Verifica se o X está em zona morta
 */
int isDeadX() {
  if(readX >= X_MID - X_DEAD_ZONE_STEPS && readX <= X_MID + X_DEAD_ZONE_STEPS) {
    return 1;
  }
  return 0;
}

/** 
 * Faz checagem de se está em standby
 */
void checkStandby() {
  int x = analogX();
  int y = analogY();
  
  // Se esta em zona morta
  if(isDeadY() == 1 && isDeadX() == 1) {
    // Se X e Y estão dentro da tolerancia de repeticao
    
    if((x >= standbyLastX - 2 && x <= standbyLastX + 2) && (y >= standbyLastY - 2 && y <= standbyLastY + 2)) {
      standbyCount = standbyCount + 1;
    } else {
      standbyCount = 0;
    }
  } else {
    standbyCount = 0;
  }

  if(standbyCount > 10 && standby < 1) {
    standby = 1;
    Serial.println("Entrou em standby");
  } else if(standbyCount < 3 && standby == 1) {
    standby = 0;
    Serial.println("Saiu de standby");
  }

  standbyLastX = x;
  standbyLastY = y;
}

/** 
 * Loop da aplicação
 */
void loop() {
  checkStandby();
  
  Joystick.setXAxis( x(0) );
  Joystick.setYAxis( y(0) );

//  map(analogThrottle(), 340, 750, 0, 255);
  Joystick.setThrottle(map(analogThrottle(), 340, 750, 0, 255));

  Joystick.setAccelerator(map(analogAccelerator(), 300, 600, 0, 255));

//  Serial.print(analogThrottle());
//  Serial.println(" -> ");
//  Serial.println(map(analogThrottle(), 340, 750, 0, 255));
  
  delay(100);
}

/** 
 * Escreve debug no serial
 */
void debug(int read_limits, int read_original, int mapped) {
  Serial.print(read_limits);
  Serial.print("/");
  Serial.print(read_original);
  Serial.print(" -> ");
  Serial.print(mapped);
  Serial.println("");  
}
