/******************************************************************
**        Trabalho Prático GAT135 - Irrigador Automático         **
**                                                               **
**  Irrigador automático com Arduino UNO(ATmega328P).            **
**  Ao ligar, o kit irá aferir a umidade do solo                 **
**  uma vez por hora. Caso a umidade esteja abaixo do limiar,    **
**  aciona o relé que controla a válvula solenoide e rega        **
**  por um tempo determinado.                                    **
**                                                               **
**  Autor: Eduardo Dezena Gonçalves                              **
**  UFLA - Lavras /MG - dezembro de 2023                         **
******************************************************************/

// Bibliotecas auxilares
#include <LiquidCrystal.h>
#include <RTClib.h>

// Pinos para controlar o LCD
const int rs = 2;   // Laranja
const int en = 4;   // Roxo
const int d4 = 10;  // Azul
const int d5 = 11;  // Branco
const int d6 = 12;  // Amarelo
const int d7 = 13;  // Verde

const int pinoRele = 8;           // Verde               // Pino que controla o relé
const int pinoSensorUmidade = A0; // Amarelo             // Pino que controla o sensor de umidade

// Pinos do RTC: A4 (verde) e A5 (marrom)
const int limiarUmidade = 50;                          // Limiar já padronizado para 100 
const int tempoRega = 3000;                            // Tempo de rega dado em milissegundos
const unsigned long intervaloVerificacao = 3600000UL;  // Intervalo de verificação em milissegundos

static int umidadeAferida = 0;                          // Variável para controle da rega

// Instancia o LCD e o RTC
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
RTC_DS3231 rtc;

// Configurações do programa
void setup() {
  
  // Inicialização do relé
  pinMode(pinoRele, OUTPUT);
  digitalWrite(pinoRele, LOW);

  // Inicialização do lcd
  lcd.begin(16,2);

  // Inicialização da serial
  Serial.begin(9600);

  // Verificações para o RTC
  if (! rtc.begin()) {
    Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Define a hora do RTC para a hora de compilação
  }
}

// Mostra a umidade e temperatura no lcd
void displayInfo(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Umidade: " + String(umidadeAferida) + "%");
  
  lcd.setCursor(0,1);
  lcd.print("Tempo: " + String(rtc.getTemperature()) + "C"); 
}

// Loop principal do programa
void loop() {

  displayInfo(); // Mostra informações no display lcd
  
  unsigned long tempoAtualEmMillis = rtc.now().unixtime() * 1000UL; //Hora atual do RTC em milissegundos
  static unsigned long ultimaVerificacao = 0;                       // Mantem a hora da ultima verificação entre as iterações

  // Checa se já é hora de verificar a umidade e regar
  if (tempoAtualEmMillis - ultimaVerificacao >= intervaloVerificacao) {
    ultimaVerificacao = tempoAtualEmMillis;                               // A última verficação se torna o tempo atual para controle futuro

    pinMode(pinoSensorUmidade, INPUT);                                    // Prepara o pino do sensor
    umidadeAferida = map(analogRead(pinoSensorUmidade), 1023, 0, 0, 100); // Faz a leitura e padroniza para um valor entre 0 e 100
    digitalWrite(pinoSensorUmidade, LOW);                                 // Desliga o pino do sensor  

    // Se a umidade estiver menor que o limiar, rega e mostra mensagem no display
    // Senão apenas mostra mensagem no display
    if(umidadeAferida < limiarUmidade){
      lcd.setCursor(0,0);
      lcd.print("Regando         ");
      
      // Desativa e ativa o relé e faz a rega
      digitalWrite(pinoRele, LOW);
      delay(tempoRega);
      digitalWrite(pinoRele, HIGH);
    }
    else{
      lcd.setCursor(0,0);
      lcd.print("Solo OK         ");
    }
  }
  
  delay(1000);
}