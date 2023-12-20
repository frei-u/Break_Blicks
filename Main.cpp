# include <Siv3D.hpp> // Siv3D v0.6.13

bool Button(const Rect& rect, const Font& font, const String& text, const Color& color, const bool& is_availale)
{
	if (rect.mouseOver())
		Cursor::RequestStyle(CursorStyle::Hand);

	if (not is_availale)
	{
		// ボタンが無効の時
		rect.draw(ColorF(0.7));
		font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2), ColorF(0.0));
		return false;
	}

	rect.draw(color);
	font(text).drawAt(40, (rect.x + rect.w / 2), (rect.y + rect.h / 2), ColorF(0.0));

	return rect.leftClicked();
}

Array<Rect> create_blocks(const Size& BrockSize)
{
	Array<Rect> brock;
	srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
	int32 brockY = Random(4, 10);
	for (int32 y = 0; y <= brockY; ++y)
	{
		for (int32 x = 0; x < (Scene::Width() / BrockSize.x); ++x)
		{
			if (Random(10) <= 8)
				brock << Rect{ (x* BrockSize.x), (60 + y * BrockSize.y), BrockSize };
		}
	}
	return brock;
}

double sign(const double& A) {
	return (A > 0) - (A < 0);
}

void Main()
{
	constexpr double PI = 3.14159265358979323846;
	const double sin45 = sin(PI / 4);
	const double sinm45 = sin(-PI / 4);
	const double cos45 = cos(PI / 4);
	const double cosm45 = cos(-PI / 4);
	const int32 Width = Scene::Width();

	// スコアの最大値
	constexpr int ScoreMax = 99999;
	// スコアの現在地
	int32 score = 0;
	// 残基数の最大
	constexpr int ZankiMax = 5;
	// 現在の残基数
	int32 zanki = ZankiMax;

	// ブロックのサイズ（Y成分、X成分は変更するため、別に指定）
	constexpr int32 BrockSizeY = 20;

	// ボールのスピード（pix/s）
	constexpr double BallSpeedPerSec = 450.0;

	// ボールのスピード（pix/s）
	Vec2 ballspeed = { 0, -BallSpeedPerSec };
	Vec2 a_ballspeed = { 0, -BallSpeedPerSec };
	// ボールのスピードの変更のための変数
	double fac_bs = 1.0; 

	// ボールの初期位置と大きさ
	Circle ball{ 400, 400, 8 };

	// ブロックの作成
	srand((unsigned int)time(NULL)); // 現在時刻の情報で初期化
	Array<Rect> brocks = create_blocks({ Random(30, 50), BrockSizeY });

	// ゲーム状態のbool値。ゲームのスタート画面 or 難易度選択画面か、
	// ゲームオーバーか、ステージクリアか、ゲームのプレイ中かを示す
	bool is_start = true;
	bool is_game_over = false;
	bool is_stage_clear = false;
	bool is_playing = false;

	// フォントの設定
	const Font font{ FontMethod::MSDF, 48, Typeface::Bold };
	const Font sfont{ FontMethod::MSDF, 20, Typeface::Bold };

	// パドル関連の定数。定数、デフォルトのパドルサイズ、
	// パドルサイズを変える変数と、変更後のパドルサイズ。
	constexpr int32 CoXTimes = 10;
	constexpr Size PaddleSize = { 60, 10 };
	double fac_paddle = 1.0;
	Size paddle_fix = PaddleSize;


	while (System::Update())
	{
		// パドル | Paddle
		const Rect paddle{ Arg::center(Cursor::Pos().x, 500), paddle_fix };

		if (is_start) // スタート画面または、難易度選択画面
		{
			// プレイ中でないときのみ、タイトルを表示する
			if (not is_playing)
				font(U"Break Brocks").drawAt(40, Vec2{ 400, 300 }, ColorF(1.0));

			// 難易度選択のためのボタン表示
			if (Button(Rect{ 50, 400, 200, 80 }, font, U"初級", ColorF(0.2, 1.0, 0.2, 1.0), true))
			{
				is_start = false, is_playing = true;
				fac_bs = 0.8, fac_paddle = 1.5;
				ballspeed *= fac_bs;
				paddle_fix.x = int32(PaddleSize.x * fac_paddle);
			}

			if (Button(Rect{ 300, 400, 200, 80 }, font, U"中級", ColorF(1.0, 1.0, 0.0, 1.0), true))
			{
				is_start = false, is_playing = true;
				fac_bs = 1.0, fac_paddle = 1.0;
				ballspeed *= fac_bs;
				paddle_fix.x = int32(PaddleSize.x * fac_paddle);
			}

			if (Button(Rect{ 550, 400, 200, 80 }, font, U"上級", ColorF(1.0, 0.2, 0.2, 1.0), true))
			{
				is_start = false, is_playing = true;
				fac_bs = 1.2, fac_paddle = 0.8;
				ballspeed *= fac_bs;
				paddle_fix.x = int32(PaddleSize.x * fac_paddle);
			}
		}
		else
		{
			if (is_stage_clear || is_game_over) //ステージクリア時とゲームオーバー時の処理
			{
				String text = U"Stage Clear!!";
				if (is_game_over)
					text = U"Game Over...";

				font(text).drawAt(40, Vec2{ 400, 300 }, ColorF(1.0));

				if (Button(Rect{ 50, 350, 300, 80 }, font, U"難易度選択", ColorF(0.2, 0.5, 0.8, 1.0), is_stage_clear))
				{
					// Score以外を初期化
					ball.x = ball.y = 400, ballspeed = { 0, -BallSpeedPerSec };
					is_start = true, is_game_over = is_stage_clear = false;
					fac_paddle = 1.0, zanki = ZankiMax;
					paddle_fix.x = int32(PaddleSize.x * fac_paddle);
					// random関数を現在時刻の情報で初期化し、ブロックを生成
					srand((unsigned int)time(NULL));
					brocks = create_blocks({ Random(30, 50), BrockSizeY });
				}

				if (Button(Rect{ 450, 350, 300, 80 }, font, U"タイトルに戻る", ColorF(1.0, 0.2, 0.2, 1.0), true))
				{
					// 初期化
					ball.x = ball.y = 400, ballspeed = { 0, -BallSpeedPerSec };
					is_start = true, is_game_over = is_stage_clear = is_playing = false;
					zanki = ZankiMax, score = 0, fac_paddle = 1.0;
					paddle_fix.x = int32(PaddleSize.x * fac_paddle);
					// random関数を現在時刻の情報で初期化し、ブロックを生成
					srand((unsigned int)time(NULL)); 
					brocks = create_blocks({ Random(30, 50), BrockSizeY });
				}
			}
			else // プレイ中の処理
			{
				// ブロック全消し時の処理
				if (brocks.size() == 0)
				{
					is_stage_clear = true;
					// スコアを残基数に応じて加算
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
					// 残基が0未満になったら、ゲームオーバー
					// その他の場合は、ボールの位置、速度を初期状態に戻して、リスタート
					if (zanki < 0)
						is_game_over = true;
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
						// ブロックの左下と右上の角に当たった場合
						if ((it->bottom().intersects(ball) && it->left().intersects(ball))
							|| (it->top().intersects(ball) && it->right().intersects(ball)))
						{
							// 角に当たった時の角度に応じて、速度の向きを回転
							Vec2 ballspeeda = { ballspeed.x * cosm45 - ballspeed.y * sinm45, ballspeed.x * sinm45 + ballspeed.y * cosm45};
							ballspeeda.y *= -1;
							ballspeed = Vec2{ ballspeeda.x * cos45 - ballspeeda.y * sin45, ballspeeda.x * sin45 + ballspeeda.y * cos45 }.setLength(BallSpeedPerSec * fac_bs);

							// 以下、Y軸方向の速度が0の場合の処理（進行不能にならない為の処理）
							if (ballspeed.y == 0) // Yの速度が0ならば、強制的にy方向の速度を与える
								ballspeed = Vec2{ ballspeed.x, 20.0 }.setLength(BallSpeedPerSec * fac_bs);
							else if (-30.0 < ballspeed.y && ballspeed.y < 30.0) // Yの速度が遅すぎるようなことがないようにy成分を加速
								ballspeed = Vec2{ ballspeed.x, 30.0 * sign(ballspeed.y)}.setLength(BallSpeedPerSec * fac_bs);
							else if (ballspeed.x == ballspeed.y == 0)
							{
								// 速度が0の場合、強制的に、Y向きの速度を付加
								if (it->top().intersects(ball))
									ballspeed = Vec2{ 0, -BallSpeedPerSec }.setLength(BallSpeedPerSec * fac_bs);
								else
									ballspeed = Vec2{ 0, BallSpeedPerSec }.setLength(BallSpeedPerSec * fac_bs);
							}
						}
						// ブロックの右下と左上の角にあったた場合
						else if ((it->bottom().intersects(ball) && it->right().intersects(ball)) ||
							(it->top().intersects(ball) && it->left().intersects(ball)))
						{
							// 角に当たった時の角度に応じて、速度の向きを回転
							Vec2 ballspeeda = { ballspeed.x * cos45 - ballspeed.y * sin45, ballspeed.x * sin45 + ballspeed.y * cos45 };
							ballspeeda.y *= -1;
							ballspeed = Vec2{ ballspeeda.x * cosm45 - ballspeeda.y * sinm45, ballspeeda.x * sinm45 + ballspeeda.y * cosm45 }.setLength(BallSpeedPerSec * fac_bs);

							// 以下、Y方向の速度が0の場合の処理（進行不能にならない為の処理）
							if (ballspeed.y == 0) // Yの速度が0ならば、強制的にY方向の速度を与える
								ballspeed = Vec2{ ballspeed.x, 20.0 }.setLength(BallSpeedPerSec * fac_bs);
							else if (-30.0 < ballspeed.y && ballspeed.y < 30.0) // Yの速度が遅すぎるようなことがないようにy成分を加速
								ballspeed = Vec2{ ballspeed.x, 30.0 * sign(ballspeed.y) }.setLength(BallSpeedPerSec * fac_bs);
							else if (ballspeed.x == ballspeed.y == 0)
							{
								// 速度が0の場合、強制的に、Yの速度を付加
								if (it->top().intersects(ball))
									ballspeed = Vec2{ 0, -BallSpeedPerSec }.setLength(BallSpeedPerSec * fac_bs);
								else
									ballspeed = Vec2{ 0, BallSpeedPerSec }.setLength(BallSpeedPerSec * fac_bs);
							}
						}
						// ブロックの上下に交差した場合
						else if (it->bottom().intersects(ball) || it->top().intersects(ball))
						{
							// ボールの速度のY成分の符号を反転
							ballspeed.y *= -1;
						}
						// ブロックの左右に交差した場合
						else
						{
							// ボールの速度のX成分の符号を反転
							ballspeed.x *= -1;
						}

						// ブロックを配列から削除
						brocks.erase(it);

						// スコアを加算
						if (score < ScoreMax)
							score++;

						break;
					}
				}

				// 天井にぶつかった場合
				if (ball.y < 0 && ballspeed.y < 0)
				{
					// ボールの速度のY成分の符号を反転
					ballspeed.y *= -1;
				}

				// 左右の壁にぶつかった場合
				if ((ball.x < 0 && ballspeed.x < 0) || (Width < ball.x && 0 < ballspeed.x))
				{
					// ボールの速度のX成分の符号を反転
					ballspeed.x *= -1;
				}

				// パドルにあたったら
				if (0 < ballspeed.y && paddle.intersects(ball))
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

		// ボールを描画
		ball.draw();

		// パドルを描画
		paddle.rounded(5).draw();
	}
}
