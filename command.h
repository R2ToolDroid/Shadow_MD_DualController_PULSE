#include <Arduino.h>    

//// Commandos vom Master_Body_Controller///////
void ProzessComando() {

    

    if (cmd == "hi-r2" )
      {             
       #ifdef SHADOW_DEBUG
            output += "hi-r2\r\n";
          #endif
       
      }

    if (cmd == "mode0" )
      {             
       #ifdef SHADOW_DEBUG
            output += "mode0\r\n";
          #endif
      mode = 0;    
       
      }

    if (cmd == "mode1" )
      {             
       #ifdef SHADOW_DEBUG
            output += "mode1\r\n";
          #endif
      mode = 1;    
       
      }
      if (cmd == "mode2" )
      {             
       #ifdef SHADOW_DEBUG
            output += "mode2\r\n";
          #endif
      mode = 2;    
       
      }
      if (cmd == "mode3" )
      {             
       #ifdef SHADOW_DEBUG
            output += "mode3\r\n";
          #endif
      mode = 3;    
       
      }

      
      
    if (cmd == "reset" )
      {         
       #ifdef SHADOW_DEBUG
            output += "reset\r\n";
          #endif
       Serial1.print(":SE19\r");
       mode = 3 ;///Service

          
      }
      
    if (cmd.startsWith(":")) ///Dome MD Command send to DOme
    {
      #ifdef SHADOW_DEBUG
            output += "Dome Panel Command from Serial\r\n";
          #endif 
      Serial1.print(cmd);
      Serial1.print("\r");
    }

    if (cmd.startsWith("$")) ///Dome MD Command send to DOme
    {
      #ifdef SHADOW_DEBUG
            output += "Dome Sound Command from Serial\r\n";
          #endif 
      Serial1.print(cmd);
      Serial1.print("\r");
    }
    
    
}
