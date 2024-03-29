#include "time/util.h"
#include "Graph.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
#include <utility>
#include <sstream>
#include <climits>
#include <cfloat>

using namespace std;

// Constantes
static const int OPTION_INVALID = -1;
static const int OPTION_EXIT = 0;
static const int OPTION_EXPORT = 1;

// Variáveis globais
bool directed = false, weightedEdge = false, weightedNode = false;
string input_file_name;
CARDINAL seed;

/****************
 * Funcao    : string formatFloat(float value, int precision, int totalLength)  	                *
 * Descricao : Converte float para string com parâmteros especificados                              *
 * Parametros: value - valor em float a ser formatado                                               *
 *             precision - precisão das casas decimais                                              *
 *             totalLength - comprimento total da string retornada                                  *
 * Retorno   : string formatada.                                                                    *
 ***************/
string formatFloat(float value, int precision, int totalLength)
{
    string returnString = "";
    if (value == FLT_MAX)
        returnString = "&";
    else
    {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(precision) << value;
        returnString = stream.str();
    }
    int missingSpaces = totalLength - returnString.length();
    if (missingSpaces > 0)
        returnString.insert(0, missingSpaces, ' ');
    return returnString;
}

/****************
 * Funcao    : string formatInt(int value, int totalLength)  	                                    *
 * Descricao : Converte inteiro para string com parâmteros especificados                            *
 * Parametros: value - valor em inteiro a ser formatado                                             *
 *             totalLength - comprimento total da string retornada                                  *
 * Retorno   : string formatada.                                                                    *
 ***************/
string formatInt(int value, int totalLength)
{
    string returnString = "";
    if (value == INT_MAX)
        returnString = "&";
    else
        returnString = to_string(value);
    int missingSpaces = totalLength - returnString.length();
    if (missingSpaces > 0)
        returnString.insert(0, missingSpaces, ' ');
    return returnString;
}

/****************
 * Funcao    : string exportGraphToDotFormat(Graph *graph, bool isPERT) 	                        *
 * Descricao : Cria string com a descrição de um grafo no formato .DOT                              *
 * Parametros: graph - objeto do tipo Graph, de que as informações serão extraídas                  *
 *             isPERT - define se o grafo pode ser utilizado para Rede PERT, ou seja, é multiaresta *
 * Retorno   : descrição no formato .DOT.                                                           *
 ***************/
string exportGraphToDotFormat(Graph *graph, bool isPERT)
{ // dot -Tpng output.dot -o graph1.png
    if (graph == nullptr)
        return "";

    Node *nextNode = graph->getFirstNode();
    string dot = "", connector;
    bool weightedNode = graph->getWeightedNode();
    bool weightedEdge = graph->getWeightedEdge();

    if (!isPERT)
    {
        dot += "strict ";
    }
    if (graph->getDirected())
    {
        connector = " -> ";
        dot += "di";
    }
    else
        connector = " -- ";

    dot += "graph grafo {\n";
    while (nextNode != nullptr)
    {
        dot += "  " + to_string(nextNode->getLabel()) +
               " [weight = " + formatFloat(nextNode->getWeight(), 2, 5) + "]";
        // if (weightedNode)
        //     dot += " [xlabel = " + formatFloat(nextNode->getWeight(), 0, 5) + "]";
        dot += ";\n";
        nextNode = nextNode->getNextNode();
    }
    nextNode = graph->getFirstNode();
    while (nextNode != nullptr)
    {
        Edge *nextEdge = nextNode->getFirstEdge();
        while (nextEdge != nullptr)
        {
            dot += "\n  " + to_string(nextNode->getLabel()) + connector +
                   to_string(graph->getNodeById(nextEdge->getTargetId())->getLabel());
            if (weightedEdge)
            {
                dot += " [weight = " + formatFloat(nextEdge->getWeight(), 2, 5) +
                       "] [label = " + formatFloat(nextEdge->getWeight(), 2, 5) + "];";
            }
            nextEdge = nextEdge->getNextEdge();
        }
        nextNode = nextNode->getNextNode();
    }
    dot += "\n}\n";

    return dot;
}

/****************
 * Funcao    : string exportGraphToDotFormat(Graph *graph) 	                                        *
 * Descricao : Chama função de exportar grafo para .DOT com o valor de isPERT falso                 *
 * Parametros: graph - objeto do tipo Graph, de que as informações serão extraídas                  *
 * Retorno   : descrição no formato .DOT.                                                           *
 ***************/
string exportGraphToDotFormat(Graph *graph)
{
    return exportGraphToDotFormat(graph, false);
}

/****************
 * Funcao    : Graph *readFileSecondPart(ifstream &input_file)                                                                                          *
 * Descricao : Constrói grafo a partir de um arquivo de definição da instância, no formato especificado para o problema do conjunto dominante ponderado *
 * Parametros: input_file - arquivo de entrada de dados, com definição do grafo                                                                         *
 * Retorno   : objeto do tipo Graph, construído conforme a entrada                                                                                      *
 ***************/
Graph *readFileSecondPart(ifstream &input_file)
{
    // Preenchimento das variáveis globais
    ::directed = false;
    ::weightedEdge = false;
    ::weightedNode = true;

    // Variáveis para auxiliar na criação dos nós no Grafo
    int labelNodeSource;
    int labelNodeTarget;
    int order;
    float weight;
    int existEdge;

    // Obtém a ordem do grafo
    string line;
    input_file >> line;
    input_file >> order;

    // Cria objeto grafo
    Graph *graph = new Graph(order, directed, weightedEdge, weightedNode);

    // Leitura de arquivo
    if (!graph->getWeightedEdge() && !graph->getWeightedNode())
    {
        while (input_file >> labelNodeSource >> labelNodeTarget)
        {
            Node *sourceNode = nullptr;
            Node *targetNode = nullptr;
            graph->insertEdge(abs(labelNodeSource), abs(labelNodeTarget), 1, &sourceNode, &targetNode, false);
        }
    }
    // Posições
    for (int i = 0; i < order + 3; i++)
        getline(input_file, line);
    // Pesos dos nós
    for (int i = 0; i < order; i++)
    {
        input_file >> weight;
        graph->insertNode(i + 1, weight);
    }
    input_file >> line;
    // Arestas
    for (int i = 0; i < order - 1; i++)
    {
        for (int j = 0; j < order; j++)
        {
            input_file >> existEdge;
            if (j > i && existEdge == 1)
                graph->insertEdge(i + 1, j + 1, 1);
        }
    }

    return graph;
}

/****************
 * Funcao    : Graph *readFileFirstPart(ifstream &input_file, int directed, int weightedEdge, int weightedNode, bool isPERT)              *
 * Descricao : Constrói grafo a partir de um arquivo de definição da instância, no formato especificado para a primeira etapa do trabalho *
 * Parametros: input_file - arquivo de entrada de dados, com definição do grafo                                                           *
 *             directed - define se o grafo é direcionado                                                                                 *
 *             weightedEdge - define se as arestas do grafo têm peso                                                                      *
 *             weightedNode- define se os vértices do grafo têm peso                                                                      *
 *             isPERT - define se o grafo está no formata para resolver rede PERT, ou seja, permite multiaresta                           *
 * Retorno   : objeto do tipo Graph, construído conforme a entrada                                                                        *
 ***************/
Graph *readFileFirstPart(ifstream &input_file, int directed, int weightedEdge, int weightedNode, bool isPERT)
{
    // Preenchimento das variáveis globais
    ::directed = directed;
    ::weightedEdge = weightedEdge;
    ::weightedNode = weightedNode;

    // Variáveis para auxiliar na criação dos nós no Grafo
    int labelNodeSource;
    int labelNodeTarget;
    int order;

    // Obtém a ordem do grafo
    input_file >> order;

    // Cria objeto grafo
    Graph *graph = new Graph(order, directed, weightedEdge, weightedNode);

    // Leitura de arquivo
    //
    // Grafo SEM peso nos nós, e SEM peso nas arestas
    if (!graph->getWeightedEdge() && !graph->getWeightedNode())
    {
        while (input_file >> labelNodeSource >> labelNodeTarget)
        {
            Node *sourceNode = nullptr;
            Node *targetNode = nullptr;
            graph->insertEdge(abs(labelNodeSource), abs(labelNodeTarget), 1, &sourceNode, &targetNode, isPERT);
        }
    }
    // Grafo SEM peso nos nós, mas COM peso nas arestas
    else if (graph->getWeightedEdge() && !graph->getWeightedNode())
    {
        float edgeWeight;

        while (input_file >> labelNodeSource >> labelNodeTarget >> edgeWeight)
        {
            Node *sourceNode = nullptr;
            Node *targetNode = nullptr;
            graph->insertEdge(abs(labelNodeSource), abs(labelNodeTarget), edgeWeight, &sourceNode, &targetNode, isPERT);
        }
    }
    // Grafo COM peso nos nós, mas SEM peso nas arestas
    else if (graph->getWeightedNode() && !graph->getWeightedEdge())
    {
        float nodeSourceWeight, nodeTargetWeight;

        while (input_file >> labelNodeSource >> nodeSourceWeight >> labelNodeTarget >> nodeTargetWeight)
        {
            Node *sourceNode = nullptr;
            Node *targetNode = nullptr;
            graph->insertEdge(abs(labelNodeSource), abs(labelNodeTarget), 1, &sourceNode, &targetNode, isPERT);
            if (sourceNode != nullptr)
                sourceNode->setWeight(nodeSourceWeight);
            if (targetNode != nullptr)
                targetNode->setWeight(nodeTargetWeight);
        }
    }
    // Grafo COM peso nos nós, e COM peso nas arestas
    else if (graph->getWeightedNode() && graph->getWeightedEdge())
    {
        float nodeSourceWeight, nodeTargetWeight, edgeWeight;

        while (input_file >> labelNodeSource >> nodeSourceWeight >> labelNodeTarget >> nodeTargetWeight >> edgeWeight)
        {
            Node *sourceNode = nullptr;
            Node *targetNode = nullptr;
            graph->insertEdge(abs(labelNodeSource), abs(labelNodeTarget), edgeWeight, &sourceNode, &targetNode, isPERT);
            if (sourceNode != nullptr)
                sourceNode->setWeight(nodeSourceWeight);
            if (targetNode != nullptr)
                targetNode->setWeight(nodeTargetWeight);
        }
    }

    int i = -1;
    while (order > graph->getNodeIdCounter())
    {
        graph->insertNode(i);
        i--;
    }

    return graph;
}

/****************
 * Funcao    : Graph *createAuxiliaryGraphFromFile(ifstream &input_file, string input_file_name, int *selectedOption, string *errors)                                                                     *
 * Descricao : Abre um arquivo de entrada de definição da instância auxiliar para construir um grafo, com os mesmos atributos do grafo inicial, no formato especificado para a primeira etapa do trabalho *
 * Parametros: input_file - arquivo de entrada de dados, com definição do grafo                                                                                                                           *
 *             input_file_name - nome do arquivo de entrada de dados                                                                                                                                      *
 *             selectedOption - retorna a opção de menu selecionada como inválido, caso não seja possível abrir o arquivo                                                                                 *
 *             errors - retorna um erro na string definida para tal caso existam                                                                                                                          *
 * Retorno   : objeto do tipo Graph, construído conforme a entrada                                                                                                                                        *
 ***************/
Graph *createAuxiliaryGraphFromFile(ifstream &input_file, string input_file_name, int *selectedOption, string *errors)
{
    if (input_file.is_open())
        return readFileFirstPart(input_file, ::directed, ::weightedEdge, ::weightedNode, false);
    else
    {
        *errors += "ERRO: Não foi possível abrir o arquivo de entrada " + input_file_name + "!\n";
        *selectedOption = OPTION_INVALID;
        return nullptr;
    }
}

/****************
 * Funcao    : Graph *readAuxiliaryGraph(int *selectedOption, string *errors)                                                                     *
 * Descricao : Solicita ao usuário o nome de um arquivo de entrada de definição da instância auxiliar, o abre, e chama função de instanciar o grafo definido
 * Parametros: selectedOption - retorna a opção de menu selecionada como inválido, caso não seja possível abrir o arquivo                                                                                 *
 *             errors - retorna um erro na string definida para tal caso existam                                                                                                                          *
 * Retorno   : objeto do tipo Graph, construído conforme a entrada                                                                                                                                        *
 ***************/
Graph *readAuxiliaryGraph(int *selectedOption, string *errors)
{
    cout << "Digite o caminho do arquivo de entrada do grafo auxiliar:" << endl;
    string auxiliary_input_file_name;
    cin >> auxiliary_input_file_name;

    ifstream auxiliary_input_file;
    auxiliary_input_file.open(auxiliary_input_file_name, ios::in);

    Graph *auxiliaryGraph = createAuxiliaryGraphFromFile(auxiliary_input_file, auxiliary_input_file_name, selectedOption, errors);

    auxiliary_input_file.close();
    return auxiliaryGraph;
}

/****************
 * Funcao    : Graph *createUnionGraph(Graph *firstGraph, Graph *secondGraph)    *
 * Descricao : Cria um novo grafo a partir da união de dois outros               *
 * Parametros: firstGraph - primeiro grafo para realizar a união                 *
 *             secondGraph - segundo grafo para realizar a união                 *
 * Retorno   : objeto do tipo Graph, construído pela união de dois outros grafos *
 ***************/
Graph *createUnionGraph(Graph *firstGraph, Graph *secondGraph)
{
    Graph *thirdGraph;
    string dot = "";
    // COPIA GRAFO1 PARA GRAFO 3
    thirdGraph = new Graph(INT_MAX, firstGraph->getDirected(), false, false);

    // aux
    Node *finalNode = firstGraph->getFirstNode();
    Edge *nextEdge;

    while (finalNode != nullptr)
    {
        nextEdge = finalNode->getFirstEdge();

        while (nextEdge != nullptr)
        {
            Node *sourceNode = nullptr;
            Node *targetNode = nullptr;
            thirdGraph->insertEdge(finalNode->getLabel(), nextEdge->getTargetLabel(), 1, &sourceNode, &targetNode, false);
            nextEdge = nextEdge->getNextEdge();
        }
        finalNode = finalNode->getNextNode();
    }
    // VERIFICA QUAIS RELAÇÕES ESTÃO NO SEGUNDO GRAFO E NAO ESTÃO NO TERCEIRO

    finalNode = secondGraph->getFirstNode();
    while (finalNode != nullptr)
    {
        nextEdge = finalNode->getFirstEdge();
        while (nextEdge != nullptr)
        {
            if (!thirdGraph->thereIsEdgeBetweenLabel(finalNode->getLabel(), nextEdge->getTargetLabel()))
            {
                Node *sourceNode = nullptr;
                Node *targetNode = nullptr;
                thirdGraph->insertEdge(finalNode->getLabel(), nextEdge->getTargetLabel(), 1, &sourceNode, &targetNode, false);
            }
            nextEdge = nextEdge->getNextEdge();
        }
        finalNode = finalNode->getNextNode();
    }

    // CORRIGE ORDEM DO GRAFO
    thirdGraph->fixOrder();
    return thirdGraph;
}

/****************
 * Funcao    : Graph *createIntersectionGraph(Graph *firstGraph, Graph *secondGraph)  *
 * Descricao : Cria um novo grafo a partir da interseção de dois outros               *
 * Parametros: firstGraph - primeiro grafo para realizar a interseção                 *
 *             secondGraph - segundo grafo para realizar a interseção                 *
 * Retorno   : objeto do tipo Graph, construído pela interseção de dois outros grafos *
 ***************/
Graph *createIntersectionGraph(Graph *firstGraph, Graph *secondGraph)
{
    Graph *thirdGraph;
    thirdGraph = new Graph(INT_MAX, firstGraph->getDirected(), false, false);

    Node *auxNodeFirstGraph = firstGraph->getFirstNode();
    Node *auxNodeSecondGraph = secondGraph->getFirstNode();
    Edge *auxEdgeFirstGraph;
    Edge *auxEdgeSecondGraph;
    int sourceLabel;
    int targetLabel;

    while (auxNodeFirstGraph != nullptr)
    {
        auxEdgeFirstGraph = auxNodeFirstGraph->getFirstEdge();
        auxEdgeSecondGraph = auxNodeSecondGraph->getFirstEdge();
        while (auxEdgeFirstGraph != nullptr)
        {
            sourceLabel = auxNodeFirstGraph->getLabel();
            targetLabel = auxEdgeFirstGraph->getTargetLabel();

            if (secondGraph->thereIsEdgeBetweenLabel(sourceLabel, targetLabel))
                thirdGraph->insertEdge(sourceLabel, targetLabel, 1);
            auxEdgeFirstGraph = auxEdgeFirstGraph->getNextEdge();
        }
        auxNodeFirstGraph = auxNodeFirstGraph->getNextNode();
    }

    thirdGraph->fixOrder();
    return thirdGraph;
}

/****************
 * Funcao    : Graph *graphDifference(Graph *originalGraph, Graph *toSubtractGraph)          *
 * Descricao : Cria um novo grafo a partir da diferença entre o primeiro grafo e o segundo   *
 * Parametros: originalGraph - grafo base para realizar a diferença                          *
 *             toSubtractGraph - grafo que indica as relações a serem eliminadas do primeiro *
 * Retorno   : objeto do tipo Graph, construído pela diferença de dois outros grafos         *
 ***************/
Graph *graphDifference(Graph *originalGraph, Graph *toSubtractGraph)
{
    // Instancia grafo resultado com características do grafo original
    Graph *resultedGraph;
    resultedGraph = new Graph(INT_MAX, originalGraph->getDirected(), false, false);

    // Variáveis auxiliares
    Node *auxNodeOriginalGraph = originalGraph->getFirstNode();     // Nó auxiliar recebe o primeiro nó do grafo original
    Node *auxNodeToSubtractGraph = toSubtractGraph->getFirstNode(); // Nó auxiliar recebe o primeiro nó do grafo a subtrair
    Edge *auxEdgeOriginalGraph;
    Edge *auxEdgeToSubtractGraph;
    int sourceLabel;
    int targetLabel;

    while (auxNodeOriginalGraph != nullptr)
    {
        auxEdgeOriginalGraph = auxNodeOriginalGraph->getFirstEdge(); // Aresta auxiliar recebe primeira aresta do primeiro nó do grafo original
        auxEdgeToSubtractGraph = auxNodeToSubtractGraph->getFirstEdge();
        // Enquanto há arestas em um nó...
        while (auxEdgeOriginalGraph != nullptr)
        {
            sourceLabel = auxNodeOriginalGraph->getLabel();
            targetLabel = auxEdgeOriginalGraph->getTargetLabel();
            // Se há aresta no grafo original, e não há no secundário, adiciona ao resultado
            if (!toSubtractGraph->thereIsEdgeBetweenLabel(sourceLabel, targetLabel))
                resultedGraph->insertEdge(sourceLabel, targetLabel, 1);
            auxEdgeOriginalGraph = auxEdgeOriginalGraph->getNextEdge();
        }
        // Avança para o próximo nó
        auxNodeOriginalGraph = auxNodeOriginalGraph->getNextNode();
    }
    resultedGraph->fixOrder();
    return resultedGraph;
}

/****************
 * Funcao    : void printResultSet(string *returnText, list<SimpleNode> resultSet, float totalCost, double timeElapsed, CARDINAL seed, bool useSeed, float bestAlfa, bool useBestAlfa) *
 * Descricao : Cria um texto com a descrição da execução de uma operação de algoritmos gulosos, inclusos os vértices elencados no conjunto solução                                     *
 * Parametros: returnText - texto construído a ser retornado                                                                                                                           *
 *             resultSet - conjunto solução do algoritmo                                                                                                                               *
 *             totalCost - custo total da solução                                                                                                                                      *
 *             timeElapsed - tempo de CPU decorrido no experimento                                                                                                                     *
 *             seed - semente de randomização utilizada                                                                                                                                *
 *             useSeed - define se imprimirá a semente de randomização                                                                                                                 *
 *             bestAlfa - melhor valor de alfa encontrado pelo algoritmo                                                                                                               *
 *             useAlfa - define se imprimirá o melhor valor de alfa                                                                                                                    *
 * Retorno   : sem retorno.                                                                                                                                                            *
 ***************/
void printResultSet(string *returnText, list<SimpleNode> resultSet, float totalCost, double timeElapsed, CARDINAL seed, bool useSeed, float bestAlfa, bool useBestAlfa)
{
    *returnText += "Custo: " + to_string(totalCost) + "\n";
    *returnText += "Vertices: " + to_string(resultSet.size()) + "\n";
    *returnText += "Tempo: " + to_string(timeElapsed) + "\n";
    if (useSeed)
        *returnText += "Semente: " + to_string(seed) + "\n";
    if (useBestAlfa)
        *returnText += "Melhor alfa: " + to_string(bestAlfa) + "\n";
    *returnText += "Label\t|\tCusto\n";
    for (auto &&node : resultSet)
        *returnText += formatInt(node.label, 4) + "\t|\t" + formatFloat(node.weight, 3, 6) + "\n";
}

/****************
 * Funcao    : void printResultVariables(string *returnText, int qtdNodes, float totalCost, double timeElapsed, CARDINAL seed, bool useSeed, float bestAlfa, bool useBestAlfa) *
 * Descricao : Cria um texto com a descrição da execução de uma operação de algoritmos gulosos, sem incluir os vértices elencados no conjunto solução                          *
 * Parametros: returnText - texto construído a ser retornado                                                                                                                   *
 *             qtdNodes - quantidade de vértices no conjunto solução do algoritmo                                                                                              *
 *             totalCost - custo total da solução                                                                                                                              *
 *             timeElapsed - tempo de CPU decorrido no experimento                                                                                                             *
 *             seed - semente de randomização utilizada                                                                                                                        *
 *             useSeed - define se imprimirá a semente de randomização                                                                                                         *
 *             bestAlfa - melhor valor de alfa encontrado pelo algoritmo                                                                                                       *
 *             useAlfa - define se imprimirá o melhor valor de alfa                                                                                                            *
 * Retorno   : sem retorno.                                                                                                                                                    *
 ***************/
void printResultVariables(string *returnText, int qtdNodes, float totalCost, double timeElapsed, CARDINAL seed, bool useSeed, float bestAlfa, bool useBestAlfa)
{
    *returnText += "Custo: " + to_string(totalCost) + "\n";
    *returnText += "Vertices: " + to_string(qtdNodes) + "\n";
    *returnText += "Tempo: " + to_string(timeElapsed) + "\n";
    if (useSeed)
        *returnText += "Semente: " + to_string(seed) + "\n";
    if (useBestAlfa)
        *returnText += "Melhor alfa: " + to_string(bestAlfa) + "\n";
}

/****************
 * Funcao    : bool verifyIfNodeCanBeUsedPredecessors(list<Edge *> predecessorEdges, bool *isInSolution, int nodeId) *
 * Descricao : Verifica se todos os predecessores de um nó estão na solução da PERT, e se o próprio já não está      *
 * Parametros: predecessorEdges - lista de arestas que têm dado vértice como destino                                 *
 *             isInSolution - vetor que indica se um vértice em determinada posição está ou não na solução           *
 *             nodeId - ID de um vértice                                                                             *
 * Retorno   : booleano que indica se um vértice pode ser testado para entrar na solução da PERT.                    *
 ***************/
bool verifyIfNodeCanBeUsedPredecessors(list<Edge *> predecessorEdges, bool *isInSolution, int nodeId)
{
    if (isInSolution[nodeId])
        return false;
    for (auto &&e : predecessorEdges)
    {
        if (!isInSolution[e->getSourceId()])
            return false;
    }
    return true;
}

/****************
 * Funcao    : bool verifyIfNodeCanBeUsedSucessors(list<Edge *> sucessorEdges, bool *isInSolution, int nodeId) *
 * Descricao : Verifica se todos os sucessores de um nó estão na solução da PERT, e se o próprio já não está   *
 * Parametros: sucessorEdges - lista de arestas que têm dado vértice como origem                               *
 *             isInSolution - vetor que indica se um vértice em determinada posição está ou não na solução     *
 *             nodeId - ID de um vértice                                                                       *
 * Retorno   : booleano que indica se um vértice pode ser testado para entrar na solução da PERT.              *
 ***************/
bool verifyIfNodeCanBeUsedSucessors(list<Edge *> sucessorEdges, bool *isInSolution, int nodeId)
{
    if (isInSolution[nodeId])
        return false;
    for (auto &&e : sucessorEdges)
    {
        if (!isInSolution[e->getTargetId()])
            return false;
    }
    return true;
}

/****************
 * Funcao    : list<Edge *> getPredecessorEdges(Graph *graph, int nodeId)                           *
 * Descricao : Busca todas as arestas que têm dado vértice como destino                             *
 * Parametros: graph - grafo onde a busca é feita                                                   *
 *             nodeId - ID de um vértice                                                            *
 * Retorno   : Lista de ponteiros para as arestas predecessoras do vértice definido nos parâmetros. *
 ***************/
list<Edge *> getPredecessorEdges(Graph *graph, int nodeId)
{
    Node *auxNode = graph->getFirstNode();
    Edge *auxEdge;
    list<Edge *> predecessorEdges;
    while (auxNode != nullptr)
    {
        auxEdge = auxNode->getFirstEdge(); // Aresta auxiliar recebe primeira aresta do primeiro nó do grafo original
        // Enquanto há arestas em um nó...
        while (auxEdge != nullptr)
        {
            if (auxEdge->getTargetId() == nodeId)
                predecessorEdges.emplace_back(auxEdge);
            auxEdge = auxEdge->getNextEdge();
        }
        // Avança para o próximo nó
        auxNode = auxNode->getNextNode();
    }
    return predecessorEdges;
}

/****************
 * Funcao    : list<Edge *> getSucessorEdges(Graph *graph, int nodeId)                           *
 * Descricao : Busca todas as arestas que têm dado vértice como origem                           *
 * Parametros: graph - grafo onde a busca é feita                                                *
 *             nodeId - ID de um vértice                                                         *
 * Retorno   : Lista de ponteiros para as arestas sucessoras do vértice definido nos parâmetros. *
 ***************/
list<Edge *> getSucessorEdges(Graph *graph, int nodeId)
{
    Node *auxNode = graph->getFirstNode();
    Edge *auxEdge;
    list<Edge *> sucessorEdges;
    while (auxNode != nullptr)
    {
        if (auxNode->getId() == nodeId)
        {
            auxEdge = auxNode->getFirstEdge(); // Aresta auxiliar recebe primeira aresta do primeiro nó do grafo original
            // Enquanto há arestas em um nó...
            while (auxEdge != nullptr)
            {
                sucessorEdges.emplace_back(auxEdge);
                auxEdge = auxEdge->getNextEdge();
            }
        }
        // Avança para o próximo nó
        auxNode = auxNode->getNextNode();
    }
    return sucessorEdges;
}

/****************
 * Funcao    : string pert(Graph *originalGraph)                                                           *
 * Descricao : Constói a ordenação de tarefas do caminho crítico de um grafo pela rede PERT                *
 * Parametros: originalGraph - grafo onde a rede PERT é verificada                                         *
 * Retorno   : string com os resultados da duração total de um projeto e sua ordenação de caminho crítico. *
 ***************/
string pert(Graph *originalGraph)
{
    // Variáveis auxiliares
    int numberOfNodes = 0;
    Node *auxNode = originalGraph->getFirstNode(); // Nó auxiliar, recebe o primeiro nó do grafo original
    bool *isInSolution = new bool[originalGraph->getOrder()];
    float *alfa = new float[originalGraph->getOrder()];
    float *beta = new float[originalGraph->getOrder()];

    for (int i = 0; i < originalGraph->getOrder(); i++)
    {
        isInSolution[i] = false;
        alfa[i] = 0;
        beta[i] = 0;
    }
    isInSolution[0] = true;

    // Cálculo dos alfas
    auxNode = auxNode->getNextNode();
    while (numberOfNodes < originalGraph->getOrder() - 1)
    {
        int i;
        list<Edge *> predecessorEdges;
        for (i = 0; i < originalGraph->getOrder(); i++)
        {
            predecessorEdges = getPredecessorEdges(originalGraph, i);
            if (verifyIfNodeCanBeUsedPredecessors(predecessorEdges, isInSolution, i))
                break;
        }

        float maximum = 0;
        for (auto &&e : predecessorEdges)
        {
            float localMaximum = alfa[e->getSourceId()] + e->getWeight();
            if (localMaximum > maximum)
                maximum = localMaximum;
        }

        alfa[i] = maximum;
        isInSolution[i] = true;
        numberOfNodes++;
    }

    numberOfNodes = 0;
    for (int i = 0; i < originalGraph->getOrder() - 1; i++)
        isInSolution[i] = false;
    isInSolution[originalGraph->getOrder() - 1] = true;

    // Cálculo dos betas
    beta[originalGraph->getOrder() - 1] = alfa[originalGraph->getOrder() - 1];
    while (numberOfNodes < originalGraph->getOrder() - 1)
    {
        int i;
        list<Edge *> sucessorEdges;
        for (i = originalGraph->getOrder() - 1; i >= 0; i--)
        {
            sucessorEdges = getSucessorEdges(originalGraph, i);
            if (verifyIfNodeCanBeUsedSucessors(sucessorEdges, isInSolution, i))
                break;
        }

        float minimum = FLT_MAX;
        for (auto &&e : sucessorEdges)
        {
            float localMinimum = beta[e->getTargetId()] - e->getWeight();
            if (localMinimum < minimum)
                minimum = localMinimum;
        }

        beta[i] = minimum;
        isInSolution[i] = true;
        numberOfNodes++;
    }

    // Encontra caminho crítico
    SimpleEdge *criticalTasks = new SimpleEdge[originalGraph->getOrder() - 1];
    int i = 0;
    for (Node *n = originalGraph->getFirstNode(); n != nullptr; n = n->getNextNode())
    {
        for (Edge *e = n->getFirstEdge(); e != nullptr; e = e->getNextEdge())
        {
            float delta = beta[e->getTargetId()] - alfa[e->getSourceId()] - e->getWeight();
            if (delta == 0)
            {
                SimpleEdge simpleEdge;
                simpleEdge.sourceNodeId = e->getSourceId();
                simpleEdge.targetNodeId = e->getTargetId();
                simpleEdge.sourceNodeLabel = e->getSourceLabel();
                simpleEdge.targetNodeLabel = e->getTargetLabel();
                simpleEdge.weight = e->getWeight();
                criticalTasks[i] = simpleEdge;
                i++;
            }
        }
    }

    // Ordena tarefas do caminho crítico
    list<SimpleEdge> orderedTasks;
    float totalCost = 0;
    numberOfNodes = 0;
    for (int i = 0; i < originalGraph->getOrder() - 1; i++)
        isInSolution[i] = false;
    while (numberOfNodes < originalGraph->getOrder())
    {
        int i;
        list<Edge *> predecessorEdges;
        for (i = 0; i < originalGraph->getOrder(); i++)
        {
            predecessorEdges = getPredecessorEdges(originalGraph, i);
            if (verifyIfNodeCanBeUsedPredecessors(predecessorEdges, isInSolution, i))
                break;
        }

        bool stop = false;
        for (auto &&e : predecessorEdges)
        {
            for (int k = 0; k < originalGraph->getOrder() - 1; k++)
            {
                if (criticalTasks[k].sourceNodeId == e->getSourceId() && criticalTasks[k].targetNodeId == e->getTargetId())
                {
                    orderedTasks.emplace_back(criticalTasks[k]);
                    totalCost += criticalTasks[k].weight;
                    stop = true;
                    break;
                }
            }
            if (stop)
                break;
        }
        isInSolution[i] = true;
        numberOfNodes++;
    }

    // Retorno
    string returnText = "";
    returnText += "Duracao: " + to_string(totalCost) + "\n";
    for (auto &&edge : orderedTasks)
        returnText += "(" + formatInt(edge.sourceNodeLabel, 4) + "," +
                      formatInt(edge.targetNodeLabel, 4) + ")\t|\t" + formatFloat(edge.weight, 3, 6) + "\n";

    // Limpa memória
    delete[] isInSolution;
    delete[] alfa;
    delete[] beta;
    delete[] criticalTasks;

    return returnText;
}

/****************
 * Funcao    : bool showResponse(string dot, bool isResultSet)                                                                 *
 * Descricao : Imprime no terminal a solução encontrada por algum algoritmo e solicita a permissão de exportar para um arquivo *
 * Parametros: dot - string com o texto que descreve a solução encontrada                                                      *
 *             isResultSet - define se a solução é um grafo ou um conjunto de vértices                                         *
 * Retorno   : booleano com a permissão, ou não, para exportar a solução.                                                      *
 ***************/
bool showResponse(string dot, bool isResultSet)
{
    string response = "";

    if (isResultSet)
        cout << "CONJUNTO";
    else
        cout << "GRAFO";
    cout << endl
         << "--------" << endl
         << dot << endl;
    while (response != "S" && response != "s" && response != "N" && response != "n")
    {
        cout << "Deseja exportar este ";
        if (isResultSet)
            cout << "conjunto";
        else
            cout << "grafo";
        cout << "? Isso vai apagar o conteúdo atual do arquivo[S / n] ";
        cin >>
            response;
    }
    cout << endl
         << endl;
    return (response == "S" || response == "s");
}

/****************
 * Funcao    : int menu(string *errors, bool isSecondPart)                                                             *
 * Descricao : Imprime no terminal o menu de seleção de operações a serem executadas, e captura seleção do usuário     *
 * Parametros: errors - string que armazena os eventuais erros encontrados                                             *
 *             isSecondPart - define se o programa está sendo executado para a primeira ou a segunda parte do trabalho *
 * Retorno   : inteiro que representa a operação selecionada pelo usuário.                                             *
 ***************/
int menu(string *errors, bool isSecondPart)
{
    string selectedOption;

    cout << "MENU" << endl;
    cout << "----" << endl;
    if (isSecondPart)
    {
        cout << "Selecione o algoritmo de processamento" << endl;
        cout << "[1] Guloso" << endl;
        cout << "[2] Guloso randomizado" << endl;
        cout << "[3] Guloso randomizado reativo" << endl;
        cout << "[9] Imprimir grafo" << endl;
    }
    else
    {
        cout << "[1] Imprimir Grafo de Entrada" << endl;
        cout << "[2] Gerar Grafo Interseçao" << endl;
        cout << "[3] Gerar Grafo Uniao" << endl;
        cout << "[4] Gerar Grafo Diferença" << endl;
        cout << "[5] Gerar Rede PERT" << endl;
        cout << "[8] Imprimir Grafo Multi-aresta" << endl;
        cout << "[9] Imprimir Grafo Simples" << endl;
    }
    cout << "[0] Sair" << endl;

    cin >> selectedOption;

    try
    {
        return stoi(selectedOption);
    }
    catch (const std::invalid_argument &)
    {
        *errors = "ERRO: Letras são inválidas. Digite um número inteiro entre 0 e 10!\n\n";
        return OPTION_INVALID;
    }
    catch (const std::out_of_range &)
    {
        *errors == "ERRO: Fora dos limites. Digite um número inteiro entre 0 e 10!\n\n";
        return OPTION_INVALID;
    }
}

/****************
 * Funcao    : string selectOptionFirstPart(int *selectedOption, string *errors, Graph *firstGraph)  *
 * Descricao : Interpreta a seleção e executa a operação associada para a primeira parte do trabalho *
 * Parametros: selectedOption - seleção de operação a ser executada                                  *
 *             errors - string que armazena os eventuais erros encontrados                           *
 *             firstGraph - grafo base sobre o qual as operações serão executadas                    *
 * Retorno   : texto de descrição da solução encontrada pelas operações.                             *
 ***************/
string selectOptionFirstPart(int *selectedOption, string *errors, Graph *firstGraph)
{
    string returnText = "";
    int option = *selectedOption;
    switch (option)
    {
    case 0:
    {
        *selectedOption = OPTION_EXIT;
        break;
    }
    // Imprimir grafo de entrada
    case 1:
    {
        returnText = exportGraphToDotFormat(firstGraph);
        break;
    }
    // Grafo interseção
    case 2:
    {
        Graph *secondGraph = readAuxiliaryGraph(selectedOption, errors);
        Graph *thirdGraph = createIntersectionGraph(firstGraph, secondGraph);

        returnText = exportGraphToDotFormat(thirdGraph);
        delete secondGraph;
        secondGraph = nullptr;
        delete thirdGraph;
        thirdGraph = nullptr;
        break;
    }
    // Grafo união
    case 3:
    {
        Graph *secondGraph = readAuxiliaryGraph(selectedOption, errors);
        Graph *thirdGraph = createUnionGraph(firstGraph, secondGraph);

        returnText = exportGraphToDotFormat(thirdGraph);
        delete secondGraph;
        secondGraph = nullptr;
        delete thirdGraph;
        thirdGraph = nullptr;
        break;
    }
    // Grafo diferença
    case 4:
    {
        Graph *secondGraph = readAuxiliaryGraph(selectedOption, errors);
        Graph *thirdGraph = graphDifference(firstGraph, secondGraph);

        returnText = exportGraphToDotFormat(thirdGraph);
        delete secondGraph;
        secondGraph = nullptr;
        delete thirdGraph;
        thirdGraph = nullptr;
        break;
    }
    // Rede Pert
    case 5:
    {
        if (::directed && ::weightedEdge)
        {
            ifstream input_file;
            input_file.open(::input_file_name, ios::in);
            Graph *secondGraph = readFileFirstPart(input_file, ::directed, ::weightedEdge, ::weightedNode, true);
            input_file.close();
            returnText = pert(secondGraph);
            delete secondGraph;
            secondGraph = nullptr;
        }
        else
            *errors += "ERRO: Rede PERT precisa ser criada a partir de um grafo direcionado e ponderado nas arestas!\n";
        break;
    }
    // Impressão de grafo multi-aresta
    case 8:
    {
        ifstream input_file;
        input_file.open(::input_file_name, ios::in);
        Graph *secondGraph = readFileFirstPart(input_file, ::directed, ::weightedEdge, ::weightedNode, true);
        input_file.close();
        returnText = exportGraphToDotFormat(secondGraph, true);
        delete secondGraph;
        secondGraph = nullptr;
        break;
    }
    // Impressão qualquer
    case 9:
    {
        Graph *secondGraph = readAuxiliaryGraph(selectedOption, errors);
        returnText = exportGraphToDotFormat(secondGraph);
        delete secondGraph;
        secondGraph = nullptr;
        break;
    }
    default:
    {
        *selectedOption = OPTION_INVALID;
    }
    }
    return returnText;
}

/****************
 * Funcao    : string selectOptionSecondPart(int *selectedOption, string *errors, Graph *graph)     *
 * Descricao : Interpreta a seleção e executa a operação associada para a segunda parte do trabalho *
 * Parametros: selectedOption - seleção de operação a ser executada                                 *
 *             errors - string que armazena os eventuais erros encontrados                          *
 *             graph - grafo base sobre o qual as operações serão executadas                        *
 * Retorno   : texto de descrição da solução encontrada pelas operações.                            *
 ***************/
string selectOptionSecondPart(int *selectedOption, string *errors, Graph *graph)
{
    string returnText = "";
    int option = *selectedOption;
    switch (option)
    {
    case 0:
    {
        *selectedOption = OPTION_EXIT;
        break;
    }
    // Guloso
    case 1:
    {
        float totalCost = 0;
        double intialTime = cpuTime();
        list<SimpleNode> resultSet = graph->dominatingSetWeighted(&totalCost);
        double finalTime = cpuTime();
        double timeElapsed = finalTime - intialTime;
        printResultSet(&returnText, resultSet, totalCost, timeElapsed, 0, false, 0, false);
        break;
    }
    // Guloso randomizado
    case 2:
    {
        int numInter;
        float alfa;
        cout << "Quantas interações?" << endl;
        cin >> numInter;
        cout << "Qual a porcentagem do alfa?" << endl;
        cin >> alfa;
        cout << endl;

        float totalCost = 0;
        double intialTime = cpuTime();
        list<SimpleNode> resultSet = graph->dominatingSetWeightedRandomized(&totalCost, ::seed, numInter, alfa);
        double finalTime = cpuTime();
        double timeElapsed = finalTime - intialTime;
        printResultSet(&returnText, resultSet, totalCost, timeElapsed, ::seed, true, 0, false);
        break;
    }
    // Guloso randomizado reativo
    case 3:
    {
        int numInter;
        int bloco;
        float *alfa;
        int tam;

        cout << "Quantos valores de alfa?" << endl;
        cin >> tam;

        alfa = new float[tam];

        cout << "Quais serão os valores de alfa?" << endl;
        for (int i = 0; i < tam; i++)
        {
            cout << "Valor (" << i + 1 << "/" << tam << "): ";
            cin >> alfa[i];
        }

        cout << "Quantas interações?" << endl;
        cin >> numInter;

        cout << "Qual tamanho de cada bloco?" << endl;
        cin >> bloco;
        cout << endl;

        float totalCost = 0;
        double intialTime = cpuTime();
        float bestAlfa = 0;
        list<SimpleNode> resultSet = graph->dominatingSetWeightedRandomizedReactive(&totalCost, ::seed, numInter, alfa, tam, bloco, &bestAlfa);
        double finalTime = cpuTime();
        double timeElapsed = finalTime - intialTime;
        printResultSet(&returnText, resultSet, totalCost, timeElapsed, ::seed, true, bestAlfa, true);

        delete[] alfa;
        break;
    }
    // Impressão
    case 9:
    {
        returnText = exportGraphToDotFormat(graph);
        break;
    }
    default:
    {
        *selectedOption = OPTION_INVALID;
    }
    }
    return returnText;
}

/****************
 * Funcao    : int mainMenu(string outputFileName, Graph *graph, bool isSecondPart)                *
 * Descricao : Implementa interface de execução do programa, e exporta resultados para arquivo     *
 * Parametros: outputFileName - nome do arquivo de saída de dados                                  *
 *             graph - grafo base sobre o qual serão executadas as operações                       *
 *             isSecondPart - define se serão executadas as operações da segunda parte do trabalho *
 * Retorno   : inteiro que define se houve erro, ou não, na execução do programa.                  *
 ***************/
int mainMenu(string outputFileName, Graph *graph, bool isSecondPart)
{
    string returnText = "", errors = "";
    int selectedOption = OPTION_INVALID;

    // Loop de interface
    while (selectedOption != OPTION_EXIT)
    {
        // Impressão do grafo resultante, caso opção não seja inválida
        if (selectedOption != OPTION_INVALID)
        {
            bool shouldExport = showResponse(returnText, isSecondPart && selectedOption != 9);
            if (shouldExport)
            {
                ofstream output_file;
                output_file.open(outputFileName, ios::out | ios::trunc);
                output_file << returnText;
                output_file.close();
            }
        }

        system("clear");

        // Imprime erros
        cout << errors;
        errors = "";

        // Imprime menu de opções
        selectedOption = menu(&errors, isSecondPart);
        cout << endl;
        if (isSecondPart)
            returnText = selectOptionSecondPart(&selectedOption, &errors, graph);
        else
            returnText = selectOptionFirstPart(&selectedOption, &errors, graph);
    }
    return 0;
}

/*  Verifies each parameter of the command line,
 *   if it is set properly calls menu function
 *   else it shows the expected model just beforing ending the program
 */
/****************
 * Funcao    : int main(int argc, char const *argv[])                                                                                                       *
 * Descricao : Função principal, interpreta o tipo de execução pelos argumentos passados, abre arquivo de entrada de dados e define semente de randomização *
 * Parametros: argc - quantidade de argumentos passados na linha de comando                                                                                 *
 *             argv - vetor dos argumentos passados pela linha de comando                                                                                   *
 * Retorno   : inteiro que define se houve erro, ou não, na execução do programa.                                                                           *
 ***************/
int main(int argc, char const *argv[])
{
    // Verifica se todos os argumentos foram fornecidos
    if (argc == 3 || argc == 4 || argc == 6 || argc >= 8)
    {
        int endingCode = 0;
        ::seed = (unsigned)(time(NULL) & 0xFFFF) | (getpid() << 16);

        // Captura argumentos
        string program_name(argv[0]);
        ::input_file_name = argv[1];
        string output_file_name(argv[2]);

        // Abre arquivos de entrada e saída
        ifstream input_file;
        ofstream output_file;
        input_file.open(input_file_name, ios::in);

        Graph *graph;

        if (input_file.is_open())
        {
            // Executa versão adequada do programa
            if (argc == 6 && atoi(argv[3]) != 2)
            {
                graph = readFileFirstPart(input_file, atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), false);
                endingCode = mainMenu(output_file_name, graph, false);
            }
            else
            {
                graph = readFileSecondPart(input_file);
                // Menu da segunda parte
                if (argc == 3)
                    endingCode = mainMenu(output_file_name, graph, true);
                else
                {
                    string returnText = "";
                    float totalCost = 0;
                    // Guloso
                    if (argc == 4)
                    {
                        // program input output 1
                        double intialTime = cpuTime();
                        list<SimpleNode> resultSet = graph->dominatingSetWeighted(&totalCost);
                        double finalTime = cpuTime();
                        double timeElapsed = finalTime - intialTime;
                        printResultVariables(&returnText, resultSet.size(), totalCost, timeElapsed, 0, false, 0, false);
                    }
                    else if (argc == 6)
                    {
                        // program input output 2 numIterations alfa
                        double intialTime = cpuTime();
                        list<SimpleNode> resultSet = graph->dominatingSetWeightedRandomized(&totalCost, ::seed, atoi(argv[4]), atof(argv[5]));
                        double finalTime = cpuTime();
                        double timeElapsed = finalTime - intialTime;
                        printResultVariables(&returnText, resultSet.size(), totalCost, timeElapsed, ::seed, true, 0, false);
                    }
                    else if (argc >= 8)
                    {
                        // program input output 3 numIterations block numAlfas alfas...
                        int numAlfas = atoi(argv[6]);
                        float *vetAlfas = new float[numAlfas];
                        for (int i = 7; i < 7 + numAlfas; i++)
                            vetAlfas[i - 7] = atof(argv[i]);

                        double intialTime = cpuTime();
                        float bestAlfa = 0;
                        list<SimpleNode> resultSet = graph->dominatingSetWeightedRandomizedReactive(&totalCost, ::seed, atoi(argv[4]), vetAlfas, atoi(argv[6]), atoi(argv[5]), &bestAlfa);
                        double finalTime = cpuTime();
                        double timeElapsed = finalTime - intialTime;
                        printResultVariables(&returnText, resultSet.size(), totalCost, timeElapsed, ::seed, true, bestAlfa, true);
                    }
                    ofstream output_file;
                    output_file.open(output_file_name, ios::out | ios::trunc);
                    output_file << returnText;
                    output_file.close();
                }
            }
        }
        else
        {
            cout << "ERRO: Não foi possível abrir o arquivo de entrada " << input_file_name << "!" << endl;
            return 1;
        }

        // Fecha arquivo de entrada
        input_file.close();
        // Fecha arquivo de saída
        output_file.close();

        return endingCode;
    }
    else
    {
        cout << "ERRO: Espera-se: ./<program_name> <input_file> [<output_file> <directed> <weighted_edge> <weighted_node>]" << endl;
        return 1;
    }
}