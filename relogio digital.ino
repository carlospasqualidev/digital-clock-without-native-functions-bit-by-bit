//================================= DISPLAY =================================//
const byte Numbers[] = { //CONFIGURA OS NUMEROS
  0xC0,
  0xF9,
  0xA4,
  0xB0,
  0x99,
  0x92,
  0x82,
  0xF8,
  0X80,
  0X90
};

const byte Display[] = { //CONFIGURA O DISPLAY
  0xF1,
  0xF2,
  0xF4,
  0xF8
};

const byte LATCH_DIO = 4, //CONFIGURA OS PINOS 
  CLK_DIO = 7,
  DATA_DIO = 8;


//=========================================================================//

//================================= LEDS =================================//
const byte led1 = 13, // CONFIGURA OS PINOS
  led2 = 12,
  led3 = 11,
  led4 = 10; 
//=========================================================================//


//================================ BUTTONS / BUZZER  ================================//

const byte buzzer = 3; // CONFIGURA OS PINOS DO BUZZER

const byte button1 = A1, //CONFIGURA OS PINOS DOS BOTÕES
  button2 = A2,
  button3 = A3; 

bool actualState1, actualState2, actualState3; //ARMAZENA O ESTADO ATUAL DO BOTÃO
bool beforeState1, beforeState2, beforeState3; //ARMAZENA O ESTADO ANTERIOR DO BOTÃO
//=========================================================================//

//================================= VARS =================================//

unsigned long millisSeconds = millis(); //ARMAZERA OS MILLIS

//CLOCK
volatile byte seconds = 0;
volatile byte minutes = 0;
volatile byte hours = 0;

//ALARM
volatile byte alarmMinutes = 0;
volatile byte alarmHours = 0;
bool callAlarm = false;

//MODE
byte mode = 3;
bool editingAlarm = false, editingClock = false;

//===========================================================================//

void setup() {
  //DISPLAY
  pinState(LATCH_DIO, OUTPUT);
  pinState(CLK_DIO, OUTPUT);
  pinState(DATA_DIO, OUTPUT);

  //BUZZER
  pinState(buzzer, OUTPUT);
  writesPin(buzzer, HIGH); //RESETA O BUZZER

  //BUTTONS
  pinState(button1, INPUT);
  pinState(button2, INPUT);
  pinState(button3, INPUT);

  //LEDS
  pinState(led1, OUTPUT);
  pinState(led2, OUTPUT);
  pinState(led3, OUTPUT);
  pinState(led4, OUTPUT);
  writesPin(led1, HIGH); //RESETA O LED
  writesPin(led2, HIGH); //RESETA O LED
  writesPin(led3, HIGH); //RESETA O LED
  writesPin(led4, HIGH); //RESETA O LED

  //CONFIGURA OS PINOS NOS REGISTRADORES PARA FUNÇÃO DE INTERRUPÇÃO
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT11);

  Serial.begin(9600);
}

void loop() {
  //BUTTONS
  actualState1 = Readpin(button1); //PEGA O ESTADO ATUAL DO BOTÃO 1
  actualState2 = Readpin(button2); //PEGA O ESTADO ATUAL DO BOTÃO 2
  actualState3 = Readpin(button3); //PEGA O ESTADO ATUAL DO BOTÃO 3

  // CHAMA A FUNÇÃO DE CONTAR OS SEGUNDOS
  countSeconds(); 

  //CHAMA A FUNÇÃO QUE SELECIONA OS MODOS DO RELOGIO
  modeSelection(); 

  switch (mode) {
  case 0:
    editingClockAlarm(); // CHAMA A FUNÇÃO DE REGULAR O ALARME
    break;
  case 1:
    editingClockHours(); // CHAMA  A FUNÇÃO DE REGULAR O RELÓGIO
    break;
  case 2:
    break; //SAI DOS MODOS DE EDIÇÃO
  }

  //DEFINE O ESTADO ANTERIOR DOS BOTÕES COM O ATUAL (EVITA MULTIPLOS CLICKS DOS BOTÕES)
  beforeState1 = actualState1;
  beforeState2 = actualState2;
  beforeState3 = actualState3;
}

// SELECIONA OS MODOS DO RELÓGIO
void modeSelection() { 
  if (actualState1 == LOW && !editingAlarm && !editingClock && beforeState1 != actualState1) { 
    editingAlarm = true; 
    mode = 0; //ENTRA NO MODO EDIÇÃO DO ALARME
  }
  if (actualState2 == LOW && !editingAlarm && !editingClock && beforeState2 != actualState2) { 
    editingClock = true;
    mode = 1; //ENTRA NO MODO EDIÇÃO DO RELÓGIO
  }
  if (actualState3 == LOW && !editingAlarm && !editingClock && beforeState3 != actualState3) { 
    callAlarm = !callAlarm; // ATIVA E DESATIVA O ALARME (SE VAI TOCAR OU NÃO NO HORARIO CONFIGURADO)
  }
}

//EDITA A HORA EM QUE O ALARME VAI TOCAR
void editingClockAlarm() { 

  writesPin(led1, LOW);// LIGA O LED DE EDIÇÃO
  if (alarmHours > 23) { // NÃO DEIXA A HORA SER MAIOR QUE 23
    alarmHours = 0;
  }
  if (alarmMinutes > 59) { // NÃO DEIXA O MINUTO SER MAIOR QUE 59
    alarmMinutes = 0;
    alarmHours += 1; // SE O MINUTO FOR MAIOR QUE 59, ADICIONA +1 A HORA
  }

  // ADICIONA MAIS 1 NA HORA
  if (actualState1 == LOW && beforeState1 != actualState1) { 
    alarmHours += 1; 
  }
  
  // ADICIONA MAIS 1 NO MINUTO
  if (actualState2 == LOW && beforeState2 != actualState2) { //muda a casa decimal de edicao
    alarmMinutes += 1;
  }

  //SAI DO MODO DE EDIÇÃO DO ALARME
  if (actualState3 == LOW && beforeState3 != actualState3) {
    editingAlarm = false; 
    callAlarm = true;//DEFINE QUE O ALARME VAI TOCAR NA HORA CONFIGURADA
    mode = 2; // SAI DO MODO DE EDIÇÃO
    writesPin(led1, HIGH); // DESLIGA O LED DE EDIÇÃO
  }
}

//EDITA A HORA DO RELÓGIO
void editingClockHours() { 

  writesPin(led2, LOW);// LIGA O LED DE EDIÇÃO
  if (hours > 23) {// NÃO DEIXA A HORA SER MAIOR QUE 23
    hours = 0;
  }
  if (minutes > 59) { // NÃO DEIXA O MINUTO SER MAIOR QUE 59
    minutes = 0;
    hours += 1;// SE O MINUTO FOR MAIOR QUE 59, ADICIONA +1 A HORA
  }

  // ADICIONA MAIS 1 NA HORA
  if (actualState1 == LOW && beforeState1 != actualState1) { 
    hours += 1;
  }

  // ADICIONA MAIS 1 NO MINUTO
  if (actualState2 == LOW && beforeState2 != actualState2) { //muda a casa decimal de edicao
    minutes += 1;
  }

  //SAI DO MODO DE EDIÇÃO DO RELÓGIO
  if (actualState3 == LOW && beforeState3 != actualState3) {
    editingClock = false;
    mode = 2; // SAI DO MODO DE EDIÇÃO
    writesPin(led2, HIGH); // DESLIGA O LED DE EDIÇÃO
  }
}

// CONTA OS SEGUNDOS
void countSeconds() { 
  if ((millis() - millisSeconds) > 1000) {
    millisSeconds = millis();

    spendTime(); // APOS 1 S CHAMAA FUNÇÃO DE PASSAR O TEMPO

  }
  showHours(); //CHAMA A FUNÇÃO DE MOSTRAR AS HORAS NO DISPLAY
}


// FUNÇÃO DE CONTROLE DO TEMPO
void spendTime() { 
  
  seconds++;
  if (seconds > 59) {
    seconds = 0;
    minutes++;
    if (minutes > 59) {
      minutes = 0;
      hours++;
      if (hours > 23) {
        hours = 0;
      }
    }
  }
}

// FUNÇÃO DE MOSTRAR AS HORAS
void showHours() { 
  if (editingAlarm) { // MOSTRA A CONFIGURAÇÃO DO ALARME NO DISPLAY
    showDisplay(0, alarmHours / 10);
    showDisplay(1, alarmHours % 10);
    showDisplay(2, alarmMinutes / 10);
    showDisplay(3, alarmMinutes % 10);
  } else { // MOSTRA AS HORAS DO RELÓGIO
    showDisplay(0, hours / 10);
    showDisplay(1, hours % 10);
    showDisplay(2, minutes / 10);
    showDisplay(3, minutes % 10);

    //VERIFICA SE ESTA NA HORA DE DESPERTAR
    alarm(); 
  }

}

//FUNÇÃO RESPONSAVEL POR TOCAR O ALARME
void alarm() { 


  if (callAlarm) { // VERIFICA SE O ALERME ESTA LIGADO OU NÃO PARA INDICAR COM O LED 
    writesPin(led4, LOW);
  } else {
    writesPin(led4, HIGH);
  }

  if ((hours == alarmHours) && (minutes == alarmMinutes) && (callAlarm)) {
    writesPin(buzzer, LOW); //QUANDO A HORA COINCIDIR COM O ALARME O BUZZER VAI LIGAR
  }
}

// INTERRUPÇÃO PARA FAZER O BUZZER PARAR DE TOCAR QUANDO O BOTÃO A3 FOR PRESSIONADO
ISR(PCINT1_vect) {
  writesPin(buzzer, HIGH); //DESLIGA O BUZZER
}

//DISPLAY FUNCTIONS
void showDisplay(byte segment, byte value) {
  writesPin(LATCH_DIO, 1);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, Numbers[value]);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, Display[segment]);
  writesPin(LATCH_DIO, 0);
}

//FUNÇÕES PARA MANIPULAÇÃO DOS REGISTRADOS (SUBSTITUI AS FUNÇÕES NATIVAS)
void pinState(int pin, int state) 
{
  if (pin >= 0 && pin <= 7) {
    if (state == INPUT) {
      DDRD &= ~(1 << pin);
    } else if (state == OUTPUT) {
      DDRD |= (1 << pin);
    }
  } else if (pin >= 8 && pin <= 13) {
    if (state == INPUT) {
      DDRB &= ~(1 << (pin - 8));
    } else if (state == OUTPUT) {
      DDRB |= (1 << (pin - 8));
    }
  } else if (pin >= 14 && pin <= 19) {
    if (state == INPUT) {
      DDRC &= ~(1 << (pin - 14));
    } else if (state == OUTPUT) {
      DDRC |= (1 << (pin - 14));
    }
  }
}

void writesPin(int pin, int state) // Funcao faz a mesma coisa que o digitalWrite
{
  if (pin >= 0 && pin <= 7) {
    if (state == 0) {
      PORTD &= ~(1 << pin);
    } else if (state == 1) {
      PORTD |= (1 << pin);
    }
  } else if (pin >= 8 && pin <= 13) {
    if (state == 0) {
      PORTB &= ~(1 << (pin - 8));
    } else if (state == 1) {
      PORTB |= (1 << (pin - 8));
    }
  } else if (pin >= 14 && pin <= 19) {
    if (state == 0) {
      PORTC &= ~(1 << (pin - 14));
    } else if (state == 1) {
      PORTC |= (1 << (pin - 14));
    }
  }
}

bool Readpin(int pin) { // Funcao faz a mesma coisa que digitalRead/analogRead
  bool pinState;

  if (pin >= 0 && pin <= 7) {
    pinState = (PIND & (1 << pin));
  }
  if (pin >= 8 && pin <= 13) {
    pinState = (PINB & (1 << (pin - 8)));
  }
  if (pin >= 14 && pin <= 19) {
    pinState = (PINC & (1 << (pin - 14)));
  }
  return (pinState);
}
