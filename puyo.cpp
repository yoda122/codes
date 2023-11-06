#include <curses.h>
#include <cstdlib>
#include <ctime>

//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor { NONE, RED, BLUE, GREEN, YELLOW };

class PuyoArray{
	private:
		//盤面状態
		puyocolor *data;
		unsigned int data_line;
		unsigned int data_column;

		//メモリ解放
		void Release(){
			if (data == NULL) {
				return;
			}

			delete[] data;
			data = NULL;
		}

	public:
		PuyoArray(){
			data=NULL;
			data_line=0;
			data_column=0;
		}

		~PuyoArray(){
			Release();
		}

		//盤面サイズ変更
		void ChangeSize(unsigned int line, unsigned int column){
			Release();

			//新しいサイズでメモリ確保
			data = new puyocolor[line*column];

			data_line = line;
			data_column = column;
		}

		//盤面の行数を返す
		unsigned int GetLine(){
			return data_line;
		}

		//盤面の列数を返す
		unsigned int GetColumn(){
			return data_column;
		}

		//盤面の指定された位置の値を返す
		puyocolor GetValue(unsigned int y, unsigned int x){
			if (y >= GetLine() || x >= GetColumn()){
				//引数の値が正しくない
				return NONE;
			}

			return data[y*GetColumn() + x];
		}

		//盤面の指定された位置に値を書き込む
		void SetValue(unsigned int y, unsigned int x, puyocolor value){
			if (y >= GetLine() || x >= GetColumn()){
				//引数の値が正しくない
				return;
			}

			data[y*GetColumn() + x] = value;
		}

};

class PuyoArrayActive : public PuyoArray{
	public:
		int puyorotate;
		int score;
		int chain;

		PuyoArrayActive(){
			puyorotate = 0;
			score = 0;
			chain = 0;
		}

		int GetScore(){
			return score;
		}

		void SetScore(int value){
			score = value;
		}

		int GetChain(){
			return chain;
		}

		void SetChain(int value){
			chain = value;
		}

};

class PuyoArrayStack : public PuyoArray{
	
};

class PuyoArrayFalling : public PuyoArray{

};

class PuyoControl{
	private:
		//色をランダムに決定
		puyocolor ChooseColor(){
			int num=rand()%4;
			switch(num){
				case 0:
					return RED;
				break;

				case 1:
					return BLUE;
				break;

				case 2:
					return GREEN;
				break;

				case 3:
					return YELLOW;
				break;
			}
		}

	public:
		//盤面に新しいぷよ生成
		void GeneratePuyo(PuyoArrayActive &puyo){
			puyocolor newpuyo1;
			newpuyo1 = ChooseColor();

			puyocolor newpuyo2;
			newpuyo2 = ChooseColor();

			puyo.puyorotate = 0;

			puyo.SetValue(0, 3, newpuyo1);
			puyo.SetValue(0, 4, newpuyo2);
		}

		//ぷよの着地判定．着地判定があるとtrueを返す
		int LandingPuyo(PuyoArrayActive &puyo, PuyoArrayStack &puyoC, PuyoArrayFalling &puyoF){
			int landed = 0;

			for (int y = 0; y < puyo.GetLine(); y++){
				for (int x = 0; x < puyo.GetColumn(); x++){
					if (puyo.GetValue(y, x) != NONE){
						if (y == puyo.GetLine() - 1 || puyoC.GetValue(y + 1, x) != NONE){

							puyoC.SetValue(y, x, puyo.GetValue(y, x));
							puyo.SetValue(y, x, NONE);
							VanishPuyo(puyo, puyoC, puyoF, y, x, puyo.GetChain());

							if (puyo.GetValue(y - 1, x) != NONE){
								puyoF.SetValue(y - 1, x, puyo.GetValue(y - 1, x));
								puyo.SetValue(y - 1, x, NONE);
							}
							if (puyo.GetValue(y, x + 1) != NONE){
								puyoF.SetValue(y, x + 1, puyo.GetValue(y, x + 1));
								puyo.SetValue(y, x + 1, NONE);
							}
							if (puyo.GetValue(y, x - 1) != NONE){
								puyoF.SetValue(y, x - 1, puyo.GetValue(y, x - 1));
								puyo.SetValue(y, x - 1, NONE);
							}

						}
					}else if (puyoF.GetValue(y, x) != NONE){
						if (y == puyo.GetLine() - 1 || puyoC.GetValue(y + 1, x) != NONE){

							puyoC.SetValue(y, x, puyoF.GetValue(y, x));
							puyoF.SetValue(y, x, NONE);
							VanishPuyo(puyo, puyoC, puyoF, y, x, puyo.GetChain());

						}
					}else{
						landed += 1;
					}
				}
			}
			
			return landed;
		}

		//ぷよ消滅処理を座標(x,y)で行う
		//消滅したぷよの数を返す
		void VanishPuyo(PuyoArrayActive &puyo, PuyoArrayStack &puyostack, PuyoArrayFalling &puyoF, unsigned int y, unsigned int x, int chain){
			
			//判定状態を表す列挙型
			//NOCHECK判定未実施，CHECKINGが判定対象，CHECKEDが判定済み
			enum checkstate{ NOCHECK, CHECKING, CHECKED };

			//判定結果格納用の配列
			enum checkstate *field_array_check;
			field_array_check = new enum checkstate[puyostack.GetLine()*puyostack.GetColumn()];

			//配列初期化
			for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++){
				field_array_check[i] = NOCHECK;
			}

			//座標(x,y)を判定対象にする
			field_array_check[y*puyostack.GetColumn() + x] = CHECKING;

			//判定対象が1つもなくなるまで，判定対象の上下左右に同じ色のぷよがあるか確認し，あれば新たな判定対象にする
			bool checkagain = true;
			while (checkagain){
				checkagain = false;

				for (int y = 0; y < puyostack.GetLine(); y++){
					for (int x = 0; x < puyostack.GetColumn(); x++){
						//(x,y)に判定対象がある場合
						if (field_array_check[y*puyostack.GetColumn() + x] == CHECKING){
							//(x+1,y)の判定
							if (x < puyostack.GetColumn() - 1){
								//(x+1,y)と(x,y)のぷよの色が同じで，(x+1,y)のぷよが判定未実施か確認
								if (puyostack.GetValue(y, x + 1) == puyostack.GetValue(y, x) && field_array_check[y*puyostack.GetColumn() + (x + 1)] == NOCHECK){
									//(x+1,y)を判定対象にする
									field_array_check[y*puyostack.GetColumn() + (x + 1)] = CHECKING;
									checkagain = true;
								}
							}

							//(x-1,y)の判定
							if (x > 0){
								if (puyostack.GetValue(y, x - 1) == puyostack.GetValue(y, x) && field_array_check[y*puyostack.GetColumn() + (x - 1)] == NOCHECK){
									field_array_check[y*puyostack.GetColumn() + (x - 1)] = CHECKING;
									checkagain = true;
								}
							}

							//(x,y+1)の判定
							if (y < puyostack.GetLine() - 1){
								if (puyostack.GetValue(y + 1, x) == puyostack.GetValue(y, x) && field_array_check[(y + 1)*puyostack.GetColumn() + x] == NOCHECK){
									field_array_check[(y + 1)*puyostack.GetColumn() + x] = CHECKING;
									checkagain = true;
								}
							}

							//(x,y-1)の判定
							if (y > 0){
								if (puyostack.GetValue(y - 1, x) == puyostack.GetValue(y, x) && field_array_check[(y - 1)*puyostack.GetColumn() + x] == NOCHECK){
									field_array_check[(y - 1)*puyostack.GetColumn() + x] = CHECKING;
									checkagain = true;
								}
							}

							//(x,y)を判定済みにする
							field_array_check[y*puyostack.GetColumn() + x] = CHECKED;
						}
					}
				}
			}

			//判定済みの数をカウント
			int puyocount = 0;
			for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++){
				if (field_array_check[i] == CHECKED){
					puyocount++;
				}
			}

			//4個以上あれば，判定済み座標のぷよを消す。上にぷよがあれば落下させるようにする
			int vanishednumber = 0;
			if (4 <= puyocount){
				for (int y = 0; y < puyostack.GetLine(); y++){
					for (int x = 0; x < puyostack.GetColumn(); x++){
						if (field_array_check[y*puyostack.GetColumn() + x] == CHECKED){
							puyostack.SetValue(y, x, NONE);
							int yy = 1;
							while (puyostack.GetValue(y - yy, x) != NONE){
								puyoF.SetValue(y - yy, x, puyostack.GetValue(y - yy, x));
								puyostack.SetValue(y - yy, x, NONE);
								yy += 1;

							}
							vanishednumber++;
						}
					}
				}
				puyo.SetChain(puyo.GetChain() + 1);
				puyo.SetScore(puyo.GetScore() + puyo.GetChain());

			}

			//メモリ解放
			delete[] field_array_check;


		}

		//左移動
		void MoveLeft(PuyoArrayActive &puyo, PuyoArrayStack &puyoC){
			//一時的格納場所メモリ確保
			puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];

			for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++){
				puyo_temp[i] = NONE;
			}

			//1つ左の位置にpuyoactiveからpuyo_tempへとコピー
			for (int y = 0; y < puyo.GetLine(); y++){
				for (int x = 0; x < puyo.GetColumn(); x++){
					if (puyo.GetValue(y, x) == NONE) {
						continue;
					}

					if (0 < x && puyo.GetValue(y, x - 1) == NONE && puyoC.GetValue(y, x - 1) == NONE){
						puyo_temp[y*puyo.GetColumn() + (x - 1)] = puyo.GetValue(y, x);
						//コピー後に元位置のpuyoactiveのデータは消す
						puyo.SetValue(y, x, NONE);
					}else{
						puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
					}
				}
			}

			//puyo_tempからpuyoactiveへコピー
			for (int y = 0; y < puyo.GetLine(); y++){
				for (int x = 0; x < puyo.GetColumn(); x++){
					puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
				}
			}

			//一時的格納場所メモリ解放
			delete[] puyo_temp;
		}

		//右移動
		void MoveRight(PuyoArrayActive &puyo, PuyoArrayStack &puyoC){
			//一時的格納場所メモリ確保
			puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];

			for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++){
				puyo_temp[i] = NONE;
			}

			//1つ右の位置にpuyoactiveからpuyo_tempへとコピー
			for (int y = 0; y < puyo.GetLine(); y++){
				for (int x = puyo.GetColumn() - 1; x >= 0; x--){
					if (puyo.GetValue(y, x) == NONE) {
						continue;
					}

					if (x < puyo.GetColumn() - 1 && puyo.GetValue(y, x + 1) == NONE && puyoC.GetValue(y, x + 1)==NONE){
						puyo_temp[y*puyo.GetColumn() + (x + 1)] = puyo.GetValue(y, x);
						//コピー後に元位置のpuyoactiveのデータは消す
						puyo.SetValue(y, x, NONE);
					}else{
						puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
					}
				}
			}

			//puyo_tempからpuyoactiveへコピー
			for (int y = 0; y <puyo.GetLine(); y++){
				for (int x = 0; x <puyo.GetColumn(); x++){
					puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
				}
			}

			//一時的格納場所メモリ解放
			delete[] puyo_temp;
		}

		//下移動
		void MoveDown(PuyoArrayActive &puyo, PuyoArrayStack &puyoC, PuyoArrayFalling &puyoF){
			//一時的格納場所メモリ確保
			puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];
			puyocolor *puyoF_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];

			for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++){
				puyo_temp[i] = NONE;
				puyoF_temp[i] = NONE;
			}

			//1つ下の位置にpuyoactiveからpuyo_tempへとコピー
			for (int y = puyo.GetLine() - 1; y >= 0; y--){
				for (int x = 0; x < puyo.GetColumn(); x++){
					if (puyo.GetValue(y, x) == NONE) {
						continue;
					}

					if (y < puyo.GetLine() - 1 && puyo.GetValue(y + 1, x) == NONE && puyoC.GetValue(y + 1, x) == NONE){
						puyo_temp[(y + 1)*puyo.GetColumn() + x] = puyo.GetValue(y, x);
						//コピー後に元位置のpuyoactiveのデータは消す
						puyo.SetValue(y, x, NONE);
					}else{
						puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
					}
				}
			}
			for (int y = puyo.GetLine() - 1; y >= 0; y--){
				for (int x = 0; x < puyo.GetColumn(); x++){
					if (puyoF.GetValue(y, x) == NONE) {
						continue;
					}

					if (y < puyo.GetLine() - 1 && puyoF.GetValue(y + 1, x) == NONE && puyoC.GetValue(y + 1, x) == NONE){
						puyoF_temp[(y + 1)*puyo.GetColumn() + x] = puyoF.GetValue(y, x);
						//コピー後に元位置のpuyoactiveのデータは消す
						puyoF.SetValue(y, x, NONE);
					}else{
						puyoF_temp[y*puyo.GetColumn() + x] = puyoF.GetValue(y, x);
					}
				}
			}


			//puyo_tempからpuyoactiveへコピー
			for (int y = 0; y < puyo.GetLine(); y++){
				for (int x = 0; x < puyo.GetColumn(); x++){
					puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
				}
			}
			for (int y = 0; y < puyo.GetLine(); y++){
				for (int x = 0; x < puyo.GetColumn(); x++){
					puyoF.SetValue(y, x, puyoF_temp[y*puyo.GetColumn() + x]);
				}
			}


			//一時的格納場所メモリ解放
			delete[] puyo_temp;
			delete[] puyoF_temp;
		}

		void Rotate(PuyoArrayActive &puyoactive, PuyoArrayStack &puyostack){
		//フィールドをラスタ順に探索（最も上の行を左から右方向へチェックして，次に一つ下の行を左から右方向へチェックして，次にその下の行・・と繰り返す）し，先に発見される方をpuyo1, 次に発見される方をpuyo2に格納
			puyocolor puyo1, puyo2;
			int puyo1_x = 0;
			int puyo1_y = 0;
			int puyo2_x = 0;
			int puyo2_y = 0;

			bool findingpuyo1 = true;
			for (int y = 0; y < puyoactive.GetLine(); y++){
				for (int x = 0; x < puyoactive.GetColumn(); x++){
					if (puyoactive.GetValue(y, x) != NONE){
						if (findingpuyo1){
							puyo1 = puyoactive.GetValue(y, x);
							puyo1_x = x;
							puyo1_y = y;
							findingpuyo1 = false;
						}else{
							puyo2 = puyoactive.GetValue(y, x);
							puyo2_x = x;
							puyo2_y = y;
						}
					}
				}
			}


			//回転前のぷよを消す
			puyoactive.SetValue(puyo1_y, puyo1_x, NONE);
			puyoactive.SetValue(puyo2_y, puyo2_x, NONE);


			//操作中ぷよの回転
			switch (puyoactive.puyorotate){
				case 0:
				//回転パターン
				//RB -> R
				//      B
				//Rがpuyo1, Bがpuyo2
				if (puyo2_x <= 0 || puyo2_y >= puyoactive.GetLine() - 1 || puyostack.GetValue(puyo2_y + 1, puyo2_x - 1) != NONE){
					puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
					puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
					break;
				}

				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y + 1, puyo2_x - 1, puyo2);
				//次の回転パターンの設定
				puyoactive.puyorotate = 1;
				break;

			case 1:
				//回転パターン
				//R -> BR
				//B
				//Rがpuyo1, Bがpuyo2
				if (puyo2_x <= 0 || puyo2_y <= 0 || puyostack.GetValue(puyo2_y - 1, puyo2_x - 1) != NONE){
					puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
					puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
					break;
				}

				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y - 1, puyo2_x - 1, puyo2);

				//次の回転パターンの設定
				puyoactive.puyorotate = 2;
				break;

			case 2:
				//回転パターン
				//      B
				//BR -> R
				//Bがpuyo1, Rがpuyo2
				if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y <= 0 || puyostack.GetValue(puyo1_y - 1, puyo1_x + 1) != NONE){
					puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
					puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
					break;
				}

				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y - 1, puyo1_x + 1, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

				//次の回転パターンの設定
				puyoactive.puyorotate = 3;
				break;

			case 3:
				//回転パターン
				//B
				//R -> RB
				//Bがpuyo1, Rがpuyo2
				if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y >= puyoactive.GetLine() - 1 || puyostack.GetValue(puyo1_y + 1, puyo1_x + 1) != NONE){
					puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
					puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
					break;
				}

				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y + 1, puyo1_x + 1, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

				//次の回転パターンの設定
				puyoactive.puyorotate = 0;
				break;

			default:
				break;
			}
		}

	
};


//表示
void Display(PuyoArrayActive &puyo, PuyoArrayStack &puyoC, PuyoArrayFalling &puyoF, int limit, int const_prev){
	//落下中ぷよ表示
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			switch (puyo.GetValue(y, x))
			{
			case NONE:
				attrset(COLOR_PAIR(5));
				mvaddch(y, x, '.');
				break;
			case RED:
				attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
				attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
				attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
				attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}

			switch (puyoC.GetValue(y, x))
			{
			case NONE:
				if(puyo.GetValue(y, x)==NONE){
					attrset(COLOR_PAIR(5));
					mvaddch(y, x, '.');
				}
				break;
			case RED:
				attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
				attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
				attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
				attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}

			switch (puyoF.GetValue(y, x))
			{
			case NONE:
				if(puyo.GetValue(y, x)==NONE && puyoC.GetValue(y, x)==NONE){
					attrset(COLOR_PAIR(5));
					mvaddch(y, x, '.');
				}
				break;
			case RED:
				attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
				attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
				attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
				attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}
			
		}
	}


	//情報表示
	int count = 0;
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			if (puyo.GetValue(y, x) != NONE || puyoC.GetValue(y, x) != NONE)
			{
				count++;
			}
		}
	}

	char msg[256], msg2[256], msg3[256], msg4[256];
	attrset(COLOR_PAIR(5));
	sprintf(msg, "Field: %d x %d, Puyo number:  %03d", puyo.GetLine(), puyo.GetColumn(), count);
	sprintf(msg2, "Score: %03d", puyo.GetScore());
	sprintf(msg3, "%d Chain!", puyo.GetChain());
	sprintf(msg4, "%d Chain!", const_prev);
	mvaddstr(2, COLS - 86, msg);
	mvaddstr(4, COLS - 64, msg2);
	if (puyo.GetChain() >= 2){
		mvaddstr(4, COLS - 103, msg3);
	}else if (limit > 0){
		mvaddstr(4, COLS - 103, msg4);
	}else{
		attrset(COLOR_PAIR(6));
		mvaddstr(4, COLS - 103, msg3);
	}



	refresh();
}


//ここから実行される
int main(int argc, char **argv){
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();
	init_pair(1,COLOR_RED, COLOR_BLACK); 
	init_pair(2,COLOR_BLUE, COLOR_BLACK);
	init_pair(3,COLOR_GREEN, COLOR_BLACK);
	init_pair(4,COLOR_YELLOW, COLOR_BLACK);
	init_pair(5,COLOR_WHITE, COLOR_BLACK);
	init_pair(6,COLOR_BLACK,COLOR_BLACK);

	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);

	std::srand(time(NULL));


	//初期化処理
	const int F_LINE = 12;
	const int F_COLS = 8;

	PuyoArrayActive puyo;
	PuyoArrayStack puyoC;
	PuyoArrayFalling puyoF;
	PuyoControl control;

	puyo.ChangeSize(F_LINE, F_COLS);	//フィールドは画面サイズの縦横1/2にする
	puyoC.ChangeSize(F_LINE, F_COLS);
	puyoF.ChangeSize(F_LINE, F_COLS);
	control.GeneratePuyo(puyo);	//最初のぷよ生成

	int delay = 0;
	int waitCount = 15000;
	int level = 0;
	int chain = 0;
	int prev = 0;
	int const_prev = 0;
	int limit = 0;

	int puyostate = 0;

	//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q')
		{
			break;
		}

		//入力キーごとの処理
		switch (ch)
		{
		case KEY_LEFT:
			control.MoveLeft(puyo, puyoC);
			break;
		case KEY_RIGHT:
			control.MoveRight(puyo, puyoC);
			break;
		case 'z':
			//ぷよ回転処理
			control.Rotate(puyo, puyoC);
			break;
		default:
			break;
		}


		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			delay = 0;
			//ぷよ下に移動
			control.MoveDown(puyo, puyoC, puyoF);
			
			//ぷよ着地判定
			if (control.LandingPuyo(puyo, puyoC, puyoF) == puyo.GetLine()*puyo.GetColumn())
			{
				//着地していたら新しいぷよ生成
				control.GeneratePuyo(puyo);
				puyo.SetChain(0);
				level += 1;
				if (waitCount >= 3000){
					waitCount = 15000 - level * 100;
				}
			}
		}
		delay++;

		//表示
		Display(puyo, puyoC, puyoF, limit, const_prev);

		if (prev >= 2 && puyo.GetChain() == 0){
			limit = 50000;
			const_prev = prev;
		}
		prev = puyo.GetChain();
		if (limit > 0){
			limit -= 1;
		}
		
	}


	//画面をリセット
	endwin();

	return 0;
}
