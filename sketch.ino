
#include<U8g2lib.h>
#include <Wire.h>
#include <SimpleTimer.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C lcd(U8G2_R0, U8X8_PIN_NONE);


//Butonlar

#define ButonYukari  4
#define ButonEnter    3
#define ButonAsagi 2

#define POT_PIN A0

#define ButonSilah 8

#define LedSilah1 A2
#define LedSilah2 9
#define LedSilah3 10



//degiskenler
byte yukari, Ok, asagi,silah;
byte secim=0;
int shipPosition = 0;
int newPosition=0;
int potValue;
const int potPin = A0;
int buzzerPin = A3; 
float artim=2;
int Topcan=3;
int engelSay=0;
int hangiLed=0;
int zorOyun=0;
int mermikonum=0;
int silahY=0;
int mermiX=0;
int mermiY=0;
int ikiVurus=0;

unsigned long previousMillis=0;
unsigned long zaman=0;
unsigned long zamanZor=0;

int LDR_PIN = A1; // LDR bağlandığı analog pin

const int LED_PIN_1 = 13;
const int LED_PIN_2 = 12;
const int LED_PIN_3 = 11;



struct Engel {
  int x; // x koordinatı
  int y; // y koordinatı
  int hiz; // hareket hızı
  int tur;//engel türü 
  int can;
  //1=engel karakteri (uzay çöpü)
  //2=ödül puanı (bu alındığında kullanıcı ekstra 1 silah hakkı kazanır, yedek can)
  //3= dokunulmazlık (alındığı zaman 3 saniye boyunca engele çarpsa bile oyuncunun hakkı gitmez)
  //4=engel karakteri meteor
};
Engel engeller[4]; // 4 engel için bir dizi


//display
constexpr uint8_t CLK_PIN = 5;
constexpr uint8_t RST_PIN = 6;
constexpr uint8_t DEI_PIN = 7;



void setup() {
  pinMode(ButonYukari,   INPUT_PULLUP);
  pinMode(ButonEnter,     INPUT_PULLUP);
  pinMode(ButonAsagi,  INPUT_PULLUP);

//display
  pinMode(CLK_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  pinMode(DEI_PIN, OUTPUT);


  pinMode(ButonSilah, INPUT_PULLUP);
  pinMode(LedSilah1, OUTPUT);
  pinMode(LedSilah2, OUTPUT);
  pinMode(LedSilah3, OUTPUT);


  Serial.begin(9600);
  // Lcd Başlatılıyor
  lcd.begin();
  lcd.setFont(u8g2_font_crox2tb_tf);
  lcd.setFontRefHeightExtendedText();
  lcd.setDrawColor(1);
  lcd.setFontPosTop();
  lcd.setFontDirection(0);




  // acilis ekrani
  lcd.clearBuffer(); // Display Clear
  lcd.drawStr( 25, 0,  "ULKU IREM");
  lcd.drawStr( 25, 15, "DENIZ");
  lcd.drawStr( 25, 30, "ASLINUR");
  lcd.drawStr( 25, 45, "TOPCU");

  lcd.sendBuffer(); // Display Update
   engelOlustur2();
  delay(2000);






}

void loop(void) {

   

delay(300);
  while ( true ) {
    butonOku();


   

//butonlari gnd ye bagladigimiz icin sifir mi diye konrolediyoruz

    if ( !asagi ) {
      secim++;
      if ( secim >= 1 ) secim = 1;
    }
    if ( !yukari ) {
      secim--;
      if ( secim>= 2 ) secim = 0;
    }
    if ( !Ok ) {
      if ( secim == 0 ) { // Kolay bolumune giris giris
    
        Kolay();
      } 
      else if ( secim == 1 ) { // zor bolume giris
      

        Zor();
      }
    }

    lcd.clearBuffer(); // Display Clear

    lcd.drawStr( 10, 0, "Menu");
    lcd.drawLine(0, 15, 127, 15);

    if ( secim == 0 ) {
      lcd.drawStr( 2, 20, "> KOLAY");
      lcd.drawStr( 2, 35, "   ZOR");
    } else if ( secim == 1 ) {
      lcd.drawStr( 2, 20, "   KOLAY");
      lcd.drawStr( 2, 35, ">  ZOR");
    }

    lcd.sendBuffer(); // Display Update
   
    

  }



}




void Kolay () {
LDRoku();
  zaman=millis(); 
  delay(300);
  while ( true ) {

    butonOku();


    if ( !Ok ) {

      loop();
    }




   lcd.clearBuffer(); // Display Clear
   
   newPosition=lcd.getWidth();
   ledleriGuncelle(Topcan);

   //Potansiyometre okuması
  potValue = analogRead(A0);
  drawShip(newPosition, lcd.getHeight() - 5); 

 clearShip(); 
 newPosition = map(potValue, 0, 1023, 0, lcd.getWidth() - 8);
 

silahY=lcd.getHeight() - 5;

  mermikonum=newPosition;

if (newPosition != shipPosition) {
  clearShip();   // mevcut pozisyonda uzay gemisini sil
  // yeni pozisyonda uzay gemisini çiz

  drawShip(newPosition, lcd.getHeight() - 5);
  shipPosition = newPosition;  // pozisyonu güncelle
  mermikonum=newPosition;
  if(!silah){
  mermiX=mermikonum;
mermiY=silahY;


}}
delay(50);  // potansiyometre okumasını yavaşlatmak için kısa bir bekleme süresi ekle

 Silah();
 engelHareketi();

    lcd.sendBuffer(); // Display Update
  }

}



void Zor () {

LDRoku();
 zamanZor=millis();

 // 
  zorOyun=1;
  delay(200);
  while ( true ) {
    
    butonOku();

  
    if ( !Ok ) {
   
      loop();
    }


   lcd.clearBuffer(); // Display Clear
   newPosition=lcd.getWidth();
   ledleriGuncelle(Topcan);

   //Potansiyometre okuması
  potValue = analogRead(A0);
  drawShip(newPosition, lcd.getHeight() - 5); 
 clearShip(); 
 newPosition = map(potValue, 0, 1023, 0, lcd.getWidth() - 8);

silahY=lcd.getHeight() - 5;

  mermikonum=newPosition;

if (newPosition != shipPosition) {
  clearShip();   // mevcut pozisyonda uzay gemisini sil
  // yeni pozisyonda uzay gemisini çiz

  drawShip(newPosition, lcd.getHeight() - 5);
  shipPosition = newPosition;  // pozisyonu güncelle
  mermikonum=newPosition;
  if(!silah){
  mermiX=mermikonum;
mermiY=silahY;


}}
delay(50);  // potansiyometre okumasını yavaşlatmak için kısa bir bekleme süresi ekle

 Silah();
 engelHareketi();

    lcd.sendBuffer(); // Display Update
  }

}

//---------------------------------------------------------------------------------



void ledleriGuncelle(int canSayisi){
    if(canSayisi==1){

      digitalWrite(LED_PIN_1, HIGH);
      digitalWrite(LED_PIN_2, LOW); 
      digitalWrite(LED_PIN_3, LOW); 
    }
     else if(canSayisi==2){
    
      digitalWrite(LED_PIN_3, LOW); 
      digitalWrite(LED_PIN_1, HIGH); 
      digitalWrite(LED_PIN_2, HIGH); 
    }
     else if(canSayisi==3){
        digitalWrite(LED_PIN_1, HIGH); 
        digitalWrite(LED_PIN_2, HIGH); 
        digitalWrite(LED_PIN_3, HIGH); 
    }
    else if(canSayisi==0){
        digitalWrite(LED_PIN_1, LOW); 
       digitalWrite(LED_PIN_2, LOW); 
        digitalWrite(LED_PIN_3, LOW); 
        
        delay(100);
       OyunBitti();
    }

}




void butonOku () {
  yukari  = digitalRead(ButonYukari);
  Ok    = digitalRead(ButonEnter);
  asagi = digitalRead(ButonAsagi);
 

}


void buzzerCal() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 100) {
    previousMillis = 0;

    tone(buzzerPin, 1000, 100); // buzzer ile 1000Hz frekansında 100ms süreyle ses çal
  }

  if (currentMillis - previousMillis >= 300) {
    noTone(buzzerPin); // sesi durdur
  }
}






void Silah()
{


 
silah=digitalRead(ButonSilah);


 if(!silah)
 {
   ikiVurus++;
   hangiLed++;
   delay(150);
 }

  if(hangiLed==0)
  {
  
  digitalWrite(LedSilah1, HIGH);
  digitalWrite(LedSilah2, HIGH);
  digitalWrite(LedSilah3, HIGH);

  }

  else if(hangiLed==1)
  {
  
  digitalWrite(LedSilah1, LOW);
   digitalWrite(LedSilah2, HIGH);
  digitalWrite(LedSilah3, HIGH);
 
  }

  else if(hangiLed==2)
  {
     
  digitalWrite(LedSilah1, LOW);
  digitalWrite(LedSilah2, LOW);
  digitalWrite(LedSilah3, HIGH);

  }

 else if(hangiLed==3)
  {

   
  digitalWrite(LedSilah1, LOW);
  digitalWrite(LedSilah2, LOW);
  digitalWrite(LedSilah3, LOW);

  }



}


unsigned long time = 0;
int kontrol=0;



void OyunBitti() {
 lcd.clearBuffer(); // önceki içeriği temizle
 //lcd.clear();
  lcd.print("Oyun Bitti!");

    shipPosition = 0;
    newPosition=0;
    potValue = 0;
    previousMillis = 0;
    Topcan=3;
    engelSay=0;
    hangiLed=0;
     zorOyun=0;
    artim=2;
   ikiVurus=0;
   time=0;
   kontrol=0;
  delay(200);
   engelOlustur2();
  loop();

}

//------------------------------------------------------------------------------------------

void engelOlustur(int i) {

//engeller[i].y = -i * 20; // her engel diğerinin bir üstünde başlar
engeller[i].hiz = 3; //hızı seviyeye göre ayarla score puanını baz alarak
//engeller[i].tur = random(1, 5); // rastgele bir tür belirle
if(engeller[i].tur==1||engeller[i].tur==6){
lcd.drawBox(engeller[i].x, engeller[i].y, 10, 10); // engeli çiz(uzay çöpü)
}
else if(engeller[i].tur==2||engeller[i].tur==7){
  dokunulmazlikY(engeller[i].x, engeller[i].y, 5);
}
else if(engeller[i].tur==3){
   odulPuaniU(engeller[i].x, engeller[i].y,10);
}
else if(engeller[i].tur==4||engeller[i].tur==5){
  Meteor(engeller[i].x, engeller[i].y);
}

}







void engelHareketi() {

  unsigned long currentMillis = millis();
  

if( zorOyun==1)
{
//10 saniyede bir %20 artma
if (currentMillis-zamanZor >= 10000)
{
  artim=artim+artim*0.2;
  zamanZor+=10000;

}

}

  
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = 0;
   for (int i = 0; i < 4; i++) {
  // Engel yukarı taşınır

  engeller[i].y += artim;
  if(Topcan==0){
    engeller[i].y = -10; 
  }
  //delay(10);
  if (engeller[i].y == 0) {
        engelSay++;

        if(engelSay%14==0){//her 14 engelde bir can ve atış sayısı artar
      Topcan++;
      hangiLed--;
        }
      }

    SkorGoster(engelSay);  
  
  //delay
  //skor arttırımı(her 5 piksel de bir)

  // Eğer engel ekranın altındaysa, yeniden başlat
  if (engeller[i].y >= lcd.getHeight()) {

   
    engeller[i].y = -10; // Ekranın üstünde başlat
    engeller[i].x = random(0, lcd.getWidth() - 10); // Yeni bir x koordinatı belirle
    for (int j = 0; j < i; j++) {
      // Yeni engel ile diğer engeller arasındaki mesafeyi kontrol et
      while (abs(engeller[i].x - engeller[j].x) < 20 || engeller[i].x%15!=0) {
        engeller[i].x = random(0, lcd.getWidth() - 10);
      }
    }
    engeller[i].tur = random(1, 7); // Rastgele bir tür belirle
    
  }
      
      // Engeli çiz
      engelOlustur(i);
      if(currentMillis>time+3000)
      {
        kontrol=0;
      }
      engelCarpma();
      //lcd.drawBox(engeller[i].x, engeller[i].y, 10, 10);
    }
  }
  
}



  


void engelCarpma() {

 
//delay

  for (int i = 0; i < 4; i++) {
   // Uzay gemisi ve engel arasındaki mesafeyi kontrol et
    int xDiff = abs(engeller[i].x - shipPosition);
    int yDiff = abs(engeller[i].y - (lcd.getHeight() - 5));

    //*silah ateşlenmesi durumu engel yok etme**
  if(!silah && hangiLed<=3){

    mermiHareketi();
    if (abs(mermikonum - engeller[i].x) < 10) {//uzay çöpü

        if (engeller[i].tur==1||engeller[i].tur==7) {// Engelli karakter (uzay çöpü)
        ikiVurus=0;
                engeller[i].y = -10; //engeli siler 
                engeller[i].tur = 0; 

      }else if (engeller[i].tur==4||engeller[i].tur==5||engeller[i].tur==6) {// Engelli karakter (meteor)

           engeller[i].can--;
            if(ikiVurus%2==0){
              engeller[i].y = -10; //engeli siler 
             engeller[i].tur = 0; 

        }

      }
    }


    }

    
    if (xDiff < 10 && yDiff < 10) {
      // Çarpışma var, engelin türüne göre işlem yap
      
        if(engeller[i].tur==1||engeller[i].tur==6){// Engelli karakter (uzay çöpü)
        
        if(kontrol==0)//hiç engele çarpmadı
        {
           Topcan--;
           buzzerCal();
           ledleriGuncelle(Topcan);
           kontrol=1;
           time=millis();
        }
            // Buzzer ile hata bildirimi yap
         
          
        }
        else if(engeller[i].tur==2){// Ödül puanı
        if(Topcan<3)
        {
          Topcan++;  
        }
        if(hangiLed>0){
          hangiLed--;
        }
          ledleriGuncelle(Topcan);
        }
         else if(engeller[i].tur==3){// Dokunulmazlık
          if(kontrol==0)//hiç engele çarpmadı
        {
           kontrol=1;
           time=millis();
        }
          
        }
         else if(engeller[i].tur==4||engeller[i].tur==5){// Engelli karakter (meteor)
         if(kontrol==0)
         {
          Topcan--;
           buzzerCal(); // Buzzer ile hata bildirimi yap
          ledleriGuncelle(Topcan);
           kontrol=1;
            time=millis();
         }
          
         
          
        }
     // Engeli tekrar başlat
      engeller[i].y = -10;
      engeller[i].x = random(0, lcd.getWidth() - 10);
      for (int j = 0; j < i; j++) {
        while (abs(engeller[i].x - engeller[j].x) < 20) {
          engeller[i].x = random(0, lcd.getWidth() - 10);
        }
      }
    }
    
   
  }
}







//ENGEL SEKİLLERİ ICIN FONKSIYONLAR
/*yuvarlak ->dokunulmazlik
  odul pugani ->ucgen
  uzay copu ->sacma sekil
  meteor->kare
*/
//------------------------------------------------------------------------------------------------------------
void dokunulmazlikY(int x0, int y0, int radius) {
  int x = radius;
  int y = 0;
  int err = 0;
  // Bresenham çember çizme algoritması
  while (x >= y) {
    lcd.drawPixel(x0 + x, y0 + y);
    lcd.drawPixel(x0 + y, y0 + x);
    lcd.drawPixel(x0 - y, y0 + x);
    lcd.drawPixel(x0 - x, y0 + y);
    lcd.drawPixel(x0 - x, y0 - y);
    lcd.drawPixel(x0 - y, y0 - x);
    lcd.drawPixel(x0 + y, y0 - x);
    lcd.drawPixel(x0 + x, y0 - y);
    
    if (err <= 0) {
      y += 1;
      err += 2*y + 1;
    } else {
      x -= 1;
      err -= 2*x + 1;
    }
  }
}

void Meteor(int x, int y) {
 
  lcd.drawBox(x, y+3, 10, 3);
 
  lcd.drawBox(x+4, y, 3, 10);
  }

void odulPuaniU(int x, int y, int size) {
  int half_size = size / 2;
  lcd.drawLine(x, y, x + size, y); // Top line
  lcd.drawLine(x, y, x + half_size, y + size); // Left line
  lcd.drawLine(x + size, y, x + half_size, y + size); // Right line
}





//---------------------------------------------------------------------------------------------------

//ROKET FONKSİYONU

void drawShip(int x, int y) {
  lcd.drawBox(x+3, y, 2, 3);
  lcd.drawBox(x+1, y+3, 6, 5);
  lcd.drawBox(x, y+8, 8, 2);
  lcd.drawBox(x+2, y+10, 4, 2);
  lcd.drawBox(x+3, y-1, 2, 1);
}

void clearShip() {
  lcd.drawBox(shipPosition+3, lcd.getHeight() - 3, 2, 3);
  lcd.drawBox(shipPosition+1, lcd.getHeight() - 8, 6, 5);
  lcd.drawBox(shipPosition, lcd.getHeight() - 2, 8, 2);
  lcd.drawBox(shipPosition+2, lcd.getHeight() - 4, 4, 2);
  lcd.drawBox(shipPosition+3, lcd.getHeight() - 9, 2, 1);
}


void SkorGoster(int num)
{

  digitalWrite(DEI_PIN, LOW);
  digitalWrite(RST_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(RST_PIN, LOW);
  for (int i = 0; i < num; ++i)
  {
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(10);
  }
  digitalWrite(DEI_PIN, HIGH);
}


void mermiHareketi() {
  //int mermiX, int mermiY
  mermiY=lcd.getHeight() - 10;
mermiX=mermikonum;

 
while(mermiY>0){
 lcd.drawBox(mermiX, mermiY, 1, 3); 
 mermiY -= 5;

lcd.drawBox(mermiX, mermiY, 1, 3); 



}


}


void LDRoku(){
   int ldr_value = analogRead(LDR_PIN); // LDR'den gelen değer okunuyor
  if (ldr_value < 500) { // LDR değeri 500'den küçükse arka plan beyaz
    lcd.setDrawColor(1); // arka plan beyaz
    
  } else { // LDR değeri 500 veya daha büyükse arka plan siyah
    lcd.setDrawColor(0); // arka plan siyah
  }

  lcd.sendBuffer(); // görüntüyü ekrana gönder
}

void engelOlustur2() {
for (int i = 0; i < 4; i++) {
engeller[i].x = random(0, lcd.getWidth() - 10);
engeller[i].y = -i * 20; // her engel diğerinin bir üstünde başlar
engeller[i].hiz = 1; //hızı seviyeye göre ayarla score puanını baz alarak
engeller[i].tur = random(1, 7); // rastgele bir tür belirle
lcd.drawBox(engeller[i].x, engeller[i].y, 10, 10); // engeli çiz
}
}