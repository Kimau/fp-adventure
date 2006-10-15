#ifndef FP_ADVENTURE_SCRIPT_HEAD
#define FP_ADVENTURE_SCRIPT_HEAD

#include <string.h>
#include <vector>

namespace FPAdv{
using namespace std;

//-------------------------------------
//	Adventure Script
//-------------------------------------
class aScript
{
public:
	string			m_Type;			// Type of Script
	vector<string>	m_Arg;			// Data
};
};

using namespace std;

#endif