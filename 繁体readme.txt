config.txt�ĸ�ʽ
����ÿ10���ַ���һ�Σ�
��һ��       �ڶ���    ������       ���Ķ�         �����              ������             ���߶�
���c�����   ���c���Q  ·�����S��   ·����IF��̖   ԓIF�B�ӵ�·����    pic�еľ��l����n   ֮���n�ζ��Ǿ��l�ĵ�����
�����磩
LAN          LAN1_3    1            0              R1                  0                   
Router       R1        5            1              LAN1_3              3                  2,5       3,5       4,5       
��Ȼ��NET�ĸ�ʽ����͡����Q���B�ӵĵ�һ��·�������B�ӵĵڶ���·������
NET          net4      R1           R3


һ��Ҫ���Cÿ�ζ���10���ַ���ÿ���Ի�܇�Yβ��config.txt��pic.txt������scratch�У���proj_rip2.cc��ͬһ��Ŀ���



proj_rip2.cc�Ƿ����ʽ����NS3Ŀ��´��_�K��ݔ�룺./waf --run scratch/proj_rip2   �\��
main����ǰ�������A�O��cmd����ɸ�����Ҫ�޸�pcap_file��vic_IP���@�ɂ�׃�����˞��޸ĵġ�

�㷨����˼·��Ҳ���Կ�rar�е� �㷨˼·.png���ǣ�
1���xȡconfig��NET����Ϣ�������е�NET��Ϣ���浽vNetInfo�У�����NET���Q���B�ӵăɂ�Router���Լ����٘��Iflag��
2��������Extract_identification()�Д�ԓpcap�ļ��Ƿ���ڣ�pcap���Ƿ��ǿյģ��l�ovictim�ķ���Ƿ�ֻ��mark=0�Ĕ�����
ֻҪ�@�����z������һ���І��}�����J��o����Դ���ж����״η���
��������z��]���}���f��������ȡ������mark�Ĺ��������mark��ֵ���S�ᱣ����Atk_ID_List�e��
3��ݔ��atk_ID_List�е�����ID��ֵ��׌�Ñ��x��׷ۙ��IDֵ��Ȼ�ύ�otraceback()׷ۙ
4��tracebackʹ�õ����f�w��ֱ���ҵ������LAN����tһֱ�ң��f�w���g���F�κΆ��}���J������Դʧ����
����Y��������Road_traceback�У��@�e�����ˏ�victim��attacker��·�������H·�����@���ĵ���
5������ݔ��road_traceback��·���������H��atker��victim�Ĺ���·��
6��Draw_Pic�������D��pic.txt��ԭ���þ����]��·������*���]
7��Slow_the_Path�����xȡvNetInfo������춹���·����NET�����٘��Iflag�޸Ğ�1
8��793-964�о��ǌ��H����Ĳ��֣�792��֮ǰ������Դ+���D+���ٵĲ��֡����H����r���ڽ���NET�B�ӵĕr����setSpeed�����O�ã����xȡvNetInfo������@�ξW·������flag��1���͌��ٶ����ƞ�speedLimit��Ĭ�J0.0001Mbps��


��һ��./waf --run scratch/proj_rip2����ĕr�򣬾���pcap_file��vic_IP�Д�ֵ��������錍�H�]��pcap�ļ��������J�����״η���
���extract_identification����ȡID��traceback��Draw_Pic��Slow_the_Path�������У����������ķ���793-964�Ĳ���
�ڶ���./waf --run scratch/proj_rip2�ĕr��pcap�ļ��ѽ������ˣ����pcap_file��vic_IP��ȫ���_���͈���extract_identification����ȡID��traceback��Draw_Pic��Slow_the_Path���ڵڶ��η���r���͌�����·�����ٶ��O�Þ�0.0001Mbps���������ٺ�pcap�ļ���



