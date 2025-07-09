#include "primx.h"
#include "main.h"

extern double GPS_arr[V_MAX][2];//7
volatile int ex_route[V_MAX]={0};;

stack_t stack_dfs;
graph_t un_greph;
depth_first_search_t dfs_t;
int smallest_tree_arr[V_MAX][2];

int MAXN = 20,INF = 0x3f3f3f;//定义一个INF表示无穷大。
int dist[V_MAX],res;
int prim_wright[V_MAX];
//我们用matrix数组存储这个图，dist[]储存到集合S的距离，res保存结果。
int book[V_MAX];//用book数组记录某个点是否加入到集合S中。
int dfs_cnt=2;

 

void stack_init(void)
{
    stack_dfs.sp=-1;
    stack_dfs.stack_mem=(int*)malloc(sizeof(int)*10);
    for(int i=0;i<10;i++)
    {
        stack_dfs.stack_mem[i]=0;
    }
}
void push(int stack[], int *top, int item) 
{
    (*top)++;
    stack[*top] = item;
}
int pop(int stack[], int *top) 
{
    int item;
    if (*top == -1) {
        return -1; // 表示栈为空
    } else {
        item = stack[*top];
        (*top)--;
        return item;
    }
}

//邻接表
void init_list_graph(int V,int E)
{

    //创建一个含有V个顶点但不含有边的图
    un_greph.vertex_num = V;
    un_greph.edge_num = 0;
    un_greph.adj_list = (graph_node_t *)malloc(sizeof(graph_node_t) * (V+1));
    for (int i = 0; i <= V; i++)
    {
        un_greph.adj_list[i].vertex = i;
        un_greph.adj_list[i].next = NULL;
    }

    un_greph.edge_num = E;
    //向图中添加一些边 边存储在数组中
    for (int i = 1; i < un_greph.edge_num; i++)
    {
        int v = smallest_tree_arr[i][0];
        int w = smallest_tree_arr[i][1];
        add_edge(v, w);
    }
}
//向图中添加一条边
void add_edge(int v,int w)
{
    graph_node_t *node = (graph_node_t *)malloc(sizeof(graph_node_t));
    node->vertex = w;
    node->next = un_greph.adj_list[v].next;//往头部添加节点
    un_greph.adj_list[v].next = node;

    node = (graph_node_t *)malloc(sizeof(graph_node_t));
    node->vertex = v;
    node->next = un_greph.adj_list[w].next;
    un_greph.adj_list[w].next = node;

}

void print_graph()
{
    for (int i = 1; i <= un_greph.vertex_num; i++)
    {
        printf("%d:", un_greph.adj_list[i].vertex);
        graph_node_t *node = un_greph.adj_list[i].next;//妙
        while (node)
        {
            printf("%d->", node->vertex);
            node = node->next;
        }
        printf("NULL\n");
    }
}

void free_graph(void)
{
	  graph_node_t *Ptr = (graph_node_t *)malloc(sizeof(graph_node_t));
    for (int i = 1; i <= un_greph.vertex_num; i++)
    {
        while (un_greph.adj_list[i].next)
        {
						Ptr=un_greph.adj_list[i].next->next;
            free(un_greph.adj_list[i].next);
					  un_greph.adj_list[i].next=Ptr;
        }
        printf("NULL\n");
    }
		free(Ptr);
}


void generateCompleteGraph(int n) 
{
    un_greph.vertex_num = n;
    un_greph.edge_num = n * (n - 1) / 2;
    // Initialize the graph with 0
    for (int i = 0; i <= n; i++) 
    {
        for (int j = 0; j <= n; j++) {
            un_greph.matrix[i][j] = 0;
        }
    }
    
    // Add edges between vertices
    for (int i = 1; i <= n; i++) 
    {
        for (int j = i+1; j <= n; j++) {
            LongLat2XY(GPS_arr[i][0],GPS_arr[i][1],&Route_Msg.map_origin_x,&Route_Msg.map_origin_y);
            LongLat2XY(GPS_arr[j][0],GPS_arr[j][1],&Route_Msg.map_destination_x,&Route_Msg.map_destination_y);
            un_greph.matrix[i][j] = (int)(Distance(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y));
            un_greph.matrix[j][i] = (int)(Distance(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y));
        }
    }
    
}
void print_matrix_graph()
{
    for (int i = 1; i <= un_greph.vertex_num; i++)
    {
        for (int j = 1; j <= un_greph.vertex_num; j++)
        {
            printf("%d ", un_greph.matrix[i][j]);
        }
        printf("\n");
    }
}



int min(int a,int b)
{
    return a<b?a:b;
}

void init_prim(void)
{
    int i;
    for(i=0;i<MAXN;i++)
    {
        dist[i]=INF;
        book[i]=0;
        prim_wright[i]=0;
    }
    res=0;
}
void prim(int v)
{
    int u=1; 
    int last_u=1;   
    dist[1]=0;
    book[1]=1;
    for(int i=2;i<=v;i++)
    {
        dist[i]=min(dist[i],un_greph.matrix[1][i]);
    }

    for(int i=2;i<=v;i++)
    {   
        int minX=INF;
        for(int j=2;j<=v;j++)
        {
            if(book[j]==0&&dist[j]<minX)
            {
                minX=dist[j];
                u=j;
   
            }
        }
        smallest_tree_arr[i-1][0]=last_u;
        smallest_tree_arr[i-1][1]=u;         
        last_u=u;  
        
        book[u]=1;
        res+=dist[u];
        prim_wright[i-1]=dist[u];

        for(int k=2;k<=v;k++)
        {
            if(book[k]==0)
            {
                dist[k]=INF;
            }  
        }

        for(int j=2;j<=v;j++)
        {
            if(book[j]==0&&dist[j]>un_greph.matrix[u][j])
            {
                dist[j]=un_greph.matrix[u][j];
            }
        }
    }
 
    printf("res=%d\n",res);
    for(int i=1;i<=v-1;i++)
    {
        printf("%d ",prim_wright[i]);
    }

}


void Minimum_Spanning_Tree(int v)
{
     printf("\n");
 

    printf("最小生成树的边为\n");
    for(int i=1;i<v;i++)
    {
        printf("%d %d\n",smallest_tree_arr[i][0],smallest_tree_arr[i][1]);
    }
    init_list_graph(v,v);
    printf("最小生成树的邻接表为\n");
    print_graph();
    
    
}

void dfs_init(graph_t *graph,int s)
{
    dfs_t.marked = (int *)malloc(sizeof(int) * (un_greph.vertex_num+1));//初始化逻辑数组
    dfs_t.edge_to = (int *)malloc(sizeof(int) * (un_greph.vertex_num+1));//初始化路径数组

    for (int i = 0; i <= un_greph.vertex_num; i++)
    {
        dfs_t.marked[i]=0;
        dfs_t.edge_to[i]=0;
    }

    dfs_t.count = 0;
    dfs_cnt=2;
    dfs(graph,s);
}


void dfs(graph_t *graph ,int v)
{
    dfs_t.marked[v] = 1;//标记
    dfs_t.count++;
    ex_route[1]=1;
    graph_node_t *node = graph->adj_list[v].next;
    
    while(node)
    {
        if(!dfs_t.marked[node->vertex])///如果没有被标记过
        {
            ex_route[dfs_cnt++]=node->vertex;
            dfs_t.edge_to[node->vertex]=v;//记录路径,记录父亲节点
            dfs(graph,node->vertex);//递归
        }
        node = node->next;
    }
}
//打印dfs结果
void print_dfs_result(graph_t *graph)
{
    printf("dfs结果为\n");
    for(int i=1;i<=un_greph.vertex_num;i++)
    {
        printf("edge_to[%d]=%d\n",i,dfs_t.edge_to[i]);
    }
   //打印路径
    printf("路径为\n");
    for(int i=1;i<=un_greph.vertex_num;i++)
    {
        printf("%d->",ex_route[i]);
    }
    printf("NULL\n");
}
 









