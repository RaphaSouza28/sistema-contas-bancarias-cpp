/*
 * ==========================================================
 *  BANCO ENLACE - Sistema de Registro e Gestao de Contas
 *  INF101 - Programacao de Computadores I - UNIVICOSA
 *  Trabalho - Etapa 3 (persistencia dos dados em arquivos)
 *  Aluno: Raphael de Souza Azevedo
 * ==========================================================
 *  Mantem tudo da Etapa 2 (arrays + historico + funcoes) e
 *  passa a GUARDAR os dados em dois arquivos de texto:
 *
 *    contas.txt        -> numero;nome;cpf;tipo;saldo;ativa
 *    movimentacoes.txt -> conta;tipo;valor
 *
 *  - Ao iniciar, carregarContas() e carregarMovimentacoes()
 *    leem os arquivos e preenchem os arrays (se os arquivos
 *    nao existirem, o sistema comeca vazio normalmente).
 *  - Toda operacao que altera dados (cadastro, deposito,
 *    saque, transferencia, ativar/desativar) chama
 *    salvarContas() e/ou salvarMovimentacoes(), entao nada
 *    se perde ao fechar o programa.
 */

#include <iostream>
#include <string>
#include <iomanip>   // fixed / setprecision / setw
#include <limits>    // numeric_limits (limpar o buffer)
#include <fstream>   // ifstream / ofstream (arquivos)
#include <sstream>   // stringstream (separar os campos por ';')

using namespace std;

// ==========================================================
//  2.1 DADOS DAS CONTAS
// ==========================================================
const int MAX_CONTAS = 100;   // limite maximo de 100 contas

int numeroConta[MAX_CONTAS];
string nomeCliente[MAX_CONTAS];
string cpf[MAX_CONTAS];
int tipoConta[MAX_CONTAS];      // 1 = Corrente; 2 = Poupanca
double saldo[MAX_CONTAS];
bool contaAtiva[MAX_CONTAS];

int quantidadeContas = 0;

// ==========================================================
//  2.2 HISTORICO DE MOVIMENTACOES
// ==========================================================
const int MAX_MOVIMENTACOES = 500;

int contaMovimentacao[MAX_MOVIMENTACOES];     // numero da conta
char tipoMovimentacao[MAX_MOVIMENTACOES];     // 'D' deposito, 'S' saque
double valorMovimentacao[MAX_MOVIMENTACOES];

int quantidadeMovimentacoes = 0;

// ==========================================================
//  2.1 ARQUIVOS DO SISTEMA (Etapa 3)
// ==========================================================
const string ARQUIVO_CONTAS = "contas.txt";
const string ARQUIVO_MOVIMENTACOES = "movimentacoes.txt";

// ==========================================================
//  Prototipos das funcoes
// ==========================================================
void cadastrarConta();
void listarContas();
void consultarConta();
void depositar();
void sacar();
void transferir();
void registrarMovimentacao(int conta, char tipo, double valor);
void exibirExtrato();
void ativarDesativarConta();

// Funcoes de arquivo (Etapa 3)
void carregarContas();
void salvarContas();
void carregarMovimentacoes();
void salvarMovimentacoes();

// Funcoes auxiliares
int buscarConta(int numero);
int lerInteiro(string mensagem);
double lerValor(string mensagem);
string nomeTipo(int tipo);

// ==========================================================
//  PROGRAMA PRINCIPAL
// ==========================================================
int main() {
    int opcao;

    cout << fixed << setprecision(2);   // valores sempre com 2 casas

    // 2.2 Recupera os dados gravados nas execucoes anteriores
    carregarContas();
    carregarMovimentacoes();
    cout << "Dados carregados: " << quantidadeContas << " conta(s) e "
         << quantidadeMovimentacoes << " movimentacao(oes).\n";

    do {
        cout << "\n********************************\n";
        cout << "**   BANCO ENLACE - ETAPA 3   **\n";
        cout << "********************************\n";
        cout << "1 - Cadastrar conta\n";
        cout << "2 - Listar contas\n";
        cout << "3 - Consultar conta\n";
        cout << "4 - Depositar\n";
        cout << "5 - Sacar\n";
        cout << "6 - Transferir\n";
        cout << "7 - Extrato\n";
        cout << "8 - Ativar/Desativar conta\n";
        cout << "0 - Sair\n";
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: cadastrarConta();        break;
            case 2: listarContas();          break;
            case 3: consultarConta();        break;
            case 4: depositar();             break;
            case 5: sacar();                 break;
            case 6: transferir();            break;
            case 7: exibirExtrato();         break;
            case 8: ativarDesativarConta();  break;
            case 0: cout << "Encerrando o sistema. Ate logo!\n"; break;
            default: cout << "Opcao invalida! Escolha um numero de 0 a 8.\n";
        }
    } while (opcao != 0);

    return 0;
}

// ==========================================================
//  FUNCOES AUXILIARES
// ==========================================================

// Procura a conta pelo numero e devolve o INDICE dela nos arrays.
// Se nao encontrar, devolve -1.
int buscarConta(int numero) {
    for (int i = 0; i < quantidadeContas; i++) {
        if (numeroConta[i] == numero) {
            return i;
        }
    }
    return -1;
}

// Le um numero inteiro; se o usuario digitar letras, pede de novo.
int lerInteiro(string mensagem) {
    int valor;
    while (true) {
        cout << mensagem;
        cin >> valor;
        if (!cin.fail()) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Erro: digite apenas numeros.\n";
    }
}

// Le um valor em reais (double); se digitar letras, pede de novo.
double lerValor(string mensagem) {
    double valor;
    while (true) {
        cout << mensagem;
        cin >> valor;
        if (!cin.fail()) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Erro: digite um valor numerico (use ponto: 150.50).\n";
    }
}

string nomeTipo(int tipo) {
    if (tipo == 1) return "Corrente";
    return "Poupanca";
}

// ==========================================================
//  1. CADASTRAR CONTA
// ==========================================================
void cadastrarConta() {
    if (quantidadeContas >= MAX_CONTAS) {
        cout << "Erro: limite de " << MAX_CONTAS << " contas atingido.\n";
        return;
    }

    int i = quantidadeContas;   // a nova conta ocupa a proxima posicao livre
    int numero;

    // Numero > 0 e que ainda nao exista
    do {
        numero = lerInteiro("Numero da conta: ");
        if (numero <= 0) {
            cout << "Erro: o numero da conta deve ser maior que zero.\n";
        } else if (buscarConta(numero) != -1) {
            cout << "Erro: ja existe uma conta com esse numero.\n";
            numero = 0;
        }
    } while (numero <= 0);
    numeroConta[i] = numero;

    do {
        cout << "Nome do titular: ";
        getline(cin, nomeCliente[i]);
        if (nomeCliente[i].empty()) {
            cout << "Erro: o nome nao pode ficar em branco.\n";
        } else if (nomeCliente[i].find(';') != string::npos) {
            // ';' e o separador do arquivo contas.txt
            cout << "Erro: o nome nao pode conter ';'.\n";
            nomeCliente[i] = "";
        }
    } while (nomeCliente[i].empty());

    do {
        cout << "CPF (somente numeros): ";
        getline(cin, cpf[i]);
        if (cpf[i].length() != 11)
            cout << "Erro: o CPF deve ter 11 digitos.\n";
    } while (cpf[i].length() != 11);

    do {
        tipoConta[i] = lerInteiro("Tipo da conta (1 = Corrente, 2 = Poupanca): ");
        if (tipoConta[i] != 1 && tipoConta[i] != 2)
            cout << "Erro: tipo invalido. Digite 1 ou 2.\n";
    } while (tipoConta[i] != 1 && tipoConta[i] != 2);

    do {
        saldo[i] = lerValor("Saldo inicial: R$ ");
        if (saldo[i] < 0)
            cout << "Erro: o saldo inicial nao pode ser negativo.\n";
    } while (saldo[i] < 0);

    contaAtiva[i] = true;    // toda conta nova comeca ativa
    quantidadeContas++;      // so conta como cadastrada no final

    salvarContas();          // 2.3 grava a nova conta em contas.txt

    cout << "\nConta " << numeroConta[i] << " cadastrada com sucesso! ("
         << quantidadeContas << "/" << MAX_CONTAS << ")\n";
}

// ==========================================================
//  2. LISTAR CONTAS
// ==========================================================
void listarContas() {
    if (quantidadeContas == 0) {
        cout << "Nenhuma conta cadastrada.\n";
        return;
    }

    cout << "\n" << left
         << setw(8)  << "Numero"
         << setw(22) << "Titular"
         << setw(10) << "Tipo"
         << setw(14) << "Saldo (R$)"
         << "Situacao\n";
    cout << string(62, '-') << "\n";

    for (int i = 0; i < quantidadeContas; i++) {
        cout << left
             << setw(8)  << numeroConta[i]
             << setw(22) << nomeCliente[i].substr(0, 20)
             << setw(10) << nomeTipo(tipoConta[i])
             << setw(14) << saldo[i]
             << (contaAtiva[i] ? "Ativa" : "Inativa") << "\n";
    }
    cout << right << "Total: " << quantidadeContas << " conta(s).\n";
}

// ==========================================================
//  3. CONSULTAR CONTA PELO NUMERO
// ==========================================================
void consultarConta() {
    int numero = lerInteiro("Numero da conta: ");
    int i = buscarConta(numero);

    if (i == -1) {
        cout << "Conta nao encontrada.\n";
        return;
    }

    cout << "\n------ DADOS DA CONTA ------\n";
    cout << "Numero:   " << numeroConta[i] << "\n";
    cout << "Titular:  " << nomeCliente[i] << "\n";
    cout << "CPF:      " << cpf[i] << "\n";
    cout << "Tipo:     " << nomeTipo(tipoConta[i]) << "\n";
    cout << "Saldo:    R$ " << saldo[i] << "\n";
    cout << "Situacao: " << (contaAtiva[i] ? "Ativa" : "Inativa") << "\n";
    cout << "----------------------------\n";
}

// ==========================================================
//  Grava UMA movimentacao no historico
// ==========================================================
void registrarMovimentacao(int conta, char tipo, double valor) {
    contaMovimentacao[quantidadeMovimentacoes] = conta;
    tipoMovimentacao[quantidadeMovimentacoes] = tipo;
    valorMovimentacao[quantidadeMovimentacoes] = valor;
    quantidadeMovimentacoes++;
}

// ==========================================================
//  4. DEPOSITAR
// ==========================================================
void depositar() {
    if (quantidadeMovimentacoes >= MAX_MOVIMENTACOES) {
        cout << "Erro: historico de movimentacoes cheio.\n";
        return;
    }

    int numero = lerInteiro("Numero da conta: ");
    int indice = buscarConta(numero);

    if (indice == -1) {
        cout << "Conta nao encontrada.\n";
        return;
    }
    if (!contaAtiva[indice]) {
        cout << "Operacao negada: a conta esta INATIVA.\n";
        return;
    }

    double valor = lerValor("Valor do deposito: R$ ");
    if (valor <= 0) {
        cout << "Erro: o valor deve ser maior que zero.\n";
        return;
    }

    saldo[indice] = saldo[indice] + valor;
    registrarMovimentacao(numeroConta[indice], 'D', valor);

    salvarContas();           // saldo novo
    salvarMovimentacoes();    // historico novo

    cout << "Deposito realizado! Novo saldo: R$ " << saldo[indice] << "\n";
}

// ==========================================================
//  5. SACAR (verifica saldo suficiente)
// ==========================================================
void sacar() {
    if (quantidadeMovimentacoes >= MAX_MOVIMENTACOES) {
        cout << "Erro: historico de movimentacoes cheio.\n";
        return;
    }

    int numero = lerInteiro("Numero da conta: ");
    int indice = buscarConta(numero);

    if (indice == -1) {
        cout << "Conta nao encontrada.\n";
        return;
    }
    if (!contaAtiva[indice]) {
        cout << "Operacao negada: a conta esta INATIVA.\n";
        return;
    }

    double valor = lerValor("Valor do saque: R$ ");
    if (valor <= 0) {
        cout << "Erro: o valor deve ser maior que zero.\n";
        return;
    }
    if (valor > saldo[indice]) {
        cout << "Saldo insuficiente. Saldo disponivel: R$ " << saldo[indice] << "\n";
        return;
    }

    saldo[indice] = saldo[indice] - valor;
    registrarMovimentacao(numeroConta[indice], 'S', valor);

    salvarContas();
    salvarMovimentacoes();

    cout << "Saque realizado! Novo saldo: R$ " << saldo[indice] << "\n";
}

// ==========================================================
//  6. TRANSFERIR ENTRE DUAS CONTAS
//     Gera 2 registros: 'S' na origem e 'D' no destino.
// ==========================================================
void transferir() {
    // Garante espaco para os DOIS registros antes de comecar
    if (quantidadeMovimentacoes + 2 > MAX_MOVIMENTACOES) {
        cout << "Erro: nao ha espaco no historico para registrar a transferencia.\n";
        return;
    }

    int numOrigem = lerInteiro("Conta de origem: ");
    int indiceOrigem = buscarConta(numOrigem);
    if (indiceOrigem == -1) {
        cout << "Conta de origem nao encontrada.\n";
        return;
    }

    int numDestino = lerInteiro("Conta de destino: ");
    int indiceDestino = buscarConta(numDestino);
    if (indiceDestino == -1) {
        cout << "Conta de destino nao encontrada.\n";
        return;
    }

    if (indiceOrigem == indiceDestino) {
        cout << "Erro: origem e destino devem ser contas diferentes.\n";
        return;
    }
    if (!contaAtiva[indiceOrigem] || !contaAtiva[indiceDestino]) {
        cout << "Operacao negada: as duas contas precisam estar ATIVAS.\n";
        return;
    }

    double valor = lerValor("Valor da transferencia: R$ ");
    if (valor <= 0) {
        cout << "Erro: o valor deve ser maior que zero.\n";
        return;
    }
    if (valor > saldo[indiceOrigem]) {
        cout << "Saldo insuficiente na conta de origem. Disponivel: R$ "
             << saldo[indiceOrigem] << "\n";
        return;
    }

    // Atualiza os saldos
    saldo[indiceOrigem] = saldo[indiceOrigem] - valor;
    saldo[indiceDestino] = saldo[indiceDestino] + valor;

    // Dois registros para uma unica transferencia (+2 movimentacoes)
    registrarMovimentacao(numeroConta[indiceOrigem], 'S', valor);
    registrarMovimentacao(numeroConta[indiceDestino], 'D', valor);

    // 2.5 grava os dois saldos e os dois registros nos arquivos
    salvarContas();
    salvarMovimentacoes();

    cout << "Transferencia realizada!\n";
    cout << "  Conta " << numeroConta[indiceOrigem] << " (" << nomeCliente[indiceOrigem]
         << "): R$ " << saldo[indiceOrigem] << "\n";
    cout << "  Conta " << numeroConta[indiceDestino] << " (" << nomeCliente[indiceDestino]
         << "): R$ " << saldo[indiceDestino] << "\n";
}

// ==========================================================
//  7. EXTRATO
//     Percorre o historico e mostra so as movimentacoes da conta.
// ==========================================================
void exibirExtrato() {
    int numero = lerInteiro("Numero da conta: ");
    int indice = buscarConta(numero);

    if (indice == -1) {
        cout << "Conta nao encontrada.\n";
        return;
    }

    cout << "\n======= EXTRATO - CONTA " << numeroConta[indice] << " =======\n";
    cout << "Titular: " << nomeCliente[indice] << "\n";
    cout << "------------------------------------\n";

    int encontradas = 0;
    for (int i = 0; i < quantidadeMovimentacoes; i++) {
        if (contaMovimentacao[i] == numero) {
            if (tipoMovimentacao[i] == 'D') {
                cout << "Deposito: + R$ " << valorMovimentacao[i] << endl;
            }
            else if (tipoMovimentacao[i] == 'S') {
                cout << "Saque:    - R$ " << valorMovimentacao[i] << endl;
            }
            encontradas++;
        }
    }

    if (encontradas == 0) {
        cout << "Nenhuma movimentacao registrada.\n";
    }
    cout << "------------------------------------\n";
    cout << "Saldo atual: R$ " << saldo[indice] << "\n";
}

// ==========================================================
//  8. ATIVAR / DESATIVAR CONTA (mantido da Etapa 1)
// ==========================================================
void ativarDesativarConta() {
    int numero = lerInteiro("Numero da conta: ");
    int i = buscarConta(numero);

    if (i == -1) {
        cout << "Conta nao encontrada.\n";
        return;
    }

    contaAtiva[i] = !contaAtiva[i];
    salvarContas();   // a situacao da conta tambem fica gravada
    cout << "A conta " << numeroConta[i] << " agora esta "
         << (contaAtiva[i] ? "ATIVA" : "INATIVA") << ".\n";
}

// ==========================================================
//  ETAPA 3 - FUNCOES DE ARQUIVO
// ==========================================================

// ----------------------------------------------------------
//  2.2 Le contas.txt e preenche os arrays de contas.
//  Cada linha: numero;nome;cpf;tipo;saldo;ativa
// ----------------------------------------------------------
void carregarContas() {
    ifstream arquivo(ARQUIVO_CONTAS);

    // Primeira execucao: o arquivo ainda nao existe -> comeca vazio
    if (!arquivo.is_open()) {
        return;
    }

    string linha;
    quantidadeContas = 0;

    while (getline(arquivo, linha) && quantidadeContas < MAX_CONTAS) {
        if (linha.empty()) continue;   // ignora linhas em branco

        stringstream ss(linha);
        string campoNumero, campoNome, campoCpf, campoTipo, campoSaldo, campoAtiva;

        // getline com ';' le cada campo ate o proximo ponto e virgula
        getline(ss, campoNumero, ';');
        getline(ss, campoNome, ';');
        getline(ss, campoCpf, ';');
        getline(ss, campoTipo, ';');
        getline(ss, campoSaldo, ';');
        getline(ss, campoAtiva, ';');

        int i = quantidadeContas;
        numeroConta[i] = stoi(campoNumero);
        nomeCliente[i] = campoNome;
        cpf[i]         = campoCpf;
        tipoConta[i]   = stoi(campoTipo);
        saldo[i]       = stod(campoSaldo);
        contaAtiva[i]  = (stoi(campoAtiva) == 1);

        quantidadeContas++;
    }

    arquivo.close();
}

// ----------------------------------------------------------
//  2.3 Grava TODAS as contas em contas.txt.
//  Sem ios::app o arquivo e reescrito por completo, o que e
//  o desejado: os arrays ja tem os dados atualizados.
// ----------------------------------------------------------
void salvarContas() {
    ofstream arquivo(ARQUIVO_CONTAS);

    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel gravar " << ARQUIVO_CONTAS << ".\n";
        return;
    }

    arquivo << fixed << setprecision(2);   // saldo como 700.00
    for (int i = 0; i < quantidadeContas; i++) {
        arquivo << numeroConta[i] << ";"
                << nomeCliente[i] << ";"
                << cpf[i] << ";"
                << tipoConta[i] << ";"
                << saldo[i] << ";"
                << contaAtiva[i] << endl;     // bool grava 1 ou 0
    }

    arquivo.close();
}

// ----------------------------------------------------------
//  2.2 Le movimentacoes.txt e preenche o historico.
//  Cada linha: conta;tipo;valor
// ----------------------------------------------------------
void carregarMovimentacoes() {
    ifstream arquivo(ARQUIVO_MOVIMENTACOES);

    if (!arquivo.is_open()) {
        return;   // ainda nao ha movimentacoes gravadas
    }

    string linha;
    quantidadeMovimentacoes = 0;

    while (getline(arquivo, linha) && quantidadeMovimentacoes < MAX_MOVIMENTACOES) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        string campoConta, campoTipo, campoValor;

        getline(ss, campoConta, ';');
        getline(ss, campoTipo, ';');
        getline(ss, campoValor, ';');

        int j = quantidadeMovimentacoes;
        contaMovimentacao[j] = stoi(campoConta);
        tipoMovimentacao[j]  = campoTipo[0];     // 'D' ou 'S'
        valorMovimentacao[j] = stod(campoValor);

        quantidadeMovimentacoes++;
    }

    arquivo.close();
}

// ----------------------------------------------------------
//  2.4 Grava TODO o historico em movimentacoes.txt.
//  Como o historico carregado no inicio ja esta nos arrays,
//  as movimentacoes das execucoes anteriores sao preservadas.
// ----------------------------------------------------------
void salvarMovimentacoes() {
    ofstream arquivo(ARQUIVO_MOVIMENTACOES);

    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel gravar " << ARQUIVO_MOVIMENTACOES << ".\n";
        return;
    }

    arquivo << fixed << setprecision(2);
    for (int i = 0; i < quantidadeMovimentacoes; i++) {
        arquivo << contaMovimentacao[i] << ";"
                << tipoMovimentacao[i] << ";"
                << valorMovimentacao[i] << endl;
    }

    arquivo.close();
}
