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

//�˴���������Ϊѭ������
cyc_list_t* Creat_List(int Num)
{
    int i;
    cyc_list_t* Ptr = (cyc_list_t*)malloc(sizeof(cyc_list_t));//�����һ����Ա�ռ�
    cyc_list_t* Ptr_Heap = Ptr;//��¼�׸���Ա�ĵ�ַ�����ڷ���
    
    for (i = 1; i <= Num; i++)
    {
        Ptr->Data = ex_route[i];
//        printf("%d\n",ex_route[i]);
        if (i == (Num))//�Ѿ������һ����Ա�ˣ�������һ��ָ���һ����Ա
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
        //����ϵתָ����
        convert_to_compass(&(Ptr->next_angle));
				
        Ptr->Next = (cyc_list_t*)malloc(sizeof(cyc_list_t));//�洢��һ����Ա�����ĵ�ַ
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
        Ptr = List->Next;//�ȴ洢��һ����Ա
        free(List);//�ͷŵ�ǰ��
        List = Ptr;
    }
    free(Ptr);
}

//ɾ����������ĳ�Ա����󷵻��׳�Ա
cyc_list_t* Delete_list_Num(cyc_list_t* List, int Numaddr, int List_Num)
{
    int i;
    cyc_list_t* Ptr=(cyc_list_t*)malloc(sizeof(cyc_list_t));
    cyc_list_t* Ptr_init = List;

    for (i = 1; i < List_Num; i++)//��������
    {
        if (i == Numaddr - 1)//���ǵ�һ����Ա
        {
            Ptr = List->Next;
            List->Next = List->Next->Next;
            free(Ptr);
            return Ptr_init;

        }
        else if (Numaddr == 1)//ɾ����һ����Ա
        {
            if (i == 1)
            {
                Ptr = List;//�ȱ���Ҫɾ����Ա
                List = List->Next;
            }
            else if (i == List_Num - 1)//���������һ����Ա������ָ��ɾ���������һ����Ա
            {
                List->Next = Ptr->Next;
                free(Ptr);//ɾ��
                break;
            }
        }
        List = List->Next;
    }
    return List->Next;
}