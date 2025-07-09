#ifndef __PRIMX_H
#define __PRIMX_H
#include "stm32f4xx.h"
#define MAX 10
#include "cyc_list.h"

typedef struct stack
{
    int sp;
    int *stack_mem;
}stack_t;

//无向图
typedef struct graph_node
{
    int vertex;
    int wright;
    struct graph_node *next;
} graph_node_t;

typedef struct graph
{
    int vertex_num;//顶点数
    int edge_num;//边数
    graph_node_t *adj_list;//邻接表
    //int **matrix;//邻接矩阵
    int matrix[MAX][MAX];

} graph_t;

//深度优先搜索
typedef struct depth_first_search
{
    int *marked;//标记数组
    int *edge_to;//边的数组
    int count;//与s连通的顶点数
} depth_first_search_t;

extern stack_t stack_dfs;
extern graph_t un_greph;
extern depth_first_search_t dfs_t;
extern volatile int ex_route[V_MAX];
 
void stack_init(void);
void push(int stack[], int *top, int item);
int pop(int stack[], int *top);

void init_list_graph(int V,int E);
void add_edge(int v,int w);
void init_prim(void);
void prim(int n);
void Minimum_Spanning_Tree(int n);

void generateCompleteGraph(int n);
void print_matrix_graph(void);
void free_graph(void);

void dfs_init(graph_t *graph,int s);
void dfs(graph_t *graph ,int v);
void print_dfs_result(graph_t *graph);

#endif
