#ifndef FP_ADVENTURE_INSTANCE
#define FP_ADVENTURE_INSTANCE

//-------------------------------
//	Includes
//-------------------------------
#include "AdvNode.h"
#include "AdvObj.h"
#include "Room.h"
#include "LoadXML.h"

namespace FPAdv{
using namespace std;

typedef std::vector<char*> VecChar;
typedef std::vector<string> VecString;

//-----------------------------------
//	Instance of Adventure
//-----------------------------------
class Instance
{
public:
	Instance();
	~Instance();

	void loadXML(const char* _filename);
	void inputCmd(const char* _input);
	// OUT GRAPHICS CMDS
private:
	//-------------------------------
	//	Loading Functions
	//-------------------------------
	void loadXitem(FPxitem& _xItem);
	void loadXnode(aNode& _node, FPxitem& _parent);
	void loadXscript(aScript& _script, FPxitem& _parent);
	//-------------------------------
	//	Script Functions
	//-------------------------------
	void runScript(aScript& _script);
	//-------------------------------
	//	Locator Functions
	//-------------------------------
	Room*		getRoom(const char* _id);
    uObject*	getObject(const char* _id);
	bool		getFlag(const char* _id);

	//-------------------------------
	//	Varibles
	//-------------------------------
	string	m_InstName;			// Name of Instance
	string	m_PlayName;			// Name of main character
	int		m_cRoom;			// Current Room Index
	VecString			m_Flags;	// List of Flags
	vector<Room>		m_Rooms;	// List of Rooms
	vector<uObject>		m_Objects;	// List of Objects
	vector<aNode>		m_Node;		// List of Nodes
};

};

#endif