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

#define DX 5  // room space
#define DP 10  //  min. power for 1 hop transmission
#define MAXPOWER 100 // max. power

using namespace Scnsl::Setup;
using namespace Scnsl::BuiltinPlugin;
using Scnsl::Tracing::Traceable_base_t;

int sc_main( int argc, char * argv[] )
{
 try {

 
        
        unsigned int NODESNUMBER_ROW=0;
        if ( argc == 2 )
        {
            std::stringstream ss;
            ss << argv[ 1 ];
            ss >> NODESNUMBER_ROW;
           
        }
 
        // Singleton.
        Scnsl::Setup::Scnsl_t * scnsl = Scnsl::Setup::Scnsl_t::get_instance();
///////////////////////////////////////////////////////////////////////////////////////////////////
// Nodes creation:
//////////////////////////////////////////////////////////////////////////////////////////////////
	Scnsl::Core::Node_t *n[NODESNUMBER_ROW][NODESNUMBER_ROW];   
        for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                        {
                    	        n[i][j]=scnsl->createNode();  
                                std::cout<<"Create NODE  S"<<i<<j<<std::endl;
                        }       

/////////////////////////////////////////////////////////////////////////////////////////
// Channel setup
////////////////////////////////////////////////////////////////////////////////////////            
        CoreChannelSetup_t ccs;

        ccs.extensionId = "core";
        ccs.channel_type = CoreChannelSetup_t::SHARED;
        ccs.name = "SharedChannel";
        ccs.capacity = 10000000;

        ccs.alpha = 0.01;
        ccs.delay = sc_core::sc_time( 1.0, sc_core::SC_US );
        ccs.nodes_number = NODESNUMBER_ROW*NODESNUMBER_ROW;

 

        Scnsl::Core::Channel_if_t * ch = scnsl->createChannel( ccs );
////////////////////////////////////////////////////////////////////////////////////
// Task id creation
////////////////////////////////////////////////////////////////////////////////////
        unsigned int k=0;
        const bool IS_SENDER = true;
        Scnsl::Core::task_id_t id[NODESNUMBER_ROW][NODESNUMBER_ROW];
             for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                        {
                id[i][j]=static_cast<const Scnsl::Core::task_id_t>(k);   std::cout<<"task id"<<id[i][j]<<" ="<<k<<" is created"<< std::endl; 
                k++;
                        }
/////////////////////////////////////////////////////////////////////////////////////
// Task creation
/////////////////////////////////////////////////////////////////////////////////////
        const Scnsl::Core::size_t ONEPROXY = 1;
        const Scnsl::Core::size_t OTHERSPROXIES = NODESNUMBER_ROW-1 ;     // only first column nodes receive  

	MyTask_t  * t[NODESNUMBER_ROW][NODESNUMBER_ROW]; // Transmit nodes
	MyTask_tr * tr[NODESNUMBER_ROW][NODESNUMBER_ROW]; // Receive nodes
        for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                        {


                        if((j==0)){ 
                                       tr[i][j]=new MyTask_tr( "Collector"          ,id[i][j], n[i][j], OTHERSPROXIES );std::cout<<"task tr "<<" in point "<<i<<j<<" is created, with proxies="<< OTHERSPROXIES<<std::endl; 
                                  }
                        else {
        	                       t[i][j]= new MyTask_t( "Sensor",IS_SENDER, id[i][j], n[i][j], ONEPROXY); std::cout<<"task t "<<"in point "<<i<<j<<" is created, with proxies="<< ONEPROXY<<std::endl; 
                                        
                              } 

                        }
///////////////////////////////////////////////////////////////////////////////////
// Creating the protocol 802.15.4:
///////////////////////////////////////////////////////////////////////////////////
        CoreCommunicatorSetup_t ccoms;
        ccoms.extensionId = "core";
        ccoms.ack_required = true;
        ccoms.short_addresses = true;
        ccoms.type = CoreCommunicatorSetup_t::MAC_802_15_4;
        // Instances:
   
	Scnsl::Core::Communicator_if_t * mac[NODESNUMBER_ROW][NODESNUMBER_ROW];
        for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                        {
          ccoms.name = "Mac0";
          ccoms.node = n[i][j];
          mac[i][j] = scnsl->createCommunicator( ccoms ); 
                         }
//////////////////////////////////////////////////////////////////////////////////
//Tracing
//////////////////////////////////////////////////////////////////////////////////
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
        for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
                for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
                     {
                if(j==0)
                        {
                          tracer->trace( tr[i][j]);
                        }
                else
                          tracer->trace( t[i][j]);
       
                     }
	 
/////////////////////////////////////////////////////////////////////////////
// Binding Setup:  
////////////////////////////////////////////////////////////////////////////
BindSetup_base_t bsb[(NODESNUMBER_ROW)][(NODESNUMBER_ROW)];
for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
 for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
 {
   if(j==0)
        {
        bsb[i][j].extensionId = "core";  std::cout<<"bsb"<<i<<j<<" is created"<< std::endl; 
        bsb[i][j].destinationNode = NULL;
        bsb[i][j].node_binding.x = j;
        bsb[i][j].node_binding.y = i;
        bsb[i][j].node_binding.z = 0;
        bsb[i][j].node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb[i][j].node_binding.transmission_power = MAXPOWER;
        bsb[i][j].node_binding.receiving_threshold = 1;
        }


        else
                {
        bsb[i][j].extensionId = "core";  std::cout<<"bsb"<<i<<j<<" is created with node "<<i<<"0"<< std::endl; 
        bsb[i][j].destinationNode = n[i][0];
        bsb[i][j].node_binding.x = j*DX;
        bsb[i][j].node_binding.y = i;
        bsb[i][j].node_binding.z = 0;
        bsb[i][j].node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb[i][j].node_binding.transmission_power = DP*j; // for adaptive power and DP*N for max fixed power
        bsb[i][j].node_binding.receiving_threshold = 10;
                }
 }
////////////////////////////////////////////////////////////////////////////////////
//Task binding
////////////////////////////////////////////////////////////////////////////////////
   for(unsigned int i=0; i<NODESNUMBER_ROW; i++)
       for(unsigned int j=0; j<NODESNUMBER_ROW; j++)
        {
          if((j==0)) 
                {
                  scnsl->bind(  n[i][j], ch, bsb[i][j] );  std::cout<<"bind node"<<i<<j<< "with ch via bsb" <<i<<j<<std::endl;
     
                }
          else 
                {
                 scnsl->bind(  tr[i][0], NULL, ch,bsb[i][0], mac[i][0] );  std::cout<<"bind task"<<i<<"0 with NULL"<<std::endl; 
                 scnsl->bind( n[i][j], ch, bsb[i][j] );  std::cout<<"bind node"<<i<<j<< "with ch via bsb" <<i<<j<<std::endl;
                 scnsl->bind( t[i][j],  tr[i][0], ch,bsb[i][j], mac[i][j] ); std::cout<<"bind task"<<i<<j<< "with t"<<i<<"0 vai bsb" <<i<<j<<std::endl;
        
                }
         }
  

/////////////////////////////////////////////////////////////////////////////////////



  


        sc_core::sc_start( sc_core::sc_time( 25000, sc_core::SC_MS ) );
        sc_core::sc_stop();
    }
    catch ( std::exception & e)
    {
        std::cerr << "Exception in sc_main(): " << e.what() << std::endl;
        return 1;
    }
	return 0;
}
