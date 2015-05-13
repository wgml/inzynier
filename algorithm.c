//
// Created by vka on 26.04.15.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "algorithm.h"

vehicle_class algorithm2(data_vector_t *vector) {

    unsigned OFFSET_NUM = 50;

    remove_offset(vector, OFFSET_NUM);


    return INVALID;
}

void remove_offset(data_vector_t *vector, unsigned num) {
    if (num == 0) {
        fputs("Ilość próbek do usuwania offsetu musi być większa od 0!\n",
              stderr);
        exit(EINVAL);
    }

    if (num > vector->length) {
        fputs("Ilość próbek do usuwania offsetu musi być mniejsza od długości wektora!\n",
              stderr);
        exit(EINVAL);
    }

    /*
     * deklaracja tablicy zawierającej offsety dla poszczególnych wektorów
     */
    double offsets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    data_node_t *n = vector->head;
    for (unsigned i = 0; i < num; i++) {
        for (unsigned j = 1; j < 13; j++) {
            offsets[j - 1] += n->cell->data[j];
        }
        n = n->next;
    }

    //wyznaczenie ostatenczej wartości offsetu dla każdego wektora
    for (int i = 0; i < 13; i++) offsets[i] /= num;

    //przesunięcie wszystkich wartości w wektorze o zadany offset
    n = vector->head;
    for (unsigned i = 0; i < vector->length; i++) {
        for (unsigned j = 1; j < 13; j++) {
            n->cell->data[j] -= offsets[j - 1];
        }
        n = n->next;
    }
}

void find_velocity_distance(data_vector_t *vector, double *v, double *d) {
    /*
     * Funkcja wykorzystuje wartości z czujników P1 i P2 wektora danych,
     * a także wektor czasu.
     * parametry v i d odpowiadają za wskaźniki do pamięci, gdzie
     * należy wpisać parametry ruchu
     *
     * Prędkość wyznaczana jest w jednostce m/s.
    */

    /*
     * Definiowanie parametrów
     */

    //zdefiniowanie progów histerezy [V]
    double p1, p2;
    p1 = 3;
    p2 = 2;

    //odległośc pomiędzy czujnikami
    double dist = 1;

    //zmienne
    bool is_impulse1 = false;
    bool is_impulse2 = false;

    unsigned num_impulse1 = 0;
    unsigned num_impulse2 = 0;

    //parametry wykorzystywane do wyliczenia odległości i prędkości
    int index1[2] = {0, 0};
    int index2[2] = {0, 0};

    data_node_t *n = vector->head;
    for (unsigned i = 0; i < vector->length; i++) {

        //piezo 1
        if (n->cell->data[DATA_P1] >= p1 && is_impulse1 == false) {
            is_impulse1 = true;
            num_impulse1 += 1;
            if (num_impulse1 <= 2) index1[num_impulse1 - 1] = i;
        }
        else if (n->cell->data[DATA_P1] < p2 && is_impulse1 == true) {
            is_impulse1 = false;
        }

        //piezo2
        if (n->cell->data[DATA_P2] >= p1 && is_impulse2 == false) {
            is_impulse2 = true;
            num_impulse2 += 1;
            if (num_impulse2 <= 2) index2[num_impulse2 - 1] = i;
        }
        else if (n->cell->data[DATA_P2] < p2 && is_impulse2 == true) {
            is_impulse2 = false;
        }
        n = n->next;
    }

    double t1 = 0; //czas do wyznaczania prędkości
    double t2 = 0; //czas do wyznaczania odległości

    t1 = (index2[0] - index1[0] + index2[1] - index1[1]) / 2;
    t2 = (index1[1] - index1[0] + index2[1] - index2[0]) / 2;

    //konwersja z ms na s
    t1 /= 1000;
    t2 /= 1000;

    *v = dist / t1;
    *d = (*v) * t2;
}