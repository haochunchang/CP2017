#ifndef MJSTAGE
#define MJSTAGE
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include "MJcollection.h"
#include "shuffler.h"
#include "MJplayer.h"
#include "MJgame.h"
using namespace std;

// In TA's dependency graph
// MJstage includes
// 1. Dealing phase
// 2. Stretagy phase
// 3. Check cangong phase
// 4. comparison phase
// 5. shooting phase

// All the rules refer to the following webpage
// http://www.dragona.com.tw/mahjong-rule/

// pickSeat
// pickBookmaker
// getTiles
// applique
// arrange
// drawTile
// playTile
// eatPongGone
// ting
// hu
// flowBureau
// countTai

class MJstage {
public:
	MJstage(void);				// 還不確定要怎寫
	void pickSeat(void);		// done
	void pickBookmaker(void);	// done
	void getTiles(void);		// done
	void initiate(void);		// done
	void mainGame(void);
	void countTai(void);

private:
	vector<MJplayer> _players;
	// It's a vector by TA originally
	int _bookmaker;
	// player index (0 - 3), not position (1 - 4)
	MJcollection mjcol;
	map<int, int> playerToPos;
	map<int, int> posToPlayer;
};

MJstage::MJstage() {
	cout << "Call MJstage constructor." << endl;
	// 在想要不要等有牌的資訊再來 init MJplayer

	Shuffler s;
	MJtile mjtiles[144];
	s.init();
	s.fill(mjtiles);
	mjcol = MJcollection(mjtiles);

	for (int i = 0; i < 4; i++) {
		_players.push_back(MJplayer());
	}
}


void swapInt(int& a, int& b) {
	int c = a;
	a = b;
	b = c;
	return;
}


void MJstage::pickSeat(void) {
	// east = 1 ,south = 2 , west = 3 , north = 4
	srand(time(NULL));
	int pos[4] = {1, 2, 3, 4};
	for (int i = 0; i < 4; i++) {
		int change_with = rand() % 4;
		swapInt(pos[i], pos[change_with]);
	}
	// cout << "pos[0] = " << pos[0] << endl;
	// cout << "pos[1] = " << pos[1] << endl;
	// cout << "pos[2] = " << pos[2] << endl;
	// cout << "pos[3] = " << pos[3] << endl;

	// now can set position for the 4 _players
	for (int i = 0; i < 4; i++) {
		_players[i].Set_Pos(pos[i]);
		playerToPos[i] = pos[i];
		posToPlayer[pos[i]] = i;
		cout << "Set _players[" << i << "]'s position to " << pos[i] << endl;
		cout << "Set playerToPos[" << i << "] to " << pos[i] << endl;
		cout << "Set posToPlayer[" << pos[i] << "] to " << i << endl;
	}
}


void MJstage::pickBookmaker(void) {
	srand(time(NULL));
	_bookmaker = rand() % 4;
	cout << "_players[" << _bookmaker << "] is the bookmaker" << endl;
}


void MJstage::getTiles(void) {
	cout << "Do getTiles:" << endl;

	for (int i = 0; i < 4; i++) {
		// 從 mjcol 從前面取走 16 張給 _players[i]
		MJtile mjtiles_for_player[16];
		for (int i = 0; i < 16; i++) {
			mjtiles_for_player[i] = mjcol.drawfronttile();
		}
		_players[i].Set_Hand(mjtiles_for_player, 16);
	}
	cout << "_players[" << _bookmaker << "] draw 17th tile" << endl;
	_players[_bookmaker].draw(mjcol);

	for (int i = 0; i < 4; i++) {
		cout << "_players[" << i << "]'s hand is: " << endl;
		_players[i].Print_Hand();
	}

	return;
}


void MJstage::initiate(void) {
	cout << "Do initiate:" << endl;
	for (int i = 0; i < 4; i++) {
		_players[i].initiate(mjcol);
		cout << "_players[" << i << "]'s hand is: " << endl;
		_players[i].Print_Hand();
	}
}


void MJstage::mainGame(void) {
	// greedy algorithm

	int currentPos;
	int currentPlayer;
	int actiontype[4] = {0, 0, 0, 0};
	int actionparameter[4] = {0, 0, 0, 0};
	// 出牌順序：逆時針 (pos 4, 3, 2, 1, 4, etc.)
	// 可以用 0 開始往下減 % 4 + 4 來實現

	// currentPos 設定為莊家
	currentPlayer = _bookmaker;
	currentPos = playerToPos[_bookmaker];
	// 首先莊家丟一張牌。
	// 另外寫個函數，只有在開場這裡會用到
	// --- 待寫 ---
	MJtile t; // 先當作莊家丟出的牌
	// 其他三家要傳進那張丟出來的牌看能不能有 strategy
	for (int i = 0; i < 4; i++) {
		if (i != currentPlayer) {
			_players[i].strategy(currentPlayer, t, actiontype[i], actionparameter[i]);
		}
	}

	// --- 我覺得在莊家丟出 tile 後，到吃碰槓都沒人會再丟出 tile，要另外用遞迴函數 ---
	// 條件：如果都沒 _players 要吃碰槓，就 return
	// 有的話就吃碰槓丟出新的 tile，call 自己


	// 換下一個人
	(currentPos == 4) ? (currentPos = 1) : (currentPos += 1);
	currentPlayer = posToPlayer[currentPos];
	return;
}

#endif
