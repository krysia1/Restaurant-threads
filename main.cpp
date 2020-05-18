//Autor: Krystyna Łosieczka
//Temat: Restauracja


#include <array>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
//#include <vector>
//#include <iomanip>
//#include <memory>

#include "main.h"

#include <ncurses.h>


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
                        pickedGlasses++;
                        glasses[i].glassMutex.unlock();


                    }
                }
            }
        }

    };



    void checkGlasses(){

        if(dirtyGlasses > 1){
            changeStatusMutex.lock();
            askToClean = true;
            changeStatusMutex.unlock();

//            changeStatusMutex.lock();
//            state = 1; //stan czekania na wyczyszczenie szklanek
//            move(3, 0);
//            clrtoeol();
//            printw("BARMAN is waiting for GLASSES");
//            refresh();
//            changeStatusMutex.unlock();

            std::cout<<"Barman asked to clean glasses"<<std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    };



    void serveDrunkard(){

        tryGlasses();

        if(pickedGlasses < 2){      //jeśli udało się wziąć tylko jedną szklankę to ją odkłada
            for(int i=0; i<NUMOFGLASSES; i++) {
                if (glasses[i].taken){
                    glasses[i].glassMutex.lock();
                    glasses[i].taken = false;


//		    changeStatusMutex.lock();
//                    move(19 + glasses[i].glassId, 0);
//                    clrtoeol();
//                    printw("GLASS %d is put back by BARMAN", glasses[i].glassId);
//                    refresh();
//                    changeStatusMutex.unlock();


                    glasses[i].state = 0; //stan na czysty
                    glasses[i].glassMutex.unlock();
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));

        } else {
            for(int i=0; i<NUMOFGLASSES; i++) {
                if (glasses[i].taken){
                    glasses[i].glassMutex.lock();
                    glasses[i].dirty = true;
                    glasses[i].glassMutex.unlock();

//		    changeStatusMutex.lock();
//		    glasses[i].dirty = true;
//        	    move(19 + glasses[i].glassId, 0);
//        	    clrtoeol();
//                    printw("GLASS %d is used by BARMAN", glasses[i].glassId);
//                    refresh();
//                    changeStatusMutex.unlock();

                }
            }
            changeStatusMutex.lock();
            state = 3; //stan na obsługę pijaka
            changeStatusMutex.unlock();

            std::cout<<"Barman is serving"<<std::endl;

            int part = std::uniform_int_distribution<int>(25, 35)(rng);
            for (auto i = 1; i <= part; i++) {
                double p = (double) i / (double) part;
                progress = (int) std::round(p * 100.0);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

//                changeStatusMutex.lock();
//                move(3, 0);
//                clrtoeol();
//                printw("BARMAN is serving\t\t%i\t%%", progress);
//                refresh();
//                changeStatusMutex.unlock();
            }



            for(int i=0; i<NUMOFGLASSES; i++){
                if (glasses[i].dirty){
                    glasses[i].glassMutex.lock();
                    glasses[i].state = 1; //stan na brudny

//		    changeStatusMutex.lock();
//                    move(19 + glasses[i].glassId, 0);
//                    clrtoeol();
//                    printw("Glass %d is dirty", glasses[i].glassId);
//                    refresh();
//                    changeStatusMutex.unlock();

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

            servedDrunkard.unlock();
            counterQueue[0].unlock();

        }

        checkGlasses();

    };



    void rest(){

        changeStatusMutex.lock();
        state = 2; //stan na odpoczywanie
        changeStatusMutex.unlock();

        std::cout<<"Barman is resting"<<std::endl;

        int part = std::uniform_int_distribution<int>(25,35)(rng);

        for(auto i=1; i<=part; i++){
            double p = (double)i / (double)part;
            progress = (int)std::round(p * 100.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

//            changeStatusMutex.lock();
//            move(3, 0);
//            clrtoeol();
//            printw("BARMAN is resting\t\t%i\t%%", progress);
//            refresh();
//            changeStatusMutex.unlock();
        }
    };



    void checkCounter(){

        while(!exitBarman) {

            if (isCounterTaken) {
                serveDrunkard();
            } else {
                rest();
            }

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

    std::mutex serveMutex;
    bool serveClient = false;
    std::thread waiterThread;
    int state = 0;  //na początku stan na czekanie


    Waiter(int const waiterId, Barman &barman)
        : waiterId(waiterId), barman(barman), waiterThread(&Waiter::work,this){}



    void clean(){
        std::cout << "Waiter "<<waiterId<<" is cleaning"<< std::endl;

        changeStatusMutex.lock();
        state = 1; //stan na czyszczenie
        changeStatusMutex.unlock();

        for(int i=0; i<NUMOFGLASSES; i++){
            if (barman.glasses[i].dirty){
                barman.glasses[i].glassMutex.lock();
                barman.glasses[i].state = 3; //stan na czyszczony

//		changeStatusMutex.lock();
//                move(19 + barman.glasses[i].glassId, 0);
//                clrtoeol();
//                printw("GLASS %d is being cleaned by WAITER %d", barman.glasses[i].glassId, waiterId+1);
//                refresh();
//                changeStatusMutex.unlock();


                int part = std::uniform_int_distribution<int>(25, 35)(rng);

                for (auto j = 1; j <= part; j++) {
                    double p = (double) j / (double) part;
                    progress = (int) std::round(p * 100.0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));

//                    changeStatusMutex.lock();
//                    move(9 + waiterId, 0);
//                    clrtoeol();
//                    printw("WAITER %d is cleaning GLASS %d \t\t%i\t%%", waiterId+1, barman.glasses[i].glassId, progress);
//                    refresh();
//                    changeStatusMutex.unlock();
                }

                barman.glasses[i].state = 0; //stan na czysty

//		changeStatusMutex.lock();
//                move(19 + barman.glasses[i].glassId, 0);
//                clrtoeol();
//                printw("Glass %d is clear", barman.glasses[i].glassId);
//                refresh();
//                changeStatusMutex.unlock();


                barman.glasses[i].dirty = false;
                barman.glasses[i].glassMutex.unlock();

                std::cout << "Waiter "<<waiterId<<" cleaned"<< std::endl;

            }
        }

        changeStatusMutex.lock();
        barman.askToClean = false;
        barman.dirtyGlasses = 0;
        changeStatusMutex.unlock();

    };




    void serve(){
        std::cout << "Waiter "<<waiterId<<" is ready to serve"<< std::endl;

//        changeStatusMutex.lock();
//        move(9 + waiterId, 0);
//        clrtoeol();
//        printw("WAITER %d is ready to serve", waiterId+1);
//        refresh();
//        changeStatusMutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        changeStatusMutex.lock();
        state = 0; //stan na czekanie
        changeStatusMutex.unlock();

        if(serveClient){
 //           servedClient = restaurantStatus[waiterId];

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
//                printw("WAITER %d is serving\t\t%i\t%%", waiterId+1,  progress);
//                refresh();
//                changeStatusMutex.unlock();
            }


            changeStatusMutex.lock();
            amIFull[waiterId] = true;
            serveMutex.unlock();
            serveClient = false;
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



    Client(int const clientId, std::vector<Waiter *> &waiters)
    : clientId(clientId), waiters(waiters), clientThread(&Client::visitRestaurant, this){}




    void takeASeat(){
        std::cout<<clientId<<"Client"<<std::endl;

        int pointer = RESTAURANTCAPACITY - 1;

//        for (int i = 0; i < NUMOFWAITERS; i++) {
//            std::cout<<waiters[i]->serveClient<<std::endl;
//        }

        while(!exitClient) {

            if (restaurantQueue[pointer].try_lock()) {
                changeStatusMutex.lock();
                restaurantStatus[pointer + 1] = clientId;
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

                    std::cout<<clientId<<"Client is waiting"<<std::endl;


                    changeStatusMutex.lock();
                    restaurantStatus[pointer + 1] = -1;
                    restaurantStatus[pointer] = clientId;
                    changeStatusMutex.unlock();

                    restaurantQueue[pointer].unlock();
                    pointer--;

                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }

            } else {
                for (int i = 0; i < NUMOFWAITERS; i++) {
                    if (waiters[i]->serveMutex.try_lock()) {
                        std::cout << clientId << "Served Client" << std::endl;


                        changeStatusMutex.lock();
                        restaurantStatus[i] = clientId;
                        waiters[i]->serveClient = true;
                        changeStatusMutex.unlock();

                        while (!amIFull[i]) {
                            std::this_thread::sleep_for((std::chrono::milliseconds(100)));
                        }

                        changeStatusMutex.lock();
                        amIFull[i] = false;
                        restaurantStatus[pointer] = -1;
                        restaurantQueue[pointer - 1].unlock();
                        changeStatusMutex.unlock();
                        break;

                    }
                }

            }

        }



    };




    void useToilet(){};






    void seatByTheCounter(){
        std::cout<<clientId<<"Drunkard"<<std::endl;

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

                    std::cout<<clientId<<"Drunkard waits in the queue"<<std::endl;

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

                    std::cout<<clientId<<"Served Drunkard"<<std::endl;

                    changeStatusMutex.lock();
                    counterStatus[0] = clientId;
                    isCounterTaken = true;
                    changeStatusMutex.unlock();

                    while(!amIDrunk){
                        std::this_thread::sleep_for((std::chrono::milliseconds(100)));
                    }
                    changeStatusMutex.lock();
                    amIDrunk = false;
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

        std::cout<<clientId<<"Bye"<<std::endl;
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











/////////////////////////////////// MAIN ///////////////////////////////////////////////////////////////////////////////

int main() {

    std::vector<Client *> clients;
    std::vector<Waiter *> waiters;
    std::array<Glass, NUMOFGLASSES> glasses;


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

    for (auto i=0; i<NUMOFWAITERS; i++){
        Waiter *w = new Waiter(i, *b);
        waiters.push_back(w);
    }


    for (auto i=0; i<NUMOFCLIENTS; i++){
        Client *c = new Client(i + 1, waiters);
        clients.push_back(c);
    }

    
//    initscr();
//    raw();
////    noecho;
////    nodelay(stdscr, TRUE);
////    cbreak;
//    curs_set(0);
//    use_default_colors();
//
//    start_color();
//    init_pair(1, COLOR_WHITE, COLOR_BLUE);
//    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
//
//    move(0, 0);
//    clrtoeol();
//    attron(COLOR_PAIR(2));
//    printw("---------------------- RESTAURANT -----------------------");
//
//    move(2, 0);
//    clrtoeol();
//    attron(COLOR_PAIR(2));
//    printw("####################### COUNTER #########################");
//
//    move(8, 0);
//    clrtoeol();
//    attron(COLOR_PAIR(2));
//    printw("####################### TABLES ###########################");
//
//    move(19, 0);
//    clrtoeol();
//    attron(COLOR_PAIR(2));
//    printw("###################### RESOURCES #########################");
//    attroff(COLOR_PAIR(2));
//
//    attron(COLOR_PAIR(1));
//
//    move(20, 0);
//    clrtoeol();
//    printw("GLASS 1 is clear");
//
//    move(21, 0);
//    clrtoeol();
//    printw("GLASS 2 is clear");
//
//    move(22, 0);
//    clrtoeol();
//    printw("GLASS 3 is clear");
//
//
//
//    refresh();
//    getch();
//    endwin();

    b->barmanThread.join();

    waiters[0]->waiterThread.join();
    waiters[1]->waiterThread.join();
    waiters[2]->waiterThread.join();

    clients[0]->clientThread.join();
    clients[1]->clientThread.join();
    clients[2]->clientThread.join();
    clients[3]->clientThread.join();
    clients[4]->clientThread.join();
    clients[5]->clientThread.join();
    clients[6]->clientThread.join();
    clients[7]->clientThread.join();
    clients[8]->clientThread.join();
    clients[9]->clientThread.join();




//    std::this_thread::sleep_for(std::chrono::seconds(1));


    return 0;
}
