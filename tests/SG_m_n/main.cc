////////////////////////////////////////////////////
// for Smart Grid simulation
// Email: esme@eng.au.dk
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

//#define DX 5  // room space
//#define DP 10  //  min. power for 1 hop transmission
#define MAXPOWER 100 // max. power

using namespace Scnsl::Setup;
using namespace Scnsl::BuiltinPlugin;
using Scnsl::Tracing::Traceable_base_t;

int sc_main( int argc, char * argv[] )
{
 try {

 
        
        unsigned int NumberOfBuildings=0;
        unsigned int NumberOfAppliances=0;
        if ( argc == 3 )
        {
            std::stringstream ss, sa;
            ss << argv[ 1 ];
            ss >> NumberOfBuildings;
	    sa << argv[ 2 ];
            sa >> NumberOfAppliances;
	    std::cout<<"The Network has "<<NumberOfBuildings<<" buildings and each building has "<< NumberOfAppliances <<" Appliances"<<std::endl; 
           
        }
	else
	{
           std::cout<<"Please enter the building number and appliances per each building."<<std::endl;
	
	} 
        // Singleton.
        Scnsl::Setup::Scnsl_t * scnsl = Scnsl::Setup::Scnsl_t::get_instance();
///////////////////////////////////////////////////////////////////////////////////////////////////
// Nodes creation:
//////////////////////////////////////////////////////////////////////////////////////////////////
	Scnsl::Core::Node_t *n[NumberOfBuildings][NumberOfAppliances];   
        Scnsl::Core::Node_t *n_b[NumberOfBuildings];

        for(unsigned int i=0; i<NumberOfBuildings; i++)
            for(unsigned int j=0; j<NumberOfAppliances; j++)
            {
                n[i][j]=scnsl->createNode();  
                std::cout<<"Create Appliance NODE["<<i<<","<<j<<"]"<< "for Building:"<<i<<"  "<<std::endl;
            }       
      

        for(unsigned int j=0; j<NumberOfBuildings; j++)
        {
            n_b[j]=scnsl->createNode();
            std::cout<<"Create Building NODE["<<j<<"]"<< "for Building:"<<j<<std::endl;
        }
        Scnsl::Core::Node_t * n_server;
	n_server=scnsl->createNode();

	std::cout<<"Create NODE Server"<<std::endl;
/////////////////////////////////////////////////////////////////////////////////////////
// Channel setup
////////////////////////////////////////////////////////////////////////////////////////            
        CoreChannelSetup_t ccsa;

        ccsa.extensionId = "core";
        ccsa.channel_type = CoreChannelSetup_t::SHARED;
        ccsa.name = "SharedChannel_Appliances<->Building";
        ccsa.capacity = 10000000;

        ccsa.alpha = 0.01;
        ccsa.delay = sc_core::sc_time( 1.0, sc_core::SC_US );
        ccsa.nodes_number = NumberOfAppliances +1;

        Scnsl::Core::Channel_if_t * ch_a[NumberOfBuildings];
     
	for(unsigned int j=0; j<NumberOfBuildings; j++)
        {
	ch_a[j] = scnsl->createChannel( ccsa );
	std::cout<<"Create Channel ch_a_"<<j<<std::endl;
	}
//////////////////////////////////////////////
        CoreChannelSetup_t ccsb;

        ccsb.extensionId = "core";
        ccsb.channel_type = CoreChannelSetup_t::SHARED;
        ccsb.name = "SharedChannel_Building<->Server";
        ccsb.capacity = 100000000;

        ccsb.alpha = 0.01;
        ccsb.delay = sc_core::sc_time( 1.0, sc_core::SC_US );
        ccsb.nodes_number = NumberOfBuildings+1;

        Scnsl::Core::Channel_if_t * ch_b = scnsl->createChannel( ccsb );

////////////////////////////////////////////////////////////////////////////////////
// Task id creation
////////////////////////////////////////////////////////////////////////////////////
        unsigned int k=0;
        const bool IS_SENDER = true;
        Scnsl::Core::task_id_t id[NumberOfBuildings][NumberOfAppliances];
             for(unsigned int i=0; i<NumberOfBuildings; i++)
                for(unsigned int j=0; j<NumberOfAppliances; j++)
                {
                id[i][j]=static_cast<const Scnsl::Core::task_id_t>(k);   
		std::cout<<"Task id"<<id[i][j]<<" = "<<k<<" is created."<< std::endl; 
                k++;
                }
//Task IDs for buildings starts from 1,000,000 
	k=1000000;
        Scnsl::Core::task_id_t id_b[NumberOfBuildings];

	for(unsigned int j=0; j<NumberOfBuildings; j++)
              {
                id_b[j]=static_cast<const Scnsl::Core::task_id_t>(k);
                std::cout<<"Building number "<< j << " gets a Task ID "<<id_b[j]<<std::endl;
                k++;
               }

//Task ID for server is 10,000,000
Scnsl::Core::task_id_t id_server = static_cast<const Scnsl::Core::task_id_t>(10000000);
/////////////////////////////////////////////////////////////////////////////////////
// Task creation
/////////////////////////////////////////////////////////////////////////////////////
        const Scnsl::Core::size_t ONEPROXY = 1;
	const Scnsl::Core::size_t ServerPROXY = NumberOfBuildings;
        const Scnsl::Core::size_t BuildingPROXY = NumberOfAppliances + 1 ;     // number of appliances + the connected server 
	const char* applianceName = "App_"; 
        const char* buildingName = "B_";


	MyTask_t  * t_appliance[NumberOfBuildings][NumberOfAppliances]; // Appliance task
	MyTask_tr * t_building[NumberOfBuildings]; //  Building Task
        MyTask_tr * t_server; //  Server Task
        for(unsigned int i=0; i<NumberOfBuildings; i++)
            for(unsigned int j=0; j<NumberOfAppliances; j++)
            { //  applianceName = "App_" + j;
                t_appliance[i][j]= new MyTask_t( applianceName,IS_SENDER, id[i][j], n[i][j], ONEPROXY); 
                std::cout<<"Appliance task  "<<i<<j<<" is created, with proxy ="<< ONEPROXY<<std::endl;  
	}

        for(unsigned int i=0; i<NumberOfBuildings; i++)
	{
           t_building[i]= new MyTask_tr( buildingName, id_b[i], n_b[i], BuildingPROXY); 
	   std::cout<<"Building task  "<<i<<" is created, with proxy ="<< BuildingPROXY<<std::endl;
	}
	      
        t_server= new MyTask_tr( "Server", id_server, n_server, ServerPROXY ); 
        std::cout<<"Server task  "<<id_server<<" is created, with proxy ="<< ONEPROXY<<std::endl;

///////////////////////////////////////////////////////////////////////////////////
// Creating the protocol 802.15.4:
///////////////////////////////////////////////////////////////////////////////////
        CoreCommunicatorSetup_t ccoms;
        ccoms.extensionId = "core";
        ccoms.ack_required = true;
        ccoms.short_addresses = true;
        ccoms.type = CoreCommunicatorSetup_t::MAC_802_15_4;

///////////////////////////////////////////////////////////////////////////////////
// Creating the protocol for wire:
///////////////////////////////////////////////////////////////////////////////////
        CoreCommunicatorSetup_t ccoms_Wire;
        ccoms_Wire.extensionId = "core";
        ccoms_Wire.ack_required = true;
        ccoms_Wire.short_addresses = true;
//        ccoms_Wire.type = CoreCommunicatorSetup_t::MAC_802_15_4;

//////////////////////////////////////////////////////////////////////////////////
// Instances:

//Appliances:
   
	Scnsl::Core::Communicator_if_t * mac[NumberOfBuildings][NumberOfAppliances];
        for(unsigned int i=0; i<NumberOfBuildings; i++)
           for(unsigned int j=0; j<NumberOfAppliances; j++)
           {
               ccoms.name = "Mac_Appliance";
               ccoms.node = n[i][j];
               mac[i][j] = scnsl->createCommunicator( ccoms ); 
	       std::cout<<"Appliance MAC  for node"<<i<<j<<" is created."<<std::endl;
            }
//Buildings
        Scnsl::Core::Communicator_if_t * mac_b[NumberOfBuildings];
        for(unsigned int j=0; j<NumberOfBuildings; j++)
           {
            ccoms_Wire.name = "Mac_Building";
            ccoms_Wire.node = n_b[j];
            mac_b[j] = scnsl->createCommunicator( ccoms_Wire ); 
            std::cout<<"Building MAC for node "<<j<<" is created."<<std::endl;
            }
//Server
            Scnsl::Core::Communicator_if_t * mac_server;     
            ccoms_Wire.name = "Mac_Server";
            ccoms_Wire.node = n_server;
            mac_server = scnsl->createCommunicator( ccoms_Wire ); 
            std::cout<<"Server MAC for node server is created."<<std::endl;

//////////////////////////////////////////////////////////////////////////////////
//Tracing
//////////////////////////////////////////////////////////////////////////////////
/*        // Adding tracing features:
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
*/	 
/////////////////////////////////////////////////////////////////////////////
// Binding Setup:  
////////////////////////////////////////////////////////////////////////////
//Appliance <-> Home bind
BindSetup_base_t bsbA_H[(NumberOfBuildings)][(NumberOfAppliances)];
    for(unsigned int i=0; i<NumberOfBuildings; i++)
	for(unsigned int j=0; j<NumberOfAppliances; j++)
 	{
	    bsbA_H[i][j].extensionId = "core";  
	    std::cout<<"Bind Setup between Appliance "<<i<<j<<" and Home "<<i<<" is created.Location ("<<j<<",0)."<< std::endl; 
            bsbA_H[i][j].destinationNode = n_b[i];  //distination node is the building
            bsbA_H[i][j].node_binding.x = j;
            bsbA_H[i][j].node_binding.y = 0;
            bsbA_H[i][j].node_binding.z = 0;
            bsbA_H[i][j].node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
            bsbA_H[i][j].node_binding.transmission_power = MAXPOWER;
            bsbA_H[i][j].node_binding.receiving_threshold = 1;
 	}
//Home <-> Appliances
BindSetup_base_t bsbH_A[(NumberOfBuildings)];
    for(unsigned int i=0; i<NumberOfBuildings; i++)
    {
        bsbH_A[i].extensionId = "core";  
        std::cout<<"Bind Setup between Home "<<i<<" and its Appliances. Home location is ("<<i<<",1)."<< std::endl; 
        bsbH_A[i].destinationNode = NULL;  //distinatin is NULL, 
        bsbH_A[i].node_binding.x = i;
        bsbH_A[i].node_binding.y = 1;
        bsbH_A[i].node_binding.z = 0;
        bsbH_A[i].node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsbH_A[i].node_binding.transmission_power = MAXPOWER;
        bsbH_A[i].node_binding.receiving_threshold = 1;
     }
//Home <-> Server
BindSetup_base_t bsbH_S[(NumberOfBuildings)];
    for(unsigned int i=0; i<NumberOfBuildings; i++)
    {
        bsbH_S[i].extensionId = "core";  
        std::cout<<"Bind Setup between Home "<<i<<" and Server is created. Home Location ("<<i<<",1)."<< std::endl; 
        bsbH_S[i].destinationNode = n_server;  //distination node is the server
        bsbH_S[i].node_binding.x = i;
        bsbH_S[i].node_binding.y = 1;
        bsbH_S[i].node_binding.z = 0;
        bsbH_S[i].node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsbH_S[i].node_binding.transmission_power = MAXPOWER;
        bsbH_S[i].node_binding.receiving_threshold = 1;
     }
//Server <-> Home
BindSetup_base_t bsbS_H;
   
        bsbS_H.extensionId = "core";  
        std::cout<<"Bind Setup between Server and Homes is created. Location (0,2)."<< std::endl; 
        bsbS_H.destinationNode = NULL;  //distination node is the server
        bsbS_H.node_binding.x = 0;
        bsbS_H.node_binding.y = 2;
        bsbS_H.node_binding.z = 0;
        bsbS_H.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsbS_H.node_binding.transmission_power = MAXPOWER;
        bsbS_H.node_binding.receiving_threshold = 1;
     
////////////////////////////////////////////////////////////////////////////////////
//Channel binding
////////////////////////////////////////////////////////////////////////////////////
//Appliances with homes through ch_a
    for(unsigned int i=0; i<NumberOfBuildings; i++)
    {
        for(unsigned int j=0; j<NumberOfAppliances; j++)
        {
	    scnsl->bind(n[i][j], ch_a[i], bsbA_H[i][j]);
	    std::cout<<"Bind between node ("<<i<<","<<j<<") and Channel_a_"<<i<<" is created" <<std::endl; 
	}    
    }

//Homes with Appliances' channel
        for(unsigned int j=0; j<NumberOfBuildings; j++)
        {
            scnsl->bind(n_b[j], ch_a[j], bsbH_A[j]);
            std::cout<<"Bind between Home node ("<<j<<") and Home Channel_a_"<<j<<" is created." <<std::endl; 
        } 

//Homes with server
        for(unsigned int j=0; j<NumberOfBuildings; j++)
        {
            scnsl->bind(n_b[j], ch_b, bsbH_S[j]);
            std::cout<<"Bind between Home node ("<<j<<") and Home Channel_b with the server is created." <<std::endl; 
        } 
//Server with Homes
                //std::cout<<"Bind between Server node and Homes is created." <<n_server<<std::endl; 

	scnsl->bind(n_server, ch_b, bsbS_H);
        std::cout<<"Bind between Server node and Homes is created." <<std::endl; 

////////////////////////////////////////////////////////////////////////////////////
//Task binding
////////////////////////////////////////////////////////////////////////////////////
     for(unsigned int i=0; i<NumberOfBuildings; i++)
     {  
         for(unsigned int j=0; j<NumberOfAppliances; j++)
         { 
             scnsl->bind(  t_appliance[i][j], t_building[i], ch_a[i],bsbA_H[i][j], mac[i][j] );  
                // scnsl->bind( n[i][j], ch, bsb[i][j] );  
	     std::cout<<"bind task Appliance("<<i<<j<< ") with ch_a_"<<i <<"via bsbA_H_" <<i<<j<<std::endl;
             scnsl->bind( t_building[i],  t_appliance[i][j], ch_a[i],bsbH_A[i], mac_b[i] ); 
             std::cout<<"bind task Building"<<i<< "with task appliance"<<i<<j<<" via channel ch_a_"<<i<<" and bsbH_A" <<i<<std::endl;
          }
     }

     for(unsigned int j=0; j<NumberOfBuildings; j++)
     {
         scnsl->bind(  t_building[j], t_server, ch_b, bsbH_S[j], mac_b[j] );  
         std::cout<<"bind task Building("<<j<< ") with ch_b via bsbH_S_" <<j<<std::endl;
         scnsl->bind( t_server, t_building[j], ch_b,bsbS_H, mac_server ); 
         std::cout<<"bind task Server with task building "<<j<<" via channel ch_b_"<<j<<" and bsbS_H" <<std::endl;
    }  

/////////////////////////////////////////////////////////////////////////////////////
        sc_core::sc_start( sc_core::sc_time( 10, sc_core::SC_S ) );
        sc_core::sc_stop();
    }
    catch ( std::exception & e)
    {
        std::cerr << "Exception in sc_main(): " << e.what() << std::endl;
        return 1;
    }
	return 0;
}
