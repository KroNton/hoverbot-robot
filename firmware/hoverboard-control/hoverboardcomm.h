

#define HOVER_SERIAL_BAUD   115200      
#define SERIAL_BAUD         115200      
#define START_FRAME         0xABCD      
#define TIME_SEND           100         
#define SPEED_MAX           300         
#define WHEEL_DIAMETER      0.165  // 165mm = 0.165m

uint8_t idx = 0;                        
uint16_t bufStartFrame;                 
byte *p;                                
byte incomingByte;
byte incomingBytePrev;

float temperature_C;
float feedback_speedR=0;
float feedback_speedL=0;

int16_t currentSpeed = 0;  // Store the current speed

typedef struct {
   uint16_t start;
   int16_t  steer;
   int16_t  speed;
   uint16_t checksum;
} SerialCommand;

SerialCommand Command;

typedef struct {
   uint16_t start;
   int16_t  cmd1;
   int16_t  cmd2;
   int16_t  speedR_meas;
   int16_t  speedL_meas;
   int16_t  batVoltage;
   int16_t  boardTemp;
   uint16_t cmdLed;
   uint16_t checksum;
} SerialFeedback;

SerialFeedback Feedback;

SerialFeedback NewFeedback;



// ########################## SEND ##########################
void hoverboard_Set_speed(int16_t uSpeed) {
  Command.start    = (uint16_t)START_FRAME;
  Command.steer    = 0;
  Command.speed    = (int16_t)uSpeed;
  Command.checksum = (uint16_t)(Command.start ^ Command.steer ^ Command.speed);

  HoverSerial.write((uint8_t *) &Command, sizeof(Command)); 
}



// ########################## RECEIVE ##########################
void hoverboard_Receive() {
    if (HoverSerial.available()) {
        incomingByte    = HoverSerial.read();                                  
        bufStartFrame = ((uint16_t)(incomingByte) << 8) | incomingBytePrev;    
    } else {
        return;
    }

    if (bufStartFrame == START_FRAME) {
        p       = (byte *)&NewFeedback;
        *p++    = incomingBytePrev;
        *p++    = incomingByte;
        idx     = 2;  
    } else if (idx >= 2 && idx < sizeof(SerialFeedback)) {
        *p++    = incomingByte; 
        idx++;
    } 

    if (idx == sizeof(SerialFeedback)) {
        uint16_t checksum;
        checksum = (uint16_t)(NewFeedback.start ^ NewFeedback.cmd1 ^ NewFeedback.cmd2 ^ NewFeedback.speedR_meas ^ NewFeedback.speedL_meas
                            ^ NewFeedback.batVoltage ^ NewFeedback.boardTemp ^ NewFeedback.cmdLed);

        if (NewFeedback.start == START_FRAME && checksum == NewFeedback.checksum) {
            memcpy(&Feedback, &NewFeedback, sizeof(SerialFeedback));

            // Motor speed feedback
            feedback_speedR = Feedback.speedR_meas ;
            feedback_speedL = Feedback.speedL_meas ;
            
            // Convert temperature to Celsius
            temperature_C = Feedback.boardTemp / 10.0;
        } 
        
        }
        idx = 0;    
 
    incomingBytePrev = incomingByte;
}
