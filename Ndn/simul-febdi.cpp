// ndn-grid-topo-plugin.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <string>

namespace ns3 {

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);
  int contsize = 100000;
  string fname = "client-control";

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-tugas-ndn.txt");
  topologyReader.Read();

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.setPolicy("nfd::cs::lru");
  ndnHelper.setCsSize(contsize); // Ini dijadiin parameter yang diamati
  ndnHelper.InstallAll();

  // Set BestRoute strategy
  // Ini dijadiin parameter yang diamati 3 saja
  ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/"+fname);

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Getting containers for the consumer/producer
  NodeContainer producer;
  producer.Add(Names::Find<Node>("SORONG"));
  producer.Add(Names::Find<Node>("KUPANG"));
  producer.Add(Names::Find<Node>("WAMENA"));


  NodeContainer consumerNodes;
  consumerNodes.Add(Names::Find<Node>("MERAUKE"));
  consumerNodes.Add(Names::Find<Node>("WAINGAPU"));
  consumerNodes.Add(Names::Find<Node>("WARIS"));
  consumerNodes.Add(Names::Find<Node>("MANOKWARI"));
  consumerNodes.Add(Names::Find<Node>("TIGI"));
  consumerNodes.Add(Names::Find<Node>("ILAGA"));
  consumerNodes.Add(Names::Find<Node>("AGATS"));
  consumerNodes.Add(Names::Find<Node>("DEKAI"));
  consumerNodes.Add(Names::Find<Node>("DOBO"));

  // Install NDN applications
  std::string prefix = "/prefix";

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetPrefix(prefix);
  consumerHelper.SetAttribute("Frequency", StringValue("10000")); // 100 interests a second
  consumerHelper.Install(consumerNodes);

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetPrefix(prefix);
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));  //inspect, what is this?
  producerHelper.Install(producer);

  ndn::L3RateTracer::InstallAll("/home/febriyanto/Desktop/ndnSIM/ndn-simul/dataset/simulrev1/rate-trace-"+fname+".txt", Seconds(1.0));
  L2RateTracer::InstallAll("/home/febriyanto/Desktop/ndnSIM/ndn-simul/dataset/simulrev1/drop-trace-"+fname+".txt", Seconds(1.0));
  ndn::CsTracer::InstallAll("/home/febriyanto/Desktop/ndnSIM/ndn-simul/dataset/simulrev1/cs-trace-"+fname+".txt", Seconds(1.0));
  ndn::AppDelayTracer::InstallAll("/home/febriyanto/Desktop/ndnSIM/ndn-simul/dataset/simulrev1/app-delays-trace-"+fname+".txt");

  // Add /prefix origins to ndn::GlobalRouter
  ndnGlobalRoutingHelper.AddOrigins(prefix, producer);

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}