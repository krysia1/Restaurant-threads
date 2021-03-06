//Autor: Krystyna Łosieczka
//Temat: Restauracja


#include <array>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <memory>
#include "main.h"
#include <ncurses.h>


std::vector<int> counterStatus;
std::vector<int> restaurantStatus;
std::vector<int> queueOneStatus;
std::vector<int> queueTwoStatus;


bool end = false;

int money;
int supplies;


/////////////////////////////////// CLASS KNIFE ///////////////////////////////////////////////////////////////////////

class Knife{
public:
	int knifeId;
	int state = 0;
	int owner;
	bool taken = false;
	std::mutex knifeMutex;
};

/////////////////////////////////// CLASS KNIFE END ///////////////////////////////////////////////////////////////////


/////////////////////////////////// CLASS TOILET //////////////////////////////////////////////////////////////////////

class Toilet{
public:
	int toiletId;
	int queue = 0;

	bool occupied = false;
	bool leftOccupied = false;
	bool rightOccupied = false;

	std::mutex leftCabinMutex;
	std::mutex rightCabinMutex;

	std::mutex toiletMutex;

};

///////////////////////////////////// CLASS TOILET END //////////////////////////////////////////////////////////////////


/////////////////////////////////// CLASS GLASS ///////////////////////////////////////////////////////////////////////

class Glass{
public:
    int glassId;
    bool taken = false;
    bool dirty = false;
    int cleaner;
    int state = 0; //stan na początku na czysty
    std::mutex glassMutex;

};
/////////////////////////////////// CLASS GLASS END/////////////////////////////////////////////////////////////////////

std::array<Glass, NUMOFGLASSES> glasses;
std::array<Knife, NUMOFKNIVES> knives;

/////////////////////////////////// CLASS BARMAN ///////////////////////////////////////////////////////////////////////
class Barman{
public:

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

    Barman()
    : barmanThread(&Barman::checkCounter, this){}



    void tryGlasses(){
        for(int i=0; i < NUMOFGLASSES; i++){
            if(pickedGlasses == 2){
                break;
            } else {
                if(!glasses[i].dirty) {
                    if (!glasses[i].taken) {
                        glasses[i].glassMutex.lock();
                        glasses[i].taken = true;

			changeStatusMutex.lock();
                        pickedGlasses++;
			changeStatusMutex.unlock();
                        glasses[i].glassMutex.unlock();


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

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    };



    void serveDrunkard(){

        tryGlasses();

        if(pickedGlasses != 2){ 	//jeśli udało się wziąć tylko jedną szklankę to ją odkłada
		
           for(int i=0; i<NUMOFGLASSES; i++) {
                if (glasses[i].taken){

                    glasses[i].glassMutex.lock();
                    glasses[i].taken = false;
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
                    
		    glasses[i].glassMutex.lock();
                    glasses[i].dirty = true;
		    glasses[i].state = 2;
		    glasses[i].glassMutex.unlock();

                }
            }
            changeStatusMutex.lock();
            state = 3; //stan na obsługę pijaka
            changeStatusMutex.unlock();

            int part = std::uniform_int_distribution<int>(25, 35)(rng);
            for (auto i = 1; i <= part; i++) {
                double p = (double) i / (double) part;
                progress = (int) std::round(p * 100.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }


	    changeStatusMutex.lock();
            state = 0;
            changeStatusMutex.unlock();

            for(int i=0; i<NUMOFGLASSES; i++){
                if (glasses[i].taken){
                    
		    glasses[i].glassMutex.lock();
		   
                    glasses[i].dirty = true;
		    glasses[i].taken = false;
                    glasses[i].state = 1; //stan na brudny
                    
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

        }

        checkGlasses();

    };



    void rest(){

        changeStatusMutex.lock();
        state = 2; //stan na odpoczywanie
        changeStatusMutex.unlock();

        int part = std::uniform_int_distribution<int>(25,35)(rng);

        for(auto i=1; i<=part; i++){
            double p = (double)i / (double)part;
            progress = (int)std::round(p * 100.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
		
        }
    }



};
/////////////////////////////////// CLASS BARMAN END ///////////////////////////////////////////////////////////////////


/////////////////////////////////// CLASS SUPPLIER /////////////////////////////////////////////////////////////////////

class Supplier{
public:
	bool exitSupplier = false;
	bool dealing = false;
	bool done = false;
	int state = 0;
	int progress;
        std::mt19937 rng{std::random_device{}()};
	std::thread supplierThread;

	Supplier()
		:supplierThread(&Supplier::supply, this){}


	void bringSupplies(){
		changeStatusMutex.lock();
		state = 2;
		suppliesAlert = true;
                changeStatusMutex.unlock();

		while(!dealing){
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		changeStatusMutex.lock();
                state = 1;
                changeStatusMutex.unlock();
		
		int part = std::uniform_int_distribution<int>(25,35)(rng);

                for(auto i=1; i<=part; i++){
             		double p = (double)i / (double)part;
                        progress = (int)std::round(p * 100.0);
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

		while(!done){
			if(money >= 3){
				changeStatusMutex.lock();
				money -= 3;
				supplies += 3;
				done = true;
				changeStatusMutex.unlock();

			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}

	};

	void supply(){
		while(!exitSupplier){
			
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			changeStatusMutex.lock();
			state = 0;
			dealing = false;
			done = false;
			changeStatusMutex.unlock();

			int part = std::uniform_int_distribution<int>(5,8)(rng);

        		for(auto i=1; i<part; i++){
				progress = part-i;
            			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}

			bringSupplies();

		}
	}



};
/////////////////////////////////// CLASS SUPPLIER END /////////////////////////////////////////////////////////////////

std::vector<Supplier *> suppliers;

/////////////////////////////////// CLASS COOK /////////////////////////////////////////////////////////////////////////

class Cook{
public:
        int cookId;
        bool exitCook = false;
        int progress;
        std::mt19937 rng{std::random_device{}()};
        int state = 0;
        bool mealRequest = false;
        std::mutex mealRequestMutex;

        std::thread cookThread;

        Cook(int const cookId, Cook &cook)
                : cookId(cookId), cookThread(&Cook::cook, this){}


        void takeSupplies(){
                changeStatusMutex.lock();
                suppliesAlert = false;
                state = 2;
		suppliers[0]->dealing = true;
                changeStatusMutex.unlock();
		
		while(suppliers[0]->done == false){
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}


        }


        void makeMeal(){
                changeStatusMutex.lock();
                state = 0;
                changeStatusMutex.unlock();

                if(mealRequest){
			bool tookKnife = false;
			bool madeMeal = false;
			int knife;

			while(!tookKnife){
				if(suppliesAlert){
	                                if(suppliesAlertMutex.try_lock()){
	                                        takeSupplies();
	                                        suppliesAlertMutex.unlock();
                                	}
                        	}

				changeStatusMutex.lock();
		                state = 3;
               			changeStatusMutex.unlock();

				for(int i=0; i<NUMOFKNIVES; i++){
					if(knives[i].taken == false){
						knives[i].knifeMutex.lock();
						knives[i].taken = true;
						knives[i].state = 1;
						knives[i].owner = cookId + 1;
						tookKnife = true;
						knife = i;
						break;
					}
				}
			}
			

			while(!madeMeal){
				changeStatusMutex.lock();
				state = 4;
				changeStatusMutex.unlock();

				if(supplies >= 2){
					changeStatusMutex.lock();
					supplies -= 2;
					state = 1;
					changeStatusMutex.unlock();

					int part = std::uniform_int_distribution<int>(25,35)(rng);

		                        for(auto i=1; i<=part; i++){
		                            double p = (double)i / (double)part;
                		            progress = (int)std::round(p * 100.0);
                      		 	    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        		}

                        		knives[knife].taken = false;
                        		knives[knife].state = 0;
                        		knives[knife].knifeMutex.unlock();


                        		changeStatusMutex.lock();
                        		mealRequest = false;
					madeMeal = true;
                        		state = 0;
                        		mealRequestMutex.unlock();
                        		changeStatusMutex.unlock();

				} else {
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}

			}

                }
        }



        void cook(){
                while(!exitCook){
                        if(suppliesAlert){
                                if(suppliesAlertMutex.try_lock()){
                                        takeSupplies();
                                        suppliesAlertMutex.unlock();
                                }
                        }
                        makeMeal();
                }




        }
};

/////////////////////////////////// CLASS COOK END /////////////////////////////////////////////////////////////////////

std::vector<Cook *> cooks;


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

        changeStatusMutex.lock();
	barman.askToClean = false;
        state = 1; //stan na czyszczenie
        changeStatusMutex.unlock();

        for(int i=0; i<NUMOFGLASSES; i++){
            if (glasses[i].dirty){

                glasses[i].glassMutex.lock();
                glasses[i].state = 3; //stan na czyszczony
		glasses[i].cleaner =  waiterId + 1;
		glassCleaned = glasses[i].glassId;

                int part = std::uniform_int_distribution<int>(25, 35)(rng);

                for (auto j = 1; j <= part; j++) {
                    double p = (double) j / (double) part;
                    progress = (int) std::round(p * 100.0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

		glasses[i].state = 0; //stan na czysty

		changeStatusMutex.lock();
		barman.dirtyGlasses--;
		changeStatusMutex.unlock();

                glasses[i].dirty = false;
                glasses[i].glassMutex.unlock();
            }
        }

        changeStatusMutex.lock();
        barman.dirtyGlasses = 0;
	state = 0;
        changeStatusMutex.unlock();

    };




    void serve(){

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        changeStatusMutex.lock();
        state = 0; //stan na czekanie
        changeStatusMutex.unlock();

        if(serveClient){
            servedClient = restaurantStatus[waiterId];

            changeStatusMutex.lock();
            state = 3; //stan na zbieranie zamowienia
            changeStatusMutex.unlock();

	    bool ordered = false;

            while(!ordered){
                for(int i=0; i<NUMOFCOOKS; i++){
                    if(cooks[i]->mealRequestMutex.try_lock()){
                            cooks[i]->mealRequest = true;
                            while(cooks[i]->mealRequest == true){
                                    std::this_thread::sleep_for((std::chrono::milliseconds(100)));
                            }
                            ordered = true;
                            break;
                    }
                }
            }

	    changeStatusMutex.lock();
            state = 2; //stan na obsługe klienta
            changeStatusMutex.unlock();



            int part = std::uniform_int_distribution<int>(25, 35)(rng);
            for (auto i = 1; i <= part; i++) {
                double p = (double) i / (double) part;
                progress = (int) std::round(p * 100.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }



            changeStatusMutex.lock();
            amIFull[waiterId] = true;
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
                    barman.cleaningMutex.unlock();
                }
            }
            serve();
        }
    };

};

/////////////////////////////////// CLASS WAITER END ///////////////////////////////////////////////////////////////////




/////////////////////////////////// CLASS CLIENT ///////////////////////////////////////////////////////////////////////

class Client{
public:
    int clientId;
    int clientPurpose = 0; //0 przechodzień, 1 pijak, 2 obiadowy
    std::mt19937 rng{std::random_device{}()};   //generator liczb losowych - do losowania czy klient czy pijak
    std::thread clientThread;
    bool exitClient = false;
    std::vector<Waiter *> &waiters;
    std::array<Toilet, NUMOFTOILETS> &toilets;

    int progressToilet;

    Client(int const clientId, std::vector<Waiter *> &waiters, std::array<Toilet, NUMOFTOILETS> &toilets)
    : clientId(clientId), waiters(waiters), toilets(toilets), clientThread(&Client::visitRestaurant, this){}




    void takeASeat(){

        int pointer = RESTAURANTCAPACITY - 1;

        while(!exitClient) {

            if (restaurantQueue[pointer].try_lock()) {
                changeStatusMutex.lock();
                restaurantStatus[pointer] = clientId;
                changeStatusMutex.unlock();
                break;

            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }
        }


        while(!exitClient){

            if(pointer > NUMOFWAITERS){

                if (restaurantQueue[pointer - 1].try_lock()) {

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

                        changeStatusMutex.lock();
			restaurantQueue[i].lock();
			restaurantStatus[i] = clientId;
			restaurantStatus[pointer] = -1;
			restaurantQueue[pointer].unlock();
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
			money += 1;
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




    void useToilet(){

	    int smallestQueue;
            int myToilet;
            int pointer = QUEUECAPACITY - 1;


	    while(!exitClient){
		    for(int i = 0; i < NUMOFTOILETS - 1; i++){
			    if(toilets[i].queue <= toilets[i+1].queue){
				    smallestQueue = toilets[i].queue;
				    myToilet = i;	
			    } else {
				    smallestQueue = toilets[i+1].queue;
				    myToilet = i+1;
			    }	    
		    }

		    if(myToilet == 0){
			    if(toiletOneQueue[pointer].try_lock()){
				    changeStatusMutex.lock();
                            	    queueOneStatus[pointer] = clientId;
                            	    toilets[myToilet].queue++;
                            	    changeStatusMutex.unlock();
                            	    break;
			    } else {
				     std::this_thread::sleep_for(std::chrono::milliseconds(200));
                	             continue;
			    }
		    } else {
			    if(toiletTwoQueue[pointer].try_lock()){
                                    changeStatusMutex.lock();
                                    queueTwoStatus[pointer] = clientId;
                                    toilets[myToilet].queue++;
                                    changeStatusMutex.unlock();
                                    break;
                            } else {
                                     std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                     continue;
                            }

		    }

	    }




	    while(!exitClient){

		   if(pointer > TOILETCAPACITY){

			   if(myToilet == 0){
			   	if (toiletOneQueue[pointer - 1].try_lock()) {
				   changeStatusMutex.lock();
				   queueOneStatus[pointer] = -1;
				   queueOneStatus[pointer - 1] = clientId;
				   changeStatusMutex.unlock();
				   toiletOneQueue[pointer].unlock();
				   pointer--;
			   	} else {
				   std::this_thread::sleep_for(std::chrono::milliseconds(200));
			   	}
			   } else {
				if (toiletTwoQueue[pointer - 1].try_lock()) {
                                   changeStatusMutex.lock();
                                   queueTwoStatus[pointer] = -1;
                                   queueTwoStatus[pointer - 1] = clientId;
                                   changeStatusMutex.unlock();
                                   toiletTwoQueue[pointer].unlock();
                                   pointer--;
                                } else {
                                   std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                }
			   }

		   } else {

			   if(toilets[myToilet].leftCabinMutex.try_lock()){
				
				   toilets[myToilet].leftOccupied == true;

				   if(myToilet == 0){

				   	changeStatusMutex.lock();
				   	toiletOneQueue[0].lock();
				   	queueOneStatus[0] = clientId;
				   	queueOneStatus[pointer] = -1;
				   	toiletOneQueue[pointer].unlock();
				   	changeStatusMutex.unlock();

				   	int part = std::uniform_int_distribution<int>(55, 95)(rng);
				   	for (auto i = 1; i <= part; i++){
					   double p = (double) i / (double) part; 
				   	   progressToilet = (int) std::round(p * 100.0);
                                           std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                   	}

				   	toilets[myToilet].leftOccupied == false;
				   	toilets[myToilet].leftCabinMutex.unlock();

				   	changeStatusMutex.lock();
				   	toiletOneQueue[0].unlock();
                                   	queueOneStatus[0] = -1;
				   	toilets[myToilet].queue--;
				   	changeStatusMutex.unlock();
			   	   	break;

				   } else {

				   	changeStatusMutex.lock();
                                   	toiletTwoQueue[0].lock();
                                   	queueTwoStatus[0] = clientId;
                                   	queueTwoStatus[pointer] = -1;
                                   	toiletTwoQueue[pointer].unlock();
                                   	changeStatusMutex.unlock();

                                   	int part = std::uniform_int_distribution<int>(55, 95)(rng);
                                   	for (auto i = 1; i <= part; i++){
                                           double p = (double) i / (double) part;
                                           progressToilet = (int) std::round(p * 100.0);
                                           std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                   	}

                                   	toilets[myToilet].leftOccupied == false;
                                   	toilets[myToilet].leftCabinMutex.unlock();


                                   	changeStatusMutex.lock();
                                   	toiletTwoQueue[0].unlock();
                                   	queueTwoStatus[0] = -1;
                                   	toilets[myToilet].queue--;
                                   	changeStatusMutex.unlock();
                                   	break;
				   }
			  
			   
			   } else  if(toilets[myToilet].rightCabinMutex.try_lock()){

                                   toilets[myToilet].rightOccupied == true;

                                   if(myToilet == 0){

                                   	changeStatusMutex.lock();
                                   	toiletOneQueue[1].lock();
                                   	queueOneStatus[1] = clientId;
                                   	queueOneStatus[pointer] = -1;
                                   	toiletOneQueue[pointer].unlock();
                                   	changeStatusMutex.unlock();

                                   	int part = std::uniform_int_distribution<int>(55, 95)(rng);
                                   	for (auto i = 1; i <= part; i++){
                                           double p = (double) i / (double) part;
                                           progressToilet = (int) std::round(p * 100.0);
                                           std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                   	}

                                   	toilets[myToilet].rightOccupied == false;
                                   	toilets[myToilet].rightCabinMutex.unlock();

                                   	changeStatusMutex.lock();
                                   	toiletOneQueue[1].unlock();
                                   	queueOneStatus[1] = -1;
                                   	toilets[myToilet].queue--;
                                   	changeStatusMutex.unlock();
                                   	break;

                                   } else {
				   
			           	changeStatusMutex.lock();
                                   	toiletTwoQueue[1].lock();
                                   	queueTwoStatus[1] = clientId;
                                   	queueTwoStatus[pointer] = -1;
                                   	toiletTwoQueue[pointer].unlock();
                                   	changeStatusMutex.unlock();

                                   	int part = std::uniform_int_distribution<int>(55, 95)(rng);
                                   	for (auto i = 1; i <= part; i++){
                                           double p = (double) i / (double) part;
                                           progressToilet = (int) std::round(p * 100.0);
                                           std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                  	}

                                   	toilets[myToilet].rightOccupied == false;
                                   	toilets[myToilet].rightCabinMutex.unlock();

                                   	changeStatusMutex.lock();
                                   	toiletTwoQueue[1].unlock();
                                   	queueTwoStatus[1] = -1;
                                   	toilets[myToilet].queue--;
                                   	changeStatusMutex.unlock();
                                   	break;
				   }



			   } else {
                        	   std::this_thread::sleep_for(std::chrono::milliseconds(200));
			   }
			   
                }
		 
	    }
    };






    void seatByTheCounter(){

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

                    changeStatusMutex.lock();
                    counterStatus[0] = clientId;
                    isCounterTaken = true;
                    changeStatusMutex.unlock();

                    while(!amIDrunk){
                        std::this_thread::sleep_for((std::chrono::milliseconds(100)));
                    }
                    changeStatusMutex.lock();
                    amIDrunk = false;
		    money += 3;
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



    void leave(){
        changeStatusMutex.lock();
        clientPurpose = 0;
        changeStatusMutex.unlock();
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
			    useToilet();
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
			 useToilet();
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
std::array<Toilet, NUMOFTOILETS> toilets;

/////////////////////////////////// THREADS FUNCTIONS //////////////////////////////////////////////////////////////////

void runBarman(Barman *b, WINDOW *counterWindow){

	while(b->exitBarman == false){
		if(b->state == 0){
			changeScreenMutex.lock();
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
                        wattron(counterWindow,COLOR_PAIR(1));
                        mvwprintw(counterWindow, 0, 0, "BARMAN is waiting");
                        wattroff(counterWindow,COLOR_PAIR(1));
                        wrefresh(counterWindow);
			changeScreenMutex.unlock();
                }
		if(b->state == 1){
			changeScreenMutex.lock();
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
			wattron(counterWindow,COLOR_PAIR(1));
    			mvwprintw(counterWindow, 0, 0, "BARMAN is waiting for GLASSES");
    			wattroff(counterWindow,COLOR_PAIR(1));
    			wrefresh(counterWindow);
			changeScreenMutex.unlock();
		}
		if(b->state == 2){
			changeScreenMutex.lock();
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
			wattron(counterWindow,COLOR_PAIR(1));
			mvwprintw(counterWindow, 0, 0, "BARMAN is resting");
		        mvwprintw(counterWindow, 0, x-4, "%i%%", b->progress);
                        wattroff(counterWindow,COLOR_PAIR(1));
                        wrefresh(counterWindow);
			changeScreenMutex.unlock();
		}
		if(b->state == 3){
			changeScreenMutex.lock();
			wmove(counterWindow, 0, 0);
			wclrtoeol(counterWindow);
                        wattron(counterWindow,COLOR_PAIR(1));
                        mvwprintw(counterWindow, 0, 0, "BARMAN is serving CLIENT %d", drinkingDrunkard);
                        mvwprintw(counterWindow, 0, x-4, "%i%%", b->progress);
                        wattroff(counterWindow,COLOR_PAIR(1));
                        wrefresh(counterWindow);
               		changeScreenMutex.unlock();
	       	}
	}



}

void runSupplier(Supplier *s, WINDOW *kitchenWindow){
	while(s->exitSupplier == false){
		if(s->state == 0){
			changeScreenMutex.lock();
			wmove(kitchenWindow, 5, 1);
			wclrtoeol(kitchenWindow);
			wattron(kitchenWindow, COLOR_PAIR(1));
			mvwprintw(kitchenWindow, 5, 1, "SUPPLIER will come in");
			mvwprintw(kitchenWindow, 5, 30, "%i", s->progress);
			wattroff(kitchenWindow, COLOR_PAIR(1));
			wrefresh(kitchenWindow);
			changeScreenMutex.unlock();
		}
		if(s->state == 1){
                        changeScreenMutex.lock();
                        wmove(kitchenWindow, 5, 1);
                        wclrtoeol(kitchenWindow);
                        wattron(kitchenWindow, COLOR_PAIR(1));
                        mvwprintw(kitchenWindow, 5, 1, "SUPPLIER is dealing");
                        mvwprintw(kitchenWindow, 5, 30, "%i%%", s->progress);
                        wattroff(kitchenWindow, COLOR_PAIR(1));
                        wrefresh(kitchenWindow);
                        changeScreenMutex.unlock();
                }
		if(s->state == 2){
                        changeScreenMutex.lock();
                        wmove(kitchenWindow, 5, 1);
                        wclrtoeol(kitchenWindow);
                        wattron(kitchenWindow, COLOR_PAIR(1));
                        mvwprintw(kitchenWindow, 5, 1, "SUPPLIER is waiting for COOK");
                        wattroff(kitchenWindow, COLOR_PAIR(1));
                        wrefresh(kitchenWindow);
                        changeScreenMutex.unlock();
                }


	}
}


void runCook(Cook *c, WINDOW *kitchenWindow){
	while(c->exitCook == false){
		if(c->state == 0){
			changeScreenMutex.lock();
                        wmove(kitchenWindow, 1 + c->cookId, 1);
                        wclrtoeol(kitchenWindow);
                        wattron(kitchenWindow,COLOR_PAIR(1));
                        mvwprintw(kitchenWindow, 1 + c->cookId, 1, "COOK %d is waiting", c->cookId+1);
                        wattroff(kitchenWindow,COLOR_PAIR(1));
                        wrefresh(kitchenWindow);
                        changeScreenMutex.unlock();

		}
		if(c->state == 1){
                        changeScreenMutex.lock();
                        wmove(kitchenWindow, 1 + c->cookId, 1);
                        wclrtoeol(kitchenWindow);
                        wattron(kitchenWindow,COLOR_PAIR(1));
                        mvwprintw(kitchenWindow, 1 + c->cookId, 1, "COOK %d is preparing a meal", c->cookId+1);
			mvwprintw(kitchenWindow, 1 + c->cookId, 30, "%i%%", c->progress);
                        wattroff(kitchenWindow,COLOR_PAIR(1));
                        wrefresh(kitchenWindow);
                        changeScreenMutex.unlock();

                }
		if(c->state == 2){
                        changeScreenMutex.lock();
                        wmove(kitchenWindow, 1 + c->cookId, 1);
                        wclrtoeol(kitchenWindow);
                        wattron(kitchenWindow,COLOR_PAIR(1));
                        mvwprintw(kitchenWindow, 1 + c->cookId, 1, "COOK %d is making a deal with SUPPLIER", c->cookId+1);
                        wattroff(kitchenWindow,COLOR_PAIR(1));
                        wrefresh(kitchenWindow);
                        changeScreenMutex.unlock();

                }
		if(c->state == 3){
                        changeScreenMutex.lock();
                        wmove(kitchenWindow, 1 + c->cookId, 1);
                        wclrtoeol(kitchenWindow);
                        wattron(kitchenWindow,COLOR_PAIR(1));
                        mvwprintw(kitchenWindow, 1 + c->cookId, 1, "COOK %d is waiting for KNIFE", c->cookId+1);
                        wattroff(kitchenWindow,COLOR_PAIR(1));
                        wrefresh(kitchenWindow);
                        changeScreenMutex.unlock();

                }
		if(c->state == 4){
                        changeScreenMutex.lock();
                        wmove(kitchenWindow, 1 + c->cookId, 1);
                        wclrtoeol(kitchenWindow);
                        wattron(kitchenWindow,COLOR_PAIR(1));
                        mvwprintw(kitchenWindow, 1 + c->cookId, 1, "COOK %d is waiting for SUPPLIES", c->cookId+1);
                        wattroff(kitchenWindow,COLOR_PAIR(1));
                        wrefresh(kitchenWindow);
                        changeScreenMutex.unlock();

                }




	}
}

void runWaiter(Waiter *w, WINDOW *tablesWindow){
	while(w->exitWaiter == false){
		if(w->state == 0){
			changeScreenMutex.lock();
			wmove(tablesWindow, 0 + w->waiterId, 0);
			wclrtoeol(tablesWindow);
			wattron(tablesWindow,COLOR_PAIR(1));
        		mvwprintw(tablesWindow, 0 + w->waiterId, 0, "WAITER %d is ready to serve", w->waiterId+1);
        		wattroff(tablesWindow,COLOR_PAIR(1));
        		wrefresh(tablesWindow);
			changeScreenMutex.unlock();

		}
		if(w->state == 1){
			changeScreenMutex.lock();
			wmove(tablesWindow, 0 + w->waiterId, 0);
			wclrtoeol(tablesWindow);
                        wattron(tablesWindow,COLOR_PAIR(1));
                        mvwprintw(tablesWindow, 0 + w->waiterId, 0, "WAITER %d is cleaning GLASS %d", w->waiterId+1, w->glassCleaned);
			mvwprintw(tablesWindow, 0 + w->waiterId, x-4, "%i%%", w->progress);
                        wattroff(tablesWindow,COLOR_PAIR(1));
                        wrefresh(tablesWindow);
			changeScreenMutex.unlock();
		}
		if(w->state == 2){
			changeScreenMutex.lock();
			wmove(tablesWindow, 0 + w->waiterId, 0);
			wclrtoeol(tablesWindow);
                        wattron(tablesWindow,COLOR_PAIR(1));
                        mvwprintw(tablesWindow, 0 + w->waiterId, 0, "WAITER %d is serving CLIENT %d", w->waiterId+1, w->servedClient);
                        mvwprintw(tablesWindow, 0 + w->waiterId, x-4, "%i%%", w->progress);
                        wattroff(tablesWindow,COLOR_PAIR(1));
                        wrefresh(tablesWindow);
			changeScreenMutex.unlock();

		}
		if(w->state == 3){
                        changeScreenMutex.lock();
                        wmove(tablesWindow, 0 + w->waiterId, 0);
                        wclrtoeol(tablesWindow);
			wattron(tablesWindow,COLOR_PAIR(1));
                        mvwprintw(tablesWindow, 0 + w->waiterId, 0, "WAITER %d is taking CLIENT %d order", w->waiterId+1, w->servedClient);
                        wattroff(tablesWindow,COLOR_PAIR(1));
			wrefresh(tablesWindow);
                        changeScreenMutex.unlock();

                }

	}
}


void runSeats(WINDOW *counterWindow, WINDOW *tablesWindow, WINDOW *toiletWindow, WINDOW *queueOneWindow, WINDOW *queueTwoWindow, WINDOW *glassesWindow, WINDOW *suppliesWindow){
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

		for(int i = 0; i < QUEUECAPACITY; i++){
			changeScreenMutex.lock();
			if(queueOneStatus[i] == -1){
				if(i<TOILETCAPACITY){
					wmove(toiletWindow, 2+i, 12);
					wclrtoeol(toiletWindow);
       					wattron(toiletWindow, COLOR_PAIR(1));
	                                mvwprintw(toiletWindow, 2+i, 12, "empty");
				} else {
					wmove(queueOneWindow, -2+i, 0);
                                        wclrtoeol(queueOneWindow);
                                        wattron(queueOneWindow, COLOR_PAIR(1));
                                        mvwprintw(queueOneWindow, -2+i, 0, "empty");

				}
			} else {
				if(i<TOILETCAPACITY){
                                         wmove(toiletWindow, 2+i, 12);
                                         wclrtoeol(toiletWindow);
                                         wattron(toiletWindow, COLOR_PAIR(1));
                                         mvwprintw(toiletWindow, 2+i, 12, "CLIENT %d", queueOneStatus[i]);
					 mvwprintw(toiletWindow, 2+i, 30, "%i%%", clients[queueOneStatus[i] - 1]->progressToilet);
                                 } else {
                                         wmove(queueOneWindow, -2+i, 0);
                                         wclrtoeol(queueOneWindow);
                                         wattron(queueOneWindow, COLOR_PAIR(1));
                                         mvwprintw(queueOneWindow, -2+i, 0, "CLIENT %d", queueOneStatus[i]);

                                 }

			}
			wrefresh(toiletWindow);
			wrefresh(queueOneWindow);
			changeScreenMutex.unlock();
		}

		for(int i = 0; i < QUEUECAPACITY; i++){
                         changeScreenMutex.lock();
                         if(queueTwoStatus[i] == -1){
                                 if(i<TOILETCAPACITY){
                                         wmove(toiletWindow, 5+i, 12);
                                         wclrtoeol(toiletWindow);
                                         wattron(toiletWindow, COLOR_PAIR(1));
                                         mvwprintw(toiletWindow, 5+i, 12, "empty");
                                 } else {
                                         wmove(queueTwoWindow, -2+i, 11);
                                         wclrtoeol(queueTwoWindow);
                                         wattron(queueTwoWindow, COLOR_PAIR(1));
                                         mvwprintw(queueTwoWindow, -2+i, 11, "empty");

                                 }
                         } else {
                                 if(i<TOILETCAPACITY){
                                          wmove(toiletWindow, 5+i, 12);
                                          wclrtoeol(toiletWindow);
                                          wattron(toiletWindow, COLOR_PAIR(1));
                                          mvwprintw(toiletWindow, 5+i, 12, "CLIENT %d", queueTwoStatus[i]);
                                          mvwprintw(toiletWindow, 5+i, 30, "%i%%", clients[queueTwoStatus[i] - 1]->progressToilet);
                                  } else {
                                          wmove(queueTwoWindow, -2+i, 11);
                                          wclrtoeol(queueTwoWindow);
                                          wattron(queueTwoWindow, COLOR_PAIR(1));
                                          mvwprintw(queueTwoWindow, -2+i, 11, "CLIENT %d", queueTwoStatus[i]);

                                  }

                         }
                         wrefresh(toiletWindow);
			 wrefresh(queueTwoWindow);
                         changeScreenMutex.unlock();
                 }

		for(int i=0; i< NUMOFGLASSES; i++){
                        changeScreenMutex.lock();
                        if(glasses[i].state == 2){
                                wmove(glassesWindow, 0+i, 0);
                                wclrtoeol(glassesWindow);
                                wattron(glassesWindow, COLOR_PAIR(1));
                                mvwprintw(glassesWindow, 0+i, 0, "GLASS %d is used by BARMAN", glasses[i].glassId);
			}
			if(glasses[i].state == 1){
                                wmove(glassesWindow, 0+i, 0);
                                wclrtoeol(glassesWindow);
                                wattron(glassesWindow, COLOR_PAIR(1));
                                mvwprintw(glassesWindow, 0+i, 0, "GLASS %d is dirty", glasses[i].glassId);
                        }
			if(glasses[i].state == 0){
                                wmove(glassesWindow, 0+i, 0);
                                wclrtoeol(glassesWindow);
                                wattron(glassesWindow, COLOR_PAIR(1));
                                mvwprintw(glassesWindow, 0+i, 0, "GLASS %d is clear", glasses[i].glassId);
                        }
			if(glasses[i].state == 3){
                                wmove(glassesWindow, 0+i, 0);
                                wclrtoeol(glassesWindow);
                                wattron(glassesWindow, COLOR_PAIR(1));
                                mvwprintw(glassesWindow, 0+i, 0, "GLASS %d is being cleaned by WAITER %d", glasses[i].glassId, glasses[i].cleaner);
                        }

			wrefresh(glassesWindow);
			changeScreenMutex.unlock();
		}

		for(int i=0; i<NUMOFKNIVES; i++){
			changeScreenMutex.lock();
			if(knives[i].state == 0){
				wmove(suppliesWindow, 0+i, 0);
				wclrtoeol(suppliesWindow);
				wattron(suppliesWindow, COLOR_PAIR(1));
				mvwprintw(suppliesWindow, 0+i, 0, "KNIFE is free");
			}
			if(knives[i].state == 1){
                                wmove(suppliesWindow, 0+i, 0);
                                wclrtoeol(suppliesWindow);
                                wattron(suppliesWindow, COLOR_PAIR(1));
                                mvwprintw(suppliesWindow, 0+i, 0, "KNIFE is used by COOK %d", knives[i].owner);
                        }
			wrefresh(suppliesWindow);
			changeScreenMutex.unlock();
		}

		changeScreenMutex.lock();
		wmove(suppliesWindow, 1, 30);
                wclrtoeol(suppliesWindow);
                wattron(suppliesWindow, COLOR_PAIR(1));
                mvwprintw(suppliesWindow, 1, 30, "%i", supplies);
		changeScreenMutex. unlock();

		changeScreenMutex.lock();
                wmove(suppliesWindow, 2, 30);
                wclrtoeol(suppliesWindow);
                wattron(suppliesWindow, COLOR_PAIR(1));
                mvwprintw(suppliesWindow, 2, 30, "%i", money);
                changeScreenMutex. unlock();

			

	}

}



void run (WINDOW *counterWindow, WINDOW *tablesWindow, WINDOW *kitchenWindow){

	std::vector<std::thread> v;

	for(auto& barman : barmans){
		v.push_back(std::thread(runBarman, &(*barman), counterWindow));
	}

	for(auto& waiter : waiters){
		v.push_back(std::thread(runWaiter, &(*waiter), tablesWindow));
	}

	for(auto& cook : cooks){
		v.push_back(std::thread(runCook, &(*cook), kitchenWindow));
	}

	 for(auto& supplier : suppliers){
                v.push_back(std::thread(runSupplier, &(*supplier), kitchenWindow));
        }


	while(!end){
		int c = getch();

		if(c == 'q'){
			for(auto& barman : barmans){
				barman->exitBarman = true;
			}
			for(auto& waiter : waiters){
				waiter->exitWaiter = true;
			}
			for(auto& client : clients){
				client->exitClient = true;
			}
			for(auto& cook : cooks){
				cook->exitCook = true;
			}
			for(auto& supplier : suppliers){
				supplier->exitSupplier = true;
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


	money = 2;
	supplies = 6;

    counterStatus.resize(COUNTERCAPACITY + 1);
    for(int i = 0; i < counterStatus.size(); i++){
        counterStatus[i] = -1;
    }

    restaurantStatus.resize(RESTAURANTCAPACITY);
    for(int i = 0; i < restaurantStatus.size(); i++){
        restaurantStatus[i] = -1;
    }

    queueOneStatus.resize(QUEUECAPACITY);
    for(int i = 0; i < queueOneStatus.size(); i++){
	    queueOneStatus[i] = -1;
    }

    queueTwoStatus.resize(QUEUECAPACITY);
    for(int i = 0; i < queueTwoStatus.size(); i++){
            queueTwoStatus[i] = -1;
    }



    amIFull.resize(NUMOFWAITERS);
    for(int i = 0; i < amIFull.size(); i++){
        amIFull[i] = false;
    }



    for(int i = 0; i < NUMOFGLASSES; i++){
        glasses.at(i).glassId = i+1;
    }

    for(int i = 0; i < NUMOFTOILETS; i++){
	    toilets.at(i).toiletId = i+1;
    }

    for(int i = 0; i < NUMOFKNIVES; i++){
	    knives.at(i).knifeId = i+1;
    }

    Barman *b = new Barman();
    barmans.push_back(b);

    Supplier *s = new Supplier();
    suppliers.push_back(s);


    for(auto i=0; i<NUMOFCOOKS; i++){
	    Cook *coo = new Cook(i, *coo);
	    cooks.push_back(coo);
    }

   for (auto i=0; i<NUMOFWAITERS; i++){
        Waiter *w = new Waiter(i, *b);
        waiters.push_back(w);
    }


    for (auto i=0; i<NUMOFCLIENTS; i++){
        Client *c = new Client(i + 1, waiters, toilets);
        clients.push_back(c);
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
    init_pair(3, COLOR_YELLOW, COLOR_BLUE);
    init_pair(4, COLOR_CYAN, COLOR_BLUE);

    getmaxyx(stdscr, h, w);
    x = w/2;

    changeScreenMutex.lock();

    wbkgd(stdscr, COLOR_PAIR(1));

    WINDOW * counterWindow = newwin(5, x+1, 3, 0);
    wbkgd(counterWindow, COLOR_PAIR(1));
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
    refresh();
    wrefresh(glassesWindow);

    WINDOW * suppliesWindow = newwin(4, x, 20, x+2);
    wbkgd(suppliesWindow, COLOR_PAIR(1));
    refresh();
    wrefresh(suppliesWindow);

    WINDOW * queueOneWindow = newwin(3, 9, 16, x+13);
    wbkgd(queueOneWindow, COLOR_PAIR(1));
    refresh();
    wrefresh(queueOneWindow);   

    WINDOW * queueTwoWindow = newwin(3, 19, 16, x+20);
    wbkgd(queueTwoWindow, COLOR_PAIR(1));
    refresh();
    wrefresh(queueTwoWindow);


    move(0, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("---------------------------------- RESTAURANT ----------------------------------");


    move(2, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("############### COUNTER ############################## KITCHEN #################");
    attroff(COLOR_PAIR(2));

    wattron(counterWindow,COLOR_PAIR(3));
    mvwprintw(counterWindow, 2, 0, "Seat[1]");

    wattron(counterWindow,COLOR_PAIR(3));
    mvwprintw(counterWindow, 3, 0, "Seat[2]");

    wattron(counterWindow,COLOR_PAIR(3));
    mvwprintw(counterWindow, 4, 0, "Seat[3]");
    wrefresh(counterWindow);
    wattroff(counterWindow,COLOR_PAIR(3));

    move(8, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("################ TABLES ############################### TOILETS ################");
    attroff(COLOR_PAIR(2));

    wattron(tablesWindow,COLOR_PAIR(3));
    mvwprintw(tablesWindow, 5, 0, "Table[1]");

    wattron(tablesWindow,COLOR_PAIR(3));
    mvwprintw(tablesWindow, 6, 0, "Table[2]");

    wattron(tablesWindow,COLOR_PAIR(3));
    mvwprintw(tablesWindow, 7, 0, "Table[3]");

    wattron(tablesWindow,COLOR_PAIR(3));
    mvwprintw(tablesWindow, 8, 0, "Table[4]");

    wattron(tablesWindow,COLOR_PAIR(3));
    mvwprintw(tablesWindow, 9, 0, "Table[5]");
    wrefresh(tablesWindow);
    wattroff(tablesWindow,COLOR_PAIR(3));

    wattron(toiletWindow, COLOR_PAIR(1));
    mvwprintw(toiletWindow, 1, 5, "TOILET 1");
    mvwprintw(toiletWindow, 4, 5, "TOILET 2");
    wattroff(toiletWindow, COLOR_PAIR(1));

    wattron(toiletWindow, COLOR_PAIR(4));
    mvwprintw(toiletWindow, 5, 1, "Cabin[1]");
    mvwprintw(toiletWindow, 6, 1, "Cabin[2]");
    mvwprintw(toiletWindow, 8, 21, "Queue[1]");
    mvwprintw(toiletWindow, 9, 21, "Queue[2]");
    mvwprintw(toiletWindow, 10, 21, "Queue[3]");

    wattroff(toiletWindow, COLOR_PAIR(4));

    wattron(toiletWindow, COLOR_PAIR(3));
    mvwprintw(toiletWindow, 2, 1, "Cabin[1]");
    mvwprintw(toiletWindow, 3, 1, "Cabin[2]");
    mvwprintw(toiletWindow, 8, 1, "Queue[1]");
    mvwprintw(toiletWindow, 9, 1, "Queue[2]");
    mvwprintw(toiletWindow, 10, 1, "Queue[3]");

    wrefresh(toiletWindow);
    wattroff(toiletWindow, COLOR_PAIR(3));

    move(19, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("#################################### RESOURCES #################################");
    attroff(COLOR_PAIR(2));

	
    wattron(glassesWindow,COLOR_PAIR(1));
    mvwprintw(glassesWindow, 0, 0, "GLASS 1 is clear");

    wattron(glassesWindow,COLOR_PAIR(1));
    mvwprintw(glassesWindow, 1, 0, "GLASS 2 is clear");

    wattron(glassesWindow,COLOR_PAIR(1));
    mvwprintw(glassesWindow, 2, 0, "GLASS 3 is clear");
    wrefresh(glassesWindow);
    wattroff(glassesWindow,COLOR_PAIR(1));

    wattron(suppliesWindow,COLOR_PAIR(1));
    mvwprintw(suppliesWindow, 0, 0, "KNIFE is free");

    wattron(suppliesWindow,COLOR_PAIR(1));
    mvwprintw(suppliesWindow, 1, 0, "SUPPLIES in the pantry:");

    wattron(suppliesWindow,COLOR_PAIR(1));
    mvwprintw(suppliesWindow, 2, 0, "MONEY in the cashbox:");
    wrefresh(suppliesWindow);
    wattroff(suppliesWindow,COLOR_PAIR(1));


    changeScreenMutex.unlock();


    std::thread create(run, counterWindow, tablesWindow, kitchenWindow);
    std::thread seats(runSeats, counterWindow, tablesWindow, toiletWindow, queueOneWindow, queueTwoWindow, glassesWindow, suppliesWindow);

    create.join();
    seats.join();

    endwin();

    return 0;
}
