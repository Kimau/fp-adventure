#ifndef FP_ADVENTURE_NODE_HEAD
#define FP_ADVENTURE_NODE_HEAD

#include "AdvScript.h"

namespace FPAdv{
//-------------------------------------
//	Adventure Node
//-------------------------------------
class aNode
{
public:
	string m_Tag;			// Type of Script
	string m_Flag;			// Flags required for Node
	string m_Input;			// The Input that activates this node
	string m_Out;			// The Basic Output of Node
	vector<aScript> m_Scripts;		// List of Scripts
	vector<aNode> m_Children;		// List of Children
};


};

using namespace std;

#endif