#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN          4   
#define SPI_SCK         3   
#define SPI_MOSI        2  
#define SPI_MISO        1  
#define RST_PIN         0   

#define RELAY_OPEN_PIN  5
#define RELAY_CLOSE_PIN 6 
#define LED_RED_PIN     10  

MFRC522 mfrc522(SS_PIN, RST_PIN); 
byte bossUID[] = {0xDE, 0xAD, 0xBE, 0xEF}; 
bool isDoorOpen = false;

void setup() {
  Serial.begin(115200);
  
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SS_PIN); 
  mfrc522.PCD_Init();   

  pinMode(RELAY_OPEN_PIN, OUTPUT);
  pinMode(RELAY_CLOSE_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);

  // Trạng thái ban đầu: Cửa đóng hoàn toàn, Đèn đỏ tắt
  digitalWrite(RELAY_OPEN_PIN, LOW);
  digitalWrite(RELAY_CLOSE_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  
  Serial.println("Hệ thống cửa tự động của Ông Chủ đã sẵn sàng hoạt động!");
}

void loop() {
  // Kiểm tra xem có thẻ quét qua đầu đọc không
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  bool isBoss = true;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != bossUID[i]) {
      isBoss = false;
      break;
    }
  }

  if (isBoss) {
    Serial.println("Xác nhận danh tính: Đúng là Ông Chủ!");
    
    if (!isDoorOpen) {
      //Bước vào
      Serial.println("Ông chủ vào phòng: Mở cửa và sáng đèn đỏ.");
      digitalWrite(LED_RED_PIN, HIGH);       
      
      digitalWrite(RELAY_OPEN_PIN, HIGH);   
      delay(2000);                          
      digitalWrite(RELAY_OPEN_PIN, LOW);   
      
      isDoorOpen = true; 
      delay(3000);                           
      
      Serial.println("Ông chủ đã vào xong: Tự động tắt đèn đỏ và đóng cửa lại.");
      digitalWrite(LED_RED_PIN, LOW);      
      
      digitalWrite(RELAY_CLOSE_PIN, HIGH);
      delay(2000);                      
         digitalWrite(RELAY_CLOSE_PIN, LOW);   
      
      isDoorOpen = false; 
      
    } else {
      //Ra ngoài
      Serial.println("Ông chủ ra ngoài: Mở cửa và sáng đèn đỏ.");
      digitalWrite(LED_RED_PIN, HIGH);     
      
      digitalWrite(RELAY_OPEN_PIN, HIGH);   
      delay(2000);
      digitalWrite(RELAY_OPEN_PIN, LOW);

      Serial.println("Chờ đúng 5 giây để ông chủ đi ra xa...");
      delay(5000);                        
      
      Serial.println("Hết 5 giây: Tắt đèn đỏ và đóng cửa lại.");
      digitalWrite(LED_RED_PIN, LOW);   
      
      digitalWrite(RELAY_CLOSE_PIN, HIGH);
      delay(2000);
      digitalWrite(RELAY_CLOSE_PIN, LOW);
      
      isDoorOpen = false; 
    }
  } else {
    Serial.println("Cảnh báo: Thẻ lạ! Không cho phép mở cửa.");
  }
  
  mfrc522.PICC_HaltA(); 
}
