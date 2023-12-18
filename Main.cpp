# include <Siv3D.hpp> // Siv3D v0.6.13

constexpr int ScoreMax = 99999;
int32 score = 0;
constexpr int ZankiMax = 5;
int32 zanki = ZankiMax;

bool Button(const Rect& rect, const Font& font, const String& text)
{
	if (rect.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	rect.draw(ColorF{ 0.3, 0.7, 1.0 });
	font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2));

	return rect.leftClicked();
}

Array<Rect> create_blocks(const Size& BrockSize)
{
	Array<Rect> brock;
	srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
	int brockY = Random(4, 10);
	for (int32 y = 0; y <= brockY; ++y)
	{
		for (int32 x = 0; x < (Scene::Width() / BrockSize.x); ++x)
		{
			if (Random(10) <= 8)
				brock << Rect{ (x * BrockSize.x), (60 + y * BrockSize.y), BrockSize };
		}
	}
	return brock;
}


void Main()
{
	//ブロックのサイズ（Y成分、X成分は変更するため、別に指定）
	constexpr int32 BrockSizeY = 20;

	//ボールのスピード（pix/s）
	constexpr double BallSpeedPerSec = 450.0;

	//ボールのスピード（pix/s）
	Vec2 ballspeed = { 0, -BallSpeedPerSec };
	double fac_bs = 1.0; // ボールのスピードの変更のための変数

	//ボールの初期位置と大きさ
	Circle ball{ 400, 400, 8 };

	//ブロックの作成
	srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
	Array<Rect> brocks = create_blocks({ Random(30, 50), BrockSizeY });

	//ゲーム状態のbool値
	bool is_start = true;
	bool is_game_over = false;
	bool is_stage_clear = false;
	bool is_playing = false;

	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Font sfont{ FontMethod::MSDF, 20, Typeface::Bold };

	//パドル関連の定数
	constexpr int32 CoXTimes = 10;
	constexpr Size PaddleSize = { 60, 10 };
	double fac_paddle = 1.0;
	Size paddle_fix = PaddleSize;


	while (System::Update())
	{
		// パドル | Paddle
		const Rect paddle{ Arg::center(Cursor::Pos().x, 500), paddle_fix };

		if(is_start) // スタート画面または、難易度選択画面
		{
			if(not is_playing)
				font(U"Break Brocks").drawAt(40, Vec2{ 400, 300 }, ColorF(1.0));

			if (Button(Rect{ 50, 400, 200, 80 }, font, U"初級"))
			{
				is_start = false;
				is_playing = true;
				fac_bs = 0.8, fac_paddle = 1.5;
				ballspeed *= fac_bs;
				paddle_fix.x = int32(PaddleSize.x * fac_paddle);
			}

			if (Button(Rect{ 300, 400, 200, 80 }, font, U"中級"))
			{
				is_start = false;
				is_playing = true;
				fac_bs = 1.0, fac_paddle = 1.0;
				ballspeed *= fac_bs;
				paddle_fix.x = int32(PaddleSize.x * fac_paddle);
			}

			if (Button(Rect{ 550, 400, 200, 80 }, font, U"上級"))
			{
				is_start = false;
				is_playing = true;
				fac_bs = 1.2, fac_paddle = 0.8;
				ballspeed *= fac_bs;
				paddle_fix.x = int32(PaddleSize.x * fac_paddle);
			}
		}
		else
		{
			// 共通の処理
			if(is_game_over) //ゲームオーバー時の処理
			{
				font(U"Game Over...").drawAt(40, Vec2{400, 300}, ColorF(1.0));
				if (Button(Rect{ 250, 350, 300, 80 }, font, U"タイトルに戻る"))
				{
					ball.x = ball.y = 400;
					ballspeed = { 0, -BallSpeedPerSec };
					is_start = true;
					is_game_over = is_stage_clear = is_playing = false;
					zanki = ZankiMax;
					score = 0;
					fac_paddle = 1.0;
					paddle_fix.x = int32(PaddleSize.x * fac_paddle);
					srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
					brocks = create_blocks({ Random(30, 50), BrockSizeY });
				}
			}
			else if(is_stage_clear) //ステージクリア時の処理
			{
				font(U"Stage Clear!!").drawAt(40, Vec2{ 400, 300 }, ColorF(1.0));
				if (Button(Rect{ 50, 350, 300, 80 }, font, U"難易度選択"))
				{
					ball.x = ball.y = 400;
					ballspeed = { 0, -BallSpeedPerSec };
					is_start = true;
					is_game_over = is_stage_clear = false;
					fac_paddle = 1.0;
					zanki = ZankiMax;
					paddle_fix.x = int32(PaddleSize.x * fac_paddle);
					srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
					brocks = create_blocks({ Random(30, 50), BrockSizeY });
				}

				if (Button(Rect{ 450, 350, 300, 80 }, font, U"タイトルに戻る"))
				{
					ball.x = ball.y = 400;
					ballspeed = { 0, -BallSpeedPerSec };
					is_start = true;
					is_game_over = is_stage_clear = is_playing = false;
					zanki = ZankiMax;
					score = 0;
					fac_paddle = 1.0;
					paddle_fix.x = int32(PaddleSize.x * fac_paddle);
					srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
					brocks = create_blocks({ Random(30, 50), BrockSizeY });
				}
			}
			else //プレイ中の処理
			{
				//ブロック全消し時の処理
				if (brocks.size() == 0)
				{
					is_stage_clear = true;
					if (score + 100 + zanki * 50 <= ScoreMax)
						score += 100 + zanki * 50;
					else
						score = ScoreMax;
				}

				// ボールを移動
				ball.moveBy(ballspeed * Scene::DeltaTime());

				// パドルの下に行ったら
				if (paddle.y + 100 < ball.y)
				{
					zanki--;
					if (zanki < 0)
					{
						is_game_over = true;
					}
					else
					{
						ballspeed.x = 0, ballspeed.y = -BallSpeedPerSec * fac_bs;
						ball.x = 400, ball.y = 400;
					}
				}
				// ブロックを順にチェック
				for (auto it = brocks.begin(); it != brocks.end(); ++it)
				{
					// ブロックとボールが交差してら
					if (it->intersects(ball))
					{
						//ブロックの上下と交差してたら
						if (it->bottom().intersects(ball) || it->top().intersects(ball))
						{
							// ボールの速度のY成分の符号を反転
							ballspeed.y *= -1;
						}
						else //ブロックの左辺または右辺と交差していたら
						{
							//ボールの速度のX成分の符号を反転
							ballspeed.x *= -1;
						}

						//ブロックを配列から削除
						brocks.erase(it);

						//スコアを加算
						if (score < ScoreMax)
							score++;

						break;
					}
				}

				//天井にぶつかったら
				if ((ball.y < 0) && (ballspeed.y < 0))
				{
					//ボールの速度のY成分の符号を反転
					ballspeed.y *= -1;
				}

				//左右の壁にぶつかったら
				if (((ball.x < 0) && (ballspeed.x < 0)) || ((Scene::Width() < ball.x) && (0 < ballspeed.x)))
				{
					// ボールの速度の X 成分の符号を反転
					ballspeed.x *= -1;
				}

				// パドルにあたったら
				if ((0 < ballspeed.y) && paddle.intersects(ball))
				{
					// パドルの中心からの距離に応じてはね返る方向を変える
					ballspeed = Vec2{ (ball.x - paddle.center().x) * CoXTimes / fac_paddle, -ballspeed.y }.setLength(BallSpeedPerSec * fac_bs);
				}
			}
		}

		//共通の描画処理
		//残基、スコアの描画
		int remain = zanki;
		if (remain < 0)
			remain = 0;
		font(U"残基:{}"_fmt(remain)).drawAt(20, Vec2{ 50, 20 }, ColorF{ 1.0 });
		font(U"Score:{}"_fmt(score)).drawAt(20, Vec2{ 720, 20 }, ColorF{ 1.0 });
		//ブロックを描画
		for (const auto& brk : brocks)
		{
			//ブロックのY座標で色を変更
			brk.stretched(-1).draw(HSV{ brk.y - 40 });
		}
		// ボールを描く
		ball.draw();

		// パドルを描く
		paddle.rounded(5).draw();
	}
}
