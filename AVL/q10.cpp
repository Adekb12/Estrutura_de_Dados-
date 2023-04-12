#include <iostream>
#include <stdexcept>

using namespace std;

struct dado {
    unsigned chave;
    string modelo;
    string marca;
    short anoFabricacao;
};

ostream& operator<<(ostream& saida, const dado& e) {
    saida << "(" << e.chave << "," << e.modelo << "," << e.marca << "," << e.anoFabricacao << ")";
    return saida;
}

istream& operator>>(istream& entrada, dado& e) {
    entrada >> e.chave >> e.modelo >> e.marca >> e.anoFabricacao;
    return entrada;
}

typedef unsigned tipoChave; // tipo da chave usada na comparação

class noh {
    friend class avl;
    private:
        dado elemento;
        noh* esq;
        noh* dir;
        unsigned altura;
    public:
        noh(const dado& umDado):
            elemento(umDado),  esq(NULL), dir(NULL), altura(1) { }
        ~noh() { }
        int fatorBalanceamento();
        void atualizarAltura(noh* umNoh);
        unsigned maximo(unsigned altEsq,unsigned altDir);
        unsigned informarAltura(noh* umNoh);
};

int noh::fatorBalanceamento() {
    int altEsq = informarAltura(this->esq);
    int altDir = informarAltura(this->dir);
    int fatorBal = altEsq - altDir;
    return fatorBal;
}

void noh::atualizarAltura(noh* umNoh){
    unsigned altEsq = informarAltura(umNoh->esq);
    unsigned altDir = informarAltura(umNoh->dir);
    umNoh->altura = 1 + maximo(altEsq,altDir);
}

unsigned noh::informarAltura(noh* umNoh){
    if(umNoh == NULL)
        return 0;
    else   
        return umNoh->altura;
}

unsigned noh::maximo(unsigned altEsq,unsigned altDir){
    if(altEsq > altDir)
        return altEsq;
    else   
        return altDir;
}

class avl {
    friend ostream& operator<<(ostream& output, avl& arvore);
    private:
        noh* raiz;
        // percorrimento em ordem da árvore
        void percorreEmOrdemAux(noh* atual, int nivel);
        // funções auxiliares para remoção
        noh* encontraMenor(noh* raizSub);
        noh* removeMenor(noh* raizSub);
        // funções auxiliares para inserção e remoção usando método recursivo
        // retorna o nó para ajustar o pai ou o raiz
        noh* insereAux(noh* umNoh, const dado& umDado);
        noh* removeAux(noh* umNoh, tipoChave chave);
        // métodos para manutenção do balanceamento
        noh* rotacaoEsquerda(noh* umNoh);
        noh* rotacaoDireita(noh* umNoh);
        noh* rotacaoEsquerdaDireita(noh* umNoh);
        noh* rotacaoDireitaEsquerda(noh* umNoh);
        noh* arrumaBalanceamento(noh* umNoh);
        // busca, método iterativo
        noh* buscaAux(tipoChave chave);
        // função auxiliar do destrutor, usa percorrimento pós-ordem
        void destruirRecursivamente(noh* umNoh);
        void imprimirDir(const std::string& prefixo, const noh* meuNoh);
        void imprimirEsq(const std::string& prefixo, const noh* meuNoh, bool temIrmao);
        void levantamentoAux(noh* umNoh, string marca, short ano,int &cont);
    public:
        avl() { raiz = NULL; }
        ~avl();
        void imprimir();
        // inserção e remoção são recursivos
        void insere(const dado& umDado);
        void remove(tipoChave chave);
        // inserção e remoção, métodos recursivos
        // busca retorna uma cópia do objeto armazenado
        dado busca(tipoChave chave);
        // efetua levantamento de quantas motos existem de uma dada marca 
        // fabricadas a partir de um dado ano
        int levantamento(string marca, short ano);
};

// destrutor
avl::~avl() {
    destruirRecursivamente(raiz);
}

// destrutor é recursivo, fazendo percorrimento pós-ordem
void avl::destruirRecursivamente(noh* umNoh) {
    if(umNoh != NULL){
        destruirRecursivamente(umNoh->esq);
        destruirRecursivamente(umNoh->dir);
        delete umNoh;
    }
}

void avl::insere(const dado& umDado) {
    raiz = insereAux(raiz, umDado);
}

// inserção recursiva, devolve nó para atribuição de pai ou raiz
noh* avl::insereAux(noh* umNoh, const dado& umDado) {
    if(umNoh == NULL){
        noh* novo = new noh(umDado);
        return novo;
    }else{
        if(umDado.chave < umNoh->elemento.chave){
            umNoh->esq = insereAux(umNoh->esq,umDado);
        }else{
            umNoh->dir = insereAux(umNoh->dir,umDado);
        }
    }
    return arrumaBalanceamento(umNoh);
}

// checa e arruma, se necessário, o balanceamento em umNoh,
// fazendo as rotações e ajustes necessários
noh* avl::arrumaBalanceamento(noh* umNoh) {
    if(umNoh == NULL)
        return umNoh;
    umNoh->atualizarAltura(umNoh);
    int fatorBal = umNoh->fatorBalanceamento();
    if((fatorBal >= -1) and (fatorBal <= 1))
        return umNoh;
    if((fatorBal > 1) and (umNoh->esq->fatorBalanceamento() >= 0))
        return rotacaoDireita(umNoh);
    if((fatorBal > 1) and (umNoh->esq->fatorBalanceamento()) < 0){
        umNoh->esq = rotacaoEsquerda(umNoh->esq);
        return rotacaoDireita(umNoh);
    }
    if((fatorBal < -1) and (umNoh->dir->fatorBalanceamento() <= 0))
        return rotacaoEsquerda(umNoh);
    if((fatorBal < -1) and (umNoh->dir->fatorBalanceamento()) > 0){
        umNoh->dir = rotacaoDireita(umNoh->dir);
        return rotacaoEsquerda(umNoh);
    }
    return umNoh;
}

// rotação à esquerda na subárvore com raiz em umNoh
// retorna o novo pai da subárvore
noh* avl::rotacaoEsquerda(noh* umNoh) {
    noh* nohAux = umNoh->dir;
    umNoh->dir = nohAux->esq;
    nohAux->esq = umNoh;
    umNoh->atualizarAltura(umNoh);
    nohAux->atualizarAltura(nohAux);
    return nohAux;
}


// rotação à direita na subárvore com raiz em umNoh
// retorna o novo pai da subárvore
noh* avl::rotacaoDireita(noh* umNoh) {
    noh* nohAux = umNoh->esq;
    umNoh->esq = nohAux->dir;
    nohAux->dir = umNoh;
    umNoh->atualizarAltura(umNoh);
    nohAux->atualizarAltura(nohAux);
    return nohAux;
}

noh* avl::rotacaoEsquerdaDireita(noh* umNoh){
    umNoh->esq = rotacaoEsquerda(umNoh->esq);
    return rotacaoDireita(umNoh);
}

noh* avl::rotacaoDireitaEsquerda(noh* umNoh){
    umNoh->dir = rotacaoDireita(umNoh->dir);
    return rotacaoEsquerda(umNoh);
}

// método de busca auxiliar (retorna o nó), iterativo
noh* avl::buscaAux(tipoChave chave) {
    noh* atual = raiz;
    while(atual!=NULL){
        if(atual->elemento.chave == chave)
            return atual;
        else if(atual->elemento.chave > chave)
            atual = atual->esq;
        else 
            atual = atual->dir;
    }
    return atual;
}

// busca elemento com uma dada chave na árvore e retorna o registro completo
dado avl::busca(tipoChave chave) {
    noh* resultado = buscaAux(chave);
    if (resultado != NULL)
        return resultado->elemento;
    else
        throw runtime_error("Erro na busca: elemento não encontrado!");
}

// nó mínimo (sucessor) de subárvore com raiz em raizSub (folha mais à esquerda)
noh* avl::encontraMenor(noh* raizSub) { 
    while(raizSub->esq != NULL){
        raizSub = raizSub->esq;
    }
    return raizSub;
}

// procedimento auxiliar para remover o sucessor substituíndo-o pelo
// seu filho à direita
noh* avl::removeMenor(noh* raizSub) {
    if(raizSub->esq == NULL)
        return raizSub->dir;
    else { 
        raizSub->esq = removeMenor(raizSub->esq);
        return arrumaBalanceamento(raizSub);
    }
}

// remoção recursiva
void avl::remove(tipoChave chave) {
    raiz = removeAux(raiz, chave);
}

noh* avl::removeAux(noh* umNoh, tipoChave chave) {
    noh* novaRaizSubArvore = umNoh;
    if (chave < umNoh->elemento.chave) {
        umNoh->esq = removeAux(umNoh->esq, chave);
    } else if (chave > umNoh->elemento.chave) {
        umNoh->dir = removeAux(umNoh->dir, chave);
    } else { 
        if (umNoh->esq == NULL) 
            novaRaizSubArvore = umNoh->dir;
        else if (umNoh->dir == NULL) \
            novaRaizSubArvore = umNoh->esq;
        else {
            novaRaizSubArvore = encontraMenor(umNoh->dir);
            novaRaizSubArvore->dir = removeMenor(umNoh->dir);  
            novaRaizSubArvore->esq = umNoh->esq;
        }
        delete umNoh;
    } 
    return arrumaBalanceamento(novaRaizSubArvore);
}
// utiliza o nó atual e seu nível na árvore (para facilitar visualização)
void avl::percorreEmOrdemAux(noh* atual, int nivel) {
    if(atual != NULL){
        percorreEmOrdemAux(atual->esq,nivel);
        imprimir();
        percorreEmOrdemAux(atual->dir,nivel);
    }
}

int avl::levantamento(string marca,short ano){
    int cont = 0;
    levantamentoAux(raiz,marca,ano,cont);
    return cont;
}

void avl::levantamentoAux(noh* umNoh, string marca,short ano,int& cont){
    if(umNoh != NULL){
        if(umNoh->elemento.marca == marca and umNoh->elemento.anoFabricacao >= ano)
            cont++;
        levantamentoAux(umNoh->esq,marca,ano,cont);
        levantamentoAux(umNoh->dir,marca,ano,cont);   
    }
}

ostream& operator<<(ostream& output, avl& arvore) {
    arvore.imprimir();
    return output;
}

// imprime formatado seguindo o padrao tree as subarvores direitas de uma avl
void avl::imprimirDir(const std::string& prefixo, const noh* meuNoh)
{
    if( meuNoh != nullptr )
    {
        std::cout << prefixo
                  << "└d─"
                  << "(" << meuNoh->elemento.chave << "," << meuNoh->elemento.modelo << ")"
                  << std::endl;

        // Repassa o prefixo para manter o historico de como deve ser a formatacao e chama no filho direito e esquerdo
        imprimirEsq( prefixo + "    " , meuNoh->esq , meuNoh->dir==nullptr );
        imprimirDir( prefixo + "    " , meuNoh->dir );
    }
}

// imprime formatado seguindo o padrao tree as subarvores direitas de uma avl
void avl::imprimirEsq(const std::string& prefixo, const noh* meuNoh, bool temIrmao)
{
    if( meuNoh != nullptr )
    {
        std::cout << prefixo ;

        // A impressao da arvore esquerda depende da indicacao se existe o irmao a direita
        if (temIrmao)
            std::cout << "└e─" ;
        else
            std::cout << "├e─";

        std::cout << "(" << meuNoh->elemento.chave << "," << meuNoh->elemento.modelo << ")"
                  << std::endl;

        // Repassa o prefixo para manter o historico de como deve ser a formatacao e chama no filho direito e esquerdo
        imprimirEsq( prefixo + "│   " , meuNoh->esq, meuNoh->dir==nullptr );
        imprimirDir( prefixo + "│   " , meuNoh->dir );
    }
}

// imprime formatado seguindo o padrao tree uma avl
void avl::imprimir()
{
    if( this->raiz != nullptr )
    {
        std::cout << "(" << this->raiz->elemento.chave << "," << this->raiz->elemento.modelo << ")" << std::endl;
        // apos imprimir a raiz, chama os respectivos metodos de impressao nas subarvore esquerda e direita
        // a chamada para a impressao da subarvore esquerda depende da existencia da subarvore direita
        imprimirEsq( " " , this->raiz->esq, this->raiz->dir==nullptr );
        imprimirDir( " " , this->raiz->dir );
    } else
        std::cout << "*arvore vazia*" << std::endl;
}

int main() {
    avl arvore;
    tipoChave chave;
    dado umDado;
    string marcaParaBusca;
    short anoParaBusca;
    int quantidade;

    char operacao;

    do {
        try {
            cin >> operacao;
            switch (operacao) {
                case 'i': // Inserir recursivamente
                    // objeto recebe chave, modelo da motocicleta, marca, ano de fabricação
                    cin >> umDado;
                    arvore.insere(umDado);
                    break;
                case 'r': // Remover recursivamente
                    cin >> chave;
                    arvore.remove(chave);
                    break;
                case 'b': // Buscar
                    cin >> chave; // ler a chave
                    umDado = arvore.busca(chave); // escreve dados da motocicleta
                    cout << "Elemento buscado: " << umDado << endl;
                    break;
                case 'l': // Levantamento por marca
                    cin >> marcaParaBusca >> anoParaBusca; // ler a marca e ano
                    quantidade = arvore.levantamento(marcaParaBusca,anoParaBusca);
                    cout << "Levantamento da marca " << marcaParaBusca << ": " << quantidade << endl;
                    break;
                case 'e': // Escrever tudo (em ordem)
                    cout << arvore;
                    break;
                case 'f': // Finalizar execução
                    break;
                default:
                    cout << "Comando invalido!\n";
            }
        } catch (runtime_error& e) {
            cout << e.what() << endl;
        }
    } while (operacao != 'f');

    return 0;
}
