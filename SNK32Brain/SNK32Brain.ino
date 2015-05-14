//ZOU LIANG 2015-04-27
//SNK32型号控制器数据处理程序
//接受上位机信号，一个字节8位对应控制8个74HC595输出端口
//数据帧：0x53|0x4E|0x4B|0x33|0x32（前5位固定标识帧头代表"SNK32"的ASCII码）|(数据总字节数)|（数据字节，以第一路开始为顺序）

int latchPin=8;  //ST_CP of 74HC595
int clockPin=12; //SH_CP of 74HC595
int dataPin=11;  //DS of 74HC595
static int StateMachine = 0; //状态机状态值
byte tempData = 0x00;//temp Serial.read data
int  coreDataNum = 0;
int  coreDataIndex = 0;
byte coreData[255]; //保存控制器状态的核心数据

//unsigned long time;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  //time = micros();
  if ( Serial.available() > 0 ){
    tempData = Serial.read(); //读取串口数据
    switch(StateMachine){
      case 0:
        if(tempData == 0x53)
          StateMachine = 1; //next frame head
      break;
      case 1:
        if(tempData == 0x4E)
          StateMachine = 2; //next frame head
        else
          StateMachine = 0;
      break;
      case 2:
        if(tempData == 0x4B)
          StateMachine = 3; //next frame head
        else
          StateMachine = 0;
      break;
      case 3:
        if(tempData == 0x33)
          StateMachine = 4; //next frame head
        else
          StateMachine = 0;
      break;
      case 4:
        if(tempData == 0x32)
          StateMachine = 5; //到这里文件头已经完毕
        else
          StateMachine = 0;
      break;
      case 5:
        coreDataNum = int(tempData);
        StateMachine = 6; //开始获取数据状态
      break;
      case 6:
        coreData[coreDataIndex] = tempData;
        coreDataIndex++;
        if(coreDataIndex == coreDataNum){
          digitalWrite(latchPin,LOW); //低电位表示启动74HC595芯片
          for(int i=(coreDataNum-1); i>=0; i--){
             shiftOut(dataPin,clockPin,LSBFIRST,coreData[i]); //压入数据，低位先入
          }
          digitalWrite(latchPin,HIGH); //高电位表示停止74HC595芯片
          StateMachine = 0; //状态机状态值复位
          tempData = 0x00;//temp Serial.read data
          coreDataNum = 0;
          coreDataIndex = 0;
        }
      break;
    }
  }
  //time = micros() - time;
  //Serial.println(time);
}
