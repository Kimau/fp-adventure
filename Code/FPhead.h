#ifndef FP_ADVENTURE_MAIN_HEAD
#define FP_ADVENTURE_MAIN_HEAD
//--------------------------------
//	Includes
//--------------------------------
#include "AdvInst.h"
#include <vector>
#include "ptk.h"
#include "KPTK.h"
#include "KSound.h"

using namespace std;
using namespace FPAdv;

//--------------------------------
//	Defines
//--------------------------------
enum GameState{GS_NULL = -1, GS_INTRO, GS_READY, GS_PLAY};

//--------------------------------
//	Globals
//--------------------------------
KWindow*	g_gameWin;
KTrueText*	g_gameTxt;
vector<KGraphic*> g_gfxVector;
unsigned long g_Delta;
char		g_inStr[50];
int			g_inSize;
GameState	g_State;

Instance	g_Inst;

//--------------------------------
//	Predeclares
//--------------------------------
bool EventHandler(KEvent* _eventPtr);
void IntroStep();
void IntroRender();
void PrintInput();
void PrintBox(UINT _topX, UINT _topY, UINT _botX, UINT _botY, const char* _input);

#endif