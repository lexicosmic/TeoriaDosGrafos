#include "Graph.h"
#include "Edge.h"
#include <math.h>
#include <cstdlib>
#include <algorithm>
#include <climits>
#include <cfloat>
#include <fstream>
#include <list>
#include <unistd.h>
using namespace std;

/**************************************************************************************************
 * Defining the Graph's methods
 **************************************************************************************************/

// Construtor
Graph::Graph(int order, bool directed, bool weightedEdge, bool weightedNode)
{
    this->order = order;
    this->directed = directed;
    this->weightedEdge = weightedEdge;
    this->weightedNode = weightedNode;
    this->firstNode = this->lastNode = nullptr;
    this->numberEdges = 0;
    this->nodeIdCounter = 0;
}

// Destrutor
Graph::~Graph()
{
    Node *nextNode = this->firstNode;

    while (nextNode != nullptr)
    {
        Node *auxNode = nextNode->getNextNode();
        delete nextNode;
        nextNode = auxNode;
    }

    this->order = 0;
    this->numberEdges = 0;
    this->nodeIdCounter = 0;
    this->firstNode = nullptr;
    this->lastNode = nullptr;
}

// Getters
int Graph::getOrder()
{
    return this->order;
}
int Graph::getNodeIdCounter()
{
    return this->nodeIdCounter;
}
int Graph::getNumberEdges()
{
    return this->numberEdges;
}
bool Graph::getDirected()
{
    return this->directed;
}
bool Graph::getWeightedEdge()
{
    return this->weightedEdge;
}
bool Graph::getWeightedNode()
{
    return this->weightedNode;
}
Node *Graph::getFirstNode()
{
    return this->firstNode;
}
Node *Graph::getLastNode()
{
    return this->lastNode;
}

// Métodos de manipulação
//

/****************
 * Funcao    : void fixOrder()                                  *
 * Descricao : Atualiza o número de vértices (ordem) do grafo   *
 * Parametros: Sem parâmetros.                                  *
 * Retorno   : Sem retorno.                                     *
 ***************/
void Graph::fixOrder()
{
    this->order = nodeIdCounter;
}

/****************
 * Funcao    : Node insertNode(int label)                                  *
 * Descricao : Insere um novo nó no grafo                                  *
 * Parametros: label - label do nó a ser inserido.                         *
 * Retorno   : Retorna nó inserido.                                        *
 ***************/
Node *Graph::insertNode(int label)
{
    Node *newNode = new Node(nodeIdCounter++, label);
    if (lastNode == nullptr)
    {
        firstNode = newNode;
        lastNode = newNode;
    }
    else
    {
        lastNode->setNextNode(newNode);
        lastNode = newNode;
    }
    return newNode;
}

/****************
 * Funcao    : Node insertNode(int label, float weight)                    *
 * Descricao : Insere um novo nó com peso no grafo                         *
 * Parametros: label - label do nó a ser inserido                          *
               weight - peso do nó a ser inserido                          *
 * Retorno   : Retorna nó inserido.                                        *
 ***************/
Node *Graph::insertNode(int label, float weight)
{
    Node *newNode = this->insertNode(label);
    newNode->setWeight(weight);
    return newNode;
}

/****************
 * Funcao    : void insertEdge(int sourceLabel, int targetLabel, float weight, Node **sourceNode, Node **targetNode, bool isPERT)*                                  *
 * Descricao : Insere uma nova aresta no grafo                         *
 * Parametros: sourceLabel - etiqueta do nó origem                     *
               targetLabel - etiqueta do nó destino                    *
               weight - peso da areasta a ser inserida                 *
               sourceNode - nó de origem                               *
               targetNode - nó de destino                              *
               isPERT - se será usado para rede PERT                   *
 * Retorno   : Sem retorno.                                            *
 ***************/
void Graph::insertEdge(int sourceLabel, int targetLabel, float weight, Node **sourceNode, Node **targetNode, bool isPERT)
{
    if (sourceLabel == targetLabel)
        return;

    *sourceNode = getNodeByLabel(sourceLabel);
    if (*sourceNode == nullptr)
        if (nodeIdCounter < order)
            *sourceNode = insertNode(sourceLabel);
        else
            return;

    *targetNode = getNodeByLabel(targetLabel);
    if (*targetNode == nullptr)
        if (nodeIdCounter < order)
            *targetNode = insertNode(targetLabel);
        else
            return;
    int targetNodeId = (*targetNode)->getId();

    Edge *nextEdge = (*sourceNode)->getFirstEdge();
    bool alreadyExists = false;
    while (!isPERT && (nextEdge != nullptr))
    {
        if (nextEdge->getTargetId() == targetNodeId)
            alreadyExists = true;
        nextEdge = nextEdge->getNextEdge();
    }
    if (isPERT || !alreadyExists)
    {
        (*sourceNode)->insertEdge((*sourceNode)->getId(), sourceLabel, targetNodeId, targetLabel, weight);
        if (!directed)
        {
            (*targetNode)->insertEdge(targetNodeId, targetLabel, (*sourceNode)->getId(), sourceLabel, weight);
            (*sourceNode)->incrementInDegree();
            (*targetNode)->incrementOutDegree();
        }
    }

    (*sourceNode)->incrementOutDegree();
    (*targetNode)->incrementInDegree();
    numberEdges++;
}

/****************
 * Funcao    : void insertEdge(int sourceLabel, int targetLabel, float weight)  *
 * Descricao : Insere uma nova aresta no grafo                                  *
 * Parametros: sourceLabel - etiqueta do nó origem                              *
               targetLabel - etiqueta do nó destino                             *
               weight - peso da areasta a ser inserida                          *
 * Retorno   : Sem retorno.                                                     *
 ***************/
void Graph::insertEdge(int sourceLabel, int targetLabel, float weight)
{
    Node *sourceNode = nullptr;
    Node *targetNode = nullptr;
    this->insertEdge(sourceLabel, targetLabel, weight, &sourceNode, &targetNode, false);
}

/****************
 * Funcao    : void insertEdge(Node *sourceNode, Node *targetNode, float weight)    *
 * Descricao : Insere uma nova aresta no grafo                                      *
 * Parametros: sourceNode - nó de origem                                 *
               targetNode - nó de destino                                 *
               weight - peso da areasta a ser inserida                              *
 * Retorno   : Sem retorno.                                                         *
 ***************/
void Graph::insertEdge(Node *sourceNode, Node *targetNode, float weight)
{
    if (sourceNode != nullptr && targetNode != nullptr)
    {
        int sourceNodeId = sourceNode->getId();
        int targetNodeId = targetNode->getId();
        int sourceNodeLabel = sourceNode->getLabel();
        int targetNodeLabel = targetNode->getLabel();
        sourceNode->insertEdge(sourceNodeId, sourceNodeLabel, targetNodeId, targetNodeLabel, weight);
        if (!directed)
        {
            targetNode->insertEdge(targetNodeId, targetNodeLabel, sourceNodeId, sourceNodeLabel, weight);
            sourceNode->incrementInDegree();
            targetNode->incrementOutDegree();
        }
    }

    sourceNode->incrementOutDegree();
    targetNode->incrementInDegree();
    numberEdges++;
}

void Graph::removeNode(int id)
{
}

// Métodos auxiliares
//

/****************
 * Funcao    : bool searchNode(int id)                                  *
 * Descricao : Verifica se um nó existe através do um identificador     *
 * Parametros: id - identificador do nó a ser verificado                *
 * Retorno   : Retorna true se o nó existe.                             *
 ***************/
bool Graph::searchNode(int id)
{
    return this->getNodeById(id) != nullptr;
}

/****************
 * Funcao    : Node getNodeById(int id)                     *
 * Descricao : Consulta um nó pelo seu identificador.       *
 * Parametros: id - identificador do nó a ser consultado    *
 * Retorno   : Retorna o nó.                                *
 ***************/
Node *Graph::getNodeById(int id)
{
    Node *nextNode = this->firstNode;
    while (nextNode != nullptr)
    {
        if (nextNode->getId() == id)
            return nextNode;

        nextNode = nextNode->getNextNode();
    }
    return nullptr;
}

/****************
 * Funcao    : Node getNodeByLabel(int label)           *
 * Descricao : Consulta um nó pelo seu label.           *
 * Parametros: label - rótulo do nó a ser consultado    *
 * Retorno   : Retorna o nó.                            *
 ***************/
Node *Graph::getNodeByLabel(int label)
{
    Node *nextNode = this->firstNode;
    while (nextNode != nullptr)
    {
        if (nextNode->getLabel() == label)
            return nextNode;

        nextNode = nextNode->getNextNode();
    }
    return nullptr;
}

/****************
 * Funcao    : int getLabelById(int id)                                     *
 * Descricao : Consulta o label de um nó através de seu identificador.      *
 * Parametros: id - identificador do nó                                     *
 * Retorno   : Retorna o label do nó.                                       *
 ***************/
int Graph::getLabelById(int id)
{
    Node *node = this->getNodeById(id);
    if (node != nullptr)
        return node->getLabel();
    else
        return INT_MAX;
}

/****************
 * Funcao    : bool thereIsEdgeBetweenLabel(int sourceLabel, int targetLabel)   *
 * Descricao : Verifica se há uma aresta entre dois nós.                        *
 * Parametros: sourceLabel - label do nó origem                                 *
               targetLabel - label do nó destino                                *
 * Retorno   : Retorna true se a aresta é existente.                            *
 ***************/
bool Graph::thereIsEdgeBetweenLabel(int sourceLabel, int targetLabel)
{
    Node *sourceNode = this->getNodeByLabel(sourceLabel);
    return sourceNode->hasEdgeBetweenLabel(targetLabel);
}

/****************
 * Funcao    : bool depthFirstSearch(int initialId, int targetId)       *
 * Descricao : Verifica se há um caminho entre dois nós.                *
 * Parametros: initialId - identificador do nó inicial                  *
               targetId - identificador do nó final                     *
 * Retorno   : Retorna true se há um caminho existente.                 *
 ***************/
bool Graph::depthFirstSearch(int initialId, int targetId)
{
    bool *visitedList = new bool[this->nodeIdCounter];
    for (int i = 0; i < this->nodeIdCounter; i++)
        visitedList[i] = false;
    return depthFirstSearchAux(this->getNodeById(initialId), targetId, visitedList);
}

/****************
 * Funcao    : bool depthFirstSearchAux(Node *currentNode, int targetId, bool visitedList[])                *
 * Descricao : Realizar a busca em profundidade para tentar localizar o nó ferente ao targetId no grafo     *
 * Parametros: currentNode - nó atual                                                                       *
               targetId - identificador do nó destino                                                       *
               visitedList - Lista que informa quais nós foram visitados ou não                             *
 * Retorno   : Retorna true se o nó foi localizado dentro do grafo                                          *
 ***************/
bool Graph::depthFirstSearchAux(Node *currentNode, int targetId, bool visitedList[])
{
    int currentNodeId = currentNode->getId();
    if (currentNodeId == targetId)
        return true;
    if (visitedList[currentNodeId])
        return false;
    visitedList[currentNodeId] = true;

    for (Edge *e = currentNode->getFirstEdge(); e != nullptr; e = e->getNextEdge())
    {
        Node *nextNode = this->getNodeById(e->getTargetId());
        bool found = this->depthFirstSearchAux(nextNode, targetId, visitedList);
        if (found)
            return true;
    }
    return false;
}

/****************
 * Funcao    : bool isConnected()                                           *
 * Descricao : Verificar se o grafo é conexo                                *
 * Parametros: Sem parâmetros                                               *
 * Retorno   : Retorna true se o grafo é conexo                             *
 ***************/
bool Graph::isConnected()
{
    int numberOfNodes;
    int *nodeComponentList;
    int component = 0;
    this->createAuxNodeComponentArray(&numberOfNodes, &nodeComponentList);

    for (int i = 0; i < numberOfNodes; i++)
    {
        if (nodeComponentList[i] == -1)
        {
            component += 1;
            if (component != 1)
            {
                return false;
            }
            marksNodeComponent(i, component, &nodeComponentList);
        }
    }

    delete[] nodeComponentList;
    return true;
}

/****************
 * Funcao    : void marksNodeComponent(int idNode, int component, int **nodeComponentList) *
 * Descricao : Marcar a componente do nó passado como parâmetro e todos os seus adjacentes *
 * Parametros: idNode - id do nó a ser marcado                                             *
               component - número da componente que será marcado o nó                      *
               nodeComponentList - Array auxiliar que me informa a componente de cada nó   *
 * Retorno   : Sem retorno                                                                 *
 ***************/
void Graph::marksNodeComponent(int idNode, int component, int **nodeComponentList)
{
    (*nodeComponentList)[idNode] = component;

    Edge *initialEdge = this->getNodeById(idNode)->getFirstEdge();
    while (initialEdge != nullptr)
    {
        int idAdjacentNode = initialEdge->getTargetId();
        if ((*nodeComponentList)[idAdjacentNode] == -1)
        {
            marksNodeComponent(idAdjacentNode, component, nodeComponentList);
        }
        initialEdge = initialEdge->getNextEdge();
    }
}

/****************
 * Funcao    : void createAuxNodeComponentArray(int *size, int **componentList)                                                  *
 * Descricao : Instanciar e criar um array que representa a componente de cada nó                           *
 * Parametros: size - endereço da variavel que guardará a quantidade de nós no grafo                        *
               componentList- endereço do ponteiro onde instanciarei o array com os valores das componentes *
               nodeComponentList - Array auxiliar que me informa a componente de cada nó                    *
 * Retorno   : Sem retorno                                                                                  *
 ***************/
void Graph::createAuxNodeComponentArray(int *size, int **componentList)
{
    *componentList = new int[this->order];
    Node *nextNode = this->firstNode;
    int i = 0;
    while (nextNode != nullptr)
    {
        (*componentList)[i] = -1;
        nextNode = nextNode->getNextNode();
        i++;
    }
    *size = i;
}

/****************
 * Funcao    : float getWeightBetweenNodes(int sourceId, int targetId)  *
 * Descricao : Consulta o peso da aresta entre dois nós.                *
 * Parametros: sourceId - identificador do nó origem                    *
               targetId - identificador do nó destino                   *
 * Retorno   : Retorna o peso da aresta.                                *
 ***************/
float Graph::getWeightBetweenNodes(int sourceId, int targetId)
{
    return this->getNodeById(sourceId)->distanceToOtherNode(targetId);
}

// Algoritmos predefinidos
//

/****************
 * Funcao    : float floydMarshall()                                             *
 * Descricao : Encontrar o caminho minimo entre todos  os pares de nós do grafo  *
 * Parametros: Sem parametros                                                    *
 * Retorno   : Uma matriz com os valores do caminho minimo entre cada par de nós *
 ***************/
float **Graph::floydMarshall()
{
    float **minPathCurrent = new float *[this->order];
    float minPathPrevious[this->order][this->order];

    // Calcula distâncias entre todos os nós sem intermediários
    for (int i = 0; i < this->order; i++)
    {
        for (int j = 0; j < this->order; j++)
        {
            minPathCurrent[i] = new float[this->order];
            minPathPrevious[i][j] = this->getWeightBetweenNodes(i, j);
        }
    }

    for (int k = 0; k < this->order; k++)
    {
        for (int i = 0; i < this->order; i++)
        {
            for (int j = 0; j < this->order; j++)
            {
                // Verifica se é melhor usar o nó K como intermediário
                float previousPath = minPathPrevious[i][j];
                float pathUsingK = minPathPrevious[i][k] + minPathPrevious[k][j];
                minPathCurrent[i][j] = min(previousPath, pathUsingK);
            }
        }

        // Copia a matriz atual para a anterior
        for (int i = 0; i < this->order; i++)
            for (int j = 0; j < this->order; j++)
                minPathPrevious[i][j] = minPathCurrent[i][j];

        // // Impressão de cada iteração
        // for (int i = 0; i < this->getOrder(); i++)
        // {
        //     for (int j = 0; j < this->getOrder(); j++)
        //     {
        //         float path = minPathCurrent[i][j];
        //         string formatedFloat = "";
        //         if (path == FLT_MAX)
        //             formatedFloat += "   &";
        //         else
        //         {
        //             if (path < 10)
        //                 formatedFloat += "0";
        //             std::stringstream stream;
        //             stream << std::fixed << std::setprecision(1) << path;
        //             formatedFloat += stream.str();
        //         }
        //         cout << formatedFloat << " ";
        //     }
        //     cout << endl;
        // }
        // cout << endl;
    }

    return minPathCurrent;
}

/****************
 * Funcao    : float dijkstra(int startId)                                                                          *
 * Descricao : Calcula o custo do caminho mínimo a partir de um nó selecionado utilizando o algoritmo de Dijkstra.  *
 * Parametros: startId - identificador do nó inicial.                                                               *
 * Retorno   : Retorna o custo da solução.                                                                          *
 ***************/
float *Graph::dijkstra(int startId)
{
    bool *solution = new bool[this->order];
    int solutionQuantity = 1;
    float *minPath = new float[this->order];
    list<Node *> nodeList;
    list<SimpleEdge> incidentEdges = this->firstNode->distanceToConnectedNodes();

    for (Node *n = this->firstNode; n != nullptr; n = n->getNextNode())
        nodeList.emplace_back(n);
    nodeList.sort([](Node *const &node1, Node *const &node2)
                  { return node1->getId() < node2->getId(); });

    solution[0] = true;
    minPath[0] = 0;
    for (int auxNodeId = 1; auxNodeId < this->order; auxNodeId++)
    {
        solution[auxNodeId] = false;
        minPath[auxNodeId] = FLT_MAX;
        for (auto &&e : incidentEdges)
        {
            if (e.targetNodeId == auxNodeId)
                minPath[auxNodeId] = e.weight;
        }
    }

    // string printPartialResult = "[";
    // for (int i = 0; i < this->getOrder(); i++)
    // {
    //     int label = this->getNodeById(i)->getLabel();
    //     printPartialResult += "    " + to_string(label) + ", ";
    // }
    // printPartialResult.pop_back();
    // printPartialResult.pop_back();
    // printPartialResult += "]\n";
    // cout << printPartialResult;

    // printPartialResult = "[";
    // for (int i = 0; i < this->getOrder(); i++)
    // {
    //     float path = minPath[i];
    //     string formatedFloat = "";
    //     if (path == FLT_MAX)
    //         formatedFloat += "    &";
    //     else
    //     {
    //         if (path < 10)
    //             formatedFloat += "0";
    //         std::stringstream stream;
    //         stream << std::fixed << std::setprecision(2) << path;
    //         formatedFloat += stream.str();
    //     }
    //     printPartialResult += formatedFloat + ", ";
    // }
    // printPartialResult.pop_back();
    // printPartialResult.pop_back();
    // printPartialResult += "]\n";
    // cout << printPartialResult;

    while (solutionQuantity < this->order)
    {
        // Seleciona o nó com menor caminho
        int minPathNodeId;
        int m;
        for (m = 1; m < this->order; m++)
        {
            if (!solution[m])
            {
                minPathNodeId = m;
                break;
            }
        }
        float pathValue = 0;
        for (int n = m + 1; n < this->order; n++)
        {
            pathValue = minPath[n];
            if (!solution[n] && pathValue < minPath[minPathNodeId])
                minPathNodeId = n;
        }

        // Adiciona o nó com menor caminho à solução
        solution[minPathNodeId] = true;
        solutionQuantity++;
        Node *minPathNode = this->getNodeById(minPathNodeId);
        incidentEdges = minPathNode->distanceToConnectedNodes();

        // Recalcula caminho para nós adjacentes
        for (auto &&e : incidentEdges)
        {
            int targetNodeId = e.targetNodeId;
            float newPath = minPath[minPathNodeId] + e.weight;
            if (newPath < minPath[targetNodeId])
            {
                solution[targetNodeId] = false;
                solutionQuantity--;
                minPath[targetNodeId] = newPath;
            }
        }

        // printPartialResult = "[";
        // for (int i = 0; i < this->getOrder(); i++)
        // {
        //     float path = minPath[i];
        //     string formatedFloat = "";
        //     if (path == FLT_MAX)
        //         formatedFloat += "    &";
        //     else
        //     {
        //         if (path < 10)
        //             formatedFloat += "0";
        //         std::stringstream stream;
        //         stream << std::fixed << std::setprecision(2) << path;
        //         formatedFloat += stream.str();
        //     }
        //     printPartialResult += formatedFloat + ", ";
        // }
        // printPartialResult.pop_back();
        // printPartialResult.pop_back();
        // printPartialResult += "]\n";
        // cout << printPartialResult;
    }

    delete solution;
    return minPath;
}

/****************
 * Funcao    : Graph kruskal()                                                          *
 * Descricao : Encontra uma Arvore Geradora Mínima utilizando o algoritmo de Kruskal.   *
 * Parametros: Sem parâmetros.                                                          *
 * Retorno   : Retorna a AGM solução.                                                   *
 ***************/
Graph *Graph::kruskal()
{
    if (this->directed)
    {
        cout << "ERRO: Algoritmo de Kruskal nao funciona para grafos direcionados!";
        return nullptr;
    }
    if (!this->isConnected())
    {
        cout << "ERRO: Algoritmo de Kruskal nao funciona para grafos nao conexos!";
        return nullptr;
    }

    // Copia nós do grafo
    Graph *solutionGraph = new Graph(INT_MAX, directed, weightedEdge, weightedNode);
    for (Node *n = firstNode; n != nullptr; n = n->getNextNode())
        solutionGraph->insertNode(n->getLabel(), n->getWeight());
    solutionGraph->fixOrder();
    int *components = new int[this->order];
    for (int i = 0; i < this->order; i++)
        components[i] = i;

    // Cria lista de arestas
    list<SimpleEdge> allEdges;
    for (Node *n = this->firstNode; n != nullptr; n = n->getNextNode())
        for (Edge *e = n->getFirstEdge(); e != nullptr; e = e->getNextEdge())
        {
            SimpleEdge simpleEdge;
            simpleEdge.sourceNodeId = e->getSourceId();
            simpleEdge.targetNodeId = e->getTargetId();
            simpleEdge.sourceNodeLabel = e->getSourceLabel();
            simpleEdge.targetNodeLabel = e->getTargetLabel();
            simpleEdge.weight = e->getWeight();
            allEdges.emplace_back(simpleEdge);
        }
    allEdges.sort([](SimpleEdge const &edge1, SimpleEdge const &edge2)
                  { return edge1.weight < edge2.weight; });

    // Constrói árvore geradora mínima
    int i = 0;
    while (i < order - 1 && !allEdges.empty())
    {
        SimpleEdge currentEdge = allEdges.front();
        allEdges.pop_front();

        if (components[currentEdge.sourceNodeId] != components[currentEdge.targetNodeId])
        {
            solutionGraph->insertEdge(currentEdge.sourceNodeLabel, currentEdge.targetNodeLabel, currentEdge.weight);
            if (components[currentEdge.sourceNodeId] > components[currentEdge.targetNodeId])
            {
                int oldComponent = components[currentEdge.sourceNodeId];
                for (int j = 0; j < this->order; j++)
                    if (components[j] == oldComponent)
                        components[j] = components[currentEdge.targetNodeId];
            }
            else
            {
                int oldComponent = components[currentEdge.targetNodeId];
                for (int j = 0; j < this->order; j++)
                    if (components[j] == oldComponent)
                        components[j] = components[currentEdge.sourceNodeId];
            }
            i++;
        }

        for (int j = 0; j < this->order; j++)
            cout << components[j] << " ";
        cout << endl;
    }

    delete components;
    return solutionGraph;
}

/****************
 * Funcao    : Graph prim()                                                         *
 * Descricao : Encontra uma Arvore Geradora Mínima utilizando o algoritmo de Prim.  *
 * Parametros: Sem parâmetros.                                                      *
 * Retorno   : Retorna a AGM solução.                                               *
 ***************/
Graph *Graph::prim()
{
    if (this->directed)
    {
        cout << "ERRO: Algoritmo de Prim nao funciona para grafos direcionados!";
        return nullptr;
    }
    if (!this->isConnected())
    {
        cout << "ERRO: Algoritmo de Prim nao funciona para grafos nao conexos!";
        return nullptr;
    }

    // Cria lista de arestas
    list<SimpleEdge> allEdges;
    list<SimpleEdge> solutionEdges;
    for (Node *n = this->firstNode; n != nullptr; n = n->getNextNode())
        for (Edge *e = n->getFirstEdge(); e != nullptr; e = e->getNextEdge())
        {
            SimpleEdge simpleEdge;
            simpleEdge.sourceNodeId = e->getSourceId();
            simpleEdge.targetNodeId = e->getTargetId();
            simpleEdge.weight = e->getWeight();
            allEdges.emplace_back(simpleEdge);
        }
    SimpleEdge shortestEdge = allEdges.front();
    for (auto &&e : allEdges)
    {
        if (e.weight < shortestEdge.weight)
            shortestEdge = e;
    }

    // Cria listas auxiliares de distância
    int *nearestNodeList = new int[this->order];
    float *auxNodeList = new float[this->order];

    // Adiciona primeira aresta
    solutionEdges.emplace_back(shortestEdge);
    int sourceNodeId = shortestEdge.sourceNodeId;
    int targetNodeId = shortestEdge.targetNodeId;
    int k = 0;
    // Preenche distâncias para nós conectados aos presentes na menor aresta
    for (Node *n = this->firstNode; n != nullptr && k < this->order; n = n->getNextNode())
    {
        float distanceCurrentToSource = n->distanceToOtherNode(sourceNodeId);
        float distanceCurrentToTarget = n->distanceToOtherNode(targetNodeId);
        if (distanceCurrentToSource < distanceCurrentToTarget)
        {
            nearestNodeList[k] = sourceNodeId;
            auxNodeList[k] = distanceCurrentToSource;
        }
        else
        {
            nearestNodeList[k] = targetNodeId;
            auxNodeList[k] = distanceCurrentToTarget;
        }
        k++;
    }
    int static alreadyInSolution = -1;
    nearestNodeList[sourceNodeId] = alreadyInSolution;
    nearestNodeList[targetNodeId] = alreadyInSolution;

    // Seleção das arestas
    for (int i = 0; i < this->order - 2; i++)
    {
        // Seleciona id do nó de menor distância
        int minDistanceSourceId;
        int m;
        for (m = 0; m < this->order; m++)
        {
            if (nearestNodeList[m] > alreadyInSolution)
            {
                minDistanceSourceId = m;
                break;
            }
        }
        float distanceNodes = 0;
        for (int n = m + 1; n < this->order; n++)
        {
            distanceNodes = auxNodeList[n];
            if (nearestNodeList[n] > alreadyInSolution && distanceNodes < auxNodeList[minDistanceSourceId])
                minDistanceSourceId = n;
        }

        // Adiciona aresta de menor custo à solução
        SimpleEdge shortestAvailableEdge;
        shortestAvailableEdge.sourceNodeId = minDistanceSourceId;
        shortestAvailableEdge.targetNodeId = nearestNodeList[minDistanceSourceId];
        shortestAvailableEdge.weight = auxNodeList[minDistanceSourceId];
        solutionEdges.emplace_back(shortestAvailableEdge);

        // Atualiza listas de próximos nós e distâncias
        nearestNodeList[minDistanceSourceId] = alreadyInSolution;
        auxNodeList[minDistanceSourceId] = 0;
        Node *currentNode = this->getNodeById(minDistanceSourceId);
        for (int m = 0; m < this->order; m++)
        {
            if (nearestNodeList[m] > alreadyInSolution)
            {
                float distanceCurrentToM = currentNode->distanceToOtherNode(m);
                if (auxNodeList[m] > distanceCurrentToM)
                {
                    nearestNodeList[m] = minDistanceSourceId;
                    auxNodeList[m] = distanceCurrentToM;
                }
            }
        }

        // Impressão passo-a-passo
        //     cout << "J = " << currentNode->getLabel() << endl;
        //     for (int i = 0; i < this->order; i++)
        //         cout << "| " << this->getNodeById(i)->getLabel() << " ";
        //     cout << endl;
        //     for (int i = 0; i < this->order; i++)
        //         if (nearestNodeList[i] == -1)
        //             cout << "| A ";
        //         else
        //             cout << "| " << this->getNodeById(nearestNodeList[i])->getLabel() << " ";
        //     cout << endl;
        //     for (int i = 0; i < this->order; i++)
        //     {
        //         float distance = auxNodeList[i];
        //         if (distance == FLT_MAX)
        //             cout << "| $ ";
        //         else
        //             cout << "| " << distance << " ";
        //     }
        //     cout << endl
        //          << endl;
    }

    // Constrói grafo resultado
    Graph *solutionGraph = new Graph(order, directed, weightedEdge, weightedNode);
    for (auto &&e : solutionEdges)
    {
        Node *sourceNode = this->getNodeById(e.sourceNodeId);
        Node *targetNode = this->getNodeById(e.targetNodeId);
        solutionGraph->insertEdge(sourceNode->getLabel(), targetNode->getLabel(), e.weight);
    }
    solutionGraph->fixOrder();

    delete nearestNodeList;
    delete auxNodeList;
    return solutionGraph;
}

// Conjunto dominante
//

/****************
 * Funcao    : void printList(Node **nodeList, int size)                    *
 * Descricao : Imprime uma lista de nós (seu label, peso e grau)            *
 * Parametros: nodeList -  lista de nós a ser ordenada                      *
               size - tamanho da lista de nós a ser impressa                *
 * Retorno   : Sem retorno.                                                 *
 ***************/
void Graph::printList(Node **nodeList, int size)
{
    for (int i = 0; i < size; i++)
        cout << "[" << nodeList[i]->getLabel() << " (" << nodeList[i]->getWeight() << ", " << nodeList[i]->getInDegree() << ")] — ";
    cout << endl;
}

/****************
 * Funcao    : void isSolved(Node **nodeList, bool *nodeCovered, int size)      *
 * Descricao : Verifica se um conjunto de nós já foi completamente visitado     *
 * Parametros: nodeList -  lista de nós a ser verificada                        *
               nodeCovered - estado booleano de cobertura do nó                 *
               size - tamanho da lista de nós a ser verificada                  *
 * Retorno   : Retorna true se o conjunto de nós já foi completamente coberto.  *
 ***************/
bool Graph::isSolved(Node **nodeList, bool *nodeCovered, int size)
{
    for (int i = 0; i < size; i++)
        if (!nodeCovered[nodeList[i]->getId()])
            return false;
    return true;
}

/****************
 * Funcao    : Node copyNodePointersToArray(int *size, bool **coveredList)                                           *
 * Descricao : Captar todos os nós do grafo e copiar os seus respectivos ponteiros em um array                       *
 * Parametros: size - endereço que guardará o tamanho do meu array gerado                                            *
               coveredList - Ponteiro para uma representação de quais nós estarão cobertos pela solução encontrada   *
 * Retorno   : Retorna um array com os ponteiros para os nós do grafo                                                *
 ***************/
Node **Graph::copyNodePointersToArray(int *size, bool **coveredList)
{
    Node **nodeList = new Node *[this->order];
    *coveredList = new bool[this->order];
    Node *nextNode = this->firstNode;
    int i = 0;
    while (nextNode != nullptr)
    {
        nodeList[i] = nextNode;
        (*coveredList)[i] = false;
        nextNode = nextNode->getNextNode();
        i++;
    }
    *size = i;
    return nodeList;
}

/****************
 * Funcao    : void sortNodesByDegreeAndWeight(Node **nodeList, int size)   *
 * Descricao : Ordena uma lista de nós pela heurística: GRAU/PESO           *
 * Parametros: nodeList -  lista de nós a ser ordenada                      *
               size - tamanho da lista                                      *
 * Retorno   : Sem retorno.                                                 *
 ***************/
void Graph::sortNodesByDegreeAndWeight(Node **nodeList, int size)
{
    float currentHeuristic = 0;
    float nextHeuristic = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            currentHeuristic = nodeList[j]->getInDegree() / nodeList[j]->getWeight();
            nextHeuristic = nodeList[j + 1]->getInDegree() / nodeList[j + 1]->getWeight();
            if (currentHeuristic > nextHeuristic)
            {
                Node *auxNode = nodeList[j];
                nodeList[j] = nodeList[j + 1];
                nodeList[j + 1] = auxNode;
            }
        }
    }
}

/****************
 * Funcao    : void dominatingSetWeighted(float *totalCost)                                 *
 * Descricao : Encontra um subconjunto dominante ponderado com um algoritmo guloso          *
 * Parametros: totalCost - custo total da solução                                           *
 * Retorno   : Retorna uma lista de nós solução.                                            *
 ***************/
list<SimpleNode> Graph::dominatingSetWeighted(float *totalCost)
{
    list<SimpleNode> solutionSet;
    int candidates;
    bool *nodeCovered;
    Node **nodeList = this->copyNodePointersToArray(&candidates, &nodeCovered);

    // Processo iterativo
    while (candidates > 0)
    {
        // Encontra melhor nó
        this->sortNodesByDegreeAndWeight(nodeList, candidates);
        int bestIndex = candidates - 1;
        int bestId = nodeList[bestIndex]->getId();
        // printList(nodeList, candidates);
        // cout << "Escolhido: " << nodeList[bestIndex]->getLabel()
        //      << endl;

        // Adiciona nó à solução
        SimpleNode simpleNode;
        simpleNode.id = bestId;
        simpleNode.label = nodeList[bestIndex]->getLabel();
        simpleNode.degree = nodeList[bestIndex]->getInDegree();
        simpleNode.weight = nodeList[bestIndex]->getWeight();
        solutionSet.emplace_back(simpleNode);
        (*totalCost) += simpleNode.weight;
        nodeCovered[bestId] = true;

        // Marca nós ligados como cobertos
        for (Edge *e = nodeList[bestIndex]->getFirstEdge(); e != nullptr; e = e->getNextEdge())
        {
            int targetId = e->getTargetId();
            nodeCovered[targetId] = true;
        }

        // Remove nós cobertos
        int k = 0;
        for (int n = 0; n < candidates; n++)
        {
            if (nodeCovered[nodeList[n + k]->getId()])
            {
                candidates--;
                k++;
                n--;
            }
            else
                nodeList[n] = nodeList[n + k];
        }
        // printList(nodeList, candidates);
        // cout << "=-=-=" << endl;

        // Verifica cobertura total
        if (isSolved(nodeList, nodeCovered, candidates))
            break;
    }

    // Limpa a memória
    delete[] nodeList;
    delete[] nodeCovered;

    // cout << "\nCusto: " << *totalCost << "\n==========" << endl
    //      << endl;

    return solutionSet;
}

/****************
 * Funcao    : void dominatingSetWeightedRandomized(float *totalCost, int numIterations, float alfa)    *
 * Descricao : Encontra um subconjunto dominante ponderado com um algoritmo guloso randomizado          *
 * Parametros: totalCost -  custo total da solução                                                      *
               seed - semente de randomização                                                           *
               numIterations - número máximo de iterações do algoritmo                                  *
               alfa - valor de alfa                                                                     *
 * Retorno   : Retorna uma lista de nós solução.                                                        *
 ***************/
list<SimpleNode> Graph::dominatingSetWeightedRandomized(float *totalCost, CARDINAL seed, int numIterations, float alfa)
{
    list<SimpleNode> bestSolutionSet;
    *totalCost = FLT_MAX;
    xrandomize(seed);

    for (int z = 0; z < numIterations; z++)
    {
        // cout << "ITERACAO " << (z + 1) << endl
        //      << endl;
        float currentTotalCost = 0;
        list<SimpleNode> solutionSet;
        int candidates;
        bool *nodeCovered;
        Node **nodeList = this->copyNodePointersToArray(&candidates, &nodeCovered);
        while (candidates > 0)
        {
            // Seleciona um dentre os melhores nós
            this->sortNodesByDegreeAndWeight(nodeList, candidates);
            int randomPosition = xrandom(ceil(candidates * alfa)); // 0 a teto da seleção (excluído)
            int randomIndex = candidates - 1 - randomPosition;
            int bestId = nodeList[randomIndex]->getId();
            // printList(nodeList, candidates);
            // cout << "Escolhido: " << nodeList[randomIndex]->getLabel()
            //      << endl;

            // Adiciona à solução
            SimpleNode simpleNode;
            simpleNode.id = bestId;
            simpleNode.label = nodeList[randomIndex]->getLabel();
            simpleNode.degree = nodeList[randomIndex]->getInDegree();
            simpleNode.weight = nodeList[randomIndex]->getWeight();
            solutionSet.emplace_back(simpleNode);
            currentTotalCost += simpleNode.weight;
            nodeCovered[bestId] = true;

            // Marca nós ligados como cobertos
            for (Edge *e = nodeList[randomIndex]->getFirstEdge(); e != nullptr; e = e->getNextEdge())
            {
                int targetId = e->getTargetId();
                nodeCovered[targetId] = true;
            }

            // Remove nós cobertos
            int k = 0;
            for (int n = 0; n < candidates; n++)
            {
                if (nodeCovered[nodeList[n + k]->getId()])
                {
                    candidates--;
                    k++;
                    n--;
                }
                else
                    nodeList[n] = nodeList[n + k];
            }
            // printList(nodeList, candidates);
            // cout << "=-=-=" << endl;

            // Verifica cobertura total
            if (isSolved(nodeList, nodeCovered, candidates))
                break;
        }

        // Verifica se a nova solução gerada é melhor que a anterior
        if (currentTotalCost < *totalCost)
        {
            // Atualiza a lista da melhor solução;
            *totalCost = currentTotalCost;
            bestSolutionSet.clear();
            for (auto &&node : solutionSet)
                bestSolutionSet.emplace_back(node);
        }

        // Limpa a memória
        delete[] nodeList;
        delete[] nodeCovered;

        // cout << "\nCusto: " << *totalCost << "\n==========" << endl
        //      << endl;
    }

    return bestSolutionSet;
}

/****************
 * Funcao    : void dominatingSetWeightedRandomizedReactive(float *totalCost, int numIterations, float *vetAlfas, int m, int block)*
 * Descricao : Encontra um subconjunto dominante ponderado com um algoritmo guloso randomizado reativo   *
 * Parametros: totalCost -  custo total da solução                                                      *
               seed - semente de randomização                                                           *
               numIterations - número máximo de iterações do algoritmo                                  *
               vetAlfas - vetor de valores que alfa pode assumir                                        *
               tam - quantidade de alfas                                                                *
               block - tamanho do bloco de iterações                                                    *
               bestAlfa - melhor alfa encontrado para a resolução do problema                           *
 * Retorno   : Retorna uma lista de nós solução.                                                        *
 ***************/
list<SimpleNode> Graph ::dominatingSetWeightedRandomizedReactive(float *totalCost, CARDINAL seed, int numIterations, float *vetAlfas, int tam, int block, float *bestAlfa)
{
    list<SimpleNode> bestSolutionSet;
    *totalCost = FLT_MAX;
    xrandomize(seed);

    // Número de iterações para cada alfa
    int *iterEachAlfa;
    // Probabilidade de cada alfa
    float *probabilities;
    int *numIterPerAlfa;
    int counterChooseAlfa;
    int auxChooseAlfa;
    float alfa = 0;
    // Média da qualidade das soluções obtidas quando se utilizou cada alfa na construção;
    float *averages;

    initializeProbabilities(&iterEachAlfa, &probabilities, &numIterPerAlfa, &averages, tam);

    // Constrói uma solução
    for (int z = 0; z < numIterations; z++)
    {
        // cout << "ITERACAO " << (z + 1) << endl
        //      << endl;
        if (z % block == 0)
        {
            counterChooseAlfa = 0;
            auxChooseAlfa = 0;
            updateProbabilities(probabilities, averages, *totalCost, tam);
            int numIterPerAlfaPrevious = 0;
            for (int y = 0; y < tam - 1; y++)
            {
                numIterPerAlfa[y] = round(probabilities[y] * block) + numIterPerAlfaPrevious;
                numIterPerAlfaPrevious = numIterPerAlfa[y];
            }
            numIterPerAlfa[tam - 1] = block;
            alfa = vetAlfas[auxChooseAlfa];
        }

        // Escolher o alfa
        if (counterChooseAlfa == numIterPerAlfa[auxChooseAlfa])
        {
            auxChooseAlfa++;
            alfa = vetAlfas[auxChooseAlfa];
        }
        (iterEachAlfa[auxChooseAlfa])++;

        // Auxiliares para uma solução
        float currentTotalCost = 0;
        list<SimpleNode> solutionSet;
        int candidates;
        bool *nodeCovered;
        Node **nodeList = this->copyNodePointersToArray(&candidates, &nodeCovered);

        while (candidates > 0)
        {
            // Seleciona um dentre os melhores nós
            this->sortNodesByDegreeAndWeight(nodeList, candidates);
            int randomPosition = xrandom(ceil(candidates * alfa)); // 0 a teto da seleção (excluído)
            int randomIndex = candidates - 1 - randomPosition;
            int bestId = nodeList[randomIndex]->getId();
            // printList(nodeList, candidates);
            // cout << "Escolhido: " << nodeList[randomIndex]->getLabel()
            //      << endl;

            // Adiciona à solução
            SimpleNode simpleNode;
            simpleNode.id = bestId;
            simpleNode.label = nodeList[randomIndex]->getLabel();
            simpleNode.degree = nodeList[randomIndex]->getInDegree();
            simpleNode.weight = nodeList[randomIndex]->getWeight();
            solutionSet.emplace_back(simpleNode);
            currentTotalCost += simpleNode.weight;
            nodeCovered[bestId] = true;

            // Marca nós ligados como cobertos
            for (Edge *e = nodeList[randomIndex]->getFirstEdge(); e != nullptr; e = e->getNextEdge())
            {
                int targetId = e->getTargetId();
                nodeCovered[targetId] = true;
            }

            // Remove nós cobertos
            int k = 0;
            for (int n = 0; n < candidates; n++)
            {
                if (nodeCovered[nodeList[n + k]->getId()])
                {
                    candidates--;
                    k++;
                    n--;
                }
                else
                    nodeList[n] = nodeList[n + k];
            }
            // printList(nodeList, candidates);
            // cout << "=-=-=" << endl;

            // Verifica cobertura total
            if (isSolved(nodeList, nodeCovered, candidates))
                break;
        }

        updateAverages(vetAlfas, averages, iterEachAlfa, tam, currentTotalCost, auxChooseAlfa);
        counterChooseAlfa++;

        // Verifica se a nova solução gerada é melhor que a anterior
        if (currentTotalCost < *totalCost)
        {
            // Atualiza a lista da melhor solução;
            *totalCost = currentTotalCost;
            bestSolutionSet.clear();
            for (auto &&node : solutionSet)
                bestSolutionSet.emplace_back(node);
        }

        // Limpa a memória
        delete[] nodeList;
        delete[] nodeCovered;

        // cout << "\nCusto: " << *totalCost << "\n==========" << endl
        //      << endl;
    }

    int greatestProbabilityIndex = 0;
    for (int i = 1; i < tam; i++)
    {
        if (probabilities[i] > probabilities[greatestProbabilityIndex])
            greatestProbabilityIndex = i;
    }
    *bestAlfa = vetAlfas[greatestProbabilityIndex];

    // Limpa a memória
    delete[] iterEachAlfa;
    delete[] probabilities;
    delete[] numIterPerAlfa;
    delete[] averages;

    return bestSolutionSet;
}

/****************
 * Funcao    : void initializeProbabilities(int **iterEachAlfa, float **probabilities, int **numIterPerAlfa, float **averages, int m)*
 * Descricao : Inicializa os vetores auxiliares                                                     *
 * Parametros: iterEachAlfa - quntidade de iterações realizadas para cada alfa                      *
               probabilities - vetor que armazena a probabilidade de cada alfa ser escolhido        *
               numIterPerAlfa - quantidade máxima de iterações a serem realizaadas com cada alfa    *
               averages - tamanho dos vetores                                                       *
               m - tamanho dos vetores (quantidade de alfas)                                        *
 * Retorno   : Sem retorno.                                                                         *
 ***************/
void Graph ::initializeProbabilities(int **iterEachAlfa, float **probabilities, int **numIterPerAlfa, float **averages, int tam)
{
    *iterEachAlfa = new int[tam];
    *probabilities = new float[tam];
    *numIterPerAlfa = new int[tam];
    *averages = new float[tam];
    for (int i = 0; i < tam; i++)
    {
        (*iterEachAlfa)[i] = 0;
        (*probabilities)[i] = 1 / (float)tam;
        (*numIterPerAlfa)[i] = 0;
        (*averages)[i] = FLT_MAX;
    }
}

/****************
 * Funcao    : void updateProbabilities(float probabilities[], float averages[], float bestCost, int m)*                                *
 * Descricao : Atualiza a probabilidade de cada alfa ser selecionado                            *
 * Parametros: probabilities - vetor que armazena a probabilidade de cada alfa ser escolhido    *
               averages - vetor de médias das soluções obtidas por cada alfa                    *
               bestCost - custo da melhor solução obtida                                        *
               tam - tamanho dos vetores (quantidade de alfas)                                  *
 * Retorno   : Sem retorno.                                                                     *
 ***************/
void Graph::updateProbabilities(float probabilities[], float averages[], float bestCost, int tam)
{
    float *q = new float[tam];

    for (int i = 0; i < tam; i++)
    {
        q[i] = bestCost / averages[i];
    }

    float qSum = 0;
    for (int j = 0; j < tam; j++)
        qSum += q[j];

    for (int i = 0; i < tam; i++)
        probabilities[i] = q[i] / qSum;

    delete[] q;
}

/****************
 * Funcao    : void updateAverages(float vetAlfas[], float averages[], int iterEachAlfa[], int m, float currentCost, int auxChooseAlfa)*
 * Descricao : Atualiza a média da qualidade de cada alfa                            *
 * Parametros: velAlfas - vetor de valores que alfa pode assumir                     *
               averages - vetor de médias das soluções obtidas por cada alfa         *
               iterEachAlfa - quntidade de iterações realizadas para cada alfa       *
               tam - tamanho dos vetores (quantidade de alfas)                       *
               currentCost - custo atual da solução                                  *
               auxChooseAlfa - iterador relativa à posição do alfa atual no vetor    *
 * Retorno   : Sem retorno.                                                          *
 ***************/
void Graph::updateAverages(float vetAlfas[], float averages[], int iterEachAlfa[], int tam, float currentCost, int auxChooseAlfa)
{
    averages[auxChooseAlfa] = (averages[auxChooseAlfa] * (iterEachAlfa[auxChooseAlfa] - 1) + currentCost) / iterEachAlfa[auxChooseAlfa];
}