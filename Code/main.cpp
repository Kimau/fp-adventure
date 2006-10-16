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

	g_gfxVector.push_back(KPTK::createKGraphic("test.gif"));

	g_gameWin->setPTKCallBack(&EventHandler);

	g_Inst.loadXML("Space.xml");
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
				g_Output = g_Inst.fetchOutput();
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
		IntroRender();

		if(g_Output.empty())
			g_Output = g_Inst.fetchOutput();

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
	g_gfxVector[0]->blitRectFx(0,0,320,200,160,100 ,0,2);

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
//	Render Output Textbox
//========================================================================
void PrintBox(const char* _input)
{
	UINT _size = strlen(_input);
	UINT _width = 0;
	UINT _height = 0;

	if(_size < 50)
	{
		_height = 1;
		_width = _size;
	}
	else if(_size < 50)
	{
		_height = 3;
		_width = _size / _height;		
	}
	else if(_size <200)
	{
		_width = _size % 24;
		_height = _size / _width;
	}
	else
	{
		_width = 28;
		_height = _size / 28;
	}
	_width  = UINT(_width * FP_CHAR_WDTH);
	_height *= 17;

	PrintBox(320 - _width,20,320 + _width, 30 + _height, _input);
}
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