/*******************************************************************
    
    Parts:
    WiFiESPWROOM32  - https://www.amazon.de/ESP-WROOM-32-Entwicklungsboard-Dual-Mode-Microcontroller-integriert/dp/B07YKBY53C/ref=sr_1_1_sspa?keywords=esp32+wroom&qid=1675237331&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1  
    Library:
    UniversalTelegramBot https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot written by Brian Lough
    ESP_Mail_Client https://github.com/mobizt/ESP-Mail-Client ESP Mail Client@2.7.6 Written by mobizt (Suwatchai K.) 
    mail https://draeger-it.blog/senden-von-e-mails-mit-dem-esp32-via-arduino-ide/
 *******************************************************************/
//************************************************************
#include <WiFi.h> // WiFi library  
#include <UniversalTelegramBot.h> // Telegram Bot
#include <WiFiClientSecure.h> 
#include <ESP_Mail_Client.h>
#include <genieArduinoDEV.h>


//************************************************************
#define BOT_TOKEN "5908976398:AAHKhHcPqPc5-AWywlUyGv3SOWoOmhWD7b0" //To access the Telegram Http API to Control the Bot 
#define CHATID "-807951512"
#define TELEGRATEXT "I need HELP!!!"
#define STATIONID "Station ID : 1"
#define PRIORITYLEVEL1 "Priority Level : HIGH!!!"
#define LED_BUILTIN 22
#define SMTP_HOST "smtp.gmail.com"
//SMTP port (SSL): 465 oder SMTP port (TLS): 587 TLs ist besser weil.. ==> Transport Layer Security  TLS (Transport Layer Security) ist lediglich eine aktualisierte Version von SSL, die höhere Sicherheit bietet. 
#define SMTP_PORT esp_mail_smtp_port_587
/* The sign in credentials */
#define AUTHOR_EMAIL "alireza.azizi.afg0093@gmail.com"
#define AUTHOR_PASSWORD "uavwezixzqiaymoz"
/* Recipient's email*/
#define RECIPIENT_EMAIL  "amirali.azizi.afg0093@gmail.com"
#define RESETLINE 4 
//***********************************************************
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
/* The SMTP Session object used for Email sending */
SMTPSession smtp;
SMTP_Message message;
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);
const char* ssid = "Azizi";// WiFi SSID 
const char* password = "Afg00933@@"; // WiFi Password

void startEmailSessionAndSetMeassageHeaderAndContent()
{
  //Debug Meldungen anzeigen
  smtp.debug(1);
  //Ein Callback definieren welcher nach dem Senden der Mail ausgeführt werden soll.
  smtp.callback(smtpCallback);
  //Mail Session
  ESP_Mail_Session session;
  //Serverdaten setzen
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "smtp.gmail.com";    
  /* Connect to server with the session config */
  if (!smtp.connect(&session)){
    return;
  }
  /* Set the message headers */
  message.sender.name = "AlarmSystem"; //steht bei "gesendet von"
  message.sender.email = AUTHOR_EMAIL; //der Absender (oder eine Fake E-Mail)
  message.subject = "Station 1 need HELP"; //der Betreff der E-Mail
  message.addRecipient("Amir", RECIPIENT_EMAIL); //der Empfänger
  /* Set the message Body or content */
  //Aufbau des Contents der E-Mail
  String textMsg = "I need Help in Station 1 with Priority level High!!!. \r\n Mit einem Zeilenumbruch.";
  message.text.content = textMsg.c_str();
  message.text.charSet = "utf-8";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  
  ESP_MAIL_PRINTF("Free Heap: %d\n", MailClient.getFreeHeap());
}
void setup()
{
  genie.BeginSerial(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(0,INPUT_PULLUP);
  delay(2000);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi**********************");  
   
  startEmailSessionAndSetMeassageHeaderAndContent();
}

 
void loop()
{
  if(!digitalRead(0))
  {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("TRIGGERED");
    bot.sendMessage(CHATID, TELEGRATEXT);
    bot.sendMessage(CHATID, STATIONID);
    bot.sendMessage(CHATID, PRIORITYLEVEL1);
    /* Start sending Email and close the session */
    if (!MailClient.sendMail(&smtp, &message)){
    //Im Fehlerfall wird der Grund auf der seriellen Schnittstelle ausgegeben.
    Serial.println("Error sending Email, " + smtp.errorReason());
    }
    startEmailSessionAndSetMeassageHeaderAndContent();
  } 
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

//Der Callback welcher ausgeführt werden soll wenn das versenden der E-Mail erfolgte.
void smtpCallback(SMTP_Status status){
  Serial.println(status.info());
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;
    for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);
      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
    //smtp.sendingResult.clear();
  }
}
  
