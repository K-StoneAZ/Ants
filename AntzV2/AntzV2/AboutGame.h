#pragma once
#include <windows.h>
#include "Renderer.h"
#include "Field.h"
#include "GameTypes.h"
extern int gWidth;
extern int gHeight;

class AboutGame
{
private:
	Renderer m_renderer;
	Field m_field;
	bool m_isRunning = false;
	std::vector<PlayerData> m_players;
	DialogState m_dialog;
	TextBox m_antBox;
	std::string m_error;
	GameSpeed m_gameSpeed = GameSpeed::Normal;
	int m_activePlayer = 1;

	HDC m_hdc = nullptr;
	bool m_showDialog = true;
	bool m_nextRequested = false;
	RECT m_beginRect = {};
	RECT m_nextRect = {};

	static constexpr int MOUSE_TRAIL_COUNT = 4;
	POINT m_mousePos = { 100, 100 };
	POINT m_mouseTarget = { 100, 100 };
	POINT m_mouseTrail[MOUSE_TRAIL_COUNT] = {};
	bool m_mouseVisible = false;
	bool m_mouseMoving = false;
	bool m_showAntOverlay = false;
	bool m_nextColors = false;
	int m_mouseTap = 0;
	int m_dialogWidth = 500;
	int m_dialogHeight = 120;
	int m_dialogOffsetX = 0;
	int m_dialogOffsetY = 35;
	int m_dialogIndex = 0;
	int m_delayTicks = 0;
	int m_overlayAnts = 0;

	bool Delay(int ticks)
	{
		if (++m_delayTicks < ticks)
		{
			return false;
		}

		m_delayTicks = 0;
		return true;
	}


	enum class WalkState
	{
		INTRO,
		GROWTH,
		ATTACK,
		MOVE,
		TURN_TRANSITION
	};

	WalkState m_state = WalkState::INTRO;
	int m_stateStep = 0;

	struct DialogData
	{
		std::vector<const wchar_t*> text;
	};
	std::vector<DialogData> m_DialogData =
	{
		{ //0
			{
				L"Red is Player 1 and is a human player.", L"Blue is the opponent.",
				L"The cursor will demonstrate Red's actions."
			}
		},
		{ //1
			{
				L"During Red's turn, the player must distribute all available ants.",
				L"Choose cells based on opponent's position and the player's",
				L"personal strategy.  Select this cell and add all available ants"
			}
		},
		{ //2
			{
				L"Click OK or <enter>to add ants to the selected cell",
				L"Click Cancel or <esc> to choose a different cell"
			}
		},
		{ //3
			{
				L"Red's Growth phase is complete.",
				L"Red can now attack an adjacent enemy cell.",
				L"Select a Red cell with ants available to attack."
			}
		},
		{ //4
			{
				L"The highlighted cells are valid attack targets.",
				L"Red can attack an adjacent cell that it does not own.",
				L"Select the highlighted Blue cell to continue."
			}
		},
		{ //5
			{
				L"Red can choose how many ants to commit to an attack.",
				L"Red must leave at least one ant behind in the source cell.",
				L"Using the number keys, enter the count. Red will commit half.",
			}
		},
		{ //6
			{
				L"The attack is ready.",
				L"Click OK or <enter> to confirm, Cancel or <esc> to choose a different cell or",
				L"click Skip or <space> to go to Move stage."
			}
		},
		{ //7
			{
				L"Red's first attack is successful.",
				L"Red may attack again, or choose Skip to go",
				L"to the move stage."
			}
		},
		{ //8
			{
				L"The highlighted cells with more than 1 ant are valid move sources.",
				L"Red can move ants to an adjacent cell that it owns.",
				L"Select the highlighted Red cell to continue."
			}
		},
		{ //9
			{
				L"Red can choose how many ants to commit to a move.",
				L"Red must leave at least one ant behind in the source cell.",
				L"Using the number keys, enter the count. Red will commit one ant.",
			}
		},
		{ //10
			{
				L"The move is ready.",
				L"Click OK or <enter> to confirm, Cancel or <esc> to choose a different cell or",
				L"click Skip or <space> to end the turn."
			}
		},
		{ //11
			{
				L"Red's turn is complete.",
				L"Blue would take its turn next.  When Blue's turn is complete,",
				L"Red's turn begins again with Growth."
			}
		}
	};

	void FlashNextButton()
	{
		m_nextColors = !m_nextColors;
	}

	void RenderNextButton()
	{
		m_nextRect = { 935, 625, 1060, 670 };

		HBRUSH brush = CreateSolidBrush(
			m_nextColors ? RGB(255, 255, 255) : RGB(60, 60, 60));
		FillRect(m_hdc, &m_nextRect, brush);
		DeleteObject(brush);

		FrameRect(m_hdc, &m_nextRect,
			static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		SetBkMode(m_hdc, TRANSPARENT);
		SetTextColor(m_hdc,
			m_nextColors ? RGB(60, 60, 60) : RGB(255, 255, 255));

		DrawTextW(m_hdc, L"Next", -1, &m_nextRect,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	void SetMouseTarget(int row, int col)
	{
		const auto& cells = m_renderer.GetCellRects();

		for (const auto& cell : cells)
		{
			if (cell.row == row && cell.col == col)
			{
				m_mouseTarget.x =
					(cell.rect.left + cell.rect.right) / 2;
				m_mouseTarget.y =
					(cell.rect.top + cell.rect.bottom) / 2;

				if (!m_mouseVisible)
				{
					m_mousePos = m_mouseTarget;

					for (int i = 0; i < MOUSE_TRAIL_COUNT; ++i)
					{
						m_mouseTrail[i] = m_mousePos;
					}

					m_mouseVisible = true;
				}

				m_mouseMoving = true;
				m_mouseTap = 0;
				return;
			}
		}
	}

	void SetMouseTarget(const RECT& rect)
	{
		m_mouseTarget.x = (rect.left + rect.right) / 2;
		m_mouseTarget.y = (rect.top + rect.bottom) / 2;

		if (!m_mouseVisible)
		{
			m_mousePos = m_mouseTarget;

			for (int i = 0; i < MOUSE_TRAIL_COUNT; ++i)
			{
				m_mouseTrail[i] = m_mousePos;
			}

			m_mouseVisible = true;
		}

		m_mouseMoving = true;
		m_mouseTap = 0;
	}

	void Mouse()
	{
		if (!m_mouseVisible)
		{
			return;
		}

		m_mouseTrail[3] = m_mouseTrail[2];
		m_mouseTrail[2] = m_mouseTrail[1];
		m_mouseTrail[1] = m_mouseTrail[0];
		m_mouseTrail[0] = m_mousePos;

		if (m_mousePos.x != m_mouseTarget.x ||
			m_mousePos.y != m_mouseTarget.y)
		{
			int dx = m_mouseTarget.x - m_mousePos.x;
			int dy = m_mouseTarget.y - m_mousePos.y;

			if (dx > 12) dx = 12;
			if (dx < -12) dx = -12;
			if (dy > 12) dy = 12;
			if (dy < -12) dy = -12;

			m_mousePos.x += dx;
			m_mousePos.y += dy;

			m_mouseMoving =
				m_mousePos.x != m_mouseTarget.x ||
				m_mousePos.y != m_mouseTarget.y;
		}
		else
		{
			m_mouseMoving = false;

			if (m_mouseTap < 3)
			{
				++m_mouseTap;
			}
		}
	}

	RECT GetDialogRect() const
	{
		RECT rect = {};

		rect.left = m_mousePos.x + m_dialogOffsetX;
		rect.top = m_mousePos.y + m_dialogOffsetY;

		// Move left if the dialog would exceed the right edge.
		if (rect.left + m_dialogWidth > gWidth)
		{
			rect.left = m_mousePos.x - m_dialogWidth - 20;
		}

		// Move above if the dialog would exceed the bottom edge.
		if (rect.top + m_dialogHeight > gHeight)
		{
			rect.top = m_mousePos.y - m_dialogHeight - 20;
		}

		// Keep the complete rectangle inside the rendering area.
		if (rect.left < 0)
		{
			rect.left = 0;
		}
		if (rect.top < 0)
		{
			rect.top = 0;
		}
		if (rect.left + m_dialogWidth > gWidth)
		{
			rect.left = gWidth - m_dialogWidth;
		}
		if (rect.top + m_dialogHeight > gHeight)
		{
			rect.top = gHeight - m_dialogHeight;
		}

		rect.right = rect.left + m_dialogWidth;
		rect.bottom = rect.top + m_dialogHeight;

		return rect;
	}
	void RenderDialog()
	{
		if (m_DialogData.empty()) return;

		RECT dialogRect = GetDialogRect();

		HBRUSH brush = CreateSolidBrush(RGB(35, 35, 35));
		FillRect(m_hdc, &dialogRect, brush);
		DeleteObject(brush);

		HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
		HGDIOBJ oldPen = SelectObject(m_hdc, pen);
		HGDIOBJ oldBrush = SelectObject(
			m_hdc, GetStockObject(HOLLOW_BRUSH));

		Rectangle(m_hdc, dialogRect.left, dialogRect.top,
			dialogRect.right, dialogRect.bottom);

		SelectObject(m_hdc, oldBrush);
		SelectObject(m_hdc, oldPen);
		DeleteObject(pen);

		SetBkMode(m_hdc, TRANSPARENT);
		SetTextColor(m_hdc, RGB(230, 210, 135));

		RECT textRect = dialogRect;
		InflateRect(&textRect, -20, -20);

		for (const wchar_t* line : m_DialogData[m_dialogIndex].text)
		{
			DrawTextW(m_hdc, line, -1, &textRect,
				DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

			textRect.top += 28;
		}
	}
	void RenderAntOverlay(int ants)
	{
		RECT rect = m_renderer.GetAnt();

		// Highlight the ant-entry box.
		HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 230, 40));
		HGDIOBJ oldPen = SelectObject(m_hdc, pen);
		HGDIOBJ oldBrush =
			SelectObject(m_hdc, GetStockObject(HOLLOW_BRUSH));

		Rectangle(m_hdc, rect.left, rect.top,
			rect.right, rect.bottom);

		SelectObject(m_hdc, oldBrush);
		SelectObject(m_hdc, oldPen);
		DeleteObject(pen);

		// Display the scripted ant count.
		SetBkMode(m_hdc, TRANSPARENT);
		SetTextColor(m_hdc, RGB(0, 0, 0));

		std::string text = std::to_string(ants);

		DrawTextA(m_hdc, text.c_str(), -1, &rect,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

public:

	bool Initialize(HDC hdc)
	{
		m_hdc = hdc;
		m_renderer.Initialize(hdc);
		GameConfig config;
		config.m_DebugField = true;
		m_field.Initialize(config);

		m_players.resize(3);

		m_players[0].m_playerID = 0;
		m_players[0].m_playerName = "Unowned";

		m_players[1].m_playerID = 1;
		m_players[1].m_playerName = "Red";
		m_players[1].m_cells_owned = m_field.CountPlayerCells(1);

		m_players[2].m_playerID = 2;
		m_players[2].m_playerName = "Blue";
		m_players[2].m_cells_owned = m_field.CountPlayerCells(2);

		m_dialog.m_mode = DIALOG_GROWTH;

		return true;
	}

	void Start()
	{
		m_isRunning = true;
	}

	void Stop()
	{
		m_isRunning = false;
	}


	void Update()
	{
		if (!m_isRunning)
		{
			return;
		}
		PlayerData& player = m_players[m_activePlayer];
		Mouse();
		bool Next = m_nextRequested;

		if (m_nextRequested)
		{
			m_nextRequested = false;

			if (m_showDialog)
			{
				m_showDialog = false;
			}
		}
		// Flash Next while a dialog or ant selection is active.
		if (!Next &&
			(m_showDialog ||
				(m_state == WalkState::GROWTH && m_stateStep == 5) ||
				(m_state == WalkState::ATTACK && m_stateStep == 8)))
		{
			FlashNextButton();
		}
		else
		{
			m_nextColors = false;
		}

		switch (m_state)
		{
		case WalkState::INTRO:
			if (!m_showDialog)
			{
				m_state = WalkState::GROWTH;
				m_stateStep = 0;
			}
			return;

		case WalkState::GROWTH:
			if (m_stateStep == 0)
			{
				m_players[1].sant =
					1 + m_players[1].m_cells_owned / 3;
				m_players[1].tant = 0;
				m_players[1].selectS = true;
				m_players[1].selectT = false;

				m_field.SetPlayerVisualMode(1);
				SetMouseTarget(2, 2);
				m_stateStep = 1;
				return;
			}

			if (m_stateStep == 1)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{
					SetMouseTarget(9, 4);
					m_stateStep = 2;
				}
				return;
			}

			if (m_stateStep == 2)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{

					m_dialogIndex = 1;
					m_showDialog = true;
					m_stateStep = 3;
				}
				return;
			}
			if (m_stateStep == 3)
			{
				if (!m_showDialog && Delay(6))
				{
					m_field.SetVisualMode(9, 4, Field::V_GROWTH);
					SetMouseTarget(m_renderer.GetAnt());
					m_stateStep = 4;
				}
				return;
			}
			if (m_stateStep == 4)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{
					m_overlayAnts = m_players[m_activePlayer].sant;
					m_showAntOverlay = true;
					m_stateStep = 5;
				}
				return;
			}
			if (m_stateStep == 5)
			{

				if (Next)
				{
					m_players[m_activePlayer].tant =
						m_players[m_activePlayer].sant;

					m_players[m_activePlayer].selectS = true;
					m_players[m_activePlayer].selectT = true;

					m_showAntOverlay = false;
					m_stateStep = 6;
				}
				return;
			}
			if (m_stateStep == 6)
			{
				m_renderer.RenderHUD(
					m_players, m_dialog, m_activePlayer, m_gameSpeed);
				SetMouseTarget(m_renderer.GetConfirm());
				m_showAntOverlay = false;
				m_stateStep = 7;
				return;
			}
			if (m_stateStep == 7)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{
					m_dialogIndex = 2;
					m_showDialog = true;
					m_stateStep = 8;
				}
				return;
			}

			if (m_stateStep == 8)
			{
				if (Next)
				{
					int row = 9;
					int col = 4;
					int ants = m_players[m_activePlayer].tant;

					m_field.setAnts(row, col,
						m_field.getAnts(row, col) + ants);

					m_players[m_activePlayer].sant -= ants;
					m_players[m_activePlayer].tant = 0;

					m_field.ClearAllVisualModes();
					m_stateStep = 0;
					m_state = WalkState::ATTACK;
				}
				return;
			}
			return;

		case WalkState::ATTACK:
		{

			if (m_stateStep == 0)
			{
				m_dialog.m_mode = DIALOG_ATTACK;

				m_field.SetPlayerVisualMode(m_activePlayer);
				m_players[m_activePlayer].selectS = false;
				m_players[m_activePlayer].selectT = false;

				SetMouseTarget(9, 4);

				m_stateStep = 1;
				return;
			}
			if (m_stateStep == 1)
			{
				if (m_mouseMoving || m_mouseTap < 3)
					return;
				m_dialogIndex = 3;
				m_showDialog = true;
				m_stateStep = 2;
				return;
			}
			if (m_stateStep == 2)
			{
				if (Next)
				{
					m_showDialog = false;

					m_players[m_activePlayer].srow = 9;
					m_players[m_activePlayer].scol = 4;
					m_players[m_activePlayer].sant =
						m_field.getAnts(9, 4) - 1;
					m_players[m_activePlayer].selectS = true;
					m_field.ClearAllVisualModes();
					m_field.SetVisualMode(9, 4, Field::V_ATTACK_SOURCE);

					m_stateStep = 3;
				}
				return;
			}

			if (m_stateStep == 3)
			{
				Field::Area area = m_field.getArea(9, 4);

				for (int row = area.rmin; row <= area.rmax; row++)
				{
					for (int col = area.cmin; col <= area.cmax; col++)
					{
						if (row == 9 && col == 4)
						{
							continue;
						}

						if (m_field.getOwner(row, col) == m_activePlayer)
						{
							continue;
						}

						m_field.SetVisualMode(row, col, Field::V_HOVER);
					}
				}

				SetMouseTarget(9, 3);
				m_stateStep = 4;
				return;
			}
			if (m_stateStep == 4)
			{
				if (m_mouseMoving || m_mouseTap < 3)
					return;

				m_dialogIndex = 4;
				m_showDialog = true;
				m_stateStep = 5;
				return;
			}
			if (m_stateStep == 5)
			{
				if (Next)
				{
					m_showDialog = false;

					m_players[m_activePlayer].trow = 9;
					m_players[m_activePlayer].tcol = 3;
					m_players[m_activePlayer].selectT = true;

					m_field.ClearVisualModeType(Field::V_HOVER);
					m_field.SetVisualMode(9, 3, Field::V_ATTACK_TARGET);

					m_dialogIndex = 5;
					m_showDialog = true;
					m_stateStep = 6;
				}
				return;
			}
			if (m_stateStep == 6)
			{
				if (Next)
				{
					m_showDialog = false;
					SetMouseTarget(m_renderer.GetAnt());
					m_stateStep = 7;
				}
				return;
			}


			if (m_stateStep == 7)
			{
				if (m_mouseMoving || m_mouseTap < 3)
					return;

				m_overlayAnts = 4;
				m_showAntOverlay = true;
				m_stateStep = 8;
				return;
			}
			if (m_stateStep == 8)
			{
				if (Next)
				{
					m_showAntOverlay = false;

					// Commit the selected number of ants.

					player.tant = m_overlayAnts;
					m_field.setAnts(
						player.srow,
						player.scol,
						m_field.getAnts(player.srow, player.scol) - player.tant);

					player.sant = player.tant;
					player.tplayerID = m_field.getOwner(player.trow, player.tcol);
					player.tant = m_field.getAnts(player.trow, player.tcol);

					player.selectS = false;
					player.selectT = false;

					m_stateStep = 9;
				}
				return;
			}
			if (m_stateStep == 9)
			{
				m_dialogIndex = 6;
				m_showDialog = true;
				m_stateStep = 10;
				return;
			}
			if (m_stateStep == 10)
			{
				if (Next)
				{
					m_showDialog = false;
					SetMouseTarget(m_renderer.GetConfirm());
					m_stateStep = 11;
				}
				return;
			}
			// Simulate the battle.
			if (m_stateStep == 11)
			{
				if (!m_mouseMoving && m_mouseTap >= 3)
				{
					m_renderer.SetBattleActive(true);
					m_stateStep = 12;
				}
				return;
			}
			if (m_stateStep == 12)
			{
				player.attackRoll = 42;
				player.defenseRoll = 67;

				m_stateStep = 13;
				return;
			}
			if (m_stateStep == 13)
			{
				player.sant--;

				m_stateStep = 14;
				return;
			}
			if (m_stateStep == 14)
			{
				player.attackRoll = 88;
				player.defenseRoll = 12;
				m_stateStep = 15;
				return;
			}
			if (m_stateStep == 15)
			{
				player.tant--;
				m_stateStep = 16;
				return;
			}
			if (m_stateStep == 16)
			{
				m_field.setOwner(player.trow, player.tcol, m_activePlayer);
				m_field.setAnts(player.trow, player.tcol, player.sant);
				m_dialogIndex = 7;
				m_showDialog = true;
				m_stateStep = 17;
				return;
			}
			if (m_stateStep == 17)
			{
				if (Next)
				{
					m_showDialog = false;
					m_renderer.SetBattleActive(false);
					m_field.ClearAllVisualModes();
					m_stateStep = 0;
					m_state = WalkState::MOVE;
					return;
				}
			}
			return;
		}
		case WalkState::MOVE:
		{
			PlayerData& player = m_players[m_activePlayer];

			if (m_stateStep == 0)
			{
				m_dialog.m_mode = DIALOG_MOVE;
				m_field.SetPlayerVisualMode(m_activePlayer);

				player.selectS = false;
				player.selectT = false;

				SetMouseTarget(9, 4);
				m_stateStep = 1;
				return;
			}
			if (m_stateStep == 1)
			{
				if (m_mouseMoving || m_mouseTap < 3)
					return;

				m_dialogIndex = 8;
				m_showDialog = true;
				m_stateStep = 2;
				return;
			}
			if (m_stateStep == 2)
			{
				if (Next)
				{
					m_showDialog = false;

					player.srow = 9;
					player.scol = 4;
					player.sant = m_field.getAnts(9, 4) - 1;
					player.selectS = true;

					m_field.ClearAllVisualModes();
					m_field.SetVisualMode(9, 4, Field::V_MOVE_SOURCE);

					m_stateStep = 3;
				}
				return;
			}

			if (m_stateStep == 3)
			{
				Field::Area area = m_field.getArea(9, 4);

				for (int row = area.rmin; row <= area.rmax; row++)
				{
					for (int col = area.cmin; col <= area.cmax; col++)
					{
						if (row == 9 && col == 4)
							continue;

						if (m_field.getOwner(row, col) != m_activePlayer)
							continue;

						m_field.SetVisualMode(row, col, Field::V_HOVER);
					}
				}

				// The cursor demonstrates one valid target.
				SetMouseTarget(9, 3);
				m_stateStep = 4;
				return;
			}

			if (m_stateStep == 4)
			{
				if (m_mouseMoving || m_mouseTap < 3)
					return;

				m_dialogIndex = 9;
				m_showDialog = true;
				m_stateStep = 5;
				return;
			}

			if (m_stateStep == 5)
			{
				if (Next)
				{
					m_showDialog = false;

					player.trow = 9;
					player.tcol = 3;
					player.selectT = true;

					m_field.ClearVisualModeType(Field::V_HOVER);
					m_field.SetVisualMode(9, 3, Field::V_MOVE_TARGET);

					// Move the mouse to the ant-count control first.
					SetMouseTarget(m_renderer.GetAnt());
					m_stateStep = 6;
				}
				return;
			}
			if (m_stateStep == 6)
			{
				if (m_mouseMoving || m_mouseTap < 3)
					return;

				m_overlayAnts = 1;
				m_showAntOverlay = true;
				// Now explain the ant commitment.
				m_dialogIndex = 9;
				m_showDialog = true;
				m_stateStep = 7;
				return;
			}
			if (m_stateStep == 7)
			{
				if (Next)
				{
					m_showDialog = false;

					// Move to confirmation after the commitment dialog.
					SetMouseTarget(m_renderer.GetConfirm());
					m_stateStep = 8;
				}
				return;
			}
			if (m_stateStep == 8)
			{
				if (m_mouseMoving || m_mouseTap < 3)
					return;

				m_dialogIndex = 10;
				m_showDialog = true;
				m_stateStep = 9;
				return;
			}
			if (m_stateStep == 9)
			{
				if (Next)
				{
					m_showDialog = false;

					int ants = 1;

					// Update the HUD commitment values.
					player.tant = ants;
					player.sant = m_field.getAnts(player.srow, player.scol) - ants;

					// Apply the move to the field.
					m_field.setAnts(
						player.srow,
						player.scol,
						player.sant);

					m_field.setAnts(
						player.trow,
						player.tcol,
						m_field.getAnts(player.trow, player.tcol) + ants);

					player.selectS = false;
					player.selectT = false;

					m_field.ClearAllVisualModes();

					m_stateStep = 0;
					m_state = WalkState::TURN_TRANSITION;
				}
				return;
			}

		}
		return;

		case WalkState::TURN_TRANSITION:
		{
			if (m_stateStep == 0)
			{
				m_mousePos = { gWidth / 2, gHeight / 2 };
				m_mouseTarget = m_mousePos;

				m_dialogIndex = 11;
				m_showDialog = true;

				m_stateStep = 1;
				return;
			}
			if (Next)
			{
				Stop();
				return;
			}
		}
		return;

		}
	}


	bool HandleMouse(UINT message, LPARAM lParam)
	{
		if (message != WM_LBUTTONDOWN) return false;

		POINT point = {
			static_cast<short>(LOWORD(lParam)),
			static_cast<short>(HIWORD(lParam))
		};

		if (PtInRect(&m_nextRect, point))
		{
			m_nextRequested = true;
			return true;
		}

		return false;
	}

	void Render() 
	{
		m_renderer.Render(m_field, m_players, m_dialog,
			m_activePlayer, m_antBox, m_error, m_gameSpeed);
		if (m_showAntOverlay)
		{
			RenderAntOverlay(m_overlayAnts);
		}
		m_renderer.RenderMouse( m_mouseTrail, MOUSE_TRAIL_COUNT,
				m_mouseVisible, m_mouseTap);
		RenderNextButton();
		if (m_showDialog)
		{
			RenderDialog();
		}

	}
	bool IsRunning() const
	{
		return m_isRunning;
	}

};
