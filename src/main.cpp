#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27 , 20 ,4);

const char* nomeRedeWiFi = "SENAI EDUCACIONAL";
const char* senhaRedeWiFi = "info@134";
const String enderecoAPI = "https://wttr.in/Sao+Caetano+do+Sul?m&lang=pt&format=";

String pedirParaApiClima(String codigo);

void setup()
{
 Serial.begin (115200);
 WiFi.begin(nomeRedeWiFi,senhaRedeWiFi);
 lcd.init();
 lcd.backlight();
 lcd.print("clima");


Serial.print("\n\rIniciando conexão WiFi\n\r");

 while(WiFi.status() != WL_CONNECTED);
 {
    Serial.print("#");
    delay(500);
 }

 Serial.print("\n\rConectado com sucesso!\n\r");
 Serial.printf("Endereço IP: %s\n\r",WiFi.localIP().toString().c_str());
Serial.printf("Endereço MAC: %s\n\r",WiFi.macAddress().c_str());

String clima;
int temperatura;
int sensacaoTermica;
int Umidade;

clima = pedirParaApiClima("%c");
temperatura = pedirParaApiClima("%t").toInt();
sensacaoTermica = pedirParaApiClima("%f").toInt();
Umidade = pedirParaApiClima("%h").toInt();

Serial.printf ("condição Climatica para hoje: %s\n\r", clima.c_str());
Serial.printf("Temperatura atual %d C e sensação termica de %d C",temperatura,sensacaoTermica);
Serial.printf("Umidade:");

}
void loop()
{

}

String pedirParaApiClima(String codigo)
{
   HTTPClient http;
   http.begin(enderecoAPI + codigo);

   int codigoHTTP = http.GET();//Retorna o codigo http ex:200 ->ok  | 404 -> pagina nao encontrada
   String resposta;

   if(codigoHTTP == 200)
   {
    resposta = http.getString();//Retorna o conteudo html da pagina
   }

   else
   {
    Serial.printf("Deu erro. Codigo: %d", codigoHTTP);
   }
   http.end();
   return resposta;


}
