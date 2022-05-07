#include <TridentTD_EasyFreeRTOS32.h>
//S
TridentOS   task1, task2;  // Crea tantas tareas como puedas.
void myblink(void*); 
void myprint(void*);   //nombre de la función la tarea se ejecutará

//----Esta área es para declarar variables que se comparten entre tareas. ------

//---------------------------------------------------------

void setup() {
  
  Serial.begin(115200); Serial.println();
  task1.start( myblink );     // Inicie la primera tarea, ejecute myblink.
  task2.start( myprint );     // Inicie la tarea dos, ejecute myprint.

}

void loop() {}           // Ya no es necesario, pero está ahí.   
