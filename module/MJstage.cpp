#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <cassert>
#include <fstream>
#include <typeinfo>
#include <locale>
#include <string>

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
	// actiontype: nothing=0 eat=1 pong=2 minggone=3 angone=4 bugone=5 hu=7 play=8
	// actionparameter: huown=1 huother=2 play=index
	extern bool print_strategy;
	if (print_strategy) {
		cout << endl;
		for (int i = 0; i < 4; i++) {
			cout << "_player[" << i << "]: ";
			switch (actiontype[i]) {
			case 7:
				if (actionparameter[i] == 1) cout << "huown" << endl;
				if (actionparameter[i] == 2) cout << "huother" << endl;
				break;
			case 0:
				cout << "do nothing" << endl;
				break;
			case 1:
				cout << "eat ";
				switch (actionparameter[i]) {
				case 1: cout << "(001)" << endl; break;
				case 2: cout << "(010)" << endl; break;
				case 3: cout << "(001) (010)" << endl; break;
				case 4: cout << "(100)" << endl; break;
				case 5: cout << "(001) (100)" << endl; break;
				case 6: cout << "(010) (100)" << endl; break;
				case 7: cout << "(001) (010) (100)" << endl; break;
				}
				break;
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
			case 8:
				cout << "play: " << actionparameter[i] << endl;
				break;
			}
		}
	}
	return;
}


void printAction(int player_to_act, int current_action_type, int current_action_param) {
	extern bool print_action;
	if (print_action) {
		if (player_to_act == -1) {
			cout << "No player would like to act." << endl;
			return;
		}
		cout << "_players[" << player_to_act << "] is going to ";
		switch (current_action_type) {
		case 7:
			if (current_action_param == 1) cout << "huown" << endl;
			if (current_action_param == 2) cout << "huother" << endl;
			break;
		case 0:
			cout << "do nothing" << endl;
			break;
		case 1:
			cout << "eat ";
			switch (current_action_param) {
			case 1: cout << "(001)"; break;
			case 2: cout << "(010)"; break;
			case 3: cout << "(001) (010)"; break;
			case 4: cout << "(100)"; break;
			case 5: cout << "(001) (100)"; break;
			case 6: cout << "(010) (100)"; break;
			case 7: cout << "(001) (010) (100)"; break;
			}
            cout << endl;
			break;
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
		case 8:
			cout << "play a tile" << endl;
			break;
		}
	}
	return;
}


void printAllHands(const vector<unique_ptr<MJplayer> > &_players) {
	extern bool print_mainGame_allHands;
	if (print_mainGame_allHands) {
		cout << "Print all players' hand." << endl;
		for (int i = 0; i < 4; i++) {
			cout << "_players[" << i << "]'s hand is: " << endl;
			_players[i]->Print_Hand();
		}
		cout << endl;
	}
	return;
}





//============ MJstage Class Methods =================
MJstage::MJstage() {
	// cout << "Call MJstage constructor." << endl;

	Shuffler s;
	MJtile mjtiles[144];
	s.init();
	s.fill(mjtiles);
	mjcol = MJcollection(mjtiles);

	for (int i = 0; i < 4; i++) {
		unique_ptr<MJplayer> ptr(new MJGreedyAIplayer);
		_players.push_back(move(ptr));
	}
}


MJstage::MJstage(int n_human, int AIkind, int money) {
	// cout << "Call MJstage constructor." << endl;

	Shuffler s;
	MJtile mjtiles[144];
	s.init();
	s.fill(mjtiles);
	mjcol = MJcollection(mjtiles);

    if (AIkind >= 2 && n_human == 0) {
        // GreedyAI v.s. CustomAI
        // AIkind = # of GreedyAI
	    for (int i = 0; i < AIkind; i++) {
		    unique_ptr<MJplayer> ptr(new MJGreedyAIplayer(money));
		    _players.push_back(move(ptr));
	    }
	    for (int i = 0; i < 4 - AIkind; i++) {
		    unique_ptr<MJplayer> ptr(new MJCustomAIplayer(money));
		    _players.push_back(move(ptr));
	    }
    }

	for (int i = 0; i < n_human; i++) {
		unique_ptr<MJplayer> ptr(new MJplayer(money));
		_players.push_back(move(ptr));
	}
	for (int i = 0; i < 4 - n_human; i++) {
		if (AIkind == 1) {
			unique_ptr<MJplayer> ptr(new MJGreedyAIplayer(money));
			_players.push_back(move(ptr));
        } else {
			unique_ptr<MJplayer> ptr(new MJCustomAIplayer(money));
			_players.push_back(move(ptr));
		}
	}
	for (int i = 0; i < 4; i++) {
		playerToPos[i] = 0;
		posToPlayer[i] = 0;
	}
}


MJstage::~MJstage() {
	// smart pointer is smart thus no need to delete it.
	// for (int i = 0; i < 4; i++) {
	// 	delete _players[i];
	// }
}


MJstage& MJstage::operator=(MJstage&& other ) {
	_bookmaker = other._bookmaker;
	mjcol = other.mjcol;
	playerToPos = other.playerToPos;
	posToPlayer = other.posToPlayer;
	for (int i = 0; i < 4; i++) {
		// set unique_ptr by providing raw pointer and deleter
		_players[i] = move(other._players[i]);
	}
	return *this;
}


void MJstage::clear(void) {
	extern bool print_stage;
	if (print_stage) cout << "Do clear." << endl;
	// Preserve settings，再打一場
	Shuffler s;
	MJtile mjtiles[144];
	s.init();
	s.fill(mjtiles);
	seed = s.getSeed();
	mjcol = MJcollection(mjtiles);
	for (int i = 0; i < 4; i++) {
		_players[i]->clear_info();
	}
}


void MJstage::pickSeat(void) {
	// east = 1 ,south = 2 , west = 3 , north = 4
	extern bool print_pickSeat;
	extern bool print_stage;
	if (print_stage) cout << "Do pickSeat." << endl;

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
		if (print_pickSeat) cout << "Set _players[" << i << "]'s position to " << pos[i] << endl;
		// cout << "Set playerToPos[" << i << "] to " << pos[i] << endl;
		// cout << "Set posToPlayer[" << pos[i] << "] to " << i << endl;
	}
}


void MJstage::pickBookmaker(void) {
	extern bool print_pickBookmaker;
	extern bool print_stage;
	if (print_stage) cout << "Do pickBookmaker." << endl;

	srand(time(NULL));
	_bookmaker = rand() % 4;
	if (print_pickBookmaker) cout << "_players[" << _bookmaker << "] is the bookmaker" << endl;
	return;
}


void MJstage::nextBookmaker(void) {
	// 東南西北輪的話，pos 依序是 1 2 3 4
	extern bool print_pickBookmaker;
	extern bool print_stage;
	if (print_stage) cout << "Do nextBookmaker." << endl;

	int bookmaker_pos = playerToPos[_bookmaker];
	(bookmaker_pos == 4) ? (bookmaker_pos = 1) : (bookmaker_pos += 1);
	_bookmaker = posToPlayer[bookmaker_pos];
	if (print_pickBookmaker) cout << "_players[" << _bookmaker << "] is the bookmaker" << endl;
	return;
}


int MJstage::getBookmaker(void) {
	return _bookmaker;
}


void MJstage::getTiles(void) {
	extern bool print_stage;
	if (print_stage) cout << "Do getTiles." << endl;

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
		_players[i]->Set_Hand(&mjtiles_for_player[i][0], 16);
	}
	return;
}


void MJstage::initiate(void) {
	extern bool print_stage;
	if (print_stage) cout << "Do initiate." << endl;

	for (int i = 0; i < 4; i++) {
		_players[i]->initiate(mjcol);
	}
	printAllHands(_players);
}


pair<int, int> MJstage::mainGame(int& rounds) {
	extern bool print_mainGame_info;
	extern bool print_mainGame_singleHands;
	extern bool print_mainGame_others;
	extern bool print_stage;
	if (print_stage) cout << "Enter mainGame." << endl;

	int currentPos = 0;
	int currentPlayer = 0;
	int actiontype[4] = {0, 0, 0, 0};
	int actionparameter[4] = {0, 0, 0, 0};
	// 出牌順序：逆時針 (pos 4, 3, 2, 1, 4, etc.)

	currentPlayer = _bookmaker;
	currentPos = playerToPos[_bookmaker];

	MJtile t;
	int tiles_num = 0;
	// 抽牌從 initiate 移來這裡
	if (print_mainGame_info) cout << "It is bookmaker (_player[" << currentPlayer << "])'s turn." << endl;
	if (print_mainGame_info) cout << "Bookmaker draw the 17th tile." << endl;
	_players[currentPlayer]->draw(mjcol);
	if (print_mainGame_others) {
		cout << "\nNow bookmaker (_player[" << currentPlayer << "])'s hand is " << endl;
		_players[currentPlayer]->Print_Hand();
	}
	// 出牌前先判斷莊家有沒有胡、暗槓，因為剛開局根本沒有槓過，所以不可能補槓
	MJtile dummy;
	//cout << "Who are you?" << endl;
	//_players[currentPlayer]->whoIam();
	_players[currentPlayer]->strategy(currentPos, dummy, actiontype[currentPlayer], actionparameter[currentPlayer]);
	if (actiontype[currentPlayer] == 7 && actionparameter[currentPlayer] == 1) {
		//huown
		if (print_mainGame_info) cout << "***** _player[" << currentPlayer << "] huown! *****" << endl;
		_players[currentPlayer]->act(7, 1, dummy, mjcol);
		//hold();
	writeInfo();
		return make_pair(currentPlayer, -1);
	}

	int current_action_type = 0;
	int current_action_param = 0;
	// 正式開局！
	while (mjcol.size() > 16) { // 留下八墩(16張)牌
		if (print_mainGame_info) cout << "\nEnter while loop in rounds " << ++rounds << "." << endl;
		if (print_mainGame_info) cout << "Currently is _players[" << currentPlayer << "](position " << currentPos
			                              << ")'s turn." << endl;

		current_action_type = 0;
		current_action_param = 0;
		//cout << "_players[" << currentPlayer << "] decides what tile to play." << endl;
		while (actiontype[currentPlayer] != 8) {
			_players[currentPlayer]->act(current_action_type, current_action_param, dummy, mjcol);
			_players[currentPlayer]->strategy(currentPos, dummy, actiontype[currentPlayer], actionparameter[currentPlayer]);
		}
		if (actiontype[currentPlayer] == 8) {
			//cout << "Bookmaker decides what tile to play." << endl;
			t = _players[currentPlayer]->play(actionparameter[currentPlayer]);
			if (print_mainGame_singleHands) {
				cout << "_players[" << currentPlayer << "] play:" << endl;
				cout << t;
			}
			tiles_num = 1;
			for (int i = 0; i < 4; i++) {
				_players[i]->getinfo(currentPos, actiontype[currentPlayer], &t, tiles_num);
			}
		}


		// 其他三家要傳進那張丟出來的牌看能不能有 strategy
		if (print_mainGame_info) cout << "Other players decide strategy." << endl;
		for (int i = 0; i < 4; i++) {
			if (i != currentPlayer) {
				// cout << "The tile is \n" << t;
				// cout << "The tile in hand is \n";
				// _players[i]->Print_Hand();
				_players[i]->strategy(currentPos, t, actiontype[i], actionparameter[i]);
			} else {
				actiontype[i] = 0;
				actionparameter[i] = 0;
			}
		}
		printStrategy(actiontype, actionparameter);
		hold();

		// Checking Actions: gone > pong > eat
		// decide which player's action is executed
		int player_to_act = -1;
		for (int i = 0; i < 4; i++) {
			if (actiontype[i] == 7) {
				_players[i]->act(actiontype[i], actionparameter[i], t, mjcol);
				if (print_mainGame_info) cout << "***** _players[" << i << "] huother! *****" << endl;
				//hold();
	writeInfo();
				return make_pair(i, currentPlayer);
			} else { // 優先順序：gone > pong > eat，同時有人同樣動作就由玩家index小的先？應該要由下家優先
				if (actiontype[i] > current_action_type) {
					player_to_act = i;
					current_action_type = actiontype[i];
					current_action_param = actionparameter[i];
				}
			}
		}

		for (int i = 0; i < 4; i++) {
			actiontype[i] = 0;
			actionparameter[i] = 0;
		}

		printAction(player_to_act, current_action_type, current_action_param);
		//hold();

		// Assign actions on players
		if (player_to_act == -1) { // 大家都沒有動作，直接換下一位
			(currentPos == 1) ? (currentPos = 4) : (currentPos -= 1);
			currentPlayer = posToPlayer[currentPos];
			if (print_mainGame_info) cout << "It is _player[" << currentPlayer << "](position " << currentPos
				                              << ")'s turn." << endl;
			//cout << "_player[" << currentPlayer << "] draw a tile." << endl;
			_players[currentPlayer]->draw(mjcol);
			if (print_mainGame_others) {
				cout << "Now _player[" << currentPlayer << "]'s hand is " << endl;
				_players[currentPlayer]->Print_Hand();
			}
			// 看其他玩家faceup的牌
			if (_players[currentPlayer]->is_human()) {
				cout << "Other players faceup hands." << endl;
				for (int i = 0; i < 4; i++) {
					if (currentPlayer != i) _players[i]->showhandtoothers();
				}
			}
			// 檢查是否胡牌
			_players[currentPlayer]->strategy(currentPos, dummy, actiontype[currentPlayer], actionparameter[currentPlayer]);
			if (actiontype[currentPlayer] == 7 && actionparameter[currentPlayer] == 1) {
				// huown
				if (print_mainGame_info) cout << "***** _player[" << currentPlayer << "] huown! *****" << endl;
				_players[currentPlayer]->act(7, 1, dummy, mjcol);
				//hold();
	writeInfo();
				return make_pair(currentPlayer, -1);
			}
		} else {
			// player_to_act 不為 0，即至少有人有動作
			_players[player_to_act]->act(current_action_type, current_action_param, t, mjcol);

			// currentPlayer 換到剛剛有動作的人
			currentPos = playerToPos[player_to_act];
			currentPlayer = player_to_act;
			//cout << "It is _player[" << currentPlayer << "](position " << currentPos
			//     << ")'s turn." << endl;
			if (current_action_type == 1) {
				// Bring eat method into getinfo
				tiles_num = current_action_param;
			} else if (current_action_type == 2) {
				tiles_num = 3;
			} else {
				tiles_num = 4;
			}
			// 每位玩家紀錄這次動作
			for (int i = 0; i < 4; i++) {
				_players[i]->getinfo(currentPos, actiontype[currentPlayer], &t, tiles_num);
			}
			// 看其他玩家faceup的牌
			if (_players[currentPlayer]->is_human()) {
				cout << "Other players faceup hands." << endl;
				for (int i = 0; i < 4; i++) {
					if (currentPlayer != i) _players[i]->showhandtoothers();
				}
			}
			// 有吃碰槓的動作就是直接出一張，不用抽
			_players[currentPlayer]->strategy(currentPos, dummy, actiontype[currentPlayer], actionparameter[currentPlayer]);
			if (actiontype[currentPlayer] == 7 && actionparameter[currentPlayer] == 1) {
				// huown
				if (print_mainGame_info) cout << "***** _player[" << currentPlayer << "] huown! *****" << endl;
				_players[currentPlayer]->act(7, 1, dummy, mjcol);
				// hold();
	writeInfo();
				return make_pair(currentPlayer, -1);
			}
			// printAllHands(_players);

			if (print_mainGame_others) {
				cout << "Now _player[" << currentPlayer << "]'s hand is " << endl;
				_players[currentPlayer]->Print_Hand();
			}
		}
		if (print_mainGame_info)
			cout << "Finish a loop. Now the remain mjcol is at size " << mjcol.size() << endl;
		if (print_mainGame_others)
			cout << "\n--------------------------------------------------\n" << endl;
	}
	// cout << "write info." << endl;
	// cout << "Now the size is " << mjcol.size() << endl;
	writeInfo();
	return make_pair(-1, -1);
}


int MJstage::get_money(int index) {
	return _players[index]->Get_Mon();
}


int MJstage::get_seed(void) {
	return seed;
}


void MJstage::writeInfo(void) {
	time_t timer = time(NULL);
	char buffer[80];
	// May third = 0503
	strftime(buffer, sizeof(buffer), "%m%d", localtime(&timer));
	// cout << buffer << endl;
	string buffer_s;
	buffer_s = buffer;

	string filename = string("data/") + buffer_s + string("test.txt");
	// cout << filename << endl;

	ofstream myfile;
	// check file exist
	myfile.open(filename, fstream::in);
	bool file_exist;
	if (myfile.is_open()) {
		// cout << "hey is open." << endl;
		file_exist = true;
	} else {
		// cout << "hey not open." << endl;
		file_exist = false;
	}
	myfile.close();

	myfile.open(filename, fstream::in | fstream::out | fstream::app);
	// write seed
	if (!file_exist) myfile << "seed, strategy, remain mycol tiles" << endl;
	myfile << seed << ",";

	// write if all players are greedy AI, if true then write function order
	bool is_all_greedyAI = true;
	for (int i = 0; i < 4; i++) {
		string class_name = _players[i]->className();
		if (class_name != "MJGreedyAIplayer") {
			is_all_greedyAI = false;
			break;
		}
	}
	if (is_all_greedyAI) myfile << _players[0]->getFunctionOrder() << ",";
	else myfile << "Not all GreedyAI" << ",";

	// write remain mjcol size
	// cout << "write " << mjcol.size() - 16 << " in the file." << endl;;
	myfile << mjcol.size() - 16;

	myfile << endl;
	myfile.close();
	return;
}
