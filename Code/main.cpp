#include "FPHead.h"

//========================================================================
//	WINDOWS MAIN FUNCTION
//========================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{	
	//-----------------------------------
	//	Init 
	//-----------------------------------
	g_State = GS_INTRO;
	g_Background = 0;
	unsigned long _oldTime = KMiscTools::getMilliseconds();
	unsigned long _newTime = 0;

	g_inSize = 0;
	memset(g_inStr,0,sizeof(g_inStr));
	//-----------------------------------
	//	Create Window
	//-----------------------------------
	g_gameWin = KPTK::createKWindow(K_OPENGL) ;
	g_gameWin->createGameWindow(640,400,32,true,"FP Adventure");

	KTrueText _font("5by5.ttf");
	g_gameTxt = &_font;

	g_gfxVector.push_back(KPTK::createKGraphic("black.gif"));
	g_gfxNames.push_back("black.gif");

	g_gameWin->setPTKCallBack(&EventHandler);

	g_Inst.loadXML("lostisland.xml");
	g_Inst.start();
	//-----------------------------------
	//	Main Loop
	//-----------------------------------
	do{
		_newTime = KMiscTools::getMilliseconds();
		g_Delta = (_oldTime - _newTime) / 100000;
		_oldTime = _newTime;

		switch(g_State)
		{
		case GS_INTRO:
			IntroStep();
			break;
		case GS_READY:
		case GS_PLAY:
			// Game Loop
			break;
		};
	} while(g_gameWin->isQuit() == false);	
    return 0;
}

//========================================================================
//	Handels Events while in Menu
//========================================================================
bool EventHandler(KEvent* _eventPtr)
{
	if(_eventPtr->type == K_EVENT_KEYDOWN)
	{
		//---------------------------------------------
		//		Go Through Output
		//---------------------------------------------
		if(!(g_Output.empty()))
		{
			if(_eventPtr->ptkKey == K_VK_RETURN)
			{
				g_Output = g_Inst.fetchOutput();
				//-----------------------------
				//	Scan for Meta Output
				//	Only if Output is returned
				//-----------------------------
				if(!(g_Output.empty()))
				{
					ScanMeta();
				}
			}
		}
		//---------------------------------------------
		//		Process Input
		//---------------------------------------------
		else
		{
			if((_eventPtr->ptkKey >= 25) && (_eventPtr->ptkKey <= 60))
			{
				if(_eventPtr->ptkKey <= 50)
					g_inStr[g_inSize++] = (_eventPtr->ptkKey + 40);	// Add LETTER, POSTcrement Size
				else
					g_inStr[g_inSize++] = (_eventPtr->ptkKey - 3);	// Add NUMBER, POSTcrement Size
			}
			else switch(_eventPtr->ptkKey)
			{
			case K_VK_BACK:
				// Remove one Letter ONLY if length is not 0
				if(g_inSize > 0)
				{
					g_inStr[--g_inSize] = 0;	// back one step, Null last character
				}
				break;
			case K_VK_RETURN:
				// Process Sentance
				g_Inst.inputCmd(g_inStr);
				// Erase Sentance
				g_inSize = 0;
				memset(g_inStr,0,sizeof(g_inStr));
				break;

			// Add Space
			case K_VK_SPACE:
				if((g_inSize > 0) && (g_inStr[g_inSize - 1] != ' '))
					g_inStr[g_inSize++] = 32;
				break;
			};
		}
	}
	return true;
}

//========================================================================
//	Game Intro Step
//========================================================================
void IntroStep()
{
	while((g_State == GS_INTRO) && (g_gameWin->isQuit() == false))
	{
		//-----------------------------
		//	Render Scence
		//-----------------------------
		IntroRender();

		//-----------------------------
		//	Fetch Output if Ready
		//-----------------------------
		if(g_Output.empty())
		{
			g_Output = g_Inst.fetchOutput();
			//-----------------------------
			//	Scan for Meta Output
			//	Only if Output is returned
			//-----------------------------
			if(!(g_Output.empty()))
			{
				ScanMeta();
			}
		}

		//-----------------------------
		//	Process Events
		//-----------------------------
		g_gameWin->processEvents();
	};
}

//========================================================================
//	Game Intro Render
//========================================================================
void IntroRender()
{
	//-------------------------
	//	Draw
	//-------------------------
	g_gameWin->setDefaultWorldView();
	g_gameWin->setClearColor(0,0,1,0);
	if(g_Background >= 0)
		g_gfxVector[g_Background]->blitRectFx(0,0,320,200,160,100 ,0,2);
	else
		g_gfxVector[0]->drawRect(0,0,640,400,0,0,0,1);

	//-------------------------
	//	Print Output
	//-------------------------
	if(!(g_Output.empty()))
		PrintBox(g_Output.c_str());

	if(g_inSize > 0)
		PrintInput();
		

	//-------------------------
	// Buffer Flip
	//-------------------------
	g_gameWin->flipBackBuffer();
}
//========================================================================
//	Render InputBox
//========================================================================
void PrintInput()
{
	//---------------------------
	// Draw Font
	//---------------------------
	g_gameTxt->setHeightPix(20);
	g_gameTxt->setColor(0,0,0,1);

	//---------------------------
	// Background Box
	//---------------------------
	g_gfxVector[0]->drawRect(12,370,632,398,0.0f,0.0f,0.0f,1.0f);
	if(g_Output.empty())
		g_gfxVector[0]->drawRect(14,372,630,396,1.0f,1.0f,1.0f,1.0f);
	else
		g_gfxVector[0]->drawRect(14,372,630,396,0.8f,0.8f,0.8f,1.0f);

	//---------------------------
	// Draw Text
	//---------------------------
	g_gameTxt->drawStringFromLeft(">",16,372);
	g_gameTxt->drawStringFromLeft(g_inStr,30,372);
}
//========================================================================
//	Render Output
//========================================================================
void PrintBox(const char* _input)
{
	//-------------------------------------
	//	Print Textual Output
	//-------------------------------------
	{
		UINT _size = strlen(_input);
		UINT _width = 0;
		UINT _height = 0;

		_width = 28;
		_height = 10;// _size / 28;

		_width  = UINT(_width * FP_CHAR_WDTH);
		_height *= 17;

		PrintBox(320 - _width,20,320 + _width, 30 + _height, _input);
	}
}

//========================================================================
//	Print a Box of Text
//========================================================================
void PrintBox(UINT _topX, UINT _topY, UINT _botX, UINT _botY, const char* _input)
{
	//---------------------------
	//	Setup Font
	//---------------------------
	g_gfxVector[0]->drawRect((float)_topX-4,(float)_topY-2,(float)_botX + 4,(float)_botY + 4,0.0f,0.0f,0.0f,1.0f);
	g_gfxVector[0]->drawRect((float)_topX-2,(float)_topY  ,(float)_botX + 2,(float)_botY + 2,1.0f,1.0f,1.0f,1.0f);
	g_gameTxt->setHeightPix(20);
	g_gameTxt->setColor(0,0,0,1);

	UINT _charWidth = UINT((_botX - _topX) / FP_CHAR_WDTH);

	//---------------------------
	//	Setup Tokens
	//	ScratchPad Needed
	//---------------------------
	char* _str = new char[strlen(_input) + 1];
	strcpy(_str,_input);
	char* _tok = strtok(_str,"\\");

	//---------------------------
	//	Loop Through Lines
	//---------------------------
	while(_tok)
	{
		//---------------------------
		// Remove Leading Spaces
		//---------------------------
		if(_tok[0] == ' ')			++_tok;

		//---------------------------
		// Break Long Tokens
		//---------------------------
		while(strlen(_tok) > _charWidth)
		{
			int c = _charWidth -1;
			while(c > 3)
			{
				if(_tok[c] == ' ')
					break;
				--c;
			}			 
			_tok[c] = 0;
			//g_gameTxt->drawStringFromLeft(_tok,_topX,_topY,0);
			g_gameTxt->drawStringCentered(_tok,_topX,_botX,_topY,0);
			_tok += c + 1;
			_topY += 18;
		}		

		//---------------------------
		// Print Token, Iterate
		//---------------------------
		//g_gameTxt->drawStringFromLeft(_tok,_topX,_topY,0);
		g_gameTxt->drawStringCentered(_tok,_topX,_botX,_topY,0);
		_tok = strtok(NULL, "\\");
		_topY += 18;
	}
	delete[] _str;
}
//========================================================================
//	Print a Box of Text
//========================================================================
void ScanMeta()
{
	//-------------------------------------
	//	Check for Meta
	//-------------------------------------
	char* _temp = new char[g_Output.size() + 1];
	strcpy(_temp,g_Output.c_str());
	char* _meta = strchr(_temp,'[');
	if(_meta)
	{
		//----------------------------
		//	Get META string
		//----------------------------
		++_meta;
		char* _end = strchr(_meta,']');
		_end[0] = 0;

		switch(_meta[0])
		{
		case 'b':
		case 'B':
			//----------------------------
			//	Backdrop Image
			//----------------------------
			{
				//---------------------------------
				//	Skip Ahead
				//---------------------------------
				++_end;
				while(_end[0] == ' ')
				{
					++_end;
				}
				//------------------------------------
				//	Attempt to Reuse Image
				//------------------------------------
				bool _found = false;
				for(unsigned int c = 0; c < g_gfxNames.size(); ++c)
				{
					if(stricmp(g_gfxNames[c].c_str(),_end) == 0)
					{
						g_Background = c;
						_found = true;
					}
				}
				//------------------------------------
				//	Add Image
				//------------------------------------
				if(!_found)
				{
					KGraphic* _gfx = KPTK::createKGraphic(_end);
					if(_gfx)
					{
						g_gfxVector.push_back(_gfx);
						g_gfxNames.push_back(_end+1);
						g_Background = g_gfxVector.size() - 1;
					}
					else
					{
						//------------------------------------
						//	NO IMAGE FOUND
						//------------------------------------
						g_Background = 0;
						g_Output = "IMAGE <";
						g_Output += _end;
						g_Output += "> NOT FOUND";

						delete[] _temp;
						return;
					}
				}
			}
			break;
		case 'd':
		case 'D':
			{
				g_Background = -1;
			}
			break;
		default:
			PrintBox(220,20,420, 40, _meta);
			PrintBox(220,50,420, 80, _end+1);
			break;
		};
		//------------------------------
		//	Remove META from Output
		//------------------------------
		g_Output.clear();
	}
	delete[] _temp;
}