#include <ctime>
#include "Juego.h"

int main()
{
    std::srand((unsigned)std::time(nullptr));

    Juego juego;
    juego.ejecutar();

    return 0;
}
