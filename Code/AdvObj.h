#ifndef FP_ADVENTURE_OBJECT_HEAD
#define FP_ADVENTURE_OBJECT_HEAD

#include "AdvScript.h"

namespace FPAdv{

//-------------------------------------
//	Triggered Action
//-------------------------------------
class TrigAction
{
public:
	//--------------------------------
	//	Conditions
	//--------------------------------
	string	m_Verb;					// Verb
	string	m_Object;				// Object action takes place on
	string	m_Room;					// Room ID action takes place in
	string	m_Flag;					// Flag required by action

	vector<aScript> m_Scripts;		// First Script	
};

//-------------------------------------
//	Useable Object
//-------------------------------------
class uObject
{
public:
	string	m_ID;			// Ident Object
	string	m_RoomID;		// Current Room ID
	string	m_Name;			// Name of Object
	string	m_Desc;			// Desc of Object
	string	m_InvImg;		// Inventory Image (64x64)
	vector<TrigAction> m_Actions;	// First Action
};
};

#endif