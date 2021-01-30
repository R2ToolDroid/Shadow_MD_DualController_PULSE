#include <Arduino.h> 

/* Gripper Control functions 
 *  Servo GripPitch; ///PIN26
*   Servo GripRoll; ///PIN 27
*   
*   #define CLAW_PWM 22 // Motor IA B PWM Speed
*   #define CLAW_DIR 23 // Motor IB B Direction
 
*   // the actual values for "fast" and "slow" depend on the motor
*   #define PWM_SLOW 150  // arbitrary slow speed PWM duty cycle
*   #define PWM_FAST 200 // arbitrary fast speed PWM duty cycle
*   
*   ///STOP CLAW MOTOR
*   digitalWrite( CLAW_DIR, LOW );
    digitalWrite( CLAW_PWM, LOW );
*   // OPEN CLAW
*   digitalWrite( CLAW_DIR, HIGH ); // direction = forward
    analogWrite( CLAW_PWM, PWM_FAST ); // PWM speed = fast
    // CLOSE CLAW
    digitalWrite( CLAW_DIR, LOW ); // direction = forward
    analogWrite( CLAW_PWM, PWM_FAST ); // PWM speed = fast


*   #define GRIP_EXT 24 // PIN A
*   #define GRIP_INT 25 // PIN B
*   
*   // EXTEND GRIPPER ARM
*   analogWrite(GRIP_EXT, 150); 
    analogWrite(GRIP_INT, 0); 
*   // STOP MOVE
*   analogWrite(GRIP_EXT, 0); 
    analogWrite(GRIP_INT, 0);  

    // PUT IN GRIPPER ARM
    analogWrite(GRIP_EXT, 0); 
    analogWrite(GRIP_INT, 150);  
*   
*   
*   
*/



/// ExtM("E"); Extend
/// ExtM("I"); Put IN
/// ExtM("x"); Stop
void ExtM(char d)
{
  if(d =='E'){
    analogWrite(GRIP_EXT, 150); 
    analogWrite(GRIP_INT, 0); 
  }else if (d =='I'){
    analogWrite(GRIP_EXT, 0); 
    analogWrite(GRIP_INT, 150); 
  }else{
    // Turn motor OFF
    analogWrite(GRIP_EXT, 0); 
    analogWrite(GRIP_INT, 0);     
  }
}// motorB end


boolean working(PS3BT* myPS3 = PS3NavFoot){

  // ARM movement:
  // Stick |||| Left -> Extend
  // Stick |||| Rright -> Pull IN
  // Stick |||| UP -> Arm up
  // Stick |||| DOWN -> Arm down
  /////////CLAW
  // L2 + Stick |||| Left -> Rotate left
  // L2 + Stick |||| Right -> Rotate right
  // L2 + Stick |||| UP -> Claw Open
  // L2 + Stick |||| DOWN -> Claw Close
  // GpPpos = 90;
  // GpRpos = 90;

  //Flood control prevention
  //if (currentMillis - previousMillis >= 500) 
  
  //previousMillis = currentMillis;

  ///Stop all Motors:
  //ExtM("x");
  //ClawM("x");
  
  if (PS3NavFoot->PS3NavigationConnected) {  ///ps3FootMotorDrive(PS3NavFoot);

        if(myPS3->getButtonPress(PS))
        {
          Serial.print("MODE ..");
          Serial.println(mode);
         }

    if (myPS3->getButtonPress(L2)){

      int joystickPositionGy = myPS3->getAnalogHat(LeftHatY);
      int joystickPositionGx = myPS3->getAnalogHat(LeftHatX);
      

     

      if (joystickPositionGy <= 120) { //50
        //Serial.println( "Hand Auf..." );

       
        analogWrite( CLAW_DIR, 150 ); // direction = forward
        digitalWrite( CLAW_PWM, LOW ); // PWM speed = fast
        
        }

        
      if (joystickPositionGy >= 136) { //200     
        
        //Serial.println( "Zu.." );
        digitalWrite( CLAW_DIR, LOW );
        analogWrite( CLAW_PWM, 150 );
        }

        
       if((joystickPositionGy <= 136) && ( joystickPositionGy >= 120)) 
       {
       //Serial.println( "Stop.." );
       digitalWrite(CLAW_DIR,LOW);
       digitalWrite(CLAW_PWM,LOW);  
       
       }
      
      
      if (SRampX <= -1000) SRampX = -1000;
      if (SRampX >= 1000) SRampX = 1000;
      GpRpos = map( SRampX, -1000, 1000, 180, 00);

      if (joystickPositionGx <= 30) {
       
        SRampX--;     
        output = "Left :";
        output += GpRpos;
        
        
       GripRoll.write(GpRpos);
      }
      if (joystickPositionGx >= 160){
       
          //GpPpos++;
          SRampX++;
          
          //Serial.print("Right : ");
          //Serial.println(GpRpos);
          output = "Right : ";
          output += GpRpos;
          
          GripRoll.write(GpRpos);
          
      }
      
      /*
      output = "CLAW Y =";
      output += joystickPositionGy;
      output += " X =";
      output += joystickPositionGx;
      */
      
    } else {
      

      int joystickPositionGy = myPS3->getAnalogHat(LeftHatY);
      int joystickPositionGx = myPS3->getAnalogHat(LeftHatX);

      

      if (joystickPositionGx <= 50){ ExtM("E");Serial.println("Extend");}
      if (joystickPositionGx >= 200){ ExtM("I");Serial.println("Pull In");}


      if (SRampY <= -1000) SRampY = -1000;
      if (SRampY >= 1000) SRampY = 1000;

      

      
      GpPpos = map( SRampY, -1000, 1000, 180, 00);

      
      ////Arm to Top
      if (joystickPositionGy <= 30) {
        
        SRampY--;     
        Serial.print("UP :");
        Serial.println(GpPpos);
        
       GripPitch.write(GpPpos);
      }

      ////Arm to Bottom
      if (joystickPositionGy >= 160){
       
          //GpPpos++;
          SRampY++;
          
          Serial.print("DOWN : ");
          Serial.println(GpPpos);
          GripPitch.write(GpPpos);
          
      }


      
     

      /*
      output = "ARM Y =";
      output += joystickPositionGy;
      output += " X =";
      output += joystickPositionGx;
      */
     
    }

  
}
         
  
}


 
