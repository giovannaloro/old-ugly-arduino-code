#include <Stepper.h>//Modulo controllo motore passo-passo erogazione acqua
#include<Servo.h>//Modulo controllo motore passo-passo erogazione cibo
const int triggerPort1 = 2; //Porta di trigger del sensore di prossimita 1
const int echoPort1 = 3; //Porta di echo del sensore di prossimita 1
const int triggerPort2 = 5; //Porta di trigger del sensore di prossimita 2
const int echoPort2 = 6; //Porta di echo del sensore di prossimita 2
long init_time = 0;//Ultimo istante di tempo calcolato a partire dall'avvio di arduino in cui sono state iseguite le istruzioni del corpo del programma
long cicle_time = 20000; //Intervallo di funzionamento
boolean  t = false;//Variabile che indica se la temperatura sopra o sotto una certa soglia
boolean d1 = false; //Variabile che indica se la quantita di acqua è sopra o sotto una certa soglia
boolean d2 = false;//Variabile che indica se la quantita di cibo è sopra o sotto una certa soglia
boolean sf = false;//Variabile di stato fsm gestione cibo
boolean sw = false;//Variabile di stato fsm gestione acqua
int keine = 0;// variabile che serve a contaggiare il numero di cicli in cui non si esegue nessuna azione
Stepper myStepper(2048,10,12,11,13); //configurazione stepper motor erogazione cibo
Servo servo1; //creazione oggetto servo-motore

boolean temp_misuration(){ //Questa funzione resistuisce il valore booleano true se la temperatura è sopra una certa soglia
  int t = analogRead(A0); //Lettura valore termistore
  if (t > 25){
    return HIGH;
  }
  return LOW;
}
  
boolean dist_misuration1(){//Questa funzione resistuisce il valore booleano true se la quantita di aqua è sopra una certa soglia
  digitalWrite(triggerPort1, LOW);
  digitalWrite(triggerPort1, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPort1, LOW);//Invio del segnale di trigger al sensore
  long durata = pulseIn(echoPort1,HIGH);//Questa funzione restituisce quanto tempo passa dall'invio del segnale alla sua ricezione
  long spazio = 0.034 * durata/2; //calcolo dello spazio ottenuto  moltiplicando la velocità del suono in centimetri/microsecondi per il tempo che il segnale ha impiegato per compiere una certa distanza diviso 2
  if (spazio > 9){
   return HIGH;
  }
  return LOW;
}

boolean dist_misuration2(){//Questa funzione resistuisce un valore booleano casuale 
  randomSeed(analogRead(3));//Lettura valore(casuale) su porta anlogica n.3
  int r = random(0,2);
  if (r==0){
    return LOW;
  }
  else{
    return HIGH;
  }
}


void setup(){
  pinMode(triggerPort1, OUTPUT);
  pinMode(echoPort1, INPUT);
  pinMode(triggerPort2, OUTPUT);
  pinMode(echoPort2, INPUT);
  servo1.attach(4); //setto il server motor per ricevere segnali dalla porta digitale n.4
  myStepper.setSpeed(10);//setto la velocità del motore passo-passo per l'erogazione dell'acqua
  Serial.begin(9600);//inizializzo la porta seriale
}

void loop(){
  while(millis() - init_time < cicle_time){
    keine += 0;
  }//Ciclo che non esegue nessuna azione finchè non trascorre un certo intervallo di tempo
  t = temp_misuration();
  d1 = dist_misuration1();
  d2 = dist_misuration2();
  boolean new_state_w = !sw&&!d1||!d1&&t; //calcolo del nuovo stato della fsm per l'erogazione dell'acqua(l'uscita corrisponde al nuovo stato)
  boolean new_state_f = !d2&&!t||!sf&&!d2;//calcolo del nuovo stato della fsm per l'erogazione del cibo(l'uscita corrisponde al nuovo stato)
  sw = new_state_w; //Lo stato futuro diventa lo stato attuale
  sf = new_state_f;//Lo stato futuro diventa lo stato attuale
  Serial.print("\n ");
  Serial.print("sw: ");
  Serial.println(sw);
  Serial.print("\n");
  Serial.print("sf: ");
  Serial.println(sf);
  Serial.print("\n");
  Serial.print("t: ");
  Serial.println(t);
  Serial.print("\n");
  Serial.print("d1: ");
  Serial.println(d1);
  Serial.print("\n");
  Serial.print("d2: ");
  Serial.println(d2);
  Serial.print("\n");
  
  
  
  if(sf){//Istruzione che viene eseguita se l'uscita della fsm per l'erogazione del cibo va alta
    servo1.write(0);
    delay(1000);
    for(int i = 0; i != 70; ++i){//Il motore si muove di  70 passi
       servo1.write(i);
       delay(20);
    }
    delay(500);
    servo1.write(0);//Il motore torna nella sua posizione iniziale 
  }
  if (sw){//Istruzione che viene eseguita se l'uscita della fsm per l'erogazione dell'acqua va alta
    float gradi = 360;
    gradi = map(gradi,0,360,0,2048);//I 2048 gradi iniziali vengono ridotti a  360 gradi in maniera tale da agevolare il controll0 del motore
    myStepper.step(-gradi); //Questa struzione  fa ruotare il motore di 360 gradi
    delay(1000);
    myStepper.step(gradi);//Questa istruzione riporta il motore nella sua posizione iniziale
    delay(1000);
  }
  init_time = millis();//L'init_time viene re-impostato all'ultimo istante in cui sono state eseguite le istruzioni principali
}
