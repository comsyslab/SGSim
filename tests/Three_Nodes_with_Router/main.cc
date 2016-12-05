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
	Scnsl::Core::Node_t *  n0=scnsl->createNode();  
       	Scnsl::Core::Node_t *  n1=scnsl->createNode();  
       	Scnsl::Core::Node_t *  n2=scnsl->createNode();  
/////////////////////////////////////////////////////////////////////////////////////////
// Channel setup
////////////////////////////////////////////////////////////////////////////////////////            
        CoreChannelSetup_t ccs;

        ccs.extensionId = "core";
        ccs.channel_type = CoreChannelSetup_t::SHARED;
        ccs.name = "SharedChannel";
        ccs.alpha = 0.1;
        ccs.delay = sc_core::sc_time( 1.0, sc_core::SC_US );
        ccs.nodes_number = 3;
        Scnsl::Core::Channel_if_t * ch = scnsl->createChannel( ccs );
////////////////////////////////////////////////////////////////////////////////////
// Task id creation
////////////////////////////////////////////////////////////////////////////////////
 
        const bool IS_SENDER = true;
        Scnsl::Core::task_id_t id0 = 0;   std::cout<<"task id"<<id0<<" is created"<< std::endl; 
        Scnsl::Core::task_id_t id1 = 1;   std::cout<<"task id"<<id1<<" is created"<< std::endl; 
        Scnsl::Core::task_id_t id2 = 2;   std::cout<<"task id"<<id2<<" is created"<< std::endl; 
 
/////////////////////////////////////////////////////////////////////////////////////
// Task creation
/////////////////////////////////////////////////////////////////////////////////////
        const Scnsl::Core::size_t ONEPROXY = 1;
        const Scnsl::Core::size_t TWOPROXIES = 2;

	 MyTask_t t( "Sensor",IS_SENDER, id0, n0, ONEPROXY);  // Transmit nodes
	 MyTask_tr trouter( "Router"          ,id1, n1, TWOPROXIES ); // Router nodes
	 MyTask_tr tr( "Collector"          ,id2, n2, ONEPROXY ); // Receive nodes
///////////////////////////////////////////////////////////////////////////////////
// Creating the protocol 802.15.4:
///////////////////////////////////////////////////////////////////////////////////
        CoreCommunicatorSetup_t ccoms;
        ccoms.extensionId = "core";
        ccoms.ack_required = true;
        ccoms.short_addresses = true;
        ccoms.type = CoreCommunicatorSetup_t::MAC_802_15_4;
        // Instances:
   
	Scnsl::Core::Communicator_if_t * mac0;
         
          ccoms.name = "Mac0";
          ccoms.node = n0;
          mac0 = scnsl->createCommunicator( ccoms ); 
          
          Scnsl::Core::Communicator_if_t * mac1;
          ccoms.name = "Mac1";
          ccoms.node = n1;
          mac1 = scnsl->createCommunicator( ccoms ); 
                         

          Scnsl::Core::Communicator_if_t * mac2;
          ccoms.name = "Mac2";
          ccoms.node = n2;
          mac2 = scnsl->createCommunicator( ccoms ); 
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
        cts.debug = 5;
        cts.log = 5;
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
 
                          tracer->trace( &t);
            tracer->trace( &trouter);
                          tracer->trace( &tr);
       
	 
/////////////////////////////////////////////////////////////////////////////
// Binding Setup:  
////////////////////////////////////////////////////////////////////////////
  BindSetup_base_t bsb0;
        bsb0.extensionId = "core";
        bsb0.destinationNode = n1;
        bsb0.node_binding.x = 0;
        bsb0.node_binding.y = 0;
        bsb0.node_binding.z = 0;
        bsb0.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb0.node_binding.transmission_power = 100;
        bsb0.node_binding.receiving_threshold = 10;

        scnsl->bind( n0, ch, bsb0 );
        scnsl->bind( & t, & trouter, ch, bsb0, mac0 );

        BindSetup_base_t bsb1;
        bsb1.extensionId = "core";
        bsb1.destinationNode = n2; // don't care: it receives only...
        bsb1.node_binding.x = 1;
        bsb1.node_binding.y = 1;
        bsb1.node_binding.z = 1;
        bsb1.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb1.node_binding.transmission_power = 1000;
        bsb1.node_binding.receiving_threshold = 1;

        scnsl->bind( n1, ch, bsb1 );
        scnsl->bind( & trouter, & tr, ch, bsb1, mac1 );
        scnsl->bind( & trouter, & t , ch, bsb1, mac1 );

        BindSetup_base_t bsb2;
        bsb2.extensionId = "core";
        bsb2.destinationNode = NULL; // don't care: it receives only...
        bsb2.node_binding.x = 2;
        bsb2.node_binding.y = 2;
        bsb2.node_binding.z = 2;
        bsb2.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb2.node_binding.transmission_power = 1000;
        bsb2.node_binding.receiving_threshold = 1;

        scnsl->bind( n2, ch, bsb2 );
        scnsl->bind( & tr , NULL, ch, bsb2, mac2);

  

/////////////////////////////////////////////////////////////////////////////////////



  


        sc_core::sc_start( sc_core::sc_time( 5000, sc_core::SC_MS ) );
        sc_core::sc_stop();
    }
    catch ( std::exception & e)
    {
        std::cerr << "Exception in sc_main(): " << e.what() << std::endl;
        return 1;
    }
	return 0;
}
