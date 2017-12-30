#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <cassert>
#include "MJcollection.h"
#include "Shuffler.h"
#include "MJplayer.h"
#include "MJAIplayer.h"
#include "MJstage.h"
using namespace std;


void swapInt(int& a, int& b) {
	int c = a;
	a = b;
	b = c;
	return;
}


void printStrategy(int* actiontype, int* actionparameter) {
	// actiontype: hu=-1 nothing=0 eat=1 pong=2 minggone=3 angone=4 bugone=5
	// actionparameter: huown=1 huother=2
	for (int i = 0; i < 4; i++) {
		cout << "_player[" << i << "]: ";
		switch (actiontype[i]) {
		case -1:
			if (actionparameter[i] == 1) cout << "huown" << endl;
			if (actionparameter[i] == 2) cout << "huother" << endl;
			break;
		case 0:
			cout << "do nothing" << endl;
			break;
		case 1:
			cout << "eat ";
			switch (actionparameter[i]) {
			case 1: cout << "(001)";
			case 2: cout << "(010)";
			case 3: cout << "(001) (010)";
			case 4: cout << "(100)";
			case 5: cout << "(001) (100)";
			case 6: cout << "(010) (100)";
			case 7: cout << "(001) (010) (100)";
			}
		case 2:
			cout << "pong" << endl;
			break;
		case 3:
			cout << "minggone" << endl;
			break;
		case 4:
			cout << "angone" << endl;
			break;
		case 5:
			cout << "bugone" << endl;
			break;
		}
	}
}


void printAction(int player_to_act, int current_action_type, int current_action_param) {
	if (player_to_act == -1) {
		cout << "No player would like to act." << endl;
		return;
	}
	cout << "_players[" << player_to_act << "] is going to ";
	switch (current_action_type) {
	case -1:
		if (current_action_param == 1) cout << "huown" << endl;
		if (current_action_param == 2) cout << "huother" << endl;
		break;
	case 0:
		cout << "do nothing" << endl;
		break;
	case 1:
		cout << "eat ";
		switch (current_action_param) {
		case 1: cout << "(001)";
		case 2: cout << "(010)";
		case 3: cout << "(001) (010)";
		case 4: cout << "(100)";
		case 5: cout << "(001) (100)";
		case 6: cout << "(010) (100)";
		case 7: cout << "(001) (010) (100)";
		}
	case 2:
		cout << "pong" << endl;
		break;
	case 3:
		cout << "minggone" << endl;
		break;
	case 4:
		cout << "angone" << endl;
		break;
	case 5:
		cout << "bugone" << endl;
		break;
	}
}


//============ MJstage Class Methods =================
MJstage::MJstage() {
	cout << "Call MJstage constructor." << endl;

	Shuffler s;
	MJtile mjtiles[144];
	s.init();
	s.fill(mjtiles);
	mjcol = MJcollection(mjtiles);

	for (int i = 0; i < 4; i++) {
        MJplayer *ptr = new (MJGreedyAIplayer);
		_players.push_back(ptr);
	}
}


MJstage::MJstage(int n_human, int AIkind) {
	cout << "Call MJstage constructor." << endl;

	Shuffler s;
	MJtile mjtiles[144];
	s.init();
	s.fill(mjtiles);
	mjcol = MJcollection(mjtiles);

	for (int i = 0; i < n_human; i++) {
        MJplayer *ptr = new (MJplayer);
		_players.push_back(ptr);
	}
	for (int i = 0; i < 4 - n_human; i++) {
		if (AIkind == 1) {
            MJplayer *ptr = new (MJGreedyAIplayer);
			_players.push_back(ptr);
		} else {
            MJplayer *ptr = new (MJCustomAIplayer);
			_players.push_back(ptr);
		}
	}
}


MJstage::~MJstage() {
    for (int i = 0; i < 4; i++) {
        delete _players[i];    
    }    
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
		_players[i]->Set_Pos(pos[i]);
		playerToPos[i] = pos[i];
		posToPlayer[pos[i]] = i;
		cout << "Set _players[" << i << "]'s position to " << pos[i] << endl;
		// cout << "Set playerToPos[" << i << "] to " << pos[i] << endl;
		// cout << "Set posToPlayer[" << pos[i] << "] to " << i << endl;
	}
}


void MJstage::pickBookmaker(void) {
	srand(time(NULL));
	_bookmaker = rand() % 4;
	cout << "_players[" << _bookmaker << "] is the bookmaker" << endl;
}


void MJstage::getTiles(void) {
	cout << "Do getTiles:" << endl;

	MJtile mjtiles_for_player[4][16];
	for (int k = 0; k < 4; k++) {
		for (int i = 0; i < 4; i++) {
			// 從 mjcol 從前面取走 4 張給 _players[i]
			for (int j = k * 4; j < (k + 1) * 4; j++) {
				mjtiles_for_player[i][j] = mjcol.drawfronttile();
			}
		}
	}
	for (int i = 0; i < 4; i++) {
		_players[i]->Set_Hand(mjtiles_for_player[i], 16);
	}
	cout << "_players[" << _bookmaker << "] draw 17th tile" << endl;
	_players[_bookmaker]->draw(mjcol);

	for (int i = 0; i < 4; i++) {
		cout << "_players[" << i << "]'s hand is: " << endl;
		_players[i]->Print_Hand();
	}

	return;
}


void MJstage::initiate(void) {
	cout << "Do initiate:" << endl;
	for (int i = 0; i < 4; i++) {
		_players[i]->initiate(mjcol);
		cout << "_players[" << i << "]'s hand is: " << endl;
		_players[i]->Print_Hand();
	}
}


void MJstage::mainGame(int& rounds) {
	// greedy algorithm
	cout << "Enter mainGame:" << endl;
	int currentPos;
	int currentPlayer;
	int actiontype[4] = {0, 0, 0, 0};
	int actionparameter[4] = {0, 0, 0, 0};
	// 出牌順序：逆時針 (pos 4, 3, 2, 1, 4, etc.)

	currentPlayer = _bookmaker;
	currentPos = playerToPos[_bookmaker];

	/*
	// 這裡要判斷開局時莊家有沒有胡，跟之後摸一張牌、別人出牌時的寫法應該會不一樣（有一張傳入的牌）
	// 在initiate就有莊家就已經摸一張牌了，所以只要判斷有沒有胡？
	// 補充：因為開局只可能是胡，之後摸一張牌還可能是補槓暗槓
	// 我覺得是可以直接不寫因為機率太低了

	MJtile dummy;
	_players[_bookmaker].strategy(currentPlayer, dummy, actiontype[_bookmaker], actionparameter[_bookmaker]);
	if (actiontype[_bookmaker] == -1) {
		// 莊家自摸
		// TODO
	}
	assert(actiontype[_bookmaker] == 6);
	*/

	// 判定莊家沒胡之後莊家丟一張牌
	int index = _players[_bookmaker]->decidePlay();
	MJtile t = _players[_bookmaker]->play(index);
	cout << "Initially, bookmaker plays:\n";
	cout << t;

	// 正式開局！

	while (mjcol.size() != 0) {
		cout << "Enter while loop. Rounds " << ++rounds << "." << endl;

		// 其他三家要傳進那張丟出來的牌看能不能有 strategy
		cout << "Other players decide strategy." << endl;
		for (int i = 0; i < 4; i++) {
			if (i != currentPlayer) {
				_players[i]->strategy(currentPlayer, t, actiontype[i], actionparameter[i]);
			}
		}
		printStrategy(actiontype, actionparameter);
		cin.get();

		// Checking Actions: gone > pong > eat
		int current_action_type = 0;
		int current_action_param = 0;
		// decide which player's action is executed
		int player_to_act = -1;
		for (int i = 0; i < 4; i++) {
			if (actiontype[i] == -1) {
				_players[i]->act(actiontype[i], actionparameter[i], t, mjcol);
				cout << "_players[" << i << "] huother!" << endl;
				cin.get();
				break;
			} else { // 優先順序：gone > pong > eat，同時有人同樣動作就由玩家index小的先？應該要由下家優先
				if (actiontype[i] > current_action_type) {
					player_to_act = i;
					current_action_type = actiontype[i];
					current_action_param = actionparameter[i];
				}
			}
		}

		printAction(player_to_act, current_action_type, current_action_param);
		cin.get();

		// Assign actions on players
		// 下一位出牌
		MJtile dummy;
		if (player_to_act == -1) { // 大家都沒有動作，直接換下一位
			(currentPos == 1) ? (currentPos = 4) : (currentPos -= 1);
			currentPlayer = posToPlayer[currentPos];
			_players[currentPlayer]->draw(mjcol);
			_players[currentPlayer]->strategy(currentPlayer, dummy, actiontype[currentPlayer], actionparameter[currentPlayer]);
			if (actiontype[currentPlayer] == -1) {
				if(actionparameter[currentPlayer]==1){
					// huown
					_players[currentPlayer]->act(-1, 1, dummy, mjcol);
				}
				//TODO
				break;
			}
			index = _players[currentPlayer]->decidePlay();
			t = _players[currentPlayer]->play(index);

		} else {
			_players[player_to_act]->act(current_action_type, current_action_param, t, mjcol);
			currentPos = playerToPos[player_to_act];
			currentPlayer = player_to_act;
			// 有吃碰槓的動作就是直接出一張，不用抽
			_players[currentPlayer]->strategy(currentPlayer, dummy, actiontype[currentPlayer], actionparameter[currentPlayer]);
			if (actiontype[currentPlayer] == -1) {
				// 自摸, huown
				//TODO
				break;
			}
			index = _players[currentPlayer]->decidePlay();
			t = _players[currentPlayer]->play(index);
		}
	}
	return;
}

