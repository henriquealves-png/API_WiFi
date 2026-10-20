#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include "esp_sntp.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* nomeRedeWiFi = "SENAI EDUCACIONAL";
const char* senhaRedeWiFi = "info@134";
const String enderecoAPI = "https://wttr.in/Sao+Caetano+do+Sul?m&lang=pt&format=";

//*============= Servidores NTP e Fuso =================
const char* NTP1 = "a.st1.ntp.br";
const char* NTP2 = "pool.ntp.org";
const char* FUSO = "<-03>3"; // Horário de Brasília

const char* DIAS[] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};

String pedirParaApiClima(String codigo);

void setup()
{
  Serial.begin(115200);
  
  // Inicialização do LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi...");

  // Conexão WiFi
  WiFi.begin(nomeRedeWiFi, senhaRedeWiFi);
  Serial.print("\n\rIniciando conexao WiFi\n\r");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("#");
    delay(500);
  }

  Serial.print("\n\rConectado com sucesso!\n\r");
  Serial.printf("Endereco IP: %s\n\r", WiFi.localIP().toString().c_str());

  //*------- CONFIGURACAO DE HORA NTP --------------
  configTzTime(FUSO, NTP1, NTP2);
  sntp_set_sync_interval(24 * 60 * 60 * 1000); // Movid do local correto (APÓS configTzTime)

  struct tm agora;
  Serial.print("\n\rSincronizando hora\n\r");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sincronizando hora...");

  while (!getLocalTime(&agora))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nHora sincronizada!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Buscando Clima...");

  String clima = pedirParaApiClima("%c");
  int temperatura = pedirParaApiClima("%t").toInt();
  int sensacaoTermica = pedirParaApiClima("%f").toInt();
  int umidade = pedirParaApiClima("%h").toInt();

  Serial.printf("Clima: %s\n\r", clima.c_str());
  Serial.printf("Temp: %d C | Sensacao: %d C\n\r", temperatura, sensacaoTermica);
  Serial.printf("Umidade: %d%%\n\r", umidade);
}

void loop()
{
  struct tm agora;
  if (getLocalTime(&agora))
  {
    Serial.printf("Hoje e %s, %02dh:%02d:%02d | Timestamp: %ld\n\r",
                  DIAS[agora.tm_wday],
                  agora.tm_hour,
                  agora.tm_min,
                  agora.tm_sec,
                  (long)time(nullptr));
  }

  // Consulta à API de clima
  String clima = pedirParaApiClima("%c");
  int temperatura = pedirParaApiClima("%t").toInt();
  int sensacaoTermica = pedirParaApiClima("%f").toInt();
  int umidade = pedirParaApiClima("%h").toInt();

  // Atualização do display LCD
  lcd.clear();
  
  // Linha 0: Dia da semana e Hora
  lcd.setCursor(0, 0);
  lcd.printf("%s %02d:%02d:%02d", DIAS[agora.tm_wday], agora.tm_hour, agora.tm_min, agora.tm_sec);

  // Linha 1: Descrição do Clima
  lcd.setCursor(0, 1);
  if (clima.length() > 0) {
    lcd.printf("Clima: %.13s", clima.c_str()); // Limita a 13 caracteres para caber na tela
  } else {
    lcd.print("Clima: Indisponivel");
  }

  // Linha 2: Temperatura e Sensação
  lcd.setCursor(0, 2);
  lcd.printf("Temp: %dC (ST: %dC)", temperatura, sensacaoTermica);

  // Linha 3: Umidade do ar
  lcd.setCursor(0, 3);
  lcd.printf("Umidade: %d%%", umidade);

  // Aguarda 10 segundos antes da próxima atualização
  delay(10000); 
}

String pedirParaApiClima(String codigo)
{
  HTTPClient http;
  http.begin(enderecoAPI + codigo);
  http.setTimeout(3000); // Timeout de 3 segundos para evitar travamento da tela

  int codigoHTTP = http.GET();
  String resposta = "";

  if (codigoHTTP == 200)
  {
    resposta = http.getString();
    resposta.trim();
  }
  else
  {
    Serial.printf("Erro na API. Codigo: %d\n\r", codigoHTTP);
  }
  
  http.end();
  return resposta;
}