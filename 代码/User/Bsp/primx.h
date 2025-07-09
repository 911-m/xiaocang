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

//����ͼ
typedef struct graph_node
{
    int vertex;
    int wright;
    struct graph_node *next;
} graph_node_t;

typedef struct graph
{
    int vertex_num;//������
    int edge_num;//����
    graph_node_t *adj_list;//�ڽӱ�
    //int **matrix;//�ڽӾ���
    int matrix[MAX][MAX];

} graph_t;

//�����������
typedef struct depth_first_search
{
    int *marked;//�������
    int *edge_to;//�ߵ�����
    int count;//��s��ͨ�Ķ�����
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
