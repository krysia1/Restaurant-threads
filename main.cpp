//Autor: Krystyna Łosieczka
//Temat: Restauracja


#include <array>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
//#include <vector>
//#include <iomanip>
#include <memory>

#include "main.h"

#include <ncurses.h>


std::vector<int> counterStatus;
std::vector<int> restaurantStatus;
bool end = false;

/////////////////////////////////// CLASS GLASS ///////////////////////////////////////////////////////////////////////

class Glass{
public:
    int glassId;
    bool taken = false;
    bool dirty = false;
    int state = 0; //stan na początku na czysty
    std::mutex glassMutex;

};
/////////////////////////////////// CLASS GLASS END/////////////////////////////////////////////////////////////////////



/////////////////////////////////// CLASS BARMAN ///////////////////////////////////////////////////////////////////////
class Barman{
public:

    std::array<Glass, NUMOFGLASSES> &glasses;
    int pickedGlasses = 0;  //liczba trzymanych przez barmana szklanek
    int dirtyGlasses = 0; //liczba brudnych szklanek
    bool counterIsEmpty = true; //zmienna do sprawdzania, czy jest ktoś do obsłużenia przy kontuarze
    std::thread barmanThread;   //wątek barmana
    bool exitBarman = false; //czy kończymy program
    int progress;
    std::mt19937 rng{std::random_device{}()};
    std::mutex cleaningMutex;
    bool askToClean = false;
    int state = 0; //na początku stan na czekanie


    Barman(std::array<Glass, NUMOFGLASSES> &glasses)
    : glasses(glasses), barmanThread(&Barman::checkCounter, this){}



    void tryGlasses(){
        for(int i=0; i < NUMOFGLASSES; i++){
            if(pickedGlasses == 2){
                break;
            } else {
                if(!glasses[i].dirty) {
                    if (!glasses[i].taken) {
                        glasses[i].glassMutex.lock();
                        glasses[i].taken = true;


//			 changeStatusMutex.lock();
//            		 move(19 + glasses[i].glassId, 0);
//            		 clrtoeol();
//              		 printw("GLASS %d is taken by BARMAN", glasses[i]. glassId);
//            		 refresh();
//            		 changeStatusMutex.unlock();


                        glasses[i].state = 2; //stan na używany

			changeStatusMutex.lock();
                        pickedGlasses++;
			changeStatusMutex.unlock();
//                        glasses[i].glassMutex.unlock();


                    }
                }
            }
        }

    };



    void checkGlasses(){

        while(dirtyGlasses > 1){
            changeStatusMutex.lock();
            askToClean = true;
	    state = 1;
            changeStatusMutex.unlock();

 //           changeStatusMutex.lock();
//            state = 1; //stan czekania na wyczyszczenie szklanek
//            move(3, 0);
//            clrtoeol();
//            printw("BARMAN is waiting for GLASSES");
//            refresh();
//            changeStatusMutex.unlock();


//	     changeScreenMutex.lock();
             //changeCounterMutex.lock();
//             move(3, 0);
//	     clrtoeol();
//             attron(COLOR_PAIR(1));
//             printw("BAAAAAAAARMAN is waiting for GLASSES");
//             attroff(COLOR_PAIR(1));
//             refresh();
            //changeCounterMutex.unlock();
//              changeScreenMutex.unlock();


//            std::cout<<"Barman asked to clean glasses"<<std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    };



    void serveDrunkard(){

        tryGlasses();

        if(pickedGlasses != 2){ 	//jeśli udało się wziąć tylko jedną szklankę to ją odkłada
// 		tryGlasses();
//		checkGlasses();
		
		
           for(int i=0; i<NUMOFGLASSES; i++) {
                if (glasses[i].taken){
//                    glasses[i].glassMutex.lock();
                    glasses[i].taken = false;


//		    changeStatusMutex.lock();
//                    move(19 + glasses[i].glassId, 0);
//                    clrtoeol();
//                    printw("GLASS %d is put back by BARMAN", glasses[i].glassId);
//                    refresh();
//                    changeStatusMutex.unlock();


                    glasses[i].state = 0; //stan na czysty
//                    glasses[i].glassMutex.unlock();

//		     changeStatusMutex.lock();
//                    move(19 + glasses[i].glassId, 0);
//                    clrtoeol();
//                    printw("GLASS %d is clear", glasses[i].glassId);
//                    refresh();
//                    changeStatusMutex.unlock();

		    glasses[i].glassMutex.unlock();

                }
            }
	   changeStatusMutex.lock();
	   pickedGlasses = 0;
	   changeStatusMutex.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(30));

        } 
	else {
            for(int i=0; i<NUMOFGLASSES; i++) {
                if (glasses[i].taken){
//                    glasses[i].glassMutex.lock();
//                    glasses[i].dirty = true;
//                    glasses[i].glassMutex.unlock();

			changeScreenMutex.lock();
//		    changeResourcesMutex.lock();
//		    glasses[i].dirty = true;
        	    move(19 + glasses[i].glassId, 0);
        	    clrtoeol();
                    printw("GLASS %d is used by BARMAN", glasses[i].glassId);
                    refresh();
//                    changeResourcesMutex.unlock();
			changeScreenMutex.unlock();

                }
            }
            changeStatusMutex.lock();
            state = 3; //stan na obsługę pijaka
            changeStatusMutex.unlock();

//            std::cout<<"Barman is serving"<<std::endl;

            int part = std::uniform_int_distribution<int>(25, 35)(rng);
            for (auto i = 1; i <= part; i++) {
                double p = (double) i / (double) part;
                progress = (int) std::round(p * 100.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

//                changeStatusMutex.lock();
//                move(3, 0);
//                clrtoeol();
//                printw("BARMAN is serving");
//		move(3, x-4);
//		clrtoeol();
//		printw("%i%%", progress);
//		printw("%i", w);
//                refresh();
//                changeStatusMutex.unlock();
            }



            for(int i=0; i<NUMOFGLASSES; i++){
                if (glasses[i].taken){
//                    glasses[i].glassMutex.lock();
		   
                    glasses[i].dirty = true;
		    glasses[i].taken = false;
                    glasses[i].state = 1; //stan na brudny

		    changeScreenMutex.lock();
//		    changeResourcesMutex.lock();
                    move(19 + glasses[i].glassId, 0);
                    clrtoeol();
                    printw("GLASS %d is dirty", glasses[i].glassId);
                    refresh();
//                    changeResourcesMutex.unlock();
			changeScreenMutex.unlock();
                    changeStatusMutex.lock();
                    dirtyGlasses++;
                    changeStatusMutex.unlock();
                    glasses[i].glassMutex.unlock();
                }
            }


            changeStatusMutex.lock();
            amIDrunk = true;
            counterStatus[0] = -1;
            counterStatus[1] = -1;
            pickedGlasses = 0;
            changeStatusMutex.unlock();

 //           servedDrunkard.unlock();
 //           counterQueue[0].unlock();

        }

        checkGlasses();

    };



    void rest(){

        changeStatusMutex.lock();
        state = 2; //stan na odpoczywanie
        changeStatusMutex.unlock();

//        std::cout<<"Barman is resting"<<std::endl;

        int part = std::uniform_int_distribution<int>(25,35)(rng);

        for(auto i=1; i<=part; i++){
            double p = (double)i / (double)part;
            progress = (int)std::round(p * 100.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

 //           changeStatusMutex.lock();
 //           move(3, 0);
 //           clrtoeol();
 //           printw("BARMAN is resting");
//	    move(3, x-4);
//            clrtoeol();
//            printw("%i%%", progress);
//            refresh();
//            changeStatusMutex.unlock();
        }
    };



    void checkCounter(){

        while(!exitBarman) {

		bool isEmpty = true;
		for(int i = 0; i < COUNTERCAPACITY; i++){
			if(counter[i].try_lock()){
				counter[i].unlock();
			} else {
				isEmpty = false;
			
			}
		}

		if(isEmpty){
			rest();
		} else {
			if(isCounterTaken){
				serveDrunkard();
			}
		}
		



//            if (isCounterTaken) {
//                serveDrunkard();
//            } else {
//                rest();
//            }

        }
    }



};
/////////////////////////////////// CLASS BARMAN END ///////////////////////////////////////////////////////////////////



/////////////////////////////////// CLASS WAITER ///////////////////////////////////////////////////////////////////////


class Waiter {
public:
    int waiterId;
    Barman &barman;
    int progress;
    std::mt19937 rng{std::random_device{}()};
    bool exitWaiter = false;

    int glassCleaned;

    int servedClient = 0;

    std::mutex serveMutex;
    bool serveClient = false;
    std::thread waiterThread;
    int state = 0;  //na początku stan na czekanie


    Waiter(int const waiterId, Barman &barman)
        : waiterId(waiterId), barman(barman), waiterThread(&Waiter::work,this){}



    void clean(){
//        std::cout << "Waiter "<<waiterId<<" is cleaning"<< std::endl;

        changeStatusMutex.lock();
	barman.askToClean = false;
        state = 1; //stan na czyszczenie
        changeStatusMutex.unlock();

        for(int i=0; i<NUMOFGLASSES; i++){
            if (barman.glasses[i].dirty){
                barman.glasses[i].glassMutex.lock();
                barman.glasses[i].state = 3; //stan na czyszczony


		glassCleaned =  barman.glasses[i].glassId;

		changeScreenMutex.lock();
//		changeResourcesMutex.lock();
                move(19 + barman.glasses[i].glassId, 0);
                clrtoeol();
                printw("GLASS %d is being cleaned by WAITER %d", barman.glasses[i].glassId, waiterId+1);
                refresh();
//                changeResourcesMutex.unlock();
		changeScreenMutex.unlock();

                int part = std::uniform_int_distribution<int>(25, 35)(rng);

                for (auto j = 1; j <= part; j++) {
                    double p = (double) j / (double) part;
                    progress = (int) std::round(p * 100.0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));

//                    changeStatusMutex.lock();
//                    move(9 + waiterId, 0);
//                    clrtoeol();
//                    printw("WAITER %d is cleaning GLASS %d", waiterId+1, barman.glasses[i].glassId);
//		    move(9 + waiterId, x-4);
//                    clrtoeol();
//                    printw("%i%%", progress);
//                    refresh();
//                    changeStatusMutex.unlock();
                }

		changeStatusMutex.lock();
                barman.glasses[i].state = 0; //stan na czysty
		barman.dirtyGlasses--;
		changeStatusMutex.unlock();

		changeScreenMutex.lock();
//		changeResourcesMutex.lock();
                move(19 + barman.glasses[i].glassId, 0);
                clrtoeol();
                printw("GLASS %d is clear", barman.glasses[i].glassId);
                refresh();
//                changeResourcesMutex.unlock();
		changeScreenMutex.unlock();


                barman.glasses[i].dirty = false;
                barman.glasses[i].glassMutex.unlock();

 //               std::cout << "Waiter "<<waiterId<<" cleaned"<< std::endl;

            }
        }

        changeStatusMutex.lock();
       // barman.askToClean = false;
        barman.dirtyGlasses = 0;
	state = 0;
        changeStatusMutex.unlock();

    };




    void serve(){
//        std::cout << "Waiter "<<waiterId<<" is ready to serve"<< std::endl;

//        changeStatusMutex.lock();
//        move(9 + waiterId, 0);
//        clrtoeol();
//        printw("WAITER %d is ready to serve", waiterId+1);
//        refresh();
 //       changeStatusMutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        changeStatusMutex.lock();
        state = 0; //stan na czekanie
        changeStatusMutex.unlock();

        if(serveClient){
            servedClient = restaurantStatus[waiterId];

            changeStatusMutex.lock();
            state = 2; //stan na obsługe klienta
            changeStatusMutex.unlock();

            int part = std::uniform_int_distribution<int>(25, 35)(rng);
            for (auto i = 1; i <= part; i++) {
                double p = (double) i / (double) part;
                progress = (int) std::round(p * 100.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

//                changeStatusMutex.lock();
//                move(9 + waiterId, 0);
//                clrtoeol();
//                printw("WAITER %d is serving", waiterId+1);
//		move(9 + waiterId, x-4);
//                clrtoeol();
//                printw("%i%%", progress);
//                refresh();
//                changeStatusMutex.unlock();
            }


            changeStatusMutex.lock();
            amIFull[waiterId] = true;
            //serveMutex.unlock();
            serveClient = false;
	    state = 0;
            changeStatusMutex.unlock();
        }

    };



    void work(){
        while(!exitWaiter) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if(barman.askToClean){
                if (barman.cleaningMutex.try_lock()) {
                    clean();
//		    changeStatusMutex.lock();
//		    barman.askToClean = false;
//		    barman.dirtyGlasses = 0;
//		    changeStatusMutex.unlock();
                    barman.cleaningMutex.unlock();
                }
            }
            serve();
        }
    };

};









/////////////////////////////////// CLASS WAITER END ///////////////////////////////////////////////////////////////////


//std::vector<std::unique_ptr<Waiter>> waiters;


/////////////////////////////////// CLASS CLIENT ///////////////////////////////////////////////////////////////////////

class Client{
public:
    int clientId;
    int clientPurpose = 0; //0 przechodzień, 1 pijak, 2 obiadowy
    std::mt19937 rng{std::random_device{}()};   //generator liczb losowych - do losowania czy klient czy pijak
    std::thread clientThread;
    bool exitClient = false;
    std::vector<Waiter *> &waiters;



    Client(int const clientId, std::vector<Waiter *> &waiters )
    : clientId(clientId), waiters(waiters), clientThread(&Client::visitRestaurant, this){}




    void takeASeat(){
//        std::cout<<clientId<<"Client"<<std::endl;

        int pointer = RESTAURANTCAPACITY - 1;

//        for (int i = 0; i < NUMOFWAITERS; i++) {
//            std::cout<<waiters[i]->serveClient<<std::endl;
//        }

        while(!exitClient) {

            if (restaurantQueue[pointer].try_lock()) {
                changeStatusMutex.lock();
                restaurantStatus[pointer] = clientId;
                changeStatusMutex.unlock();
//                std::cout<<clientId<<"LOLOLOLO"<<std::endl;
//                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                break;

            } else {
 //               std::cout<<clientId<<"LEEEEEEE"<<std::endl;
//                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }
        }


        while(!exitClient){

            if(pointer > NUMOFWAITERS){

                if (restaurantQueue[pointer - 1].try_lock()) {

//                  std::cout<<clientId<<"Client is waiting"<<std::endl;


                    changeStatusMutex.lock();
                    restaurantStatus[pointer] = -1;
                    restaurantStatus[pointer - 1] = clientId;
                    changeStatusMutex.unlock();

                    restaurantQueue[pointer].unlock();
                    pointer--;

                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

            } else {
		    bool served = false;

                for (int i = 0; i < NUMOFWAITERS; i++) {
                    if (waiters[i]->serveMutex.try_lock()) {
//                        std::cout << clientId << "Served Client" << std::endl;


                        changeStatusMutex.lock();
			restaurantQueue[i].lock();
			restaurantStatus[i] = clientId;
			restaurantStatus[pointer] = -1;
			restaurantQueue[pointer].unlock();
                        //restaurantStatus[i] = clientId;
                        waiters[i]->serveClient = true;
                        changeStatusMutex.unlock();

                        while (!amIFull[i]) {
                            std::this_thread::sleep_for((std::chrono::milliseconds(100)));
                        }

                        changeStatusMutex.lock();
                        amIFull[i] = false;
			waiters[i]->serveMutex.unlock();
                        restaurantStatus[i] = -1;
                        served = true;
			restaurantQueue[i].unlock();
                        changeStatusMutex.unlock();
                        break;

		    } else {
                    	std::this_thread::sleep_for(std::chrono::milliseconds(200));
               	    }
		}
		if(served == true){
			break;
		}

            }

        }



    };




    void useToilet(){};






    void seatByTheCounter(){
//        std::cout<<clientId<<"Drunkard"<<std::endl;

        int pointer = COUNTERCAPACITY - 1;

        while(!exitClient) {

            if (counterQueue[pointer].try_lock()) {

                changeStatusMutex.lock();
                counterStatus[pointer + 1] = clientId;
                changeStatusMutex.unlock();

                break;

            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }
        }

        while(!exitClient){

            if(pointer != 0){

                if (counterQueue[pointer - 1].try_lock()) {

//                    std::cout<<clientId<<"Drunkard waits in the queue"<<std::endl;

                    changeStatusMutex.lock();
                    counterStatus[pointer + 1] = -1;
                    counterStatus[pointer] = clientId;
                    changeStatusMutex.unlock();

                    counterQueue[pointer].unlock();
                    pointer--;

                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

            } else {
                if(servedDrunkard.try_lock()){

                    drinkingDrunkard = clientId;

//                    std::cout<<clientId<<"Served Drunkard"<<std::endl;

                    changeStatusMutex.lock();
                    counterStatus[0] = clientId;
                    isCounterTaken = true;
                    changeStatusMutex.unlock();

                    while(!amIDrunk){
                        std::this_thread::sleep_for((std::chrono::milliseconds(100)));
                    }
                    changeStatusMutex.lock();
                    amIDrunk = false;
		    counterQueue[0].unlock();
		    servedDrunkard.unlock();
                    isCounterTaken = false;
                    changeStatusMutex.unlock();
                    break;

                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                }

            }

        }

    };







    void pay(){};



    void leave(){
        changeStatusMutex.lock();
        clientPurpose = 0;
        changeStatusMutex.unlock();

//        std::cout<<clientId<<"Bye"<<std::endl;
    };






    void visitRestaurant(){
        while(!exitClient) {	
	    int decision = std::uniform_int_distribution<int>(0, 100)(rng);

             if (decision > 70) {


                    for (int i = 0; i < COUNTERCAPACITY; i++) {
                        if (counter[i].try_lock()) {

                            changeStatusMutex.lock();
                            clientPurpose = 1;
                            changeStatusMutex.unlock();

                            seatByTheCounter();
                            counter[i].unlock();
                            break;
                        }
                    }

             } else {
                 for (int i = 0; i < RESTAURANTCAPACITY; i++) {
                     if (restaurant[i].try_lock()) {

                         changeStatusMutex.lock();
                         clientPurpose = 2;
                         changeStatusMutex.unlock();

                         takeASeat();
                         restaurant[i].unlock();
                         break;
                     }
                 }
             }

             leave();
             std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }

};
/////////////////////////////////// CLASS CLIENT END ///////////////////////////////////////////////////////////////////


std::vector<Barman *> barmans;
std::vector<Client *> clients;
std::vector<Waiter *> waiters;

//std::vector<std::unique_ptr<Barman>> barmans;
//std::vector<std::unique_ptr<Client>> clients;
//std::vector<std::unique_ptr<Waiter>> waiters;

std::array<Glass, NUMOFGLASSES> glasses;




/////////////////////////////////// THREADS FUNCTIONS //////////////////////////////////////////////////////////////////

void runBarman(Barman *b, WINDOW *counterWindow){

	while(b->exitBarman == false){
		if(b->state == 0){
			changeScreenMutex.lock();
//                        changeCounterMutex.lock();
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
                        wattron(counterWindow,COLOR_PAIR(1));
                        mvwprintw(counterWindow, 0, 0, "BARMAN is waiting");
                        wattroff(counterWindow,COLOR_PAIR(1));
                        wrefresh(counterWindow);
                   //     changeCounterMutex.unlock();
			changeScreenMutex.unlock();
                }
		if(b->state == 1){
			changeScreenMutex.lock();
			//changeCounterMutex.lock();
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
			wattron(counterWindow,COLOR_PAIR(1));
    			mvwprintw(counterWindow, 0, 0, "BARMAN is waiting for GLASSES");
    			wattroff(counterWindow,COLOR_PAIR(1));
    			wrefresh(counterWindow);
			//changeCounterMutex.unlock();
			changeScreenMutex.unlock();
		}
		if(b->state == 2){
			changeScreenMutex.lock();
			//changeCounterMutex.lock();
			int progress = b->progress;
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
			wattron(counterWindow,COLOR_PAIR(1));
			mvwprintw(counterWindow, 0, 0, "BARMAN is resting");
		        mvwprintw(counterWindow, 0, x-4, "%i%%", progress);
                        wattroff(counterWindow,COLOR_PAIR(1));
                        wrefresh(counterWindow);
			//changeCounterMutex.unlock();
			changeScreenMutex.unlock();
		}
		if(b->state == 3){
			changeScreenMutex.lock();
                        //changeCounterMutex.lock();
                        int progress = b->progress;
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
                        wattron(counterWindow,COLOR_PAIR(1));
                        mvwprintw(counterWindow, 0, 0, "BARMAN is serving CLIENT %d", drinkingDrunkard);
                        mvwprintw(counterWindow, 0, x-4, "%i%%", progress);
                        wattroff(counterWindow,COLOR_PAIR(1));
                        wrefresh(counterWindow);
			//changeCounterMutex.unlock();
               		changeScreenMutex.unlock();
	       	}
	}



}

void runWaiter(Waiter *w, WINDOW *tablesWindow){
	while(w->exitWaiter == false){
		if(w->state == 0){
			changeScreenMutex.lock();
			//changeTablesMutex.lock();
			wmove(tablesWindow, 0 + w->waiterId, 0);
			wclrtoeol(tablesWindow);
			wattron(tablesWindow,COLOR_PAIR(1));
        		mvwprintw(tablesWindow, 0 + w->waiterId, 0, "WAITER %d is ready to serve", w->waiterId+1);
        		wattroff(tablesWindow,COLOR_PAIR(1));
        		wrefresh(tablesWindow);
        		//changeTablesMutex.unlock();
			changeScreenMutex.unlock();

		}
		if(w->state == 1){
			changeScreenMutex.lock();
			//changeTablesMutex.lock();
			int progress = w->progress;
			wmove(tablesWindow, 0 + w->waiterId, 0);
			wclrtoeol(tablesWindow);
                        wattron(tablesWindow,COLOR_PAIR(1));
                        mvwprintw(tablesWindow, 0 + w->waiterId, 0, "WAITER %d is cleaning GLASS %d", w->waiterId+1, w->glassCleaned);
			mvwprintw(tablesWindow, 0 + w->waiterId, x-4, "%i%%", progress);
                        wattroff(tablesWindow,COLOR_PAIR(1));
                        wrefresh(tablesWindow);
                        //changeTablesMutex.unlock();
			changeScreenMutex.unlock();
		}
		if(w->state == 2){
			changeScreenMutex.lock();
			//changeTablesMutex.lock();
                        int progress = w->progress;
			wmove(tablesWindow, 0 + w->waiterId, 0);
			wclrtoeol(tablesWindow);
                        wattron(tablesWindow,COLOR_PAIR(1));
                        mvwprintw(tablesWindow, 0 + w->waiterId, 0, "WAITER %d is serving CLIENT %d", w->waiterId+1, w->servedClient);
                        mvwprintw(tablesWindow, 0 + w->waiterId, x-4, "%i%%", progress);
                        wattroff(tablesWindow,COLOR_PAIR(1));
                        wrefresh(tablesWindow);
                        //changeTablesMutex.unlock();
			changeScreenMutex.unlock();

		}
	}
}

void runClient(Client *c, WINDOW *counterWindow, WINDOW *tablesWindow){
	while(c->exitClient == false){
//		for(int i=0; i<counterStatus.size(); i++){
//			}
		
	}
}

void runSeats(WINDOW *counterWindow, WINDOW *tablesWindow){
	while(!end){
		for(int i=1; i<=COUNTERCAPACITY; i++){
			changeScreenMutex.lock();
			if(counterStatus[i] == -1){
				wmove(counterWindow, 1+i, 20);
                        	wclrtoeol(counterWindow);
				wattron(counterWindow, COLOR_PAIR(1));
	                        mvwprintw(counterWindow, 1+i, 20, "empty");
			}else{
			wmove(counterWindow, 1+i, 20);
			wclrtoeol(counterWindow);
			wattron(counterWindow, COLOR_PAIR(1));
			mvwprintw(counterWindow, 1+i, 20, "CLIENT %d", counterStatus[i]);
			}
			changeScreenMutex.unlock();
		}

		for(int i=0; i<RESTAURANTCAPACITY; i++){
                        changeScreenMutex.lock();
                        if(restaurantStatus[i] == -1){
                                wmove(tablesWindow, 5+i, 20);
                                wclrtoeol(tablesWindow);
                                wattron(tablesWindow, COLOR_PAIR(1));
                                mvwprintw(tablesWindow, 5+i, 20, "empty");
                        }else{
                        wmove(tablesWindow, 5+i, 20);
                        wclrtoeol(tablesWindow);
                        wattron(tablesWindow, COLOR_PAIR(1));
                        mvwprintw(tablesWindow, 5+i, 20, "CLIENT %d", restaurantStatus[i]);
                        }
                        changeScreenMutex.unlock();
                }

	}

}



void run (WINDOW *counterWindow, WINDOW *tablesWindow){

	std::vector<std::thread> v;

	for(auto& barman : barmans){
		v.push_back(std::thread(runBarman, &(*barman), counterWindow));
	}

	for(auto& waiter : waiters){
		v.push_back(std::thread(runWaiter, &(*waiter), tablesWindow));
	}

	for(auto& client : clients){
		v.push_back(std::thread(runClient, &(*client), counterWindow, tablesWindow));
	}



//	bool end = false;

	while(!end){
		int c = getch();

		if(c == 27){
			for(auto& barman : barmans){
				barman->exitBarman = true;
			}
			for(auto& waiter : waiters){
				waiter->exitWaiter = true;
			}
			for(auto& client : clients){
				client->exitClient = true;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			end = true;
		}
	}
	for(auto &t : v){
		t.join();
	}
	return;
}




/////////////////////////////////// THREADS FUNCTIONS END //////////////////////////////////////////////////////////////





/////////////////////////////////// MAIN ///////////////////////////////////////////////////////////////////////////////


int main() {

    counterStatus.resize(COUNTERCAPACITY + 1);
    for(int i = 0; i < counterStatus.size(); i++)
    {
        counterStatus[i] = -1;
    }

    restaurantStatus.resize(RESTAURANTCAPACITY);
    for(int i = 0; i < restaurantStatus.size(); i++)
    {
        restaurantStatus[i] = -1;
    }

    amIFull.resize(NUMOFWAITERS);
    for(int i = 0; i < amIFull.size(); i++)
    {
        amIFull[i] = false;
    }



    for (int i=0; i < NUMOFGLASSES; i++){
        glasses.at(i).glassId = i+1;
    }


    Barman *b = new Barman(glasses);
    barmans.push_back(b);

//    barmans.push_back(std::make_unique<Barman>(glasses));



    for (auto i=0; i<NUMOFWAITERS; i++){
        Waiter *w = new Waiter(i, *b);
        waiters.push_back(w);

//	  waiters.push_back(std::make_unique<Waiter>(i, barmans[0]));
    }


    for (auto i=0; i<NUMOFCLIENTS; i++){
        Client *c = new Client(i + 1, waiters);
        clients.push_back(c);

//	clients.push_back(std::make_unique<Client>(i+1));
    }


    initscr();
    raw();
    noecho();
    cbreak();
    curs_set(0);
    use_default_colors();
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);

    getmaxyx(stdscr, h, w);
    x = w/2;

    changeScreenMutex.lock();
//    changeCounterMutex.lock();
//    changeTablesMutex.lock();
//    changeResourcesMutex.lock();

    wbkgd(stdscr, COLOR_PAIR(1));


    WINDOW * counterWindow = newwin(5, x+1, 3, 0);
    wbkgd(counterWindow, COLOR_PAIR(1));
//    wattron(counterWindow, COLOR_PAIR(2));
//    wborder(counterWindow, 35, 35, 35, 35, 35, 35, 35, 35);
    refresh();
    wrefresh(counterWindow);

    WINDOW * kitchenWindow = newwin(7, x, 2, x+1);
    wbkgd(kitchenWindow, COLOR_PAIR(1));
    wattron(kitchenWindow, COLOR_PAIR(2));
    wborder(kitchenWindow, 35, 35, 35, 35, 35, 35, 35, 35);
    refresh();
    wrefresh(kitchenWindow);


    WINDOW * tablesWindow = newwin(10, x+1, 9, 0);
    wbkgd(tablesWindow, COLOR_PAIR(1));
//    wattron(tablesWindow, COLOR_PAIR(2));
//    wborder(tablesWindow, 35, 35, 35, 35, 35, 35, 35, 35);
    refresh();
    wrefresh(tablesWindow);

    WINDOW * toiletWindow = newwin(12, x, 8, x+1);
    wbkgd(toiletWindow, COLOR_PAIR(1));
    wattron(toiletWindow, COLOR_PAIR(2));
    wborder(toiletWindow, 35, 35, 35, 35, 35, 35, 35, 35);
    refresh();
    wrefresh(toiletWindow);

    WINDOW * glassesWindow = newwin(4, x+1, 20, 0);
    wbkgd(glassesWindow, COLOR_PAIR(1));
//    wattron(glassesWindow, COLOR_PAIR(2));
   // wborder(glassesWindow, 35, 0, 35, 35, 35, 35, 35, 35);
    refresh();
    wrefresh(glassesWindow);

    WINDOW * suppliesWindow = newwin(4, x, 20, x);
    wbkgd(suppliesWindow, COLOR_PAIR(1));
//    wattron(suppliesWindow, COLOR_PAIR(2));
  //  wborder(suppliesWindow, 0, 35, 35, 35, 35, 35, 35, 35);
    refresh();
    wrefresh(suppliesWindow);



    move(0, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("---------------------------------- RESTAURANT ----------------------------------");


    move(2, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("############### COUNTER ############################## KITCHEN #################");
    attroff(COLOR_PAIR(2));

    wattron(counterWindow,COLOR_PAIR(1));
    mvwprintw(counterWindow, 2, 0, "Seat[1]");
//    wrefresh(counterWindow);

    wattron(counterWindow,COLOR_PAIR(1));
    mvwprintw(counterWindow, 3, 0, "Seat[2]");
//    wrefresh(counterWindow);

    wattron(counterWindow,COLOR_PAIR(1));
    mvwprintw(counterWindow, 4, 0, "Seat[3]");
    wrefresh(counterWindow);
    wattroff(counterWindow,COLOR_PAIR(1));

    move(8, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("################ TABLES ############################### TOILET #################");
    attroff(COLOR_PAIR(2));

    wattron(tablesWindow,COLOR_PAIR(1));
    mvwprintw(tablesWindow, 5, 0, "Table[1]");
//    wrefresh(tablesWindow);

    wattron(tablesWindow,COLOR_PAIR(1));
    mvwprintw(tablesWindow, 6, 0, "Table[2]");
//    wrefresh(glassesWindow);

    wattron(tablesWindow,COLOR_PAIR(1));
    mvwprintw(tablesWindow, 7, 0, "Table[3]");
//    wrefresh(tablesWindow);

    wattron(tablesWindow,COLOR_PAIR(1));
    mvwprintw(tablesWindow, 8, 0, "Table[4]");
//    wrefresh(tablesWindow);

    wattron(tablesWindow,COLOR_PAIR(1));
    mvwprintw(tablesWindow, 9, 0, "Table[5]");
    wrefresh(tablesWindow);
    wattroff(tablesWindow,COLOR_PAIR(1));



    move(19, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("#################################### RESOURCES #################################");
    attroff(COLOR_PAIR(2));

	
    wattron(glassesWindow,COLOR_PAIR(1));
    mvwprintw(glassesWindow, 0, 0, "GLASS 1 is clear");
//    wrefresh(glassesWindow);

    wattron(glassesWindow,COLOR_PAIR(1));
    mvwprintw(glassesWindow, 1, 0, "GLASS 2 is clear");
//    wrefresh(glassesWindow);

    wattron(glassesWindow,COLOR_PAIR(1));
    mvwprintw(glassesWindow, 2, 0, "GLASS 3 is clear");
    wrefresh(glassesWindow);
    wattroff(glassesWindow,COLOR_PAIR(1));


    changeScreenMutex.unlock();
//    changeCounterMutex.unlock();
//    changeTablesMutex.unlock();
//    changeResourcesMutex.unlock();

    std::thread create(run, counterWindow, tablesWindow);
    std::thread seats(runSeats, counterWindow, tablesWindow);

    create.join();
    seats.join();

//for(auto b : barmans){
//	b->barmanThread.join();
//}

//for(auto c : clients){
//	c->clientThread.join();
//}

//    getch();
    endwin();


//    std::this_thread::sleep_for(std::chrono::seconds(1));


    return 0;
}
