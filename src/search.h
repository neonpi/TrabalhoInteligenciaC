//
// Created by user on 25/11/2025.
//

#ifndef TRABALHO_INTELIGENCIA_COMPUTACIONAL_SEARCH_H
#define TRABALHO_INTELIGENCIA_COMPUTACIONAL_SEARCH_H

#endif //TRABALHO_INTELIGENCIA_COMPUTACIONAL_SEARCH_H

class Search {
public:
    Search(Instance* instance);
    ~Search();
    void run();

    //Tem que mudar isso aqui, é só pra ter uma base
    void rvnd_intra();
    void ls_intra_exchange(bool *improved);
    void ls_intra_2opt(bool *improved);
    void ls_intra_or_opt_1(bool *improved);
    void ls_intra_or_opt_k(int k, bool *improved);

};