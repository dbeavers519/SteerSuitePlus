//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


/// @file TestCaseWriter.cpp
/// @brief Implements the SteerLib::TestCaseWriter class
///

#include "util/GenericException.h"
#include "util/Misc.h"
#include "mersenne/MersenneTwister.h"
// #include "griddatabase/GridDatabase2D.h"
#include "interfaces/SpatialDataBaseInterface.h"

#include "SteerLib.h"

using namespace std;
using namespace SteerLib;
using namespace Util;

TestCaseWriter::TestCaseWriter()
{
    _testCaseName = "" ;
    return ;
}


void TestCaseWriter::writeTestCaseToFile(
	const std::string & testCaseName, 
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::BoxObstacle> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	std::string testCaseFilename = testCaseName + ".xml" ;
    FILE * fp = fopen(testCaseFilename.c_str(), "w") ;
    if( !fp ) {
        fprintf(stderr, "TestCaseWriter::writeTestCaseToFile: ERROR: Cannot open file %s\n", testCaseFilename.c_str()) ;
        return ;
    }

    _testCaseName = testCaseName ;
    writeTestCaseToFile(fp, agents, obstacles,engineInfo) ;
    fclose(fp) ;
    return ;
}

void TestCaseWriter::writeTestCaseToFile(
	const std::string & testCaseName, 
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::ObstacleInterface*> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	std::string testCaseFilename = testCaseName + ".xml" ;
    FILE * fp = fopen(testCaseFilename.c_str(), "w") ;
    if( !fp ) {
        fprintf(stderr, "TestCaseWriter::writeTestCaseToFile: ERROR: Cannot open file %s\n", testCaseFilename.c_str()) ;
        return ;
    }

    _testCaseName = testCaseName ;
    writeTestCaseToFile(fp, agents, obstacles,engineInfo) ;
    fclose(fp) ;
    return ;
}

void TestCaseWriter::writeTestCaseToFile(
	const std::string & testCaseName,
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::ObstacleInitialConditions*> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	std::string testCaseFilename = testCaseName + ".xml" ;
    FILE * fp = fopen(testCaseFilename.c_str(), "w") ;
    if( !fp ) {
        fprintf(stderr, "TestCaseWriter::writeTestCaseToFile: ERROR: Cannot open file %s\n", testCaseFilename.c_str()) ;
        return ;
    }

    _testCaseName = testCaseName ;
    writeTestCaseToFile(fp, agents, obstacles,engineInfo) ;
    fclose(fp) ;
    return ;
}

void TestCaseWriter::writeTestCaseToFile(const std::string & testCaseName,
	std::vector<SteerLib::AgentInitialConditions> & agentIC,
	const std::vector<SteerLib::AgentInterface*> & agents,
	const std::vector<SteerLib::ObstacleInterface*> obstacles,
	SteerLib::EngineInterface *engineInfo) {
	// Create new filename
	std::string testCaseFilename = testCaseName + ".xml";

	// Attempt to open
	FILE * fp = fopen(testCaseFilename.c_str(), "w");
	if (!fp) {
		fprintf(stderr, "TestCaseWriter::writeTestCaseToFile: ERROR: Cannot open file %s\n", testCaseFilename.c_str());
		return;
	}

	_testCaseName = testCaseName;

	writeTestCaseToFile(fp, agentIC, agents, obstacles, engineInfo);
	// Close the file
	fclose(fp);
}

void TestCaseWriter::writeTestCaseToFile(FILE *fp,
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::ObstacleInitialConditions*> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	if( engineInfo == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR engine is null\n") ;
	}
	SteerLib::SpatialDataBaseInterface * grid = engineInfo->getSpatialDatabase() ;
	if( grid == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR no grid database\n") ;
	}

	/// Write header
	fprintf(fp, "<!-- \n Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman\n See license.txt for complete license.\n  -->\n") ;


	fprintf(fp, "<SteerBenchTestCase xmlns=\"http://www.magix.ucla.edu/steerbench\"\n"
                    "\t\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                    "\t\txsi:schemaLocation=\"http://www.magix.ucla.edu/steerbench\n"
                    "\t\t\t\tTestCaseSchema.xsd\">") ;

	fprintf(fp,"<header>\n\t<version>1.0</version>\n") ;
	fprintf(fp,"\t<name>%s</name>\n", _testCaseName.c_str()) ;

	/// write the world bounds
	float originX = grid->getOriginX() ;
	float originZ = grid->getOriginZ() ;
	float sizeX = grid->getOriginX() ;
	float sizeZ = grid->getOriginZ() ;
	float xmin = originX ;
	float zmin = originZ ;
	float xmax = originX + sizeX ;
	float zmax = originZ + sizeZ ;
	fprintf(fp,"\t<worldBounds>\n") ;
	fprintf(fp,"\t\t<xmin>%f</xmin>\n", xmin) ;
	fprintf(fp,"\t\t<xmax>%f</xmax>\n", xmax) ;

	fprintf(fp,"\t\t<ymin>%f</ymin>\n", 0.0f) ; // @TODO FIXME
	fprintf(fp,"\t\t<ymax>%f</ymax>\n", 0.0f) ; // @TODO FIXME

	fprintf(fp,"\t\t<zmin>%f</zmin>\n", zmin) ;
	fprintf(fp,"\t\t<zmax>%f</zmax>\n", zmax) ;
	fprintf(fp,"\t</worldBounds>\n") ;

	fprintf(fp,"</header>\n") ;

	/// write the obstacles
	std::vector<SteerLib::ObstacleInitialConditions> ::iterator iter ;
	for( unsigned int i = 0 ; i < obstacles.size() ;  i++)
	{
        SteerLib::ObstacleInitialConditions* obs = obstacles[i];

		if (typeid(*obs) == typeid(SteerLib::BoxObstacleInitialConditions))
		{
			const Util::AxisAlignedBox & box = obs->createObstacle()->getBounds() ;
			fprintf(fp,"\t<obstacle>\n") ;
			fprintf(fp,"\t\t<xmin>%f</xmin>\n", box.xmin) ;
			fprintf(fp,"\t\t<xmax>%f</xmax>\n", box.xmax) ;
			fprintf(fp,"\t\t<ymin>%f</ymin>\n", box.ymin) ;
			fprintf(fp,"\t\t<ymax>%f</ymax>\n", box.ymax) ;
			fprintf(fp,"\t\t<zmin>%f</zmin>\n", box.zmin) ;
			fprintf(fp,"\t\t<zmax>%f</zmax>\n", box.zmax) ;
			fprintf(fp,"\t</obstacle>\n") ;
		}
		else if (typeid(*obs) == typeid(SteerLib::CircleObstacleInitialConditions))
		{
			assert(false && "No support for writing this kind of obstacle");
		}
		else
		{
			assert(false && "No support for writing this kind of obstacle");
		}

	}

	/// write agents
	for( unsigned int i = 0 ; i < agents.size() ; i++)
	{
		SteerLib::AgentInitialConditions *ic = &agents[i] ;

		fprintf(fp,"\t<agent>\n") ;
		fprintf(fp,"\t<name>%s</name>\n", ic->name.c_str()) ;
		fprintf(fp,"<initialConditions>\n") ;
		fprintf(fp,"\t\t<radius>%f</radius>\n", ic->radius) ;
		fprintf(fp,"\t\t<position> <x>%f</x> <y>%f</y> <z>%f</z> </position>\n",
			ic->position[0],ic->position[1],ic->position[2]) ;
		fprintf(fp,"\t\t<direction> <x>%f</x> <y>%f</y> <z>%f</z> </direction>\n",
			ic->direction[0],ic->direction[1],ic->direction[2]) ;
		fprintf(fp,"\t\t<speed>%f</speed>\n", ic->speed) ;
		fprintf(fp, "\t\t<startTime>%f</startTime>\n", ic->startTime);
		fprintf(fp,"\t</initialConditions>\n") ;

		#ifdef _DEBUG
		std::cout << "agent desired speed " << agents[i].goals[0].desiredSpeed << "\n";
		#endif
		/// write goals of each agent
		fprintf(fp,"\t<goalSequence>\n") ;
		for( unsigned int g = 0 ; g <  agents[i].goals.size() ; g++)
		{
			std::string goalType = "seekStaticTarget" ;
			switch ( agents[i].goals[g].goalType){
				case GOAL_TYPE_SEEK_STATIC_TARGET:
					goalType = "seekStaticTarget" ;
					break ;
				case GOAL_TYPE_FLEE_STATIC_TARGET:
					goalType = "fleeStaticTarget" ;
					break ;
				case GOAL_TYPE_SEEK_DYNAMIC_TARGET:
					goalType = "seekDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLEE_DYNAMIC_TARGET:
					goalType = "fleeDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLOW_STATIC_DIRECTION:
					goalType = "flowStaticDirection" ;
					break ;
				case GOAL_TYPE_FLOW_DYNAMIC_DIRECTION:
					goalType = "flowDynamicDirection" ;
					break ;
				case GOAL_TYPE_IDLE:
					goalType = "idle" ;
				default: break ;
			}
			fprintf(fp,"\t\t<%s>\n", goalType.c_str()) ;
			fprintf(fp,"\t\t\t<targetLocation> <x>%f</x> <y>%f</y> <z>%f</z> </targetLocation>\n",
			agents[i].goals[g].targetLocation[0],agents[i].goals[g].targetLocation[1],agents[i].goals[g].targetLocation[2]) ;
			fprintf(fp,"\t\t\t<desiredSpeed>%f</desiredSpeed>\n", agents[i].goals[g].desiredSpeed) ;
			fprintf(fp,"\t\t\t<timeDuration>%f</timeDuration>\n",  agents[i].goals[g].timeDuration) ;

			//if ( !goal->targetName.empty() )
			//	fprintf(fp,"\t\t\t<targetName>%s</targetName>\n", goal->targetName.c_str()) ;

			fprintf(fp,"\t\t\t<targetDirection> <x>%f</x> <y>%f</y> <z>%f</z> </targetDirection>\n",
			agents[i].goals[g].targetDirection[0],agents[i].goals[g].targetDirection[1],agents[i].goals[g].targetDirection[2]) ;
			fprintf(fp,"\t\t\t<flowType>%s</flowType>\n", agents[i].goals[g].flowType.c_str()) ;
			std::string isRandom = "false" ;
			if( agents[i].goals[g].targetIsRandom ) isRandom = "true" ;
			fprintf(fp,"\t\t\t<random>%s</random>\n", isRandom.c_str()) ;

			if (agents[i].goals[g].targetBehaviour.getSteeringAlg() != "" )
			{
				// std::cout << "Found a Behavior while writing testcase" << std::endl;
				fprintf(fp,"\t\t\t<Behaviour>\n") ;
					fprintf(fp,"\t\t\t\t<SteeringAlgorithm>%s</SteeringAlgorithm>\n",
							agents[i].goals[g].targetBehaviour.getSteeringAlg().c_str()) ;

					if ( agents[i].goals[g].targetBehaviour.getParameters().size() > 0)
					{
						fprintf(fp,"\t\t\t\t<Parameters>\n") ;
						int p;
						for (p=0; p<agents[i].goals[g].targetBehaviour.getParameters().size(); p++ )
						{
							fprintf(fp,"\t\t\t\t\t<parameter>\n") ;
								fprintf(fp,"\t\t\t\t\t\t<key>%s</key>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).key.c_str()) ;
								fprintf(fp,"\t\t\t\t\t\t<value>%s</value>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).value.c_str()) ;
							fprintf(fp,"\t\t\t\t\t</parameter>\n") ;
						}
						fprintf(fp,"\t\t\t\t</Parameters>\n") ;
					}
				fprintf(fp,"\t\t\t</Behaviour>\n") ;
			}

			fprintf(fp,"\t\t</%s>\n", goalType.c_str()) ;
		}
		fprintf(fp,"\t</goalSequence>\n") ;

		fprintf(fp,"</agent>\n") ;
	}
		fprintf(fp,"</SteerBenchTestCase>\n") ;

}

void TestCaseWriter::writeTestCaseToFile(FILE *fp,
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::BoxObstacle> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	if( engineInfo == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR engine is null\n") ;
	}
	SteerLib::SpatialDataBaseInterface * grid = engineInfo->getSpatialDatabase() ;
	if( grid == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR no grid database\n") ;
	}	

	/// Write header
	fprintf(fp, "<!-- \n Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman\n See license.txt for complete license.\n  -->\n") ;
	

	fprintf(fp, "<SteerBenchTestCase xmlns=\"http://www.magix.ucla.edu/steerbench\"\n"
                    "\t\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                    "\t\txsi:schemaLocation=\"http://www.magix.ucla.edu/steerbench\n"
                    "\t\t\t\tTestCaseSchema.xsd\">") ;

	fprintf(fp,"<header>\n\t<version>1.0</version>\n") ;
	fprintf(fp,"\t<name>%s</name>\n", _testCaseName.c_str()) ;
	
	/// write the world bounds
	float originX = grid->getOriginX() ;
	float originZ = grid->getOriginZ() ;
	float sizeX = grid->getOriginX() ;
	float sizeZ = grid->getOriginZ() ;
	float xmin = originX ;
	float zmin = originZ ;
	float xmax = originX + sizeX ;
	float zmax = originZ + sizeZ ;
	fprintf(fp,"\t<worldBounds>\n") ;
	fprintf(fp,"\t\t<xmin>%f</xmin>\n", xmin) ;
	fprintf(fp,"\t\t<xmax>%f</xmax>\n", xmax) ;

	fprintf(fp,"\t\t<ymin>%f</ymin>\n", 0.0f) ; // @TODO FIXME
	fprintf(fp,"\t\t<ymax>%f</ymax>\n", 0.0f) ; // @TODO FIXME

	fprintf(fp,"\t\t<zmin>%f</zmin>\n", zmin) ;
	fprintf(fp,"\t\t<zmax>%f</zmax>\n", zmax) ;
	fprintf(fp,"\t</worldBounds>\n") ;

	fprintf(fp,"</header>\n") ;
	
	/// write the obstacles
	std::vector<SteerLib::BoxObstacle> ::iterator iter ;
	for( unsigned int i = 0 ; i < obstacles.size() ;  i++)
	{
		const Util::AxisAlignedBox & box = obstacles[i].getBounds() ;
		fprintf(fp,"\t<obstacle>\n") ;
		fprintf(fp,"\t\t<xmin>%f</xmin>\n", box.xmin) ;
		fprintf(fp,"\t\t<xmax>%f</xmax>\n", box.xmax) ;
		fprintf(fp,"\t\t<ymin>%f</ymin>\n", box.ymin) ;
		fprintf(fp,"\t\t<ymax>%f</ymax>\n", box.ymax) ;
		fprintf(fp,"\t\t<zmin>%f</zmin>\n", box.zmin) ;
		fprintf(fp,"\t\t<zmax>%f</zmax>\n", box.zmax) ;
		fprintf(fp,"\t</obstacle>\n") ;
	}
	
	/// write agents
	for( unsigned int i = 0 ; i < agents.size() ; i++)
	{
		SteerLib::AgentInitialConditions *ic = &agents[i] ;

		fprintf(fp,"\t<agent>\n") ;
		fprintf(fp,"\t<name>%s</name>\n", ic->name.c_str()) ;
		fprintf(fp,"<initialConditions>\n") ;
		fprintf(fp,"\t\t<radius>%f</radius>\n", ic->radius) ;
		fprintf(fp,"\t\t<position> <x>%f</x> <y>%f</y> <z>%f</z> </position>\n", 
			ic->position[0],ic->position[1],ic->position[2]) ;
		fprintf(fp,"\t\t<direction> <x>%f</x> <y>%f</y> <z>%f</z> </direction>\n", 
			ic->direction[0],ic->direction[1],ic->direction[2]) ;
		fprintf(fp,"\t\t<speed>%f</speed>\n", ic->speed) ;
		fprintf(fp, "\t\t<startTime>%f</startTime>\n", ic->startTime);
		fprintf(fp,"\t</initialConditions>\n") ;
		
		#ifdef _DEBUG
		std::cout << "agent desired speed " << agents[i].goals[0].desiredSpeed << "\n";
		#endif
		/// write goals of each agent
		fprintf(fp,"\t<goalSequence>\n") ;
		for( unsigned int g = 0 ; g <  agents[i].goals.size() ; g++)
		{
			std::string goalType = "seekStaticTarget" ;
			switch ( agents[i].goals[g].goalType){
				case GOAL_TYPE_SEEK_STATIC_TARGET:
					goalType = "seekStaticTarget" ;
					break ;
				case GOAL_TYPE_FLEE_STATIC_TARGET:
					goalType = "fleeStaticTarget" ;
					break ;
				case GOAL_TYPE_SEEK_DYNAMIC_TARGET:
					goalType = "seekDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLEE_DYNAMIC_TARGET:
					goalType = "fleeDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLOW_STATIC_DIRECTION:
					goalType = "flowStaticDirection" ;
					break ;
				case GOAL_TYPE_FLOW_DYNAMIC_DIRECTION:
					goalType = "flowDynamicDirection" ;
					break ;
				case GOAL_TYPE_IDLE:
					goalType = "idle" ;
				default: break ;
			}
			fprintf(fp,"\t\t<%s>\n", goalType.c_str()) ;
			fprintf(fp,"\t\t\t<targetLocation> <x>%f</x> <y>%f</y> <z>%f</z> </targetLocation>\n", 
			agents[i].goals[g].targetLocation[0],agents[i].goals[g].targetLocation[1],agents[i].goals[g].targetLocation[2]) ;
			fprintf(fp,"\t\t\t<desiredSpeed>%f</desiredSpeed>\n", agents[i].goals[g].desiredSpeed) ;
			fprintf(fp,"\t\t\t<timeDuration>%f</timeDuration>\n",  agents[i].goals[g].timeDuration) ;
			
			//if ( !goal->targetName.empty() )
			//	fprintf(fp,"\t\t\t<targetName>%s</targetName>\n", goal->targetName.c_str()) ;
			
			fprintf(fp,"\t\t\t<targetDirection> <x>%f</x> <y>%f</y> <z>%f</z> </targetDirection>\n", 
			agents[i].goals[g].targetDirection[0],agents[i].goals[g].targetDirection[1],agents[i].goals[g].targetDirection[2]) ;
			fprintf(fp,"\t\t\t<flowType>%s</flowType>\n", agents[i].goals[g].flowType.c_str()) ;
			std::string isRandom = "false" ;
			if( agents[i].goals[g].targetIsRandom ) isRandom = "true" ;	
			fprintf(fp,"\t\t\t<random>%s</random>\n", isRandom.c_str()) ;

			if (agents[i].goals[g].targetBehaviour.getSteeringAlg() != "" )
			{
				// std::cout << "Found a Behavior while writing testcase" << std::endl;
				fprintf(fp,"\t\t\t<Behaviour>\n") ;
					fprintf(fp,"\t\t\t\t<SteeringAlgorithm>%s</SteeringAlgorithm>\n",
							agents[i].goals[g].targetBehaviour.getSteeringAlg().c_str()) ;

					if ( agents[i].goals[g].targetBehaviour.getParameters().size() > 0)
					{
						fprintf(fp,"\t\t\t\t<Parameters>\n") ;
						int p;
						for (p=0; p<agents[i].goals[g].targetBehaviour.getParameters().size(); p++ )
						{
							fprintf(fp,"\t\t\t\t\t<parameter>\n") ;
								fprintf(fp,"\t\t\t\t\t\t<key>%s</key>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).key.c_str()) ;
								fprintf(fp,"\t\t\t\t\t\t<value>%s</value>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).value.c_str()) ;
							fprintf(fp,"\t\t\t\t\t</parameter>\n") ;
						}
						fprintf(fp,"\t\t\t\t</Parameters>\n") ;
					}
				fprintf(fp,"\t\t\t</Behaviour>\n") ;
			}

			fprintf(fp,"\t\t</%s>\n", goalType.c_str()) ;
		}
		fprintf(fp,"\t</goalSequence>\n") ;

		fprintf(fp,"</agent>\n") ;
	}
	fprintf(fp, "</SteerBenchTestCase>\n");
}

void TestCaseWriter::writeTestCaseToFile(FILE *fp,
	std::vector<SteerLib::AgentInitialConditions> & agents,
        std::vector<SteerLib::ObstacleInterface*> & obstacles,
	SteerLib::EngineInterface *engineInfo)
{
	if( engineInfo == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR engine is null\n") ;
	}
	SteerLib::SpatialDataBaseInterface * grid = engineInfo->getSpatialDatabase() ;
	if( grid == NULL )
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR no grid database\n") ;
	}	

	/// Write header
	fprintf(fp, "<!-- \n Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman\n See license.txt for complete license.\n  -->\n") ;
	

	fprintf(fp, "<SteerBenchTestCase xmlns=\"http://www.magix.ucla.edu/steerbench\"\n"
                    "\t\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                    "\t\txsi:schemaLocation=\"http://www.magix.ucla.edu/steerbench\n"
                    "\t\t\t\tTestCaseSchema.xsd\">") ;

	fprintf(fp,"<header>\n\t<version>1.0</version>\n") ;
	fprintf(fp,"\t<name>%s</name>\n", _testCaseName.c_str()) ;
	
	/// write the world bounds
	float originX = grid->getOriginX() ;
	float originZ = grid->getOriginZ() ;
	float sizeX = grid->getOriginX() ;
	float sizeZ = grid->getOriginZ() ;
	float xmin = originX ;
	float zmin = originZ ;
	float xmax = originX + sizeX ;
	float zmax = originZ + sizeZ ;
	fprintf(fp,"\t<worldBounds>\n") ;
	fprintf(fp,"\t\t<xmin>%f</xmin>\n", xmin) ;
	fprintf(fp,"\t\t<xmax>%f</xmax>\n", xmax) ;

	fprintf(fp,"\t\t<ymin>%f</ymin>\n", 0.0f) ; // @TODO FIXME
	fprintf(fp,"\t\t<ymax>%f</ymax>\n", 0.0f) ; // @TODO FIXME

	fprintf(fp,"\t\t<zmin>%f</zmin>\n", zmin) ;
	fprintf(fp,"\t\t<zmax>%f</zmax>\n", zmax) ;
	fprintf(fp,"\t</worldBounds>\n") ;

	fprintf(fp,"</header>\n") ;
	
	/// write the obstacles
	std::vector<SteerLib::BoxObstacle> ::iterator iter ;	
	for( unsigned int i = 0 ; i < obstacles.size() ;  i++)
	{
        SteerLib::ObstacleInterface* obs = obstacles[i];

		if (typeid(*obs) == typeid(PolygonObstacle))
		{
			const std::vector<Util::Point> & vertices = obs->get2DStaticGeometry();
			fprintf(fp,"\t<polygonObstacle>\n") ;			

			for(int k=0; k<vertices.size(); k++)
			{
				fprintf(fp,"\t\t<vertex> ") ;
				fprintf(fp," <x>%f</x>", vertices[k].x) ;
				fprintf(fp," <y>%f</y>", vertices[k].y) ;
				fprintf(fp," <z>%f</z>", vertices[k].z) ;
				fprintf(fp,"  </vertex>\n") ;
			}			
			fprintf(fp,"\t</polygonObstacle>\n") ;			
		}
		else	//for SteerLib::BoxObstacle and other obstacles
		{
			const Util::AxisAlignedBox & box = obs->getBounds() ;
			fprintf(fp,"\t<obstacle>\n") ;
			fprintf(fp,"\t\t<xmin>%f</xmin>\n", box.xmin) ;
			fprintf(fp,"\t\t<xmax>%f</xmax>\n", box.xmax) ;
			fprintf(fp,"\t\t<ymin>%f</ymin>\n", box.ymin) ;
			fprintf(fp,"\t\t<ymax>%f</ymax>\n", box.ymax) ;
			fprintf(fp,"\t\t<zmin>%f</zmin>\n", box.zmin) ;
			fprintf(fp,"\t\t<zmax>%f</zmax>\n", box.zmax) ;
			fprintf(fp,"\t</obstacle>\n") ;
		}
	}
	
	/// write agents
	for( unsigned int i = 0 ; i < agents.size() ; i++)
	{
		SteerLib::AgentInitialConditions *ic = &agents[i] ;

		fprintf(fp,"\t<agent>\n") ;
		fprintf(fp,"\t<name>%s</name>\n", ic->name.c_str()) ;
		fprintf(fp,"<initialConditions>\n") ;
		fprintf(fp,"\t\t<radius>%f</radius>\n", ic->radius) ;
		fprintf(fp,"\t\t<position> <x>%f</x> <y>%f</y> <z>%f</z> </position>\n", 
			ic->position[0],ic->position[1],ic->position[2]) ;
		fprintf(fp,"\t\t<direction> <x>%f</x> <y>%f</y> <z>%f</z> </direction>\n", 
			ic->direction[0],ic->direction[1],ic->direction[2]) ;
		fprintf(fp,"\t\t<speed>%f</speed>\n", ic->speed) ;
		fprintf(fp, "\t\t<startTime>%f</startTime>\n", ic->startTime);
		fprintf(fp,"\t</initialConditions>\n") ;
		
		#ifdef _DEBUG
		std::cout << "agent desired speed " << agents[i].goals[0].desiredSpeed << "\n";
		#endif
		/// write goals of each agent
		fprintf(fp,"\t<goalSequence>\n") ;
		for( unsigned int g = 0 ; g <  agents[i].goals.size() ; g++)
		{
			std::string goalType = "seekStaticTarget" ;
			switch ( agents[i].goals[g].goalType){
				case GOAL_TYPE_SEEK_STATIC_TARGET:
					goalType = "seekStaticTarget" ;
					break ;
				case GOAL_TYPE_FLEE_STATIC_TARGET:
					goalType = "fleeStaticTarget" ;
					break ;
				case GOAL_TYPE_SEEK_DYNAMIC_TARGET:
					goalType = "seekDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLEE_DYNAMIC_TARGET:
					goalType = "fleeDynamicTarget" ;
					break ;
				case GOAL_TYPE_FLOW_STATIC_DIRECTION:
					goalType = "flowStaticDirection" ;
					break ;
				case GOAL_TYPE_FLOW_DYNAMIC_DIRECTION:
					goalType = "flowDynamicDirection" ;
					break ;
				case GOAL_TYPE_IDLE:
					goalType = "idle" ;
				default: break ;
			}
			fprintf(fp,"\t\t<%s>\n", goalType.c_str()) ;
			fprintf(fp,"\t\t\t<targetLocation> <x>%f</x> <y>%f</y> <z>%f</z> </targetLocation>\n", 
			agents[i].goals[g].targetLocation[0],agents[i].goals[g].targetLocation[1],agents[i].goals[g].targetLocation[2]) ;
			fprintf(fp,"\t\t\t<desiredSpeed>%f</desiredSpeed>\n", agents[i].goals[g].desiredSpeed) ;
			fprintf(fp,"\t\t\t<timeDuration>%f</timeDuration>\n",  agents[i].goals[g].timeDuration) ;
			
			//if ( !goal->targetName.empty() )
			//	fprintf(fp,"\t\t\t<targetName>%s</targetName>\n", goal->targetName.c_str()) ;
			
			fprintf(fp,"\t\t\t<targetDirection> <x>%f</x> <y>%f</y> <z>%f</z> </targetDirection>\n", 
			agents[i].goals[g].targetDirection[0],agents[i].goals[g].targetDirection[1],agents[i].goals[g].targetDirection[2]) ;
			fprintf(fp,"\t\t\t<flowType>%s</flowType>\n", agents[i].goals[g].flowType.c_str()) ;
			std::string isRandom = "false" ;
			if( agents[i].goals[g].targetIsRandom ) isRandom = "true" ;	
			fprintf(fp,"\t\t\t<random>%s</random>\n", isRandom.c_str()) ;

			if (agents[i].goals[g].targetBehaviour.getSteeringAlg() != "" )
			{
				// std::cout << "Found a Behavior while writing testcase" << std::endl;
				fprintf(fp,"\t\t\t<Behaviour>\n") ;
					fprintf(fp,"\t\t\t\t<SteeringAlgorithm>%s</SteeringAlgorithm>\n",
							agents[i].goals[g].targetBehaviour.getSteeringAlg().c_str()) ;

					if ( agents[i].goals[g].targetBehaviour.getParameters().size() > 0)
					{
						fprintf(fp,"\t\t\t\t<Parameters>\n") ;
						int p;
						for (p=0; p<agents[i].goals[g].targetBehaviour.getParameters().size(); p++ )
						{
							fprintf(fp,"\t\t\t\t\t<parameter>\n") ;
								fprintf(fp,"\t\t\t\t\t\t<key>%s</key>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).key.c_str()) ;
								fprintf(fp,"\t\t\t\t\t\t<value>%s</value>\n", agents[i].goals[g].targetBehaviour.getParameters().at(p).value.c_str()) ;
							fprintf(fp,"\t\t\t\t\t</parameter>\n") ;
						}
						fprintf(fp,"\t\t\t\t</Parameters>\n") ;
					}
				fprintf(fp,"\t\t\t</Behaviour>\n") ;
			}

			fprintf(fp,"\t\t</%s>\n", goalType.c_str()) ;
		}
		fprintf(fp,"\t</goalSequence>\n") ;

		fprintf(fp,"</agent>\n") ;
	}
		fprintf(fp,"</SteerBenchTestCase>\n") ;
	    
}

void TestCaseWriter::writeTestCaseToFile(FILE *fp,
	std::vector<SteerLib::AgentInitialConditions> & agentIC,
	const std::vector<SteerLib::AgentInterface*> & agents,
	const std::vector<SteerLib::ObstacleInterface*> obstacles,
	SteerLib::EngineInterface *engineInfo) {
	if (engineInfo == NULL)
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR engine is null\n");
	}
	SteerLib::SpatialDataBaseInterface * grid = engineInfo->getSpatialDatabase();
	if (grid == NULL)
	{
		fprintf(stderr, "writeTestCaseToFile: ERROR no grid database\n");
	}

	/// Write header
	fprintf(fp, "<!-- \n Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman\n See license.txt for complete license.\n  -->\n");


	fprintf(fp, "<SteerBenchTestCase xmlns=\"http://www.magix.ucla.edu/steerbench\"\n"
		"\t\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
		"\t\txsi:schemaLocation=\"http://www.magix.ucla.edu/steerbench\n"
		"\t\t\t\tTestCaseSchema.xsd\">");

	fprintf(fp, "<header>\n\t<version>1.0</version>\n");
	fprintf(fp, "\t<name>%s</name>\n", _testCaseName.c_str());
	fprintf(fp, "\t<scale>%f</scale>\n", 1.0);

	/// write the world bounds
	float originX = grid->getOriginX();
	float originZ = grid->getOriginZ();
	float sizeX = grid->getOriginX();
	float sizeZ = grid->getOriginZ();
	float xmin = originX;
	float zmin = originZ;
	float xmax = originX + sizeX;
	float zmax = originZ + sizeZ;
	fprintf(fp, "\t<worldBounds>\n");
	fprintf(fp, "\t\t<xmin>%f</xmin>\n", xmin);
	fprintf(fp, "\t\t<xmax>%f</xmax>\n", xmax);

	fprintf(fp, "\t\t<ymin>%f</ymin>\n", 0.0f); // @TODO FIXME
	fprintf(fp, "\t\t<ymax>%f</ymax>\n", 0.0f); // @TODO FIXME

	fprintf(fp, "\t\t<zmin>%f</zmin>\n", zmin);
	fprintf(fp, "\t\t<zmax>%f</zmax>\n", zmax);
	fprintf(fp, "\t</worldBounds>\n");

	fprintf(fp, "</header>\n");

	/// write the obstacles
	std::vector<SteerLib::BoxObstacle> ::iterator iter;
	for (unsigned int i = 0; i < obstacles.size(); i++)
	{
		SteerLib::ObstacleInterface* obs = obstacles[i];

		if (typeid(*obs) == typeid(PolygonObstacle))
		{
			const std::vector<Util::Point> & vertices = obs->get2DStaticGeometry();
			fprintf(fp, "\t<polygonObstacle>\n");

			for (int k = 0; k<vertices.size(); k++)
			{
				fprintf(fp, "\t\t<vertex> ");
				fprintf(fp, " <x>%f</x>", vertices[k].x);
				fprintf(fp, " <y>%f</y>", vertices[k].y);
				fprintf(fp, " <z>%f</z>", vertices[k].z);
				fprintf(fp, "  </vertex>\n");
			}
			fprintf(fp, "\t</polygonObstacle>\n");
		}
		else	//for SteerLib::BoxObstacle and other obstacles
		{
			const Util::AxisAlignedBox & box = obs->getBounds();
			if (typeid(*obs) == typeid(CircleObstacle)) {
				fprintf(fp, "\t<circleObstacle>\n");
				fprintf(fp, "\t\t<radius>%f</radius>\n", abs(box.xmax - box.xmin) / 2.0);
				fprintf(fp, "\t\t<height>%f</height>\n", abs(box.ymax - box.ymin));
				fprintf(fp, "\t\t<position>\n");
				fprintf(fp, "\t\t\t<x>%f</x>\n", box.xmin + (abs(box.xmax - box.xmin) / 2.0));
				fprintf(fp, "\t\t\t<y>0</y>\n");
				fprintf(fp, "\t\t\t<z>%f</z>\n", box.zmin + (abs(box.zmax - box.zmin) / 2.0));
				fprintf(fp, "\t\t</position>\n");
				fprintf(fp, "\t</circleObstacle>\n");
			}
			else {
				fprintf(fp, "\t<obstacle>\n");
				fprintf(fp, "\t\t<xmin>%f</xmin>\n", box.xmin);
				fprintf(fp, "\t\t<xmax>%f</xmax>\n", box.xmax);
				fprintf(fp, "\t\t<ymin>%f</ymin>\n", box.ymin);
				fprintf(fp, "\t\t<ymax>%f</ymax>\n", box.ymax);
				fprintf(fp, "\t\t<zmin>%f</zmin>\n", box.zmin);
				fprintf(fp, "\t\t<zmax>%f</zmax>\n", box.zmax);
				fprintf(fp, "\t</obstacle>\n");
			}
		}
	}

	// Write agents
	int i;
	for (i = 0; i < agentIC.size(); i++) {
		SteerLib::AgentInitialConditions *ic = &agentIC[i];

		fprintf(fp, "\t<agent>\n");
		fprintf(fp, "\t<name>%s</name>\n", ic->name.c_str());
		fprintf(fp, "<initialConditions>\n");
		fprintf(fp, "\t\t<radius>%f</radius>\n", ic->radius);
		fprintf(fp, "\t\t<position> <x>%f</x> <y>%f</y> <z>%f</z> </position>\n",
			ic->position[0], ic->position[1], ic->position[2]);
		fprintf(fp, "\t\t<direction> <x>%f</x> <y>%f</y> <z>%f</z> </direction>\n",
			ic->direction[0], ic->direction[1], ic->direction[2]);
		fprintf(fp, "\t\t<speed>%f</speed>\n", ic->speed);
		fprintf(fp, "\t\t<startTime>%f</startTime>\n", ic->startTime);
		fprintf(fp, "\t</initialConditions>\n");

#ifdef _DEBUG
		std::cout << "agent desired speed " << agentIC[i].goals[0].desiredSpeed << "\n";
#endif
		/// write goals of each agent
		fprintf(fp, "\t<goalSequence>\n");
		for (unsigned int g = 0; g < agentIC[i].goals.size(); g++)
		{
			std::string goalType = "seekStaticTarget";
			switch (agentIC[i].goals[g].goalType) {
			case GOAL_TYPE_SEEK_STATIC_TARGET:
				goalType = "seekStaticTarget";
				break;
			case GOAL_TYPE_FLEE_STATIC_TARGET:
				goalType = "fleeStaticTarget";
				break;
			case GOAL_TYPE_SEEK_DYNAMIC_TARGET:
				goalType = "seekDynamicTarget";
				break;
			case GOAL_TYPE_FLEE_DYNAMIC_TARGET:
				goalType = "fleeDynamicTarget";
				break;
			case GOAL_TYPE_FLOW_STATIC_DIRECTION:
				goalType = "flowStaticDirection";
				break;
			case GOAL_TYPE_FLOW_DYNAMIC_DIRECTION:
				goalType = "flowDynamicDirection";
				break;
			case GOAL_TYPE_IDLE:
				goalType = "idle";
			default: break;
			}
			fprintf(fp, "\t\t<%s>\n", goalType.c_str());
			fprintf(fp, "\t\t\t<targetLocation> <x>%f</x> <y>%f</y> <z>%f</z> </targetLocation>\n",
				agentIC[i].goals[g].targetLocation[0], agentIC[i].goals[g].targetLocation[1], agentIC[i].goals[g].targetLocation[2]);
			fprintf(fp, "\t\t\t<desiredSpeed>%f</desiredSpeed>\n", agentIC[i].goals[g].desiredSpeed);
			fprintf(fp, "\t\t\t<timeDuration>%f</timeDuration>\n", agentIC[i].goals[g].timeDuration);

			//if ( !goal->targetName.empty() )
			//	fprintf(fp,"\t\t\t<targetName>%s</targetName>\n", goal->targetName.c_str()) ;

			fprintf(fp, "\t\t\t<targetDirection> <x>%f</x> <y>%f</y> <z>%f</z> </targetDirection>\n",
				agentIC[i].goals[g].targetDirection[0], agentIC[i].goals[g].targetDirection[1], agentIC[i].goals[g].targetDirection[2]);
			fprintf(fp, "\t\t\t<flowType>%s</flowType>\n", agentIC[i].goals[g].flowType.c_str());
			std::string isRandom = "false";
			if (agentIC[i].goals[g].targetIsRandom) isRandom = "true";
			fprintf(fp, "\t\t\t<random>%s</random>\n", isRandom.c_str());

			if (agentIC[i].goals[g].targetBehaviour.getSteeringAlg() != "")
			{
				// std::cout << "Found a Behavior while writing testcase" << std::endl;
				fprintf(fp, "\t\t\t<Behaviour>\n");
				fprintf(fp, "\t\t\t\t<SteeringAlgorithm>%s</SteeringAlgorithm>\n",
					agentIC[i].goals[g].targetBehaviour.getSteeringAlg().c_str());

				if (agentIC[i].goals[g].targetBehaviour.getParameters().size() > 0)
				{
					fprintf(fp, "\t\t\t\t<Parameters>\n");
					int p;
					for (p = 0; p < agentIC[i].goals[g].targetBehaviour.getParameters().size(); p++)
					{
						fprintf(fp, "\t\t\t\t\t<parameter>\n");
						fprintf(fp, "\t\t\t\t\t\t<key>%s</key>\n", agentIC[i].goals[g].targetBehaviour.getParameters().at(p).key.c_str());
						fprintf(fp, "\t\t\t\t\t\t<value>%s</value>\n", agentIC[i].goals[g].targetBehaviour.getParameters().at(p).value.c_str());
						fprintf(fp, "\t\t\t\t\t</parameter>\n");
					}
					fprintf(fp, "\t\t\t\t</Parameters>\n");
				}
				fprintf(fp, "\t\t\t</Behaviour>\n");
			}

			fprintf(fp, "\t\t</%s>\n", goalType.c_str());
		}
		fprintf(fp, "\t</goalSequence>\n");
		// Write agent locations
		fprintf(fp, "\t<sim_real>\n");
		for (int j = 0; j < agents[i]->getPositionList().size(); j++) {
			fprintf(fp, "\t\t<location>\n");
			fprintf(fp, "\t\t\t<time>%f</time>\n", agents[i]->getPositionList().at(j).first);
			fprintf(fp, "\t\t\t<x>%f</x>\n", agents[i]->getPositionList().at(j).second.x);
			fprintf(fp, "\t\t\t<y>%f</y>\n", agents[i]->getPositionList().at(j).second.y);
			fprintf(fp, "\t\t\t<z>%f</z>\n", agents[i]->getPositionList().at(j).second.z);
			fprintf(fp, "\t\t</location>\n");
		}
		fprintf(fp, "\t</sim_real>\n");
		fprintf(fp, "</agent>\n");
	}
	fprintf(fp, "</SteerBenchTestCase>\n");
}