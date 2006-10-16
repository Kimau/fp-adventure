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
#define FP_CHAR_WDTH	10.5

//--------------------------------
//	Globals
//--------------------------------
KWindow*	g_gameWin;			// PTK Window
KTrueText*	g_gameTxt;			// PTK Text Instance
vector<KGraphic*> g_gfxVector;	// List of PTK graphic containers

char		g_inStr[50];		// Input Buffer
int			g_inSize;			// Input Size Holder
GameState	g_State;			// Game State
unsigned long g_Delta;			// Delta Time
string		g_Output;			// If game is in playback mode

Instance	g_Inst;				// Instance of Game

//--------------------------------
//	Predeclares
//--------------------------------
bool EventHandler(KEvent* _eventPtr);
void IntroStep();
void IntroRender();
void PrintInput();
void PrintBox(const char* _input);
void PrintBox(UINT _topX, UINT _topY, UINT _botX, UINT _botY, const char* _input);

#endif