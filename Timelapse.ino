#include <LiquidCrystal_I2C.h> //I2C 라이브러리 다운받아야 함
LiquidCrystal_I2C lcd(0x27, 16, 2); //Analog A5 = SCL, A4 = SDA
#define BTN_PIN1 9 //값 모드 조정
#define BTN_PIN2 8 //방향 조정
#define BTN_PIN3 2 //시작 종료
int encoder0PinA = 3; //로터리 인코더 A
int encoder0PinB = 4; //로터리 인코더 B
void setup()
{
  lcd.begin();
  Serial.begin(9600); 
  pinMode(7,OUTPUT); // Enable 핀지정
  pinMode(6,OUTPUT); // Step 핀지정
  pinMode(5,OUTPUT); // Dir 핀지정
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  pinMode(BTN_PIN1, INPUT_PULLUP);
  pinMode(BTN_PIN2, INPUT_PULLUP);
  pinMode(BTN_PIN3, INPUT_PULLUP);
  attachInterrupt(0,swInterrupt, FALLING);
  digitalWrite(7,LOW); //  Enable의 초기값을 low 로 지정 
}

int encoder0Pos = 0; //로터리 값
int encoder0PinALast = LOW;
int n = LOW;
int i; //motor for문 조건
int j; //motor for문 조건
int time_ = 1; //LCD 출력 시간
int angle_ = 90; //LCD 출력 각도
int btn1=0; //버튼 flag
int btn2=0; //버튼 flag
boolean stopinterrupt = false; //모터 정지 인터럽트
int flag = 1; //값 모드 조정 flag
int run_motor = 1; //모터 시작 및 시간 각도 조절관련 flag
float setAngle = angle_; //각도 dumo
float angleCondition=setAngle*8.8889; // 각도를 a4988에 맞게 변경
float setTime = 6 * time_; //time이 1분 단위이기 때문에 분당 60초를 맞추기 위해 6을 곱함
int timeCondition = (360/setAngle)*3125; 
int time_pre; //모터 움직이는 시간 체크용
int dir = 1; //방향 조절 flag
int startInterrupt = 1; //채터링 문제로 flag 만든 것
/*
 *******************************************************************
*/
void loop()
{
  if(run_motor == 1){
    n = digitalRead(encoder0PinA);
    encoder0Pos=0;
    if ((encoder0PinALast == LOW) && (n == HIGH)) {
      if (digitalRead(encoder0PinB) == LOW) {
        encoder0Pos++;
      } else {
        encoder0Pos--;
      }
      if(flag == 1) {
        time_ = time_ + encoder0Pos;
        if(time_ <= 0)time_ = 1;
      }
      else{         
        angle_ = angle_+ encoder0Pos*10;
      if(angle_<=89)angle_ = 90;
      else if(angle_ >= 361)angle_ = 360;
      }
        lcdprint();
    }
    encoder0PinALast = n;

    if(btn1 != digitalRead(BTN_PIN1)){
      btn1 = digitalRead(BTN_PIN1);
      if(btn1 == 0){
        flag = -flag; 
      }
    }
    if(btn2 != digitalRead(BTN_PIN2)){
      btn2 = digitalRead(BTN_PIN2);
      if(btn2 == 0){
        dir = -dir;lcdprint(); 
      }
    }
  }
  else if(run_motor == -1){
      motorRun();run_motor = 1;
  }
}
/*
 ***************************************************************************
*/
void swInterrupt(){
  delayMicroseconds(2000);
  if(startInterrupt == 1){
    delayMicroseconds(1);
    startInterrupt = 0;
  }
  else{
    if(run_motor == 1){
      run_motor = -1;
      stopinterrupt = false;
    }
    else{
      run_motor = 1;
      stopinterrupt = true;
    }
  }
}

void motorRun(){
  setAngle = angle_;
  angleCondition=setAngle*8.8889;
  setTime = 6 * time_;
  timeCondition = (360/setAngle)*3125; //360도를 10초동안 돌릴려면 3125로. 180도는 6250.
  if(dir == 1)
    digitalWrite(5,HIGH); // Set Dir high 
  else  
    digitalWrite(5,LOW); // Set Dir high 
  time_pre = millis()/1000;
  for(i = 0; i < angleCondition; i++) // (변경하여 회전각 조절 가능)
  {//800 = 90도, 1600=180도
      if(stopinterrupt == true){ //종료버튼 누르면 for문 종료
        stopinterrupt = false;
        break;
      }
      digitalWrite(6,HIGH); // Output high
      delayMicroseconds(1);
      digitalWrite(6,LOW); // Output low
      for(j=0;j<setTime;j++)
        delayMicroseconds(timeCondition);//3125=>10초, 18750=>60초
      }
  //Serial.println((millis()/1000)-time_pre); //주석 해제하면 serial로 모터 움직인 시간 체크 가능
  delay(1000); // pause one second (1000ms=1s 간 정지)
}
void lcdprint(){
  lcd.setCursor(0,0);
  lcd.print("                "); //LCD 초기화
  lcd.setCursor(0,0);
  lcd.print("Time : ");lcd.print(time_);
  lcd.setCursor(0,1);
  lcd.print("                "); //LCD 초기화
  lcd.setCursor(0,1);
  if(dir == 1){
    lcd.print("Angle:+");lcd.print(angle_);
  }
  else{
    lcd.print("Angle:-");lcd.print(angle_);
  }
}
