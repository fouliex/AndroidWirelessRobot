
//#include <AFMotor.h>
#include <Servo.h>
//#include <MsTimer2.h>
#include <EEPROM.h> 

int ledpin = 13;//设置系统启动指示灯
int ENA = 5;//
int ENB = 6;//
int INPUT2 = 7;//
int INPUT1 = 8;//
int INPUT3 = 12;//
int INPUT4 = 13;//

int inputPin = A0;  // Define the ultrasonic signal to receive the pin
int outputPin =A1;  // Defines the ultrasonic signal firing pin
int Input_Detect_LEFT = A2;    //Define the left side of the car infrared
int Input_Detect_RIGHT = A3;  //Define the right side of the car infrared
int Input_Detect=A4;//Define the car front infrared
int Carled = 2;
int Cruising_Flag = 0;
int Pre_Cruising_Flag = 0 ;

#define MOTOR_GO_FORWARD    {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}  
#define MOTOR_GO_BACK  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}    	                              
#define MOTOR_GO_LEFT	  {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}    
#define MOTOR_GO_RIGHT  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}    
#define MOTOR_GO_STOP	  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,LOW);}    

//AF_DCMotor motor1(1, MOTOR12_64KHZ);// Create motor # 1, the speed is 64KHz pwm
//AF_DCMotor motor2(2, MOTOR12_64KHZ);// Create motor # 2, speed is 64KHz pwm

Servo servo1;// 创建舵机#1号
Servo servo2;// 创建舵机#2号

byte angle1=70;//舵机#1
byte angle2=60;

int buffer[3];  // The serial port receives the data cache
int rec_flag;
int serial_data;
int Uartcount;
int IR_R;
int IR_L;
int IR;
unsigned long Pretime;
unsigned long Nowtime;
unsigned long Costtime;

void Open_Light()// Turn on the headlights
    {      
      digitalWrite(Carled,HIGH);   // Pull low, is connected to the power supply, negative Io mouth
      delay(1000);             
    }
void Close_Light()//关大灯
    {  
      digitalWrite(Carled, LOW);   //拉低电平，正极接电源，负极接Io口
      delay(1000);             
    }
    
void  Avoiding()
    {  
      IR = digitalRead(Input_Detect);
       if((IR == HIGH))
       {
          MOTOR_GO_FORWARD;//直行 
          return;            
       }
       if((IR == LOW))
       {
            MOTOR_GO_STOP;//停止
            return;
       }
    }
           
   

 

void ask_Distance()   // 量出距离   
 {  
      delay(200);
      digitalWrite(outputPin, LOW);   // 让超声波发射低电压2μs  
      delayMicroseconds(2);  
      digitalWrite(outputPin, HIGH);  // 让超声波发射高电压10μs，这里至少是10μs  
      delayMicroseconds(10);  
      digitalWrite(outputPin, LOW);    // 维持超声波发射低电压  
      float Ldistance = pulseIn(inputPin, HIGH);  // 读差相差时间  
      Ldistance= Ldistance/5.8/10;       // 将时间转为距离距离（单位：公分）  
    //  Serial.print("L distance:");       //输出距离（单位：公分）  
    //  Serial.println(Ldistance);         //显示距离  
      if(Ldistance < 15)
      {
          MOTOR_GO_STOP;
      }
      else
      {
          MOTOR_GO_FORWARD;
      }
      
  }    

void  Delayed()
{
    int i;
    for(i=0;i<20;i++)
    {
        digitalWrite(ledpin,LOW);
        delay(1000);
        digitalWrite(ledpin,HIGH);
        delay(1000);
    }
}

/*
*********************************************************************************************************
** function name: setup ()
** Function: System initialization (serial, motor, servo, indicator light initialization).
** Entry parameters: none
** Export parameters: none
*********************************************************************************************************
*/
void setup()
{
    pinMode(ledpin,OUTPUT); 
    pinMode(ENA,OUTPUT); 
    pinMode(ENB,OUTPUT); 
    pinMode(INPUT1,OUTPUT); 
    pinMode(INPUT2,OUTPUT); 
    pinMode(INPUT3,OUTPUT); 
    pinMode(INPUT4,OUTPUT); 
    pinMode(Input_Detect_LEFT,INPUT);
    pinMode(Input_Detect_RIGHT,INPUT);
    pinMode(Carled, OUTPUT);
    pinMode(Input_Detect,INPUT);
    pinMode(inputPin,INPUT);
    pinMode(outputPin,OUTPUT);
    Delayed();
    digitalWrite(ledpin,LOW);
    
    Serial.begin(9600);// The serial port baud rate is set to 9600 bps
    digitalWrite(ENA,HIGH);
    digitalWrite(ENB,HIGH);
    
    servo1.attach(9);// Define the steering gear 7 control port
    servo2.attach(10);// Define the steering gear 8 control port
    Init_Steer();

} 


void Init_Steer()
{
    servo1.write(angle1);
    servo2.write(angle2);
    
    angle1 = EEPROM.read(0x01);
    angle2 = EEPROM.read(0x02);
    if(angle1 == 255 && angle2 == 255)
    {
        EEPROM.write(0x01,70);
        EEPROM.write(0x02,60);
        return;
    }
 
    servo1.write(angle1);
    servo2.write(angle2);
}


void loop()
  {  
    while(1)
    {
        Get_uartdata();
        UartTimeoutCheck();
      }  
    }



/*
*********************************************************************************************************
** Function name: Communication_Decode ()
** Function: Serial command decode
** Entry parameters: none
** Export parameters: none
*********************************************************************************************************
*/
void Communication_Decode()
{   
    if(buffer[0]==0x00)
    {
        switch(buffer[1])   //Motor command
        {
            case 0x01:MOTOR_GO_FORWARD; return;
	    case 0x02:MOTOR_GO_BACK;    return;
	    case 0x03:MOTOR_GO_LEFT;    return;
            case 0x04:MOTOR_GO_RIGHT;   return;
	    case 0x00:MOTOR_GO_STOP;    return;
           default: return;
        }	
    }
   else if(buffer[0]==0x01)//Steering command
    {
        if(buffer[2]>160)return;
        switch(buffer[1])
        {
            case 0x07:angle1 = buffer[2];servo1.write(angle1);return;
            case 0x08:angle2 = buffer[2];servo2.write(angle2);return;
            default:return;
        }
    }
    else if(buffer[0]==0x33)//Read the steering angle and assign the value
	{
		 Init_Steer();return;
        }
    else if(buffer[0]==0x32)//Save command
    { 
        EEPROM.write(0x01,angle1);
        EEPROM.write(0x02,angle2);
        return;
    }
    	else if(buffer[0]==0x13)//模式切换开关
	{
	}
        else if(buffer[0]==0x05)
    {
        switch(buffer[1])   //
        {
            case 0x00:Open_Light(); return;
	    case 0x02:Close_Light(); return;
            default: return;
        }	
    }
    
    
}
/*
*********************************************************************************************************
** Function Name: Get_uartdata ()
** Function: Read the serial command
** Entry parameters: none
** Export parameters: none
*********************************************************************************************************
*/
void Get_uartdata(void)
{
    static int i;
   
    if (Serial.available() > 0) //Determines whether the serial buffer is loaded
    {
        serial_data = Serial.read();//Read the serial port
        if(rec_flag==0)
        {
            if(serial_data==0xff)
            {
                rec_flag = 1;
                i = 0;
                //Uartcount = 0;
                //Pretime = millis();
               Costtime = 0; 
            }
        }
        else
        {
            if(serial_data==0xff)
            {
                rec_flag = 0;
                if(i==3)
                {
                    Communication_Decode();
                }
                i = 0;
            }
            else
            {
                buffer[i]=serial_data;
                i++;
            }
        }
    }
}
/*
*********************************************************************************************************
** Function name: UartTimeoutCheck ()
** Function: Serial timeout detection
** Entry parameters: none
** Export parameters: none
*********************************************************************************************************
*/
void UartTimeoutCheck(void)
{
    if(rec_flag == 1)
    {
       //Nowtime = millis(); 
       Costtime++;  
      if(Costtime == 100000)
      {
           rec_flag = 0;
      }
    }
   // Uartcount++;
}
