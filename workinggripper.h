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


void ClawM(char d)
{
  if(d =='R'){
    digitalWrite(A1A,LOW);
    digitalWrite(A1B,HIGH); 
  }else if (d =='L'){
    digitalWrite(A1A,HIGH);
    digitalWrite(A1B,LOW);    
  }else{
    //Robojax.com L9110 Motor Tutorial
    // Turn motor OFF
    digitalWrite(A1A,LOW);
    digitalWrite(A1B,LOW);    
  }
}// motorA end



 
