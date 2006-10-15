#ifndef FP_ADVENTURE_ROOM_HEAD
#define FP_ADVENTURE_ROOM_HEAD

#include "AdvObj.h"
#include <string.h>
#include <vector>

namespace FPAdv{
using namespace std;
//------------------------------
//	Exit Class
//------------------------------
class RoomExit
{
public:
	string		m_RoomID;	// The ID of the room it exits to
	string		m_Name;		// The short desc of exit
};

//------------------------------
//	Room Class
//------------------------------
class Room
{
public:
	string		m_ID;			// ID of the room
	string		m_Name; 		// Short Name of the Room
	string		m_Desc;			// Long Desc of Room
	string		m_BackImg;		// Room Image
	vector<RoomExit> m_Exits;	// List of Valid Exits
};

};

#endif