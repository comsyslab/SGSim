////////////////////////////////////////////////////
// If you have any question please send to me
// an email under title "SCNSL for NES";
// Email: emadsamuelmalki.ebeid@univr.it
//////////////////////////////////////////////////

#include <sstream>
#include <systemc>
#include <tlm.h>
#include <exception>

#include <scnsl.hh>
#include "MyTask_t.hh"
#include "MyTask_tr.hh"
#include <fstream>
#include <iostream>

using namespace Scnsl::Setup;
using namespace Scnsl::BuiltinPlugin;
using Scnsl::Tracing::Traceable_base_t;

int sc_main( int argc, char * argv[] )
{
	try {

        // Which testcase? (from 1 to 3)
        unsigned int TESTCASE = 2; 
        unsigned int NODESNUMBER_ROW=0;
        if ( argc == 2 )
        {
            std::stringstream ss;
            ss << argv[ 1 ];
            ss >> NODESNUMBER_ROW;
           
        }
        if ( TESTCASE > 3 || TESTCASE < 1 )
        {
            TESTCASE = 2;
        }

        // Singleton.
        Scnsl::Setup::Scnsl_t * scnsl = Scnsl::Setup::Scnsl_t::get_instance();

        // Nodes creation:
	Scnsl::Core::Node_t *n[NODESNUMBER_ROW][NODESNUMBER_ROW];   //        Scnsl::Core::Node_t * n1 = sim->createNode();
        for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                        {
                    	        n[i][j]=scnsl->createNode();  
                                std::cout<<"Create NODE  S"<<i<<j<<std::endl;
                        }       


                        
        CoreChannelSetup_t ccs;

        ccs.extensionId = "core";
        ccs.channel_type = CoreChannelSetup_t::SHARED;
        ccs.name = "SharedChannel";
        ccs.alpha = 0.1;
        ccs.delay = sc_core::sc_time( 1.0, sc_core::SC_US );
        ccs.nodes_number = NODESNUMBER_ROW*NODESNUMBER_ROW;

        Scnsl::Core::Channel_if_t * ch = scnsl->createChannel( ccs );

        const bool IS_SENDER = true;
        Scnsl::Core::task_id_t id[NODESNUMBER_ROW*NODESNUMBER_ROW];
        for(unsigned int k=0; k<NODESNUMBER_ROW*NODESNUMBER_ROW; k++)
        {
                id[k]=static_cast<const Scnsl::Core::task_id_t>(k);   std::cout<<"task id"<<k<<" is created"<< std::endl; 

        }
        const Scnsl::Core::size_t ONEPROXY = 1;
        const Scnsl::Core::size_t OTHERSPROXIES = NODESNUMBER_ROW*NODESNUMBER_ROW-1 ;        

        MyTask_tr *t0;
        t0=new MyTask_tr( "Task0",TESTCASE, id[0], n[0][0], OTHERSPROXIES );std::cout<<"task t0"<<" is created, proxies="<< OTHERSPROXIES<<std::endl; 
        unsigned int k=1;
	MyTask_t * t[NODESNUMBER_ROW*NODESNUMBER_ROW-1]; // std::cout<<"hiiiiiiiiiiiiiiiiii"<<sizeof(&t) <<std::endl;
        for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                        {

                        if((i==0) && (j==0)){std::cout<<"00 is cancelled"<< std::endl; }
                        else {
        	                t[k]= new MyTask_t( "Task1",IS_SENDER, TESTCASE, id[k], n[i][j], ONEPROXY); std::cout<<"task t"<<k<<" is created"<< " id ="<<id[k]<<std::endl; 
                                k++ ;        
                              } 


                        }
        
     //   MyTask_t t1 ( "Task1",IS_SENDER, TESTCASE, id[1], n[0][1], ONEPROXY);    //    MyTas  k_tr t1b( "Task1"            , TESTCASE, id1, n1, TWOPROXIES);

        // Creating the protocol 802.15.4:
        CoreCommunicatorSetup_t ccoms;
        ccoms.extensionId = "core";
        ccoms.ack_required = true;
        ccoms.short_addresses = true;
        ccoms.type = CoreCommunicatorSetup_t::MAC_802_15_4;
        // First instance:
      unsigned int m=0;
	Scnsl::Core::Communicator_if_t * mac[NODESNUMBER_ROW*NODESNUMBER_ROW];
      for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                        {
        ccoms.name = "Mac0";
        ccoms.node = n[i][j];
        mac[m] = scnsl->createCommunicator( ccoms ); 
        m++;
                        }

        // Adding tracing features:
        CoreTracingSetup_t cts;
        cts.extensionId = "core";
        // - Setting the formatter:
        cts.formatterExtensionId = "core";
        cts.formatterName = "basic";
        // Setting the filter:
        cts.filterExtensionId = "core";
        cts.filterName = "basic";
        // Core formatter specific option:
        // printing also the type of trace:
        cts.print_trace_type = true;
     cts.info = 5;
        cts.debug = 0;
        cts.log = 0;
        cts.error = 5;
        cts.warning = 5;
        cts.fatal = 5;
        // Core formatter specific option:
        //  printing also the type of trace:
        cts.print_trace_type = true;
        // - Creating:
        Scnsl_t::Tracer_t * tracer = scnsl->createTracer( cts );
 
// creat output file
         tracer->addOutput( & std::cout );
      //std::ofstream x("log");
        // - Adding to trace:
      // tracer->addOutput(& x);
for(unsigned int i=1;i<NODESNUMBER_ROW*NODESNUMBER_ROW;i++){
        tracer->trace( t[i]);tracer->trace( t0);
       
}
	
 
        // Binding:
BindSetup_base_t bsb[(NODESNUMBER_ROW)][(NODESNUMBER_ROW)];

        bsb[0][0].extensionId = "core";  std::cout<<"bsb00"<<" is created"<< std::endl; 
        bsb[0][0].destinationNode = NULL;
        bsb[0][0].node_binding.x = 0;
        bsb[0][0].node_binding.y = 0;
        bsb[0][0].node_binding.z = 0;
        bsb[0][0].node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb[0][0].node_binding.transmission_power = 100;
        bsb[0][0].node_binding.receiving_threshold = 10;
        


for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
        {   
   if((i==0) && (j==0)) {std::cout<<"nothing"<< std::endl; }
        else
                {
        bsb[i][j].extensionId = "core";  std::cout<<"bsb"<<i<<j<<" is created"<< std::endl; 
        bsb[i][j].destinationNode = n[0][0];
        bsb[i][j].node_binding.x = i;
        bsb[i][j].node_binding.y = j;
        bsb[i][j].node_binding.z = 0;
        bsb[i][j].node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb[i][j].node_binding.transmission_power = i+j;
        bsb[i][j].node_binding.receiving_threshold = 10;
                }
        }

unsigned int mco=1;
   for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                  {
                if((i==0) && j==0) {std::cout<<"00 is cancelled"<< std::endl; }
       else 
                {
        scnsl->bind( n[i][j], ch, bsb[i][j] );  std::cout<<"bind node"<<i<<j<< "with ch via bsb" <<i<<j<<std::endl;
        scnsl->bind( t[mco],  t0, ch,bsb[i][j], mac[mco] ); std::cout<<"bind task"<<mco<< "with t0 vai bsb" <<i<<j<<std::endl;
                 mco++; 
                  }}
  
scnsl->bind( n[0][0], ch, bsb[0][0] );  
std::cout<<"bind node00"<< "with ch via bsb00"<<std::endl;
for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                               {
                if((i==0) && j==0) {std::cout<<"00 is cancelled"<< std::endl; }
       else {
  scnsl->bind(  t0, NULL, ch,bsb[0][0], mac[0] );  std::cout<<"bind node00 with ch via bsb"<<i<<j<<std::endl;
}
                         }



  


        sc_core::sc_start( sc_core::sc_time( 500, sc_core::SC_MS ) );
        sc_core::sc_stop();
}
    catch ( std::exception & e)
    {
        std::cerr << "Exception in sc_main(): " << e.what() << std::endl;
        return 1;
    }
	return 0;
}
