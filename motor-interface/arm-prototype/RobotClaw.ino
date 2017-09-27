#include<Servo.h> //Servo   library
#include<IRremote.h> //InfraRed remote library, also we need to remove <RobotIRremote> if installed

//Declaring the pin for the remote
const int ReceiverPin = 2; //This will be the signal pin for the receiver module

//Here are some variables for the remote
IRrecv irrecv(ReceiverPin); //This create a Receiver object
decode_results results; //This will store and code received

//Declaring the servo's Pins
const int CRPin = 6;//Claw rotation
const int CUDPin = 7;//Claw up'n'down

//Creating the servos
Servo CRServo; //CR Servo
Servo CUDServo; //CUD Servo

//Declaring constants for the coordinate systems
//I will use Polar coordinates to control the arm
int CRPos = 90, CRMin = 0, CRMax = 180;
int CUDPos = 90, CUDMin = 0, CUDMax = 180;


//Variables in arrays for the memorized positions
int CRPosEQ[9];
int CUDPosEQ[9];

//Variables use throught out the program
char Last = 'O';//This is to know what button was pressed last
int wait = 0; //This control the delays throught the loop
int Step = 2; //Angle difference for each time a button is pressed
bool EQ = 0; //This is the variable for GOTO VS AddPosition 


/*------------------------------------------------------------------------------------------------------------*/
/*                                                  FUNCTIONS                                                 */
/*------------------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------ADD POSITION------------------------------------------------*/
void AddPosition(int i)
  {
  if (EQ == 1){
    CRPosEQ[i] = CRPos;
    CUDPosEQ[i] = CUDPos;
    EQ = 0;
  }
  else
  {
    GOTO(CRPosEQ[i],CUDPosEQ[i]);
  }
}

/*-------------------------------------------------GOTO---------------------------------------------------------*/

void GOTO(int CR, int CUD)
{
  CRPos = CR;
  CUDPos = CUD;

  CRServo.write(CRPos);
  CUDServo.write(CUDPos);  
}

/*------------------------------------------------HOME-----------------------------------------------------------*/
//Will reset the robot to its "Home" position when called
void Home()
{
  CRPos = 90;
  CUDPos = 90;
  
  CRServo.write(CRPos);
  CUDServo.write(CUDPos);
}

/*-----------------------------------------------REMOTE MODE------------------------------------------------------*/
//This function controls the robot arm from the IR remote
void RemoteMode()
{
  //Note: the values of the cases where obtained on the internet.
  switch(results.value) 
  {
    //0 //This will turn on/off the remote control mode
    case 0xFF4AB5:
    {
      Serial.println("0");
      Last = 'O';
      break;
    }
    //OK //This will  move the arm to its home position
    case 0xFF02FD:
    {
      Serial.println("OK");
      Home();
      Last = 'H';
      break;
    }
    //LEFT //This will rotate claw to the left
    case 0xFF22DD:
    {
      Serial.println("LEFT"); 
      if (CRPos > CRMin)
      {
        CRPos-=Step;
        CRServo.write(CRPos);
        Last = 'L';
        delay(wait);
      }
      break;
    }
    //RIGHT //This will rotate claw to the right
    case 0xFFC23D:
    {
      Serial.println("RIGHT");
      if (CRPos < CRMax)
      {
        CRPos+=Step;
        CRServo.write(CRPos);
        Last = 'R';
        delay(wait);
      }
      break;
    }
    //UP //This will move the claw upwards
    case 0xFF629D:
    {
      Serial.println("UP");
      if (CUDPos < CUDMax)
      {
        CUDPos+=Step;
        CUDServo.write(CUDPos);
        Last = 'U';
        delay(wait);
      }
      break;
    }
    //DOWN //This will move the claw downwards
    case 0xFFA857:
    {
      Serial.println("DOWN");    
      if (CUDPos > CUDMin)
      {
        CUDPos-=Step;
        CUDServo.write(CUDPos);
        Last = 'D';
        delay(wait);
      }
      break;
    }
    
//    //# // This decide whether to register the new coordinate or to simply goto the position
//    case 0xFF52AD:
//    {
//      Serial.println("EQ");  
//      if (EQ ==0)
//      {
//        EQ = 1;
//      }
//      else if (EQ == 1) {
//        EQ = 0;
//      }
//      break;
//    }
//    
//    case 0xFF30CF: { //1//Registed Position #1
//      Serial.println("1"); 
//      AddPosition(1);
//      break;
//    }
//    case 0xFF18E7: { //2 //Registed Position #2
//      Serial.println("2");    
//      AddPosition(2);
//      break;
//    }
//    case 0xFF7A85: { //3 //Registed Position #3
//      Serial.println("3");
//      AddPosition(3);    
//      break;
//    }
//    case 0xFF10EF: { //4 //Registed Position #4
//      Serial.println("4");
//      AddPosition(4);    
//      break;
//    }
//    case 0xFF38C7: { //5 //Registed Position #5
//      Serial.println("5");
//      AddPosition(5);    
//      break;
//    }
//    case 0xFF5AA5: { //6 //Registed Position #6
//      Serial.println("6");
//      AddPosition(6);    
//      break;
//    }
//    case 0xFF42BD: { //7 //Registed Position #7
//      Serial.println("7");
//      AddPosition(7);
//      break;
//    }
//    case 0xFF4AB5: { //8 //Registed Position #8
//      Serial.println("8");
//      AddPosition(8);    
//      break;
//    }
//    case 0xFF52AD: { //9 //Registed Position #9
//      Serial.println("9"); 
//      AddPosition(9);   
//      break;
//    }

    //This is toggle when a button is hold, so it makes the last move it made
    case 0xFFFFFFFF:
    {
      
      Serial.println("REPEAT");
      // If last was Right
      if (Last == 'R' && CRPos < CRMax)
      { 
         CRPos+=Step;
         CRServo.write(CRPos);
         delay(wait);
      }
      // If last was Left
      if (Last == 'L' && CRPos > CRMin)
      {
         CRPos-=Step;
         CRServo.write(CRPos);
         delay(wait);
      }
      // If last was  Up
      if (Last == 'U' && CUDPos < CUDMax)
      {
        CUDPos+=Step;
        CUDServo.write(CUDPos);
        delay(wait);
      }
      // If last was Down
      if (Last == 'D' && CUDPos > CUDMin)
      {
        CUDPos-=Step;
        CUDServo.write(CUDPos);
        delay(wait);
      }
    }
    //This is when the IR sensor doen't know what was the button pressed
    default:
    {
      Serial.println(" other button   ");
      Last = 'O'; //Putting last = to O as an "Do Nothing"
    }

  }//End Case
  
} //End of RemoteMode

/*----------------------------------SETUP-------------------------------------------*/
void setup() {
  Serial.begin(9600);//Specifying the baudrate
  
  //Attaching the servos to their Pin
  CRServo.attach(CRPin);
  CUDServo.attach(CUDPin);

  //I will initialize the Robot position to "home"
  Home ();
  
  //Enable the IR Receiver
  irrecv.enableIRIn();

  //Initializing the values in the arrays to home for the registed position
  for(int i=0;i<9;i++)
  {
    CRPosEQ[i] = 90;
    CUDPosEQ[i] = 90;
    Serial.println(CRPosEQ[i]);
    Serial.println(CUDPosEQ[i]);
  }
  
} //End of Setup

/*-------------------------------------LOOP-------------------------------------------*/
void loop() {
 
  if(irrecv.decode(&results))
  { //If the sensor detected any result

    RemoteMode(); //Switch the result to know what to do

    //For debug only
    Serial.print("Last = ");
    Serial.println(Last);
    Serial.print("CRPos = ");
    Serial.println(CRPos);
    Serial.print("CUDPos = ");
    Serial.println(CUDPos);
    Serial.print("Hex Code = ");
    Serial.println(results.value, (HEX));
    Serial.print("EQ = ");
    Serial.println(EQ);
    Serial.println();
    
    irrecv.resume(); //Get ready for the next result
        
  }//End If
  
  delay(25);

} // End of Loop



//The goal of this program is to be able to control my robot arm with a IR remote control.
//I would like that the robot is in sleep mode by default and when it receives an signal
//it goes into remote mode. Then when a button is pressed, the robot moves accordingly.
