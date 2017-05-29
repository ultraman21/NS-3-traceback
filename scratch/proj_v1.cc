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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RipSimpleRouting");

void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
  nodeA->GetObject<Ipv4> ()->SetDown (interfaceA);
  nodeB->GetObject<Ipv4> ()->SetDown (interfaceB);
}

// Default Network Topology
//                 Rank 0   |   Rank 1               | Rank 2
// -------------------------|------------------------|------------
//                                192.168.3.0
//                     IF_0__________________________
//                         |          |     |      |         
//                       Node(12)     13    14     vic(15)                
//                         |IF_1
//                         |    
//                         |10.1.7.0                                 
//                         |                            
//                    IF_3 |     10.1.6.0       IF_2                    
//                         R11---------------------R10                          
//                    IF_1 /   IF_2                 |IF_1     
//                        /                         |
//                       /10.1.3.0                  |10.1.5.0 
//                  IF_3/        10.1.4.0    IF_1   |IF_3
//                     R4---------------------------R5                  
//                 IF_1|  IF_2                      |IF_2
//                     |10.1.1.0                    |10.1.2.0
//                     |                            |  
//  ___________________|IF_1                        |______________________
//  |    |     | IF_0  |                            |       |       |      | 
//(0)    1     2     Atk_1(3)                   (6)Node     7    atk_2(8)  9     
//    192.168.1.0                                         192.168.2.0
//
//
//
//
//
//
//
//



int main (int argc, char **argv)
{
  bool verbose = false;
  bool printRoutingTables = true;
  std::string SplitHorizon ("PoisonReverse");

  CommandLine cmd;
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds", printRoutingTables);
  cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon, SplitHorizon, PoisonReverse)", SplitHorizon);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE));
      LogComponentEnable ("RipSimpleRouting", LOG_LEVEL_INFO);
      LogComponentEnable ("Rip", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
      LogComponentEnable ("Icmpv4L4Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
      LogComponentEnable ("V4Ping", LOG_LEVEL_ALL);
    }
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);


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

  NS_LOG_INFO ("Create nodes.");
  Ptr<Node> atk1_0 = CreateObject<Node> ();
  Names::Add ("atk1_0", atk1_0);
  Ptr<Node> atk1_1 = CreateObject<Node> ();
  Names::Add ("atk1_1", atk1_1);
  Ptr<Node> atk1_2 = CreateObject<Node> ();
  Names::Add ("atk1_2", atk1_2);
  Ptr<Node> atk1_3 = CreateObject<Node> ();
  Names::Add ("atk1_3", atk1_3);
  Ptr<Node> R4 = CreateObject<Node> ();
  Names::Add ("Router4", R4);
  Ptr<Node> R5 = CreateObject<Node> ();
  Names::Add ("Router5", R5);
  Ptr<Node> atk2_6 = CreateObject<Node> ();
  Names::Add ("atk2_6", atk2_6);
  Ptr<Node> atk2_7 = CreateObject<Node> ();
  Names::Add ("atk2_7", atk2_7);
  Ptr<Node> atk2_8 = CreateObject<Node> ();
  Names::Add ("atk2_8", atk2_8);
  Ptr<Node> atk2_9 = CreateObject<Node> ();
  Names::Add ("atk2_9", atk2_9);
  Ptr<Node> R10 = CreateObject<Node> ();
  Names::Add ("Router10", R10);
  Ptr<Node> R11 = CreateObject<Node> ();
  Names::Add ("Router11", R11);
  Ptr<Node> vic1_12 = CreateObject<Node> ();
  Names::Add ("vic1_12", vic1_12);
  Ptr<Node> vic1_13 = CreateObject<Node> ();
  Names::Add ("vic1_13", vic1_13);
  Ptr<Node> vic1_14 = CreateObject<Node> ();
  Names::Add ("vic1_14", vic1_14);
  Ptr<Node> vic1_15 = CreateObject<Node> ();
  Names::Add ("vic1_15", vic1_15);

  NodeContainer net1 (atk1_3, R4);
  NodeContainer net2 (R4, R5);
  NodeContainer net3 (R4, R11);
  NodeContainer net4 (R5, atk2_6);
  NodeContainer net5 (R5, R10);
  NodeContainer net6 (R10, R11);
  NodeContainer net7 (R11, vic1_12);
  NodeContainer routers (R4, R5, R10, R11);
  NodeContainer LAN_1_nodes (atk1_0,atk1_1,atk1_2,atk1_3);
  NodeContainer LAN_2_nodes (atk2_6,atk2_7,atk2_8,atk2_9);
  NodeContainer LAN_3_nodes (vic1_12,vic1_13,vic1_14,vic1_15);

  NS_LOG_INFO ("Create IPv4 and routing");
  RipHelper ripRouting;
  // Interfaces are added sequentially, starting from 0
  // However, interface 0 is always the loopback...
  ripRouting.ExcludeInterface (atk1_3, 2);
  ripRouting.ExcludeInterface (atk2_6, 2);
  ripRouting.ExcludeInterface (vic1_12, 2);

  ripRouting.SetInterfaceMetric (R4, 3, 10);
  ripRouting.SetInterfaceMetric (R11, 1, 10);

  Ipv4ListRoutingHelper listRH;
  listRH.Add (ripRouting, 0);

  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);
  internet.SetRoutingHelper (listRH);
  internet.Install (routers);
  
  internet.Install (atk1_3);
  internet.Install (atk2_6);
  internet.Install (vic1_12);

  InternetStackHelper internetNodes;
  internetNodes.SetIpv6StackInstall (false);
  internetNodes.Install (atk1_0);
  internetNodes.Install (atk1_1);
  internetNodes.Install (atk1_2);

  internetNodes.Install (atk2_7);
  internetNodes.Install (atk2_8);
  internetNodes.Install (atk2_9);

  internetNodes.Install (vic1_13);
  internetNodes.Install (vic1_14);
  internetNodes.Install (vic1_15);


//loopback is IF 0 , then csma is 1 to xx
  NS_LOG_INFO ("Create channels.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("200Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer ndc_LAN_1 = csma.Install (LAN_1_nodes);
  NetDeviceContainer ndc_LAN_2 = csma.Install (LAN_2_nodes);
  NetDeviceContainer ndc_LAN_3 = csma.Install (LAN_3_nodes);
  NetDeviceContainer R4D = csma.Install (R4);
  NetDeviceContainer R5D = csma.Install (R5);
  NetDeviceContainer R10D = csma.Install (R10);
  NetDeviceContainer R11D = csma.Install (R11);

  csma.SetChannelAttribute ("DataRate",  StringValue ("200Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("10ms"));
  NetDeviceContainer ndc1 = csma.Install (net1);
  NetDeviceContainer ndc2 = csma.Install (net2);
  NetDeviceContainer ndc3 = csma.Install (net3);
  NetDeviceContainer ndc4 = csma.Install (net4);
  NetDeviceContainer ndc5 = csma.Install (net5);
  NetDeviceContainer ndc6 = csma.Install (net6);
  NetDeviceContainer ndc7 = csma.Install (net7);




  NS_LOG_INFO ("Assign IPv4 Addresses.");
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0","255.255.255.0");
  Ipv4InterfaceContainer iic1 = ipv4.Assign (ndc1);

  ipv4.SetBase ("10.1.4.0","255.255.255.0");
  Ipv4InterfaceContainer iic2 = ipv4.Assign (ndc2);

  ipv4.SetBase ("10.1.3.0","255.255.255.0");
  Ipv4InterfaceContainer iic3 = ipv4.Assign (ndc3);

  ipv4.SetBase ("10.1.2.0","255.255.255.0");
  Ipv4InterfaceContainer iic4 = ipv4.Assign (ndc4);

  ipv4.SetBase ("10.1.5.0","255.255.255.0");
  Ipv4InterfaceContainer iic5 = ipv4.Assign (ndc5);

  ipv4.SetBase ("10.1.6.0","255.255.255.0");
  Ipv4InterfaceContainer iic6 = ipv4.Assign (ndc6);

  ipv4.SetBase ("10.1.7.0","255.255.255.0");
  Ipv4InterfaceContainer iic7 = ipv4.Assign (ndc7);

  ipv4.SetBase ("192.168.1.0","255.255.255.0");
  Ipv4InterfaceContainer iic_LAN_1 = ipv4.Assign (ndc_LAN_1);
  ipv4.SetBase ("192.168.2.0","255.255.255.0");
  Ipv4InterfaceContainer iic_LAN_2 = ipv4.Assign (ndc_LAN_2);
  ipv4.SetBase ("192.168.3.0","255.255.255.0");
  Ipv4InterfaceContainer iic_LAN_3 = ipv4.Assign (ndc_LAN_3);

  Ptr<Ipv4StaticRouting> staticRouting;
 
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atk1_2->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.1.4", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atk1_1->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.1.4", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atk1_0->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.1.4", 1 );

  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atk2_7->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.2.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atk2_8->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.2.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atk2_9->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.2.1", 1 );

  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (vic1_13->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.3.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (vic1_14->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.3.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (vic1_15->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("192.168.3.1", 1 );
/*
  if (printRoutingTables)
    {
      RipHelper routingHelper;

      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);
      routingHelper.PrintRoutingTableAt (Seconds (2.0), atk1_3, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (2.0), R4, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (2.0), R5, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (2.0), atk2_6, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (2.0), R10, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (2.0), R11, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (2.0), vic1_12, routingStream);
    }
*/
/*
  NS_LOG_INFO ("Create Applications.");
  uint32_t packetSize = 1024;
  Time interPacketInterval = Seconds (1.0);
  V4PingHelper ping ("192.168.3.4");

  ping.SetAttribute ("Interval", TimeValue (interPacketInterval));
  ping.SetAttribute ("Size", UintegerValue (packetSize));
  ping.SetAttribute ("Verbose", BooleanValue (true));   //
  ApplicationContainer apps = ping.Install (atk1_0);
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (110.0));
*/
/*
  Address RxAddress(InetSocketAddress (iic_LAN_3.GetAddress(3), 10)); 
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", RxAddress);
  ApplicationContainer sinkApp = sinkHelper.Install (LAN_3_nodes.Get(3)); 
  sinkApp.Start (Seconds (41.0));
  sinkApp.Stop (Seconds (100.0));

  Address TxAddress(InetSocketAddress(iic_LAN_3.GetAddress(3),10));
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", TxAddress);

// setting up attributes for onoff application helper  
  clientHelper.SetAttribute("DataRate",StringValue("100Mbps"));
  clientHelper.SetAttribute("PacketSize",UintegerValue(1280));
  ApplicationContainer Tx = clientHelper.Install (LAN_1_nodes.Get (0));
  Tx.Start (Seconds (41.0));
  Tx.Stop (Seconds (50.0));
//////
  ApplicationContainer Tx_2 = clientHelper.Install (LAN_2_nodes.Get (3));
  Tx_2.Start (Seconds (91.0));
  Tx_2.Stop (Seconds (100.0));
*/


  //1号攻击者发送UDP封包
UdpEchoServerHelper echoServer (9);
ApplicationContainer serverApps = echoServer.Install (LAN_2_nodes.Get (3));
serverApps.Start (Seconds (41.0));
serverApps.Stop (Seconds (50.0));
UdpEchoClientHelper echoClient (iic_LAN_2.GetAddress (3), 9);
echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
ApplicationContainer clientApps = echoClient.Install (vic1_15);
clientApps.Start (Seconds (41.0));
clientApps.Stop (Seconds (50.0));

//2号攻击者发送ICMP封包
V4PingHelper ping ("192.168.2.4");//受害者的IP
ping.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
ping.SetAttribute ("Size", UintegerValue (1024));
ping.SetAttribute ("Verbose", BooleanValue (true));   
ApplicationContainer apps = ping.Install (vic1_14);
apps.Start (Seconds (41.0));
apps.Stop (Seconds (50.0));

//3号攻击者发送TCP封包
Address RxAddress(InetSocketAddress (iic_LAN_2.GetAddress(3), 10)); 
PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", RxAddress);
ApplicationContainer sinkApp = sinkHelper.Install (LAN_2_nodes.Get(3)); 
sinkApp.Start (Seconds (41.0));
sinkApp.Stop (Seconds (50.0));
Address TxAddress(InetSocketAddress(iic_LAN_2.GetAddress(3),10));
OnOffHelper clientHelper ("ns3::TcpSocketFactory", TxAddress); 
  clientHelper.SetAttribute("DataRate",StringValue("100Mbps"));
  clientHelper.SetAttribute("PacketSize",UintegerValue(1280));
ApplicationContainer Tx = clientHelper.Install (LAN_1_nodes.Get (0));
Tx.Start (Seconds (41.0));
Tx.Stop (Seconds (50.0));


  //60s R5-R10 is down
  Simulator::Schedule (Seconds (51), &TearDownLink, R5, R10, 3, 1);

/*
  AnimationInterface anim("123.xml");
  anim.SetConstantPosition (atk1_0, 0.0, 0.0);
  anim.SetConstantPosition (atk1_1, 1.0, 0.0);
  anim.SetConstantPosition (atk1_2, 2.0, 0.0);
  anim.SetConstantPosition (atk1_3, 3.0, 0.0);
  anim.SetConstantPosition (R4, 4.0, 1.0);
  anim.SetConstantPosition (R5, 5.0, 1.0);
  anim.SetConstantPosition (atk2_6, 6.0, 0.0);
  anim.SetConstantPosition (atk2_7, 7.0, 0.0);
  anim.SetConstantPosition (atk2_8, 8.0, 0.0);
  anim.SetConstantPosition (atk2_9, 9.0, 0.0);
  anim.SetConstantPosition (R10, 5.0, 2.0);
  anim.SetConstantPosition (R11, 4.0, 2.0);
  anim.SetConstantPosition (vic1_12, 3.0, 3.0);
  anim.SetConstantPosition (vic1_13, 4.0, 3.0);
  anim.SetConstantPosition (vic1_14, 5.0, 3.0);
  anim.SetConstantPosition (vic1_15, 6.0, 3.0);
*/
  //  AsciiTraceHelper ascii;
  // csma.EnableAsciiAll (ascii.CreateFileStream ("rip-simple-routing.tr"));
  csma.EnablePcapAll ("rip", true);
  

  /* Now, do the actual simulation. */
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (131.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}


