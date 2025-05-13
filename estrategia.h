#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include "mapa.h"
#include "nivel.h"

typedef void (*Estrategia)(Nivel*, Mapa*);

void colocacion_basica(Nivel* nivel, Mapa* mapa);

void estrategia_avanzada(Nivel* nivel, Mapa* mapa);

#endif
