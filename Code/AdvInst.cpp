#include "AdvInst.h"
#include <stdio.h>

#define FPX_CASE(var,low,high)		\
			case low:	case high:	\
			var = _subItem.m_Data;	\
			break;
#define FPY_CASE(var,low,high)		\
			case low:	case high:	\
			var = _subItem->m_Data;	\
			break;

#define FP_LOOP_OBJ(var)																\
			for(unsigned int var = 0; var < m_Objects.size(); ++var)					\
			if( stricmp(m_Objects[var].m_RoomID.c_str(),m_Rooms[1].m_ID.c_str()) ||		\
				stricmp(m_Objects[var].m_RoomID.c_str(),m_Rooms[m_cRoom].m_ID.c_str()))

#define FP_RUN_SCRIPTS(var)									\
			for(vector<aScript>::iterator var;				\
				var != _act->m_Scripts.begin(); ++var)		\
					runScript(*(var));

using namespace FPAdv;
//==============================================================
//	Create Blank Instance
//==============================================================
Instance::Instance()
{
	m_cRoom = 2;
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
	m_Objects[0].m_ID = "NOOBJ";
	m_Objects[0].m_RoomID = "INVENTORY";
	m_Objects[0].m_Name = "No Object, just actionspace";
	m_Objects[0].m_Desc = "This is a blank action space.";
}
//==============================================================
//	Free up Memory
//==============================================================
Instance::~Instance()
{
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
	vector<uObject>::iterator _obj = m_Objects.begin();
	while(_obj != m_Objects.end())
	{
		if(_obj->m_ID.empty())
		{
			m_Objects[0].m_Actions.insert(m_Objects[0].m_Actions.end(),_obj->m_Actions.begin(),_obj->m_Actions.end());
			m_Objects.erase(_obj);
		}
		else if(_obj->m_RoomID.empty())
		{
			_obj->m_RoomID = m_Rooms[0].m_ID;
		}
		++_obj;
	}
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
//==============================================================
void Instance::inputCmd(const char* _input)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//				PREP INPUT
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//---------------------------
	//	Setup Tokens
	//	ScratchPad Needed
	//---------------------------
	unsigned int _length = (unsigned int)strlen(_input);
	char* _str = new char[_length];
	strcpy(_str,_input);

	//---------------------------
	//	Remove Trailing Space
	//---------------------------
	if(_str[_length-1] == ' ')
		_str[_length-1] = 0;

	//---------------------------
	//	Seperate Words
	//---------------------------
	char* _tok = strtok(_str," ");
	VecChar _words;
	while(_tok)
	{
		if((strlen(_tok) > 2) && (strlen(_tok) < 12))
			_words.push_back(_tok);
        
		_tok = strtok(0," ");
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//				PROCESS INPUT
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int _objIndex = -1;
	int _subIndex = -1;
	bool _flag = false;
	vector<TrigAction>::iterator _act;
	//---------------------------
	//	Find VERB
	//---------------------------
	FP_LOOP_OBJ(i)
	{
		uObject& _obj = m_Objects[i];
		for(vector<TrigAction>::iterator _iter = _obj.m_Actions.begin(); _iter != _obj.m_Actions.end(); ++_iter)
		{
			//---------------------------
			//	Check Verb
			//	Check Room
			//---------------------------
			if( (stricmp(_words.front(),_iter->m_Verb.c_str()) == 0) && 
				(stricmp(m_Rooms[m_cRoom].m_ID.c_str(),_iter->m_Room.c_str()) == 0))
			{
				_objIndex = i;
				_act = _iter;
			}
		}
	}
	//------------------------------
	//	No Action Valid
	//------------------------------
	if(_objIndex < 0)
		return;

	//---------------------------
	//	Check Subject
	//---------------------------
	if(_act->m_Object.empty())
	{
		_subIndex = 0;
	}
	else
	{
		for(VecChar::iterator _word = (_words.begin() + 1); _word != _words.end(); ++_word)
		{
			//----------------------------
			//	Subject Word Used
			//----------------------------
			if(stricmp(_act->m_Object.c_str(),*_word) == 0)
			{
				FP_LOOP_OBJ(k)
				{
					if(stricmp(m_Objects[k].m_ID.c_str(),*_word) == 0)
					{
						_subIndex = k;
					}
				}
			}
		}
	}
	//------------------------------
	//	Required Subject Missing
	//------------------------------
	if(_subIndex < 0)
		return;

	//---------------------------
	//	Check Flag
	//---------------------------
	if(_act->m_Flag.empty())
	{
		_flag = true;
	}
	else
	{
		for(unsigned int i = 0; i < m_Flags.size(); ++i)
		{
			if(stricmp(_act->m_Flag.c_str(),m_Flags[i].c_str()) == 0)
			{
				_flag = true;
			}
		}
	}
	//------------------------------
	//	Required Flag Missing
	//------------------------------
	if(_flag == 0)
		return;

	//--------------------------------
	//	Run Scripts
	//--------------------------------
	FP_RUN_SCRIPTS(_iter)
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
	case 'a':
		break;
	};
	//---------------------------
	//	Process Args
	//---------------------------

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