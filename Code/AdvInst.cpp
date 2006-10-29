#include "AdvInst.h"
#include <stdio.h>
#include <assert.h>
#include <time.h>

#define FPX_CASE(var,low,high)		\
			case low:	case high:	\
			var = _subItem.m_Data;	\
			break;
#define FPY_CASE(var,low,high)		\
			case low:	case high:	\
			var = _subItem->m_Data;	\
			break;

using namespace FPAdv;
//==============================================================
//	Create Blank Instance
//==============================================================
Instance::Instance()
{
	m_cRoom = 0;
	m_TimeStart = time(0);
	//-------------------------------
	//	Create Nowhere
	//-------------------------------
	m_Rooms.push_back(Room());
	m_Rooms[0].m_ID = "NOWHERE";
	m_Rooms[0].m_Name = "Is nowhere to be found.";
	//-------------------------------
	//	Create Inventory
	//-------------------------------
	m_Rooms.push_back(Room());
	m_Rooms[1].m_ID = "INVENTORY";
	m_Rooms[1].m_Name = "Your Bottomless pockets.";
	m_Rooms[1].m_Desc = "Your not sure how many rules of euclidean space and constants of c it breaks but its useful.";
	//-------------------------------
	//	Create Action Space
	//-------------------------------
	m_Objects.push_back(uObject());
	m_Objects[0].m_ID = "PLAYER";
	m_Objects[0].m_RoomID = "NOWHERE";
	m_Objects[0].m_Name = "No Object, just actionspace";
	m_Objects[0].m_Desc = "This is a blank action space.";
}
//==============================================================
//	Free up Memory
//==============================================================
Instance::~Instance()
{
}
//==============================================================
//==============================================================
void Instance::start()
{
	m_cRoom = 0;
	m_TimeStart = time(0);
	inputCmd("START");
}
//==============================================================
//	Fetch Output
//==============================================================
string Instance::fetchOutput()
{
	if(m_Output.empty())
		return "";

	string _str = m_Output.front();
	m_Output.pop();
	return _str;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//				FILE LOADING FUNCTIONS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//==============================================================
//	Load File
//==============================================================
void Instance::loadXML(const char* _filename)
{
	FPxml _xmlFile;
	_xmlFile.loadFile(_filename);

	//-------------------------------
	//	Load XML data
	//-------------------------------
	loadXitem(_xmlFile.m_base);

	//-------------------------------
	//	Cleans out Bad Room
	//-------------------------------
	vector<Room>::iterator _room = m_Rooms.begin();
	while(_room != m_Rooms.end())
	{
		if(_room->m_ID.empty())
		{
			m_Rooms.erase(_room);
		}
		++_room;
	}
	//-------------------------------
	//	Cleans out Bad Objects
	//-------------------------------
	vector<uObject>::iterator _obj = m_Objects.begin() + 1;
	while(_obj != m_Objects.end())
	{
		//--------------------------------------
		//	Takes Actions out of 
		//	NULL Objects and PLAYER Objects
		//	Then places into Player
		//--------------------------------------
		if((_obj->m_ID.empty()) || (stricmp(_obj->m_ID.c_str(),"PLAYER") == 0))
		{
			m_Objects[0].m_Actions.insert(m_Objects[0].m_Actions.end(),_obj->m_Actions.begin(),_obj->m_Actions.end());
			m_Objects.erase(_obj);
		}
		else 
		{
			//--------------------------------------
			//	Meld Duplicates
			//	Saves Duplicates Actions
			//	Discards Duplicates information
			//--------------------------------------
			/* NEEDS FIX
			for(vector<uObject>::iterator _cur = _obj; _cur != m_Objects.end(); ++_cur)
			{
				if(stricmp(_obj->m_ID.c_str(),_cur->m_ID.c_str()) == 0)
				{
					_obj->m_Actions.insert(_obj->m_Actions.end(),_cur->m_Actions.begin(),_cur->m_Actions.end());
					m_Objects.erase(_cur);
				}
			}*/

			if(_obj->m_RoomID.empty())
			{
				_obj->m_RoomID = m_Rooms[0].m_ID;
			}
		}
		++_obj;
	}

	//----------------------------------
	//	Places Player in Starting Room
	//----------------------------------
	m_Objects[0].m_RoomID = m_Rooms[m_cRoom].m_ID;
}
//==============================================================
//	Process XML Item
//==============================================================
void Instance::loadXitem(FPxitem& _xItem)
{
	switch(_xItem.m_Name[0])
	{
		//-------------------------------
		//	Instance :: Expected Nodes
		//-------------------------------
	case 'i':
	case 'I':
	{
		for(unsigned int c = 0; c < _xItem.m_Children.size(); ++c)
		{
			FPxitem& _subItem = _xItem.m_Children[c];

			switch(_subItem.m_Name[0])
			{
				FPX_CASE(m_PlayName,	'p','P')

			//-------------------------------
			//	Save Flag
			//-------------------------------
			case 'f':
			case 'F':
				m_Flags.push_back(_subItem.m_Data);
				break;
			//-------------------------------
			//	Load Sub Object
			//-------------------------------
			case 'n':
			case 'N':
				m_Node.push_back(aNode());
				loadXnode(m_Node.back(),_subItem);
				break;
			case 'r':
			case 'R':
				m_Rooms.push_back(Room());
				loadXitem(_subItem);
				break;
			case 'o':
			case 'O':
				m_Objects.push_back(uObject());
				loadXitem(_subItem);
				break;
			};
		}
		break;
	}
		//-------------------------------
		//	Room :: Expected Nodes
		//-------------------------------
	case 'r':
	case 'R':
	{
		for(unsigned int c = 0; c < _xItem.m_Children.size(); ++c)
		{
			FPxitem& _subItem = _xItem.m_Children[c];
			Room& _room = m_Rooms.back();

			switch(_subItem.m_Name[0])
			{
				FPX_CASE(_room.m_ID,		'i','I')
				FPX_CASE(_room.m_Name,		'n','N')
				FPX_CASE(_room.m_Desc,		'd','D')
				FPX_CASE(_room.m_BackImg,	'p','P')

			//-------------------------------
			//	Load Sub Object
			//-------------------------------
			case 'e':
			case 'E':
				_room.m_Exits.push_back(RoomExit());
				loadXitem(_subItem);
				break;
			};
		}
		break;
	}
		//-------------------------------
		//	Exit :: Expected Nodes
		//-------------------------------
	case 'e':
	case 'E':
	{
		RoomExit& _exit = m_Rooms.back().m_Exits.back();
		for(unsigned int c = 0; c < _xItem.m_Children.size(); ++c)
		{
			FPxitem& _subItem = _xItem.m_Children[c];			
			switch(_subItem.m_Name[0])
			{
				FPX_CASE(_exit.m_RoomID,	'i','I')
				FPX_CASE(_exit.m_Name,		'n','N')
			};
		}
		break;
	}

		//-------------------------------
		//	Object :: Expected Nodes
		//-------------------------------
	case 'o':
	case 'O':
	{
		uObject& _obj = m_Objects.back();
		for(unsigned int c = 0; c < _xItem.m_Children.size(); ++c)
		{
			FPxitem& _subItem = _xItem.m_Children[c];	
			switch(_subItem.m_Name[0])
			{
				FPX_CASE(_obj.m_ID,			'i','I')
				FPX_CASE(_obj.m_Name,		'n','N')
				FPX_CASE(_obj.m_Desc,		'd','D')
				FPX_CASE(_obj.m_InvImg,		'p','P')
				FPX_CASE(_obj.m_RoomID,		'r','R')
			//-------------------------------
			//	Load Sub Object
			//-------------------------------
			case 'a':
			case 'A':
				_obj.m_Actions.push_back(TrigAction());
				loadXitem(_subItem);
				break;
			};
		}
		break;
	}

		//-------------------------------
		//	Action :: Expected Nodes
		//-------------------------------
	case 'a':
	case 'A':
	{
		TrigAction& _act = m_Objects.back().m_Actions.back();
		for(unsigned int c = 0; c < _xItem.m_Children.size(); ++c)
		{
			FPxitem& _subItem = _xItem.m_Children[c];	
			switch(_subItem.m_Name[0])
			{
				FPX_CASE(_act.m_Verb,	'v','V')
				FPX_CASE(_act.m_Object,	'o','O')
				FPX_CASE(_act.m_Room,	'r','R')
				FPX_CASE(_act.m_Flag,	'f','F')
			//-------------------------------
			//	Load Sub Object
			//-------------------------------
			case 's':
			case 'S':
				_act.m_Scripts.push_back(aScript());
				loadXscript(_act.m_Scripts.back(),_subItem);
				break;
			};
		}
		break;
	}
	};
}
//==============================================================
//	Process Adventure NODE
//==============================================================
void Instance::loadXnode(aNode& _node, FPxitem& _parent)
{
	for(vector<FPxitem>::iterator _subItem = _parent.m_Children.begin(); _subItem != _parent.m_Children.end(); ++_subItem)
	{
		switch(_subItem->m_Name[0])
		{
			FPY_CASE(_node.m_Tag,	't','T')
			FPY_CASE(_node.m_Input,	'i','I')
			FPY_CASE(_node.m_Out,	'o','O')
			FPY_CASE(_node.m_Flag,	'f','F')
		//-------------------------------
		//	Add Sub Node
		//-------------------------------
		case 'n':
		case 'N':
			{
				_node.m_Children.push_back(aNode());
				loadXnode(_node.m_Children.back(),(*_subItem));
				break;
			}
		//-------------------------------
		//	Add Script
		//-------------------------------
		case 's':
		case 'S':
			{
				_node.m_Scripts.push_back(aScript());
				loadXscript(_node.m_Scripts.back(),(*_subItem));
				break;
			}
		};
	}
}
//==============================================================
//	Process Adventure NODE
//==============================================================
void Instance::loadXscript(aScript& _script, FPxitem& _parent)
{
	for(vector<FPxitem>::iterator _subItem = _parent.m_Children.begin(); _subItem != _parent.m_Children.end(); ++_subItem)
	{
		switch(_subItem->m_Name[0])
		{
			FPY_CASE(_script.m_Type,'t','T')
		//-------------------------------
		//	Add Argument
		//-------------------------------
		case 'a':
		case 'A':
			_script.m_Arg.push_back(_subItem->m_Data);
			break;
		};
	}
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//				PROCESS INPUT FUNCTIONS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//==============================================================
//	Input Command
//	Assumes Only A-Z,a-z, and ' '
//==============================================================
void Instance::inputCmd(char* _input)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//				PREP INPUT
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	prepString(_input);
	//---------------------------
	//	Check Length
	//---------------------------
	if(strlen(_input) < 4)
		return;
	//---------------------------
	//	Seperate Words
	//---------------------------
	char* _tok = strtok(_input," ");
	VecChar _words;
	while(_tok)
	{
		if(
/*			(strlen(_tok) > 2)			&&		// Not too Short
			(strlen(_tok) < 12)			&&		// Not too Long*/
			(stricmp(_tok,"WITH") != 0)	&&		// ------------- 
			(stricmp(_tok,"THE") != 0)	&&		// Ignore words players use that messes things up
			(stricmp(_tok,"THIS") != 0)	&&		// ------------- 
			(stricmp(_tok,"AROUND") != 0))
				_words.push_back(_tok);
        
		_tok = strtok(0," ");
	}
	//---------------------------
	//	Not Valid Input
	//---------------------------
	if(_words.empty())
	{
		m_Output.push("Not Valid Input");
		return;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			PROCESS OBJECT SPECIFIC INPUT
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//------------------------------
	//	Search for Matching Objects
	//	Skip FIRST object
	//------------------------------
	int _objectID = -1;
	int _subjectID = -1;

	if(_words.size() > 2)
		findObject(_words[1],_objectID,_words[2],_subjectID);
	else if(_words.size() > 1)
		findObject(_words[1],_objectID,0,_subjectID);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			PROCESS OBJECT VERB
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if(_objectID >= 0)
	{
		for(int k = m_Objects[_objectID].m_Actions.size() - 1; k >= 0;  --k)
		{
			TrigAction& _act = m_Objects[_objectID].m_Actions[k];
			if(checkAction(_act,_words[0],_objectID))
			{
				//----------------------------------
				//	Run Scripts
				//----------------------------------
				for(unsigned int i = 0; i < _act.m_Scripts.size(); ++i)
					runScript(_act.m_Scripts[i]);
				return;
			}
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			PROCESS GLOBAL VERB
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for(int k = m_Objects[0].m_Actions.size() - 1; k >= 0;  --k)
	{
		TrigAction& _act = m_Objects[0].m_Actions[k];
		if(checkAction(_act,_words[0],_objectID))
		{
			//----------------------------------
			//	Run Scripts
			//----------------------------------
			for(unsigned int i = 0; i < _act.m_Scripts.size(); ++i)
				runScript(_act.m_Scripts[i]);
			return;
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			PROCESS STANDARD VERB
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//-----------------------------------
	//	LOOK
	//	Prints out Desc
	//-----------------------------------
	if( (stricmp(_words[0],"LOOK") == 0)		|| 
		(stricmp(_words[0],"EXAMINE") == 0)		||
		(stricmp(_words[0],"INSPECT") == 0))
	{
		if(_objectID >= 0)
			m_Output.push(m_Objects[_objectID].m_Desc);
		else
			m_Output.push(m_Rooms[m_cRoom].m_Desc);
		return;
	}
	else 
	//-------------------------------------
	//	TIME
	//	Prints out Time Since play started
	//-------------------------------------
	if( (stricmp(_words[0],"TIME") == 0)		|| 
		(stricmp(_words[0],"CLOCK") == 0))
	{
		int _sec = (int)difftime(time(0), m_TimeStart);

		int _hours = _sec / (60*60);
		_sec -= _hours * (60*60);
		int _min = _sec / (60);
		_sec -= _min * (60);

		char _buffer[50] = {0};
		sprintf(_buffer,"Play Time \\ %i : %02i : %02i",_hours, _min, _sec);
		m_Output.push(_buffer);
		return;
	}
	else
	//-----------------------------------
	//	EXIT
	//	Uses Exit
	//-----------------------------------
	if( (stricmp(_words[0],"EXIT") == 0)		|| 
		(stricmp(_words[0],"MOVE") == 0)		||
		(stricmp(_words[0],"GOTO") == 0))
	{
		//------------------------
		//	TAKE EXIT
		//------------------------
		if(_words.size() > 1)
		{
			int c = findRoom(m_Rooms[m_cRoom].m_Exits[atoi(_words[1])].m_RoomID.c_str());
			if((c != m_cRoom) && (c >= 0))
			{
				m_cRoom = c;
				string _str = "[BACKDROP] ";
				_str += m_Rooms[m_cRoom].m_BackImg;
				m_Output.push(_str.c_str()); 
			}
		}
		//------------------------
		//	PRINT OUT EXITS
		//------------------------
		else if(m_Rooms[m_cRoom].m_Exits.size() > 0)
		{
			string _str = "EXITS \\";
			for(unsigned int i = 0; i < m_Rooms[m_cRoom].m_Exits.size(); ++i)
			{
				_str += i + 48;
				_str += ". ";
				_str += m_Rooms[m_cRoom].m_Exits[i].m_Name;
				_str += "\\ ";
			}
			m_Output.push(_str);
		}
		//------------------------
		//	NO VISIBLE EXIT
		//------------------------
		else
		{
			m_Output.push("There are no visible exits.");
		}
		return;
	}

	//-----------------------------
	//	Inform Player of Results
	//-----------------------------
	if(_words.size() > 1)
	{
		if(_objectID < 0)
		{
			string _buffer;
			_buffer = "I see no ";
			_buffer = _words[1];
			_buffer = " nearby.";
			m_Output.push(_buffer.c_str());
		}
		else
			m_Output.push("You cant do that at the moment");
	}
	else
	{
		//----------------------
		//	Non Standerd Verb
		//----------------------
		string _buffer;
		_buffer = _words[0];
		_buffer += " is not a action you can perform.";
		m_Output.push(_buffer.c_str());
	}
}
//==============================================================
//	Removes Leading/Trailing Spaces
//==============================================================
void Instance::prepString(char* _input)
{
	//---------------------------
	//	Remove Leading Space
	//---------------------------
	while((_input[0] == ' ') && (_input[0] != 0))
	{
		_input = _input + 1;
	}
	//---------------------------
	//	Remove Trailing Space
	//---------------------------
	{
		char* _end = _input + strlen(_input) - 1;
		while((_end[0] == ' ') && (_end > _input))
		{
			_end = 0;
			--_end;
		}
	}
}
//==============================================================
//	Check if action conditions are met
//==============================================================
bool Instance::checkAction(TrigAction& _act, const char* _verb, int _sID)
{
	//--------------------------------
	//	Check Verb
	//	MUST BE PRESENT
	//--------------------------------
	if((_verb == 0) || (stricmp(_act.m_Verb.c_str(), _verb) != 0))
		return false;
	//--------------------------------
	//	Check Subject
	//	ASSUME :: Subject ID is valid
	//--------------------------------
	if(!(_act.m_Object.empty())	&& (stricmp(_act.m_Object.c_str(), m_Objects[_sID].m_ID.c_str()) != 0))
		return false;

	//--------------------------------
	//	Check Room
	//--------------------------------
	if(!(_act.m_Room.empty())	&& (stricmp(_act.m_Room.c_str(), m_Rooms[m_cRoom].m_ID.c_str()) != 0))
		return false;

	//--------------------------------
	//	Check Flag
	//--------------------------------
	if(_act.m_Flag.empty())
		return true;
	else for(int i = m_Flags.size() - 1; i >= 0; --i)
	{
		if(stricmp(m_Flags[i].c_str(),_act.m_Flag.c_str()) == 0)
			return true;
	}

	return false;
}
//==============================================================
//	Find Object and Subject
//==============================================================
void Instance::findObject(const char* _object, int& _oID, const char* _subject, int& _sID)
{
	//------------------------------
	//	Search for Matching Objects
	//	Skip FIRST object
	//------------------------------
	if(m_Objects.size() > 1)
	for(int i = m_Objects.size() - 1; i > -1; --i)
	{
		//-------------------------------
		//	Find VALID Objects
		//-------------------------------
		uObject& _obj = m_Objects[i];
		if( (stricmp(_obj.m_RoomID.c_str(),m_Rooms[1].m_ID.c_str()		) == 0)		||		// Is Object in Inventory
			(stricmp(_obj.m_RoomID.c_str(),m_Rooms[m_cRoom].m_ID.c_str()) == 0))			// Is Object in Room
		{
			//-------------------------------
			//	Match Object
			//	ASSUME :: OBJECT [1] Word
			//-------------------------------
			if((_object) && (stricmp(_obj.m_ID.c_str(),_object) == 0))
			{
				_oID = i;
			}
			//-------------------------------
			//	Match Subject
			//	ASSUME :: SUBJECT [2] Word
			//-------------------------------
			else if((_subject) && (stricmp(_obj.m_ID.c_str(),_subject) == 0))
			{
				_sID = i;
			}			
		}
	}
	//-------------------------------
	//	CANNNOT FIND OBJECT
	//-------------------------------
	assert(("ERROR - BAD OBJECT") && (_object));
}
//==============================================================
//	Find Room
//==============================================================
int Instance::findRoom(const char* _room)
{
	for(unsigned int c = 0; c < m_Rooms.size(); ++c)
	{
		if(stricmp(m_Rooms[c].m_ID.c_str(),_room) == 0)
			return c;
	}
	//---------------------------------
	//	Cannot Find Room
	//---------------------------------
	assert(("ERROR - BAD ROOM") && (_room));
	return -1;
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//								SCRIPT
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//==============================================================
//	Run Script
//==============================================================
void Instance::runScript(aScript& _script)
{
	//---------------------------
	//	Get Script Type
	//---------------------------
	switch(_script.m_Type[0])
	{
	case 'p':
	case 'P':
		{
			//---------------------------------
			//	PRINT  
			//	Arg N - Text
			//	[META] Text		(Skipping Meta)
			//---------------------------------
			for(VecString::iterator _arg = _script.m_Arg.begin(); _arg != _script.m_Arg.end(); ++_arg)
			{
				const char* _str =_arg->c_str();
				m_Output.push(_str);
			}
			return;
		}
	case 'a':
	case 'A':
		{
			//---------------------------------
			//	ADD FLAGS
			//	Arg N - Flags to Add
			//---------------------------------
			for(VecString::iterator _arg = _script.m_Arg.begin(); _arg != _script.m_Arg.end(); ++_arg)
			{
				m_Flags.push_back(_arg->c_str());
			}
			return;
		}
	case 'r':
	case 'R':
		{
			//---------------------------------
			//	REMOVE FLAGS
			//	Arg N - Flags to Remove
			//---------------------------------
			for(VecString::iterator _arg = _script.m_Arg.begin(); _arg != _script.m_Arg.end(); ++_arg)
			{
				unsigned int f = 0;
				while(f < m_Flags.size())
				{
					if(stricmp(m_Flags[f].c_str(),_arg->c_str()) == 0)
					{
						// REMOVE FLAG
					}
					else
					{
						++f;
					}
				}
			}
			return;
		}
	case 'm':
	case 'M':
		{
			//---------------------------------
			//	MOVE OBJECTS
			//	Arg 0 - Destination Room
			//	Arg [1..N] - Objects
			//---------------------------------
			assert((_script.m_Arg.size() >= 2) && ("SCRIPT ERROR - DOES NOT PROVIDE ENOUGH ARGS"));
			{
				VecString::iterator _arg = _script.m_Arg.begin();
				++_arg;
				while(_arg != _script.m_Arg.end())
				{
					//---------------------------------
					//	Checks PLAYER
					//---------------------------------
					if(stricmp(_arg->c_str(),"PLAYER") == 0)
					{
						m_Objects[0].m_RoomID = _script.m_Arg[0];
					}
					else
					{
						int _objID = -1;
						findObject(_arg->c_str(),_objID,0,_objID);
						//---------------------------------
						//	OBJECT found
						//---------------------------------
						if(_objID >= 0)
						{
							m_Objects[_objID].m_RoomID = _script.m_Arg[0];
						}
					}
					++_arg;
				}
				//---------------------------------
				//	Updates Quick Room Index
				//---------------------------------
				int _newRoom = findRoom(m_Objects[0].m_RoomID.c_str());
				if((m_cRoom != _newRoom) && (_newRoom >= 0))
				{
					m_cRoom = _newRoom;
					string _str = "[BACKDROP] ";
					_str += m_Rooms[m_cRoom].m_BackImg;
					m_Output.push(_str.c_str()); 
				}
			}
			return;
		}
	case 'd':
	case 'D':
		{
			//---------------------------------
			//	CHANGE DESC
			//	Arg 0 - Object ID
			//	Arg 1 - New Desc
			//---------------------------------
			assert((_script.m_Arg.size() >= 2) && ("SCRIPT ERROR - DOES NOT PROVIDE ENOUGH ARGS"));

			{
				int _objID = -1;
				findObject(_script.m_Arg[0].c_str(),_objID,0,_objID);

				assert((_objID >= 0) && ("ERROR - BAD OBJECT"));

				if(_objID >= 0)
				{
					m_Objects[_objID].m_Desc = _script.m_Arg[1];
				}
			}
			return;
		}
	default:
			//---------------------------------
			//	UNHANDELED SCRIPT
			//---------------------------------
		{
			m_Output.push("Unhandled Script");
			errorScript(_script);
			return;
		};
	};
}
//==============================================================
//	PRINT OUT SCRIPT :: Used for Debugging
//==============================================================
void Instance::errorScript(aScript& _script)
{
	string _temp = "TYPE <";
	_temp += _script.m_Type;
	_temp += ">";
	
	for(VecString::iterator _arg = _script.m_Arg.begin(); _arg != _script.m_Arg.end(); ++_arg)
	{
		_temp += "\\ - ";
		_temp += *(_arg);
	}

	m_Output.push(_temp);
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//							ACCESSOR FUNCTIONS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//==============================================================
//	Retrieve Room Pointer
//==============================================================
Room* Instance::getRoom(const char* _id)
{
	for(unsigned int c = 0; c < m_Rooms.size(); ++c)
	{
		if(stricmp(m_Rooms[c].m_ID.c_str(),_id))
			return &(m_Rooms[c]);
	}
	return 0;
}
//==============================================================
//	Retrieve Object Pointer
//==============================================================
uObject* Instance::getObject(const char* _id)
{
	for(unsigned int c = 0; c < m_Objects.size(); ++c)
	{
		if(stricmp(m_Objects[c].m_ID.c_str(),_id))
			return &(m_Objects[c]);
	}
	return 0;
}
//==============================================================
//	Check Flag
//==============================================================
bool Instance::getFlag(const char* _id)
{
	for(unsigned int c = 0; c < m_Flags.size(); ++c)
	{
		if(stricmp(m_Flags[c].c_str(),_id))
			return true;
	}
	return false;
}