#include "cyc_list.h"
#include "main.h"

double GPS_arr[V_MAX][2]={{0,0},{118.08699,24.623113},{118.086706,24.623186},{118.087044,24.622879},{118.086985,24.622737}};//7


cyc_list_t* My_List;

void map_to_wgs(u8 MAX_NUM)
{
    for(int i=1;i<=MAX_NUM;i++)
    {
        gcj02_to_wgs84(&GPS_arr[i][0],&GPS_arr[i][1]);
    }

    for(int i=1;i<=MAX_NUM;i++)
    {
        printf("%lf,%lf\n",GPS_arr[i][0],GPS_arr[i][1]);
    }
}

//此创建的链表为循环链表
cyc_list_t* Creat_List(int Num)
{
    int i;
    cyc_list_t* Ptr = (cyc_list_t*)malloc(sizeof(cyc_list_t));//分配第一个成员空间
    cyc_list_t* Ptr_Heap = Ptr;//记录首个成员的地址，用于返回
    
    for (i = 1; i <= Num; i++)
    {
        Ptr->Data = ex_route[i];
//        printf("%d\n",ex_route[i]);
        if (i == (Num))//已经到最后一个成员了，将其下一个指向第一个成员
        {
            Ptr->Next = Ptr_Heap;
					
					  Ptr->Data = ex_route[i];					
            Ptr->next_maplon=GPS_arr[ex_route[1]][0];
            Ptr->next_maplat=GPS_arr[ex_route[1]][1];
					
            LongLat2XY(GPS_arr[ex_route[i]][0],GPS_arr[ex_route[i]][1],&Route_Msg.map_origin_x,&Route_Msg.map_origin_y);
            LongLat2XY(GPS_arr[ex_route[1]][0],GPS_arr[ex_route[1]][1],&Route_Msg.map_destination_x,&Route_Msg.map_destination_y);
            Ptr->next_distance=Distance(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/1000;
            Ptr->next_angle=Angle(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/PI*180;
            convert_to_compass(&(Ptr->next_angle));
            break;
        }
				
            Ptr->next_maplon=GPS_arr[ex_route[i+1]][0];
            Ptr->next_maplat=GPS_arr[ex_route[i+1]][1];
				
        LongLat2XY(GPS_arr[ex_route[i]][0],GPS_arr[ex_route[i]][1],&Route_Msg.map_origin_x,&Route_Msg.map_origin_y);
        LongLat2XY(GPS_arr[ex_route[i+1]][0],GPS_arr[ex_route[i+1]][1],&Route_Msg.map_destination_x,&Route_Msg.map_destination_y);
        Ptr->next_distance=Distance(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/1000;
        Ptr->next_angle=Angle(Route_Msg.map_origin_x,Route_Msg.map_origin_y,Route_Msg.map_destination_x,Route_Msg.map_destination_y)/PI*180;
        //坐标系转指南针
        convert_to_compass(&(Ptr->next_angle));
				
        Ptr->Next = (cyc_list_t*)malloc(sizeof(cyc_list_t));//存储下一个成员变量的地址
        Ptr = Ptr->Next;

    }
    return Ptr_Heap;
}

void Free_List(cyc_list_t* List, int Num)
{
    int i;
    cyc_list_t* Ptr = (cyc_list_t*)malloc(sizeof(cyc_list_t));
    for (i = 0; i < (Num - 1); i++)
    {
        Ptr = List->Next;//先存储下一个成员
        free(List);//释放当前的
        List = Ptr;
    }
    free(Ptr);
}

//删除环形链表的成员，最后返回首成员
cyc_list_t* Delete_list_Num(cyc_list_t* List, int Numaddr, int List_Num)
{
    int i;
    cyc_list_t* Ptr=(cyc_list_t*)malloc(sizeof(cyc_list_t));
    cyc_list_t* Ptr_init = List;

    for (i = 1; i < List_Num; i++)//遍历链表
    {
        if (i == Numaddr - 1)//不是第一个成员
        {
            Ptr = List->Next;
            List->Next = List->Next->Next;
            free(Ptr);
            return Ptr_init;

        }
        else if (Numaddr == 1)//删除第一个成员
        {
            if (i == 1)
            {
                Ptr = List;//先保存要删除成员
                List = List->Next;
            }
            else if (i == List_Num - 1)//遍历到最后一个成员，将其指向删除对象的下一个成员
            {
                List->Next = Ptr->Next;
                free(Ptr);//删除
                break;
            }
        }
        List = List->Next;
    }
    return List->Next;
}