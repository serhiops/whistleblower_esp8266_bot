#define WIFI_SSID "SSID"                                                //your WIFI SSID
#define WIFI_PASS "PASSWORD OF SSID"                                    //your WIFI password
#define BOT_TOKEN "5142064188:AAHRw1qptbzvM0nOakxyWNRMmyMZ_t44QWM"      //your token of Telegram bot
#define ESP8266
//#define DEBUG                                                    //if you debug your code. I advise to uncomment this line of code

#include <FastBot.h>                                               //https://github.com/GyverLibs/FastBot
#include <LiquidCrystal.h>                                         //library installed by default

FastBot bot(BOT_TOKEN);
constexpr int rs = 12, en = 16, d4 = 5, d5 = 4, d6 = 3, d7 = 2;    //pins for display
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

constexpr int8_t LED_PIN_GREEN{ 14 };                              //pin for green led
constexpr int8_t LED_PIN_YELLOW{ 15 };                             //pin for yellow led
constexpr int8_t LED_PIN_RED{ 0 };                                 //pin for red led
int8_t LED_PIN_ACTIVE;                    

constexpr int8_t GREEN_NUM{ 5 };                                   //the number of messages at which the green and
constexpr int8_t YELLOW_NUM{ 10 };                                 //                                             yellow leds will be active

int countOfMessages{ 0 };                                          //total count of messages
const String USERNAME{ "username" };                               //name of user in telegram 
bool ledStatus{ LOW };
bool isMessageFromUser{ false };                                   //whether the message is from the user, specified in the variable USERNAME

unsigned long timeOfUserMessage{ 0 };                              //time of last message from user, specified in the variable USERNAME
constexpr unsigned timeOfActiveLed{ 3000 };                        //time(in seconds) during which the led will be on

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_YELLOW, OUTPUT);
  pinMode(LED_PIN_RED, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Messages number:");
  lcd.setCursor(0, 1);
  lcd.print(countOfMessages);
  
  delay(2000);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG
      Serial.print('.');
    #endif            
    if (millis() > 15000) ESP.restart();
  }
  #ifdef DEBUG
    Serial.print("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());    
  #endif                                                  
  bot.attach(newMsg);                                               //add message callback function
  bot.skipUpdates();                                                //skip messages when bot is unactive 
  #ifdef DEBUG
    Serial.println("Bot online!");    
  #endif  
}

void newMsg(FB_msg& msg) { 
  if(msg.username == USERNAME){
    #ifdef DEBUG
      Serial.println("Message from the monitored user:");
      Serial.println("text : " + msg.text);    
    #endif
    /* looking for the pin we need and updating data on display */                      
    if(countOfMessages < GREEN_NUM)
	LED_PIN_ACTIVE = LED_PIN_GREEN;
    else if(countOfMessages < YELLOW_NUM) 
	LED_PIN_ACTIVE = LED_PIN_YELLOW;
    else 
	LED_PIN_ACTIVE = LED_PIN_RED; 
       
    ++countOfMessages;
    isMessageFromUser = true;
    timeOfUserMessage = millis(); 
    digitalWrite(LED_PIN_ACTIVE, HIGH);  
    lcd.setCursor(0, 1);
    lcd.print(countOfMessages);        
  }
  #ifdef DEBUG
  else{
      Serial.println("Message from some user:");
      Serial.println("author : " + msg.username + "\ntext : " + msg.text);  
  }  
  #endif    
}

void loop() {   
  if(isMessageFromUser && millis()-timeOfUserMessage > timeOfActiveLed){
    /* unactivate led if the time is up  */
    digitalWrite(LED_PIN_ACTIVE, LOW);
    isMessageFromUser = false;    
  }
  bot.tick();
}