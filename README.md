# Banco Enlace – Sistema de Registro e Gestão de Contas Bancárias

   Trabalho da disciplina **INF101 – Programação de Computadores I** (UNIVIÇOSA)

   **Professores:** Anderson R. Lamas e Vanderlea Queiroz

   **Aluno:** Raphael de Souza Azevedo - 26338

## Etapa 1 – Variáveis e estruturas de controle

| Arquivo | Descrição |
|---|---|
| `main.cpp` | Sistema principal: cadastro e gestão de **uma** conta usando apenas variáveis individuais |
| `banco_5_contas.cpp` | Desafio 3.5: versão que cadastra **até 5 contas** |

### Funcionalidades (menu com `switch` dentro de `do-while`)

1. Cadastrar conta
2. Consultar conta
3. Verificar saldo
4. Alterar tipo da conta
5. Ativar/Desativar conta
6. Sair

### Validações

- Número da conta deve ser maior que zero
- Saldo inicial não pode ser negativo
- Tipo da conta deve ser 1 (Corrente) ou 2 (Poupança)
- Verificar saldo e alterar tipo só funcionam com a conta **ativa** (`contaAtiva`)
- Extras: CPF com 11 dígitos, nome não vazio, proteção contra letras digitadas em campos numéricos

### Variáveis extras (desafio)

- `telefone` (string): telefone do titular
- `contaCadastrada` (bool): impede consultar/alterar antes de existir uma conta

## Etapa 2 – Arrays e histórico de movimentações

Arquivo: [`etapa2/main.cpp`](etapa2/main.cpp)

O sistema passa a guardar **até 100 contas** e **até 500 movimentações** em arrays
(`numeroConta[MAX_CONTAS]`, `saldo[MAX_CONTAS]`, `contaMovimentacao[MAX_MOVIMENTACOES]` …),
resolvendo a limitação de `numeroConta1`, `numeroConta2`… da Etapa 1.
Como desafio, o programa foi **modularizado em funções** (`cadastrarConta`, `listarContas`,
`consultarConta`, `depositar`, `sacar`, `transferir`, `registrarMovimentacao`, `exibirExtrato`).

### Menu

| Opção | Função |
|---|---|
| 1 | Cadastrar conta |
| 2 | Listar contas |
| 3 | Consultar conta pelo número |
| 4 | Depositar (registra `D`) |
| 5 | Sacar, verificando saldo suficiente (registra `S`) |
| 6 | Transferir entre duas contas (registra `S` na origem e `D` no destino) |
| 7 | Extrato da conta |
| 8 | Ativar/Desativar conta |
| 0 | Sair |

### Validações

- Número da conta maior que zero e **sem repetição**; limite de 100 contas
- Depósito, saque e transferência só com valor positivo e contas **ativas**
- Saque e transferência verificam saldo suficiente
- Transferência exige contas diferentes e confere se há espaço para os **2 registros** no histórico
- Proteção contra letras digitadas em campos numéricos

### Exemplo (enunciado 2.5)

Ana (1001, R$ 1.000) transfere R$ 300 para Bruno (1002, R$ 500):
saldos finais R$ 700 e R$ 800, com os registros `1001 | S | 300` e `1002 | D | 300`.

> Na Etapa 2 os dados ficam só na memória; a persistência em arquivo foi feita na Etapa 3.

```bash
g++ etapa2/main.cpp -o banco2
./banco2           # Linux/Mac
banco2.exe         # Windows
```

## Etapa 3 – Persistência dos dados em arquivos

Arquivo: [`etapa3/main.cpp`](etapa3/main.cpp)

Mantém todas as funcionalidades da Etapa 2 e passa a **gravar os dados em arquivos de texto**
(biblioteca `fstream`), para que nada se perca ao encerrar o programa.

| Arquivo | Conteúdo (campos separados por `;`) | Exemplo |
|---|---|---|
| `contas.txt` | número;nome;CPF;tipo;saldo;ativa | `1001;Ana Silva;12345678900;1;700.00;1` |
| `movimentacoes.txt` | conta;tipo;valor | `1001;S;300.00` |

### Funções de arquivo

| Função | O que faz |
|---|---|
| `carregarContas()` | No início, abre `contas.txt` com `ifstream`, lê cada linha com `getline()`, separa os campos por `;` e preenche os arrays, atualizando `quantidadeContas` |
| `carregarMovimentacoes()` | Mesmo processo para `movimentacoes.txt`, atualizando `quantidadeMovimentacoes` |
| `salvarContas()` | Reescreve `contas.txt` com `ofstream` percorrendo todos os arrays de contas |
| `salvarMovimentacoes()` | Reescreve `movimentacoes.txt` com todo o histórico (inclusive o das execuções anteriores, que já foi carregado) |

- Se os arquivos não existirem (primeira execução), `is_open()` retorna falso e o sistema começa vazio normalmente.
- Cadastro, depósito, saque, transferência e ativar/desativar chamam as funções de salvamento logo após alterar os arrays.
- Na transferência são gravados os dois saldos e os dois registros (`S` na origem, `D` no destino).
- O nome do titular não pode conter `;`, porque esse caractere é o separador do arquivo.

### Testes realizados (enunciado 2.8)

1. **Cadastro e recuperação:** 3 contas cadastradas, programa encerrado e reaberto → as 3 contas aparecem na listagem.
2. **Saldos:** depósito na 1003 e saque na 1002, programa reaberto → saldos mantidos.
3. **Transferência:** 1001 → 1002 de R$ 300, programa reaberto → saldos atualizados e extratos com `S` e `D`.
4. **Histórico:** novo depósito em outra execução → o extrato mostra as movimentações antigas e a nova.

```bash
g++ etapa3/main.cpp -o banco3
./banco3           # Linux/Mac
banco3.exe         # Windows
```

> Os arquivos `contas.txt` e `movimentacoes.txt` são criados na pasta de onde o programa é executado.

## Como compilar e executar (Etapa 1)

```bash
g++ main.cpp -o banco
./banco            # Linux/Mac
banco.exe          # Windows
```

Para a versão de 5 contas:

```bash
g++ banco_5_contas.cpp -o banco5
./banco5
```

## Desafio: e se quiséssemos cadastrar até 5 contas?

Usando só variáveis individuais, a única saída é criar **5 conjuntos de variáveis**
(`numeroConta1` … `numeroConta5`, `saldo1` … `saldo5` etc.). Em `banco_5_contas.cpp`:

1. a conta é localizada pelo número com uma cadeia de `if/else`;
2. os dados dela são copiados para variáveis "de trabalho";
3. a operação é feita nessas variáveis;
4. os dados são copiados de volta para a conta original.

**Limitação:** o código fica longo e repetitivo, e cada conta a mais exige novas
variáveis e mais linhas em todos os `if` e `switch`. Para 100 contas seria inviável.
A solução adequada é usar **arrays (vetores)** e **funções**, conteúdo da Etapa 2.
