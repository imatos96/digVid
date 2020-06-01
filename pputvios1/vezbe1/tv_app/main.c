#include "main.h"
#include "remote.h"
#include "graph.h"
#include "player.h"
#include "parse.h"

//thread za player, thread za sig, thread za remote

int32_t main(){
	//reći OS-u da notifikaciju šalje pozivanjem specificirane funkcije iz posebne niti
	signalEvent.sigev_notify = SIGEV_THREAD; 
	//funkcija koju će OS prozvati kada interval istekne
	signalEvent.sigev_notify_function = inbetweenFunc; 
	//argumenti funkcije
	signalEvent.sigev_value.sival_ptr = NULL;
	//atributi niti - if NULL default attributes are applied
	signalEvent.sigev_notify_attributes = NULL; 
	timer_create(/*sistemski sat za mjerenje vremena*/ CLOCK_REALTIME,                
             /*podešavanja timer-a*/ &signalEvent,                      
            /*mjesto gdje će se smjestiti ID novog timer-a*/ &timerId);
  
        //brisanje strukture prije setiranja vrijednosti
	memset(&timerSpec,0,sizeof(timerSpec));
	//specificiranje vremenskih podešavanja timer-a
	timerSpec.it_value.tv_sec = 3; //3 seconds timeout
	timerSpec.it_value.tv_nsec = 0;

	/* create thread for remote */
        pthread_t thread_id;
        printf("Opening thread...");
        pthread_create(&thread_id, NULL, &pthreadRemoteCntrl, NULL);
       

        pthread_join(thread_id, NULL);
        printf("Nighty night thready...\n");

	return 0;

}
