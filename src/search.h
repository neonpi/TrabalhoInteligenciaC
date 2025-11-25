//
// Created by user on 25/11/2025.
//

#ifndef TRABALHO_INTELIGENCIA_COMPUTACIONAL_SEARCH_H
#define TRABALHO_INTELIGENCIA_COMPUTACIONAL_SEARCH_H

#endif //TRABALHO_INTELIGENCIA_COMPUTACIONAL_SEARCH_H

//Construtor
Search::Search(Instance *instance) {
    this->instance = instance;
    this->best_solution = nullptr;

    this->solution = new Solution(instance);
    //this->virtual_sequence = new Sequence();

}

//Destrutor
Search::~Search() {

    if (this->best_solution != nullptr) {
        delete this->best_solution;
    }

    //delete this->virtual_sequence;
}

//Chama o iterated greedy
void Search::run() {
    this->construct();
    //this->rvnd_inter();
    this->iterated_greedy();

}

//Chama as buscas locais
//TODO estruturar ssas buscas locais e ver se vai diferenciar intra e inter
void Search::rvnd_intra() {

    vector<int> neighb = {0,1,2,3,4};
    int last_improved_neighb = -1;
    random_shuffle(neighb.begin(),neighb.end());
    bool improved = false;
    for(int i=0;i<5;i++) {
        if(neighb[i] != last_improved_neighb) {
            switch (neighb[i]) {
                case 0:
                    this->ls_intra_2opt(&improved);
                    break;
                case 1:
                    this->ls_intra_exchange(&improved);
                    break;
                case 2:
                    this->ls_intra_or_opt_1(&improved);
                    break;
                case 3:
                    this->ls_intra_or_opt_k(2, &improved);
                    break;
                case 4:
                    this->ls_intra_or_opt_k(3, &improved);
                    break;
                default:
                    cout<<"Unknown LS"<<endl;
            }
            if(improved) {
                last_improved_neighb = neighb[i];
                i=-1;
                improved = false;
            }
        }
    }
}