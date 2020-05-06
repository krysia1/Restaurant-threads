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
            if(glasses[i].glassMutex.try_lock()){
                if(!glasses[i].dirty) {
                    if(!glasses[i].taken) {
                        glasses[i].taken = true;
                        glasses[i].state = 2; //stan na używany
                        pickedGlasses++;
                        for (int j = 0; j < NUMOFGLASSES; j++) {
                            if (i == j) {
                            continue;
                            }

                            if(glasses[j].glassMutex.try_lock()) {
                                if (!glasses[j].dirty) {
                                    if (!glasses[j].taken) {
                                        glasses[j].taken = true;
                                        glasses[j].state = 2; //stan na używany
                                        pickedGlasses++;
                                    } else {
                                        glasses[j].glassMutex.unlock();
                                    }
                                }
                            }

                            if (pickedGlasses == 2) {
                                break;
                            }

                        }
                    } else {
                        glasses[i].glassMutex.unlock();
                    }
                }
            }
            if(pickedGlasses == 2){
                break;
            }
        }

    };



    void checkGlasses(){

        if(dirtyGlasses > 1){
            askToClean = true;

            changeStatusMutex.lock();
            state = 1; //stan czekania na wyczyszczenie szklanek
            changeStatusMutex.unlock();

            std::cout<<"Barman is waiting for glasses"<<std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

    };



    void serveDrunkard(){

        tryGlasses();

        if(pickedGlasses < 2){      //jeśli udało się wziąć tylko jedną szklankę to ją odkłada
            for(int i=0; i<NUMOFGLASSES; i++) {
                if (glasses[i].taken){
                    glasses[i].taken = false;
                    glasses[i].state = 0; //stan na czysty
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));

        } else {
            for(int i=0; i<NUMOFGLASSES; i++) {
                if (glasses[i].taken){
                    glasses[i].dirty = true;

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

            for(int i=0; i<NUMOFGLASSES; i++){
                if (glasses[i].dirty){
                    glasses[i].state = 1; //stan na brudny
                    dirtyGlasses++;
                    glasses[i].glassMutex.unlock();
                }
            }


            changeStatusMutex.lock();
            amIDrunk = true;
            counterStatus[0] = -1;
            counterStatus[1] = -1;
            changeStatusMutex.unlock();

            pickedGlasses = 0;

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
        }
    };



    void checkCounter(){

        while(!exitBarman) {

            for (int i = 0; i < COUNTERCAPACITY; i++) {
                if (counter[i].try_lock()) {
                counter[i].unlock();
                } else {
                counterIsEmpty = false;
                }
            }


            if(counterIsEmpty){
                rest();

            } else {

                if (servedDrunkard.try_lock()) {
                    servedDrunkard.unlock();
                } else {
                    serveDrunkard();
                }
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

                int part = std::uniform_int_distribution<int>(25, 35)(rng);

                for (auto i = 1; i <= part; i++) {
                    double p = (double) i / (double) part;
                    progress = (int) std::round(p * 100.0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                barman.glasses[i].state = 0; //stan na czysty
                barman.glasses[i].dirty = false;
                barman.glasses[i].glassMutex.unlock();

                std::cout << "Waiter "<<waiterId<<" cleaned"<< std::endl;

            }
        }
    };




    void serve(){
        std::cout << "Waiter "<<waiterId<<" is ready to serve"<< std::endl;
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
                    changeStatusMutex.unlock();

                    while(!amIDrunk){
                        std::this_thread::sleep_for((std::chrono::milliseconds(100)));
                    }
                    amIDrunk = false;
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

    
    initscr();
    raw();
//    noecho;
//    nodelay(stdscr, TRUE);
//    cbreak;
    curs_set(0);
    use_default_colors();

    start_color();
    init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_YELLOW);

    move(0, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("------RESTAURANT------");   

    refresh();
    getch();
//    endwin();

//    b->barmanThread.join();

//    waiters[0]->waiterThread.join();
//    waiters[1]->waiterThread.join();
//    waiters[2]->waiterThread.join();

//    clients[0]->clientThread.join();
//    clients[1]->clientThread.join();
//    clients[2]->clientThread.join();
//    clients[3]->clientThread.join();
//    clients[4]->clientThread.join();
//    clients[5]->clientThread.join();
//    clients[6]->clientThread.join();
//    clients[7]->clientThread.join();
//    clients[8]->clientThread.join();
//    clients[9]->clientThread.join();




//    std::this_thread::sleep_for(std::chrono::seconds(1));


    return 0;
}
