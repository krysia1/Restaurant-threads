//Autor: Krystyna Łosieczka
//Opis: Klasa Glass reprezentująca Szklanke - zasób

#ifndef SO2P_GLASS_H
#define SO2P_GLASS_H

class Glass{
public:
    int glassId;
    bool dirty = false;

    void tryUse();
    void endUse();
};

#endif //SO2P_GLASS_H
