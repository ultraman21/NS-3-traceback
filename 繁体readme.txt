config.txt的格式
（以每10個字符為一段）
第一段       第二段    第三段       第四段         第五段              第六段             第七段
節點的類型   節點名稱  路由器維度   路由器IF編號   該IF連接的路由器    pic中的線條總數n   之後的n段都是線條的的坐標
（例如）
LAN          LAN1_3    1            0              R1                  0                   
Router       R1        5            1              LAN1_3              3                  2,5       3,5       4,5       
（然後NET的格式是類型、名稱、連接的第一個路由器、連接的第二個路由器）
NET          net4      R1           R3


一定要保證每段都是10個字符，每行以回車結尾，config.txt和pic.txt都放在scratch中，和proj_rip2.cc在同一個目錄下



proj_rip2.cc是仿真程式，在NS3目錄下打開終端輸入：./waf --run scratch/proj_rip2   運行
main函數前幾行是預設的cmd命令，可根據需要修改pcap_file和vic_IP，這兩個變數是人為修改的。

算法大致思路（也可以看rar中的 算法思路.png）是，
1、讀取config中NET的信息，將所有的NET信息保存到vNetInfo中（包括NET名稱、連接的兩個Router、以及限速標誌flag）
2、首先用Extract_identification()判斷該pcap文件是否存在，pcap中是否是空的，發給victim的封包是否只有mark=0的數據包
只要這三個檢查其中一個有問題，就認為無法溯源，判定為首次仿真
如果三個檢查沒問題，說明可以提取出非零mark的攻擊封包的mark數值，隨後保存在Atk_ID_List裡面
3、輸出atk_ID_List中的所有ID數值，讓用戶選擇追蹤的ID值，然後交給traceback()追蹤
4、traceback使用的是遞歸，直到找到最後的LAN，否則一直找，遞歸期間出現任何問題都認為是溯源失敗。
最後結果保存在Road_traceback中，這裡保存了從victim到attacker的路徑，實際路徑是這個的倒敘
5、倒敘輸出road_traceback的路徑，即實際從atker到victim的攻擊路徑
6、Draw_Pic（）畫圖，pic.txt中原本用線標註的路徑改用*標註
7、Slow_the_Path（）讀取vNetInfo，將屬於攻擊路徑的NET的限速標誌flag修改為1
8、793-964行就是實際仿真的部分，792行之前的是溯源+畫圖+限速的部分。實際仿真時，在建立NET連接的時候，用setSpeed函數設置，會讀取vNetInfo，如果這段網路的限速flag是1，就將速度限制為speedLimit（默認0.0001Mbps）


第一次./waf --run scratch/proj_rip2仿真的時候，就算pcap_file和vic_IP有數值，但是因為實際沒有pcap文件，所以認為是首次仿真
因此extract_identification的提取ID、traceback、Draw_Pic、Slow_the_Path都不執行，就是正常的仿真793-964的部分
第二次./waf --run scratch/proj_rip2的時候，pcap文件已經存在了，如果pcap_file和vic_IP完全正確，就執行extract_identification的提取ID、traceback、Draw_Pic、Slow_the_Path，在第二次仿真時，就將攻擊路徑的速度設置為0.0001Mbps，生成限速后pcap文件。



