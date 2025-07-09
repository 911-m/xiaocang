#ifndef __CYC_LIST_H
#define __CYC_LIST_H
#include "stm32f4xx.h"
 
typedef struct cyc_list
{
    struct cyc_list* Next;
    int Data;
    double next_angle;
    double next_distance;
    double next_maplon;
    double next_maplat;
	
}cyc_list_t;

#define V_MAX 10

cyc_list_t* Creat_List(int Num);
void Free_List(cyc_list_t* List, int Num);
cyc_list_t* Delete_list_Num(cyc_list_t* List, int Numaddr, int List_Num);
void map_to_wgs(u8 MAX_NUM);

extern cyc_list_t* My_List;

#endif
