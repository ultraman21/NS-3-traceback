#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/socket.h"
#include "ns3/netanim-module.h"
using namespace ns3;
#include <stdio.h>
#include <assert.h>
#include "string"
#include "strstream"
#include "iostream"
#include "typeinfo"
#include "vector"
// Default Network Topology
// LAN1(192.168.1.0)                                 LAN2(192.168.2.0)
// _________________                             __________________
// |     |    |     |                            |     |     |    |
//R5,1    2     3                          R9  5     6    7
//                  |                            |
//                  |                            |
//                  |net1                        |net2
//                  |                            |
//                  |         net3               |
//                 R1 ---------------------------R2
//                 | \                           |
//                 |  \                          |
//                 |   \      net5               |net6
//             net4|    ----------------------   |
//                 |                          \  |
//                 |                           \ |
//                 R3---------------------------R4     13   14   15
//                 |         net7                 |         |    |     |
//                 |net8                              ----------------------
//                 |                                   LAN4(192.168.4.0)
// 8    9   10  R16
// |____|____|_____|
//     LAN3(192.168.3.0)
//
// net1~net8的建立順序就是其命名順序，IP網段也是從10.1.1.0~10.1.8.0，metric均為1
// 因此，當LAN1、LAN2、LAN3分別發送數據給LAN4時，封包路徑沒有重合部分
//
//
#define node_type 0
#define node_name 10
#define node_dimension 20
#define node_IFnum 30
#define IF_linked_Router 40
#define draw_pic 50
NS_LOG_COMPONENT_DEFINE ("RipTraceback");
class Network_Info
{
public:
  char netName[11];
  char netNode1[11];
  char netNode2[11];
  int flag;//flag==0时，说明该条路线不限速

};
void vector_sort(std::vector<std::vector<int> > &vpic)
{//这里vpic用引用的方式，因此可以对其中的数据进行修改
  int i;//设置行数rows和列column,排序
  int j;
  int tmprow,tmpcolumn;
  for(i=1;i<=vpic[0][0]-1;i++)
  {
    for(j=i+1;j<=vpic[0][0];j++)
    {
      if(vpic[0][i]>vpic[0][j])
      {
        tmprow=vpic[0][i];
        tmpcolumn=vpic[1][i];
        vpic[0][i]=vpic[0][j];
        vpic[1][i]=vpic[1][j];
        vpic[0][j]=tmprow;
        vpic[1][j]=tmpcolumn;
      }
    }
  }
}
int GetId_start(char *recv)
{
  char recv1[10240];
  strcpy(recv1,recv);
  char str1[]=", id ";
  char *_id_start=strstr(recv1,str1);
  if(_id_start==NULL)return -1;
  else return  (_id_start - recv1 + 5);
}
int GetId_end(char *recv)
{
  char recv1[10240];
  strcpy(recv1,recv);
  char str2[]=", offset ";
  char *_id_end=strstr(recv1,str2);
  if(_id_end==NULL)return -1;
  else return  (_id_end - recv1);
}
bool Is_Victim_Sending_Packet(const char recv[10240],const char victim_IP[16])
{
  char recv1[10240];
  strcpy(recv1,recv);
  int i;
  for(i=0;victim_IP[i]!='\0';i++)
  {
    if(victim_IP[i] != recv1[i+4])return false;
  }
  return true;
}
bool Is_In_Packet_List(std::vector<int> Packet_List,int ID)
{//如果已经在atk_ID_List中，则不再存储
  std::vector<int>::iterator it;
  for(it=Packet_List.begin();it!=Packet_List.end();it++)
  {
    if(*it==ID)return true;
  }
  return false;
}
bool Extract_Identification(const char final_cmd[50],const char victim_IP[16] , std::vector<int> &_Atk_Id_List)
{//这里的atk_ID_List用引用的方式，因此可以对其中的数据进行修改
  FILE *fp = popen(final_cmd, "r");
  if(fp==NULL)
  {
    std::cout<<"script cannot start!"<<std::endl;
    return false;
  }
  char recv[10240];
  char recv2[10240];
  int lines_num = 0;
  while(fgets(recv,10240,fp) != NULL)
  {
    //std::cout<<recv<<std::endl;
    fgets(recv2,10240,fp) ;
    //std::cout<<recv2<<std::endl;
    if(Is_Victim_Sending_Packet(recv2,victim_IP))continue;
    lines_num++;
    int Id_Length = 0;
    int Id_Start = GetId_start(recv);
    if(Id_Start != -1)
    {
	      Id_Length = GetId_end(recv) - Id_Start;
        char ID[6]; //max:65535 and \0
        int i;
        for(i=0;i<Id_Length;i++)
        {
	         ID[i] = recv[Id_Start+i];
        }
        ID[i]='\0';
        if(atoi(ID) != 0)
	       {
	        if (!Is_In_Packet_List(_Atk_Id_List,atoi(ID))) {
            std::cout<<"Mark is "<<atoi(ID)<<std::endl;
            _Atk_Id_List.push_back(atoi(ID));
	          }
	       }
    }
  }
  pclose(fp);
  if(lines_num==0||_Atk_Id_List.size()==0)
  {
    std::cout<<"No packet meets our demands or is from Attacker!Please check our parameter!"<<std::endl;
    return false;
  }
  return true;
}
int substr(char dst[], char src[], int start, unsigned int len)
{
    assert(dst);
    assert(src);
    int ret = 0;
    while (start)
    {
        src++;
        start--;
    }
    if (strlen(src) < len)
    {
        len = strlen(src);
    }
    ret = len;
    while (len)
    {
        *dst++ = *src++;
        len--;
    }
    *dst = '\0';

    return ret;
}
bool Traceback(int thisID,std::vector<std::string> &Road_traceback,std::string pcap_file,bool done)
{//这里的Road_traceback用引用的方式，因此可以对其中的数据进行修改
  char thisRouter[11];
  int router_dimension_plus_LAN=0;
  int UI_i=0;
  int thismark=thisID;
  int mark_old=0;
  if(done){
    std::cout<<"已完成溯源"<<std::endl;
    return true;
  }
    FILE *fconfig = fopen("scratch/config.txt", "r");
    if(fconfig==NULL)
        {     std::cout<<"config.txt cannot load!"<<std::endl;
              return false; }
    char char_config_line[200];
    char char_config_10[11];
    int k;
    bool got_LAN_Node=false;
    if(Road_traceback.size()!=0)
    {
      got_LAN_Node=true;
      strcpy(thisRouter,(Road_traceback[Road_traceback.size()-1]).c_str());
      //std::cout<<"Read from Traceback, thisRouter:"<<thisRouter<<std::endl;
    }
    //读取一行数据
    while(fgets(char_config_line,200,fconfig) != NULL)
    {//提取第一组字符,判断是LAN还是Router
          memset(char_config_10,0,11);
          substr(char_config_10,char_config_line,node_type,10);
          char_config_10[10]=0;
          for(k=0;k<10;k++)//删除空格
                {if(char_config_10[k]==' ')  char_config_10[k]=0; }
          if(strcmp(char_config_10,"LAN")==0)//如果是LAN，则寻找是LAN中哪个节点
            {
              if(!got_LAN_Node)
              {
                memset(char_config_10,0,11);
                substr(char_config_10,char_config_line,node_name,10);
                char_config_10[10]=0;
                for(k=0;k<10;k++)
                      {if(char_config_10[k]==' ')  char_config_10[k]=0;}
                //还没找到victim，因此需要从pcap_file的名字中提取victim节点的名字
                if(pcap_file.length()>199)
                {
                  std::cout<<"文档路径过长，请检查！"<<std::endl;
                  return false;
                }
                char mid_str[200];//暂定其长度为200
                memset(mid_str,0,200);
                unsigned int victim_name_start;
                unsigned int victim_name_end;
                unsigned j;
                for(j=0;j<pcap_file.length();j++)
                    {     if(pcap_file[j]=='-')
                        {   victim_name_start=j;
                            break;
                        }
                    }
                for(j=pcap_file.length()-1;j>=0;j--)
                    {     if(pcap_file[j]=='-')
                          {
                            victim_name_end=j;
                          break;
                          }
                    }
                for(j=victim_name_start+1;j<victim_name_end;j++)
                {
                  mid_str[j-victim_name_start-1]=pcap_file[j];
                }
                //std::cout<<char_config_10<<mid_str<<std::endl;
                if(strcmp(char_config_10,mid_str)==0)
                  {
                    got_LAN_Node=true;
                  //找到lan节点，读取与他相连接的Router
                  memset(char_config_10,0,11);
                  substr(char_config_10,char_config_line,IF_linked_Router,10);
                  char_config_10[10]=0;
                  for(k=0;k<10;k++)
                      { if(char_config_10[k]==' ')  char_config_10[k]=0; }
                  strcpy(thisRouter,char_config_10);
                  //把这个与victim相连接的路由器写入路径
                  std::string mid_string=thisRouter;
                  std::cout<<"受害者 "<<mid_str<<" 与该边界路由器相连接:"<<mid_string<<std::endl;
                  Road_traceback.push_back(mid_string);
                  continue;
                }
                else
                  continue;
              }
              else
                continue;
            }
          else if(strcmp(char_config_10,"Router")==0)//不是LAN的节点,是一个Router，那么开始溯源
          {
            memset(char_config_10,0,11);
            substr(char_config_10,char_config_line,node_name,10);
            char_config_10[10]=0;
            for(k=0;k<10;k++)//删除空格
                {if(char_config_10[k]==' ')  char_config_10[k]=0;        }
            if(strcmp(thisRouter,char_config_10)==0)
            {//找到该Router，读取路由器维度
              memset(char_config_10,0,11);
              substr(char_config_10,char_config_line,node_dimension,10);
              char_config_10[10]=0;
              for(k=0;k<10;k++)//删除空格
                  { if(char_config_10[k]==' ')  char_config_10[k]=0;        }
              router_dimension_plus_LAN=atoi(char_config_10);
              //计算UI_i和mark_old
              UI_i=thismark%(router_dimension_plus_LAN)-1;
              if(UI_i<0)UI_i=UI_i+(router_dimension_plus_LAN);
              mark_old=(thismark-1)/(router_dimension_plus_LAN); //注意，这里的公式与理论不符合，请事后验证
              std::cout<<"追踪到该封包来自"<<thisRouter<<"的"<<UI_i<<"号Interface, 追踪mark由"<<thismark<<"更改为"<<mark_old<<std::endl;
              //std::cout<<thismark<<":"<<thisRouter<<":"<<router_dimension_plus_LAN<<":"<<UI_i<<":"<<mark_old<<std::endl;
              if(mark_old==0)//说明已经计算出边界路由器，准备读取
              {
                for(k=1;k<=router_dimension_plus_LAN;k++)
                {
                  if(fgets(char_config_line,200,fconfig)==NULL)
                  {
                      std::cout<<"路由器各个Interface读取失败！请检查config.txt中路由器interface信息是否正确！"<<std::endl;
                      fclose(fconfig);
                      return false;
                  }
                  if (k==UI_i)
                  {
                    memset(char_config_10,0,11);
                    substr(char_config_10,char_config_line,node_IFnum,10);
                    char_config_10[10]=0;
                    for(k=0;k<10;k++)//删除空格
                        {  if(char_config_10[k]==' ')  char_config_10[k]=0;        }
                    if(UI_i==atoi(char_config_10))
                    {//找到了该IF，查看它所连接的边界路由器
                      memset(char_config_10,0,11);
                      substr(char_config_10,char_config_line,IF_linked_Router,10);
                      char_config_10[10]=0;
                      for(k=0;k<10;k++)//删除空格
                          { if(char_config_10[k]==' ')  char_config_10[k]=0; }
                      std::string mid_string=char_config_10;
                      Road_traceback.push_back(mid_string);
                      fclose(fconfig);
                      std::cout<<"最终追查到攻击者是来自 "<<mid_string<<"的LAN网络"<<std::endl;
                      return true;//只有找到了边界路由器后才算溯源成功
                    }
                  }
                }
                std::cout<<"未找到该Interface，请检查config.txt文档内容！"<<std::endl;
                fclose(fconfig);
                return false;
              }
              else if(mark_old>0)
              {
                for(k=1;k<router_dimension_plus_LAN;k++)
                {
                  if(fgets(char_config_line,200,fconfig)==NULL)
                  {
                      std::cout<<"路由器各个Interface读取失败！请检查config.txt中路由器interface信息是否正确！"<<std::endl;
                      fclose(fconfig);
                      return false;
                  }
                  if (k==UI_i)
                  {
                    memset(char_config_10,0,11);
                    substr(char_config_10,char_config_line,node_IFnum,10);
                    char_config_10[10]=0;
                    for(k=0;k<10;k++)//删除空格
                        {  if(char_config_10[k]==' ')  char_config_10[k]=0;        }
                    if(UI_i==atoi(char_config_10))
                      {//找到了该IF，查看它所连接的Router
                        memset(char_config_10,0,11);
                        substr(char_config_10,char_config_line,IF_linked_Router,10);
                        char_config_10[10]=0;
                        for(k=0;k<10;k++)//删除空格
                            { if(char_config_10[k]==' ')  char_config_10[k]=0;        }
                        std::string mid_string=char_config_10;
                        Road_traceback.push_back(mid_string);
                        fclose(fconfig);
                        //std::cout<<mark_old<<":"<<mid_string<<std::endl;
                        return Traceback(mark_old,Road_traceback,pcap_file,false);
                      }
                  }
                }
              }
            }
            else//这行没找到指定Router，那么继续找下一行
              continue;
          }
          else{//是NET，保存了NodeContainer的信息，但在traceback这里没用
            continue;// 其实这个else没必要
          }
      }
  fclose(fconfig);
  return false;//啥都没找到，返回个失败
}
bool Draw_Pic(std::vector<std::string> Road_traceback,int thisID)
{
  char char_config_line[200];
  char char_config_10[11];
  char thisNode[11];
  char nextNode[11];
  char tmpNode[11];
  char tmpX[11];
  char tmpY[11];
  char tmpXY[11];
  int i,k;
  unsigned int j;
  std::vector<std::vector<int> >vpic(2,std::vector<int>(1,0));//2*1的二维vector
  //vpic用于记录所有需要加粗的路径的坐标
  //vpic   1    2    3     4    5    6  。。。
  //0     横坐标
  //1     纵坐标

  FILE *fpic = fopen("scratch/pic.txt", "r");
  if(fpic==NULL)
      {   std::cout<<"pic.txt cannot load!"<<std::endl;
            return false; }
  std::vector<std::string>::iterator it2=Road_traceback.end()-1;
  for(;it2>Road_traceback.begin();it2--)
  {
    FILE *fconfig = fopen("scratch/config.txt", "r");
    if(fconfig==NULL)
        {     std::cout<<"config.txt cannot load!"<<std::endl;
              return false; }
    while(fgets(char_config_line,200,fconfig) != NULL)
          {
            memset(char_config_10,0,11);
            substr(char_config_10,char_config_line,node_type,10);
            char_config_10[10]=0;
            for(k=0;k<10;k++)//删除空格
               {if(char_config_10[k]==' ')  char_config_10[k]=0; }
            if(strcmp(char_config_10,"Router")==0)//如果是Router，则寻找路径
              {
                memset(thisNode,0,11);
                substr(thisNode,char_config_line,node_name,10);
                thisNode[10]=0;
                for(k=0;k<10;k++)//删除空格
                   {if(thisNode[k]==' ')  thisNode[k]=0; }
                strcpy(tmpNode,(*it2).c_str());
                if(strcmp(thisNode,tmpNode)==0)
                {
                  //std::cout<<"找到第一个Router"<<std::endl;
                  memset(nextNode,0,11);
                  substr(nextNode,char_config_line,IF_linked_Router,10);
                  nextNode[10]=0;
                  for(k=0;k<10;k++)//删除空格
                     {if(nextNode[k]==' ')  nextNode[k]=0; }
                  strcpy(tmpNode,(*(it2-1)).c_str());
                  if(strcmp(nextNode,tmpNode)==0)
                  {
                    //std::cout<<"\n----------------------\n"<<thisNode<<":"<<nextNode<<std::endl;
                    memset(tmpNode,0,11);//这里使用tmpNode作为中间变量，其名字无含义
                    substr(tmpNode,char_config_line,draw_pic,10);
                    tmpNode[10]=0;
                    for(k=0;k<10;k++)//删除空格
                       {if(tmpNode[k]==' ')  tmpNode[k]=0; }
                    //std::cout<<"("<<atoi(tmpNode)<<")  ";
                    for(i=1;i<=atoi(tmpNode);i++)
                    {
                      memset(tmpXY,0,11);
                      substr(tmpXY,char_config_line,draw_pic+i*10,10);
                      tmpXY[10]=0;
                      for(k=0;k<10;k++)//删除空格
                         {if(tmpXY[k]==' ')  tmpXY[k]=0; }
                      memset(tmpX,0,11);
                      memset(tmpY,0,11);
                      for(j=0;j<strlen(tmpXY);j++)
                      {
                        if(tmpXY[j]==',')
                        {tmpX[j]=0;
                          break;}
                        else
                          tmpX[j]=tmpXY[j];
                      }
                      //此时已经将行数写到tmpX
                      for(j=j+1;j<=strlen(tmpXY);j++)
                      {
                        tmpY[j-strlen(tmpX)-1]=tmpXY[j];
                      }
                      tmpY[j]=0;//此时已经将列数写到tmpY
                      vpic[0][0]=vpic[0][0]+1;
                      vpic[1][0]=vpic[1][0]+1;
                      vpic[0].push_back(atoi(tmpX));
                      vpic[1].push_back(atoi(tmpY));
                      //std::cout<<"("<<tmpX<<","<<tmpY<<")  ";
                    }
                    //std::cout<<std::endl;
                    break;
                  }
                  else
                  continue;
                }
                else
                continue;
              }
            else
              continue;
          }
    fclose(fconfig);
  }
  vector_sort(vpic);
  unsigned int column;
  int row=0;
  int vit,vit2,flag,flag2;
  std::cout<<"--------------------------------------------------"<<std::endl;
  std::cout<<"Mark ="<<thisID<<" ,将其追踪结果显示在网络拓扑图中:\n"<<std::endl;
  while(fgets(char_config_line,200,fpic) != NULL)
        {
          row++;
          flag=0;
          for(vit=1;vit<=vpic[0][0];vit++)
          {
            if(vpic[0][vit]==row)
            {
              flag=1;
              break;
            }
          }
          if(flag==1)
          {
            for(column=0;column<strlen(char_config_line);column++)
              {
                flag2=0;
                for(vit2=vit;vit2<=vpic[1][0];vit2++)
                {
                  int intcolumn=column+1;
                  if(vpic[0][vit2]==row&&vpic[1][vit2]==intcolumn)
                    {   flag2=1;   }
                }
                if(flag2==1)
                {
                  switch (char_config_line[column]) {
                    case '-':
                      std::cout<<"*";
                      break;
                      case '|':
                      std::cout<<"*";
                      //column++;
                      break;
                      case '\\':
                      std::cout<<"*";
                      //column++;
                      break;
                      case '/':
                      std::cout<<"*";
                      //column++;
                      break;
                      default:
                      std::cout<<"errors";
                    }
                  }
              else
                std::cout<<char_config_line[column];
              }
            }
            else
              std::cout<<char_config_line;
        }
  fclose(fpic);
  if(vpic.size()>1)return true;
  else return false;
}
void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
  nodeA->GetObject<Ipv4> ()->SetDown (interfaceA);
  nodeB->GetObject<Ipv4> ()->SetDown (interfaceB);
}
bool Read_NetInfo(std::vector<class Network_Info> &vNetInfo)
{
  FILE *fconfig = fopen("scratch/config.txt", "r");
  if(fconfig==NULL)
      {     std::cout<<"config.txt cannot load!"<<std::endl;
            return false; }
  char char_config_line[200];
  char char_config_10[11];
  int k;
  int flag;
  while(fgets(char_config_line,200,fconfig) != NULL)
  {
    memset(char_config_10,0,11);
    substr(char_config_10,char_config_line,node_type,10);
    char_config_10[10]=0;
    for(k=0;k<10;k++)//删除空格
          {if(char_config_10[k]==' ')  char_config_10[k]=0; }
    if(strcmp(char_config_10,"NET")==0)//如果是NET，说明有NodeContainer信息
    {

      flag=0;
      memset(char_config_10,0,11);
      substr(char_config_10,char_config_line,node_name,10);
      char_config_10[10]=0;
      for(k=0;k<10;k++)//删除空格
          {if(char_config_10[k]==' ')  char_config_10[k]=0;        }
      std::vector<class Network_Info>::iterator it;
      for(it=vNetInfo.begin();it!=vNetInfo.end();it++)
      {
        if(strcmp((*it).netName,char_config_10)==0)
        {
          flag=1;
          break;
        }
      }
      if(flag){continue;   }//发现已经存在了这个信息，跳过，继续下一行
      class Network_Info tmpNetInfo;
      strcpy(tmpNetInfo.netName,char_config_10);
      memset(char_config_10,0,11);
      substr(char_config_10,char_config_line,node_dimension,10);
      char_config_10[10]=0;
      for(k=0;k<10;k++)//删除空格
          {if(char_config_10[k]==' ')  char_config_10[k]=0; }
      strcpy(tmpNetInfo.netNode1,char_config_10);
      memset(char_config_10,0,11);
      substr(char_config_10,char_config_line,node_IFnum,10);
      char_config_10[10]=0;
      for(k=0;k<10;k++)//删除空格
          {if(char_config_10[k]==' ')  char_config_10[k]=0;        }
      strcpy(tmpNetInfo.netNode2,char_config_10);
      tmpNetInfo.flag=0;
      vNetInfo.push_back(tmpNetInfo);
    }
  }
  return true;
}
bool Slow_the_Path(std::vector<std::string> Road_traceback,std::vector<class Network_Info> &vNetInfo)
{
  int count=Road_traceback.size()-1;//用于记录网段个数，是Road_traceback.size()-1的大小
  std::cout<<"==============================="<<std::endl;
  std::vector<std::string>::iterator it1=Road_traceback.end()-1;
  for(;it1>Road_traceback.begin();it1--)
  {
    char tmpNode1[11];
    char tmpNode2[11];
    strcpy(tmpNode1,(*it1).c_str());
    strcpy(tmpNode2,(*(it1-1)).c_str());
    //std::cout<<tmpNode1<<":"<<tmpNode2<<":"<<std::endl;
    std::vector<class Network_Info>::iterator it2;
    for(it2=vNetInfo.begin();it2<vNetInfo.end();it2++)
    {
      if((strcmp((*it2).netNode1,tmpNode1)==0&&strcmp((*it2).netNode2,tmpNode2)==0)||(strcmp((*it2).netNode1,tmpNode2)==0&&strcmp((*it2).netNode2,tmpNode1)==0))
      {
        (*it2).flag=1;
        //std::cout<<(*it2).netName<<std::endl;
        count--;
      }
    }
  }
  //std::cout<<vNetInfo.size()<<std::endl;
  if(count!=0)
  {
    std::cout<<"路径信息错误，无法降速！"<<std::endl;
    return false;
  }
  return true;
}
NetDeviceContainer setSpeed(CsmaHelper csma,std::vector<class Network_Info> vNetInfo,NodeContainer net,std::string netName,double Limit)
{
  NetDeviceContainer tmpNDC;
  char tmpNetName[11];
  strcpy(tmpNetName,netName.c_str());
  std::vector<class Network_Info>::iterator it;
  for(it=vNetInfo.begin();it<vNetInfo.end();it++)
  {
    if(strcmp(tmpNetName,(*it).netName)==0)
    {
      std::cout<<(*it).netName<<"的限速标志flag为"<<(*it).flag;
      if((*it).flag==1)
      {
        std::strstream ss;
        std::string tmpSpeed;
        ss<<Limit;
        ss>>tmpSpeed;
        tmpSpeed=tmpSpeed+"Mbps";
        csma.SetChannelAttribute ("DataRate",  StringValue (tmpSpeed));
        csma.SetChannelAttribute ("Delay", StringValue ("10ms"));
        tmpNDC=csma.Install(net);
        std::cout<<",传输速率为"<<tmpSpeed<<std::endl;
        return tmpNDC;
      }
    }
  }
  csma.SetChannelAttribute ("DataRate",  StringValue("200Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("10ms"));
  tmpNDC=csma.Install(net);
  std::cout<<",传输速率为200Mbps"<<std::endl;
  return tmpNDC;
}
int main (int argc, char **argv)
{
  bool verbose = false;
  bool printRoutingTables = false;
  double speedLimit=0.0001;
  std::string SplitHorizon ("PoisonReverse");
  std::string cmd1("tcpdump -r ");
  std::string cmd2(" -v host ");
  std::string pcap_file("rip-R19-1.pcap");//pcap_file和vic_IP命令必须同时修改
  std::string vic_IP("192.168.4.3");

  CommandLine cmd;
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds", printRoutingTables);
  cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon, SplitHorizon, PoisonReverse)", SplitHorizon);
  cmd.AddValue("speedLimit","Limit the speed of attack path into your value(Mbps, default 100Mbps)",speedLimit);
  cmd.AddValue("vic_IP","Victim's IP",vic_IP);
  cmd.AddValue("pcap_file","Victim's pcap file",pcap_file);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      /*
      LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE));
      LogComponentEnable ("RipSimpleRouting", LOG_LEVEL_INFO);
      LogComponentEnable ("Rip", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
      LogComponentEnable ("Icmpv4L4Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
      LogComponentEnable ("V4Ping", LOG_LEVEL_ALL);
      */
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
    }

  if (SplitHorizon == "NoSplitHorizon")
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::NO_SPLIT_HORIZON));
    }
  else if (SplitHorizon == "SplitHorizon")
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::SPLIT_HORIZON));
    }
  else
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));
    }

  std::vector<int> _Atk_Id_List;
  std::vector<class Network_Info> vNetInfo;
  if(!Read_NetInfo(vNetInfo))
  {
    std::cout<<"config.txt文件中NodeContainer信息有误，请检查！本次仿真将无法限流！"<<std::endl;
  }
//此时netinfo已经写入vnetinfo
  //std::cout<<"hahahahaahah"<<std::endl;
  std::string command = cmd1 + pcap_file + cmd2 + vic_IP;
  char final_cmd[50];
  strcpy(final_cmd , command.c_str());
  char victim_IP[16];
  strcpy(victim_IP, vic_IP.c_str());
  if(Extract_Identification(final_cmd,victim_IP,_Atk_Id_List))
    {   //pcap_file has at least one packet from attacker
      std::vector<std::string> Road_traceback;
      int choose;
      //std::cout<<"开始溯源"<<std::endl;
      std::cout<<"请选择你需要追踪的Mark(0表示将追踪所有Mark): ";
      std::cin>>choose;
      if(choose==0)
      {
        std::vector<int>::iterator it;
        for(it=_Atk_Id_List.begin();it!=_Atk_Id_List.end();it++)
        {
          int thismark=*it;
          std::cout<<"开始溯源"<<std::endl;
          if(Traceback(thismark,Road_traceback,pcap_file,false))
          {//如果返回值为true，说明已经计算完成，准备输出路径
            std::vector<std::string>::iterator it2=Road_traceback.end()-1;
            std::cout<<"攻击路径是："<<*it2--;
            for(;it2>=Road_traceback.begin();it2--)
                {    std::cout<<"-->"<<*it2;    }
            std::cout<<std::endl;
            Draw_Pic(Road_traceback,thismark);
            Slow_the_Path(Road_traceback,vNetInfo);
            Road_traceback.clear();
            //std::cout<<std::endl;
          }
        }
      }
      else
      {
        std::cout<<"开始溯源"<<std::endl;
        if(Traceback(choose,Road_traceback,pcap_file,false))
        {//如果返回值为true，说明已经计算完成，准备输出路径
          std::vector<std::string>::iterator it2=Road_traceback.end()-1;
          std::cout<<"攻击路径是："<<*it2--;
          for(;it2>=Road_traceback.begin();it2--)
              {  std::cout<<"-->"<<*it2;     }
          std::cout<<std::endl;
          Draw_Pic(Road_traceback,choose);
          Slow_the_Path(Road_traceback,vNetInfo);
          Road_traceback.clear();
          //std::cout<<std::endl;
        }
      }
    }
  else
    {
      std::cout<<"程序无法溯源，原因可能是：\n1.首次仿真\n2.您提供的.pcap文件、受害者IP有误！"<<std::endl;
    }

  NS_LOG_INFO ("Create nodes.");
  Ptr<Node> R5 = CreateObject<Node> ();
  Names::Add ("R5", R5);
  Ptr<Node> R6 = CreateObject<Node> ();
  Names::Add ("R6", R6);
  Ptr<Node> R7 = CreateObject<Node> ();
  Names::Add ("R7", R7);
  Ptr<Node> R8 = CreateObject<Node> ();
  Names::Add ("R8", R8);
  Ptr<Node> R9 = CreateObject<Node> ();
  Names::Add ("R9", R9);
  Ptr<Node> R10 = CreateObject<Node> ();
  Names::Add ("R10", R10);
  Ptr<Node> R11 = CreateObject<Node> ();
  Names::Add ("R11", R11);
  Ptr<Node> R12 = CreateObject<Node> ();
  Names::Add ("R12", R12);
  Ptr<Node> R13 = CreateObject<Node> ();
  Names::Add ("R13", R13);
  Ptr<Node> R14 = CreateObject<Node> ();
  Names::Add ("R14", R14);
  Ptr<Node> R15 = CreateObject<Node> ();
  Names::Add ("R15", R15);
  Ptr<Node> R16 = CreateObject<Node> ();
  Names::Add ("R16", R16);
  Ptr<Node> R4 = CreateObject<Node> ();
  Names::Add ("R4", R4);
  Ptr<Node> R17 = CreateObject<Node> ();
  Names::Add ("R17", R17);
  Ptr<Node> R18 = CreateObject<Node> ();
  Names::Add ("R18", R18);
  Ptr<Node> R19 = CreateObject<Node> ();
  Names::Add ("R19", R19);
  Ptr<Node> R1 = CreateObject<Node> ();
  Names::Add ("R1", R1);
  Ptr<Node> R2 = CreateObject<Node> ();
  Names::Add ("R2", R2);
  Ptr<Node> R3 = CreateObject<Node> ();
  Names::Add ("R3", R3);

  NodeContainer net1 (R8, R1);
  NodeContainer net2 (R2, R9);
  NodeContainer net3 (R1, R2);
  NodeContainer net4 (R1, R3);
  NodeContainer net5 (R1, R4);
  NodeContainer net6 (R2, R4);
  NodeContainer net7 (R3, R4);
  NodeContainer net8 (R3, R16);
  NodeContainer routers (R1, R2, R3);
  NodeContainer LAN_1_nodes (R5,R6,R7,R8);
  NodeContainer LAN_2_nodes (R9,R10,R11,R12);
  NodeContainer LAN_3_nodes (R13,R14,R15,R16);
  NodeContainer LAN_4_nodes (R4,R17,R18,R19);
  NS_LOG_INFO ("Create IPv4 and routing");
  RipHelper ripRouting;
  // Interfaces are added sequentially, starting from 0
  // However, interface 0 is always the loopback...
  ripRouting.ExcludeInterface (R8, 2);
  ripRouting.ExcludeInterface (R9, 2);
  ripRouting.ExcludeInterface (R16, 2);
  ripRouting.ExcludeInterface (R4, 4);
  ripRouting.SetInterfaceMetric (R2, 2, 10);
  ripRouting.SetInterfaceMetric (R1, 2, 10);

  Ipv4ListRoutingHelper listRH;
  listRH.Add (ripRouting, 0);

  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);
  internet.SetRoutingHelper (listRH);
  internet.Install (routers);

  internet.Install (R8);
  internet.Install (R9);
  internet.Install (R16);
  internet.Install (R4);

  InternetStackHelper internetNodes;
  internetNodes.SetIpv6StackInstall (false);
  internetNodes.Install (R5);
  internetNodes.Install (R6);
  internetNodes.Install (R7);
  internetNodes.Install (R10);
  internetNodes.Install (R11);
  internetNodes.Install (R12);
  internetNodes.Install (R13);
  internetNodes.Install (R14);
  internetNodes.Install (R15);
  internetNodes.Install (R17);
  internetNodes.Install (R18);
  internetNodes.Install (R19);
//when we setup, loopback is IF 0 , then csma is 1 to xx
//but in Ipv4L3Protocol.cc, loopback seems to be ignored
//and  LAN is IF_0, others are 1 to x
  NS_LOG_INFO ("Create channels.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("200Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer ndcLAN_1 = csma.Install (LAN_1_nodes);
  NetDeviceContainer ndcLAN_2 = csma.Install (LAN_2_nodes);
  NetDeviceContainer ndcLAN_3 = csma.Install (LAN_3_nodes);
  NetDeviceContainer ndcLAN_4 = csma.Install (LAN_4_nodes);
  NetDeviceContainer R1D = csma.Install (R1);
  NetDeviceContainer R2D = csma.Install (R2);
  NetDeviceContainer R3D = csma.Install (R3);

  NetDeviceContainer ndc1 = setSpeed(csma,vNetInfo,net1,"net1",speedLimit);
  NetDeviceContainer ndc2 = setSpeed(csma,vNetInfo,net2,"net2",speedLimit);
  NetDeviceContainer ndc3 = setSpeed(csma,vNetInfo,net3,"net3",speedLimit);
  NetDeviceContainer ndc4 = setSpeed(csma,vNetInfo,net4,"net4",speedLimit);
  NetDeviceContainer ndc5 = setSpeed(csma,vNetInfo,net5,"net5",speedLimit);
  NetDeviceContainer ndc6 = setSpeed(csma,vNetInfo,net6,"net6",speedLimit);
  NetDeviceContainer ndc7 = setSpeed(csma,vNetInfo,net7,"net7",speedLimit);
  NetDeviceContainer ndc8 = setSpeed(csma,vNetInfo,net8,"net8",speedLimit);

  NS_LOG_INFO ("Assign IPv4 Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0","255.255.255.0");
  Ipv4InterfaceContainer iic1 = ipv4.Assign (ndc1);
  ipv4.SetBase ("10.1.2.0","255.255.255.0");
  Ipv4InterfaceContainer iic2 = ipv4.Assign (ndc2);
  ipv4.SetBase ("10.1.3.0","255.255.255.0");
  Ipv4InterfaceContainer iic3 = ipv4.Assign (ndc3);
  ipv4.SetBase ("10.1.4.0","255.255.255.0");
  Ipv4InterfaceContainer iic4 = ipv4.Assign (ndc4);
  ipv4.SetBase ("10.1.5.0","255.255.255.0");
  Ipv4InterfaceContainer iic5 = ipv4.Assign (ndc5);
  ipv4.SetBase ("10.1.6.0","255.255.255.0");
  Ipv4InterfaceContainer iic6 = ipv4.Assign (ndc6);
  ipv4.SetBase ("10.1.7.0","255.255.255.0");
  Ipv4InterfaceContainer iic7 = ipv4.Assign (ndc7);
  ipv4.SetBase ("10.1.8.0","255.255.255.0");
  Ipv4InterfaceContainer iic8 = ipv4.Assign (ndc8);

  ipv4.SetBase ("192.168.1.0","255.255.255.0");
  Ipv4InterfaceContainer iic_LAN_1 = ipv4.Assign (ndcLAN_1);
  ipv4.SetBase ("192.168.2.0","255.255.255.0");
  Ipv4InterfaceContainer iic_LAN_2 = ipv4.Assign (ndcLAN_2);
  ipv4.SetBase ("192.168.3.0","255.255.255.0");
  Ipv4InterfaceContainer iic_LAN_3 = ipv4.Assign (ndcLAN_3);
  ipv4.SetBase ("192.168.4.0","255.255.255.0");
  Ipv4InterfaceContainer iic_LAN_4 = ipv4.Assign (ndcLAN_4);
  Ptr<Ipv4StaticRouting> staticRouting;

  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R7->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.1.4", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R6->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.1.4", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R5->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.1.4", 1 );

  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R10->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.2.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R11->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.2.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R12->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.2.1", 1 );

  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R13->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.3.4", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R14->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.3.4", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R15->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.3.4", 1 );

  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R17->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.4.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R18->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.4.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (R19->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.4.1", 1 );

/*
      RipHelper routingHelper;
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);
      routingHelper.PrintRoutingTableAt (Seconds (45.0), R8, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (45.0), R9, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (45.0), R16, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (45.0), R4, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (45.0), R1, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (45.0), R2, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (45.0), R3, routingStream);
*/

/*
  NS_LOG_INFO ("Create Applications.");
  uint32_t packetSize = 1024;
  Time interPacketInterval = Seconds (1.0);
  V4PingHelper ping ("192.168.3.3");
  ping.SetAttribute ("Interval", TimeValue (interPacketInterval));
  ping.SetAttribute ("Size", UintegerValue (packetSize));
  ping.SetAttribute ("Verbose", BooleanValue (true));   //
  ApplicationContainer apps = ping.Install (R7);
  apps.Start (Seconds (36.0));
  apps.Stop (Seconds (40.0));
*/
  Address RxAddress(InetSocketAddress (iic_LAN_4.GetAddress(3), 10));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", RxAddress);
  ApplicationContainer sinkApp = sinkHelper.Install (LAN_4_nodes.Get(3));
  sinkApp.Start (Seconds (41.0));
  sinkApp.Stop (Seconds (50.0));
  Address TxAddress(InetSocketAddress(iic_LAN_4.GetAddress(3),10));
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", TxAddress);
// setting up attributes for onoff application helper
  clientHelper.SetAttribute("DataRate",StringValue("100Mbps"));
  clientHelper.SetAttribute("PacketSize",UintegerValue(1280));
  ApplicationContainer Tx = clientHelper.Install (LAN_1_nodes.Get (0));
  Tx.Start (Seconds (41.0));
  Tx.Stop (Seconds (50.0));

  Address RxAddress2(InetSocketAddress (iic_LAN_4.GetAddress(3), 10));
  PacketSinkHelper sinkHelper2 ("ns3::TcpSocketFactory", RxAddress2);
  ApplicationContainer sinkApp2 = sinkHelper2.Install (LAN_4_nodes.Get(3));
  sinkApp2.Start (Seconds (41.0));
  sinkApp2.Stop (Seconds (50.0));
  Address TxAddress2(InetSocketAddress(iic_LAN_4.GetAddress(3),10));
  OnOffHelper clientHelper2 ("ns3::TcpSocketFactory", TxAddress2);
// setting up attributes for onoff application helper
  clientHelper2.SetAttribute("DataRate",StringValue("100Mbps"));
  clientHelper2.SetAttribute("PacketSize",UintegerValue(1280));
  ApplicationContainer Tx2 = clientHelper2.Install (LAN_2_nodes.Get (3));
  Tx2.Start (Seconds (41.0));
  Tx2.Stop (Seconds (50.0));

  Address RxAddress3(InetSocketAddress (iic_LAN_4.GetAddress(3), 10));
  PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory", RxAddress2);
  ApplicationContainer sinkApp3 = sinkHelper3.Install (LAN_4_nodes.Get(3));
  sinkApp3.Start (Seconds (41.0));
  sinkApp3.Stop (Seconds (50.0));
  Address TxAddress3(InetSocketAddress(iic_LAN_4.GetAddress(3),10));
  OnOffHelper clientHelper3 ("ns3::TcpSocketFactory", TxAddress3);
// setting up attributes for onoff application helper
  clientHelper3.SetAttribute("DataRate",StringValue("100Mbps"));
  clientHelper3.SetAttribute("PacketSize",UintegerValue(1280));
  ApplicationContainer Tx3 = clientHelper3.Install (LAN_3_nodes.Get (0));
  Tx3.Start (Seconds (41.0));
  Tx3.Stop (Seconds (50.0));

/*
  //UDP server to recieve packets
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (LAN_4_nodes.Get (2));
  serverApps.Start (Seconds (95.0));
  serverApps.Stop (Seconds (100.0));
  //UDP client to send packets(41s-50s)
  UdpEchoClientHelper echoClient (iic_LAN_4.GetAddress (2), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer clientApps = echoClient.Install (R6);
  clientApps.Start (Seconds (95.0));
  clientApps.Stop (Seconds (100.0));

  UdpEchoServerHelper echoServer2 (9);
  ApplicationContainer serverApps2 = echoServer2.Install (LAN_4_nodes.Get (2));
  serverApps2.Start (Seconds (95.0));
  serverApps2.Stop (Seconds (100.0));
  //UDP client to send packets(41s-50s)
  UdpEchoClientHelper echoClient2 (iic_LAN_4.GetAddress (2), 9);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer clientApps2 = echoClient2.Install (R11);
  clientApps2.Start (Seconds (95.0));
  clientApps2.Stop (Seconds (100.0));

  UdpEchoServerHelper echoServer3 (9);
  ApplicationContainer serverApps3 = echoServer3.Install (LAN_4_nodes.Get (2));
  serverApps3.Start (Seconds (95.0));
  serverApps3.Stop (Seconds (100.0));
  //UDP client to send packets(41s-50s)
  UdpEchoClientHelper echoClient3 (iic_LAN_4.GetAddress (2), 9);
  echoClient3.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient3.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer clientApps3 = echoClient3.Install (R14);
  clientApps3.Start (Seconds (95.0));
  clientApps3.Stop (Seconds (100.0));

  //Simulator::Schedule (Seconds (51), &TearDownLink, R1, R2, 2, 2);
  //Simulator::Schedule (Seconds (51), &TearDownLink, R1, R3, 3, 1);

  AnimationInterface anim ("anim2.xml");
  anim.SetConstantPosition (R5, 0.0, 0.0);
  anim.SetConstantPosition (R6, 10.0, 0.0);
  anim.SetConstantPosition (R7, 20.0, 0.0);
  anim.SetConstantPosition (R8, 30.0, 0.0);
  anim.SetConstantPosition (R9, 50.0, 10.0);
  anim.SetConstantPosition (R10, 60.0, 10.0);
  anim.SetConstantPosition (R11, 70.0, 10.0);
  anim.SetConstantPosition (R12, 80.0, 10.0);
  anim.SetConstantPosition (R13, 0.0, 50.0);
  anim.SetConstantPosition (R14, 10.0, 50.0);
  anim.SetConstantPosition (R15, 20.0, 50.0);
  anim.SetConstantPosition (R16, 30.0, 50.0);
  anim.SetConstantPosition (R4, 50.0, 40.0);
  anim.SetConstantPosition (R17, 60.0, 40.0);
  anim.SetConstantPosition (R18, 70.0, 40.0);
  anim.SetConstantPosition (R19, 80.0, 40.0);
  anim.SetConstantPosition (R1, 30.0, 20.0);
  anim.SetConstantPosition (R2, 50.0, 20.0);
  anim.SetConstantPosition (R3, 30.0, 40.0);
*/
/*
  //  AsciiTraceHelper ascii;
  // csma.EnableAsciiAll (ascii.CreateFileStream ("rip-simple-routing.tr"));
  */
  csma.EnablePcapAll ("rip", true);

  /* Now, do the actual simulation. */
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (131.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
